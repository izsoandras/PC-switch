#include<ESP8266WiFi.h>
#include<ESP8266mDNS.h>
#include<ESP8266WebServer.h>
#include<FS.h>
#include<ArduinoOTA.h>

//D1 = rerset
#define PC_RST 5
//D8 = power
#define PC_PWR 15
//D5 = power LED
#define PC_PWR_LED 14
//D4 = HDD LED
#define PC_HDD_LED 2

//SSID of the WiFi network to connect to
const char* ssid = "1601-rotacioskappa_p";
//Password of the WiFi network to connect to
const char* password = "16Konyharuha01";
//mDNS address
const char* dns = "pcswitch";//.local

//authentication data for the web server
const char* username = "seedow";
const char* userpwd = "turnon";
//authentication for OTA
const char* ota_user = "pcswitch_ota";
const char* ota_pwd = "secretpwdOta123";
//server instance
ESP8266WebServer server(80);

void connectWiFi();
void setupServer();
void turnOn();
void initSPIFFS();
void initOTA();
void getPcState();
void kill();
bool handleReadFile(String fileName);
void setup(){
    //start serial
    Serial.begin(115200);
    //put ESP to station mode
    WiFi.mode(WIFI_STA);
    //print MAC address
    Serial.print("MAC: ");
    Serial.println(WiFi.macAddress());
    //init the file system
    initSPIFFS();
    //connect to the WiFi network
    WiFi.begin(ssid, password);
    connectWiFi();
    //start mDNS server
    if(!MDNS.begin(dns))
        Serial.println("MDSN setup error");
    else
        Serial.println("MDNS started");
    //set up the webserver
    setupServer();
    //initialize over the air update
    initOTA();
    //configure pin modes
    pinMode(PC_PWR, OUTPUT);
    digitalWrite(PC_PWR, LOW);
    pinMode(PC_RST, OUTPUT);
    digitalWrite(PC_RST, LOW);
    pinMode(PC_HDD_LED, INPUT);
    pinMode(PC_PWR_LED, INPUT);
}

//call handlers of the servers
void loop(){
    ArduinoOTA.handle();
    server.handleClient();
}

void initSPIFFS(){
    //start the SPIFFS file system
    SPIFFS.begin();
    //print out the available files on serial
    Dir dir = SPIFFS.openDir("/");
    while (dir.next()) {
      String fileName = dir.fileName();
      size_t fileSize = dir.fileSize();
      Serial.printf("FS File: %s, size: %d\n", fileName.c_str(), fileSize);
    }
    Serial.printf("\n");
  
}

void connectWiFi(){
    //wait till connection is established to the WiFi
    Serial.print("Connecting to ");
    Serial.println(ssid);
    while(WiFi.status() != WL_CONNECTED){
        delay(1000);
        Serial.print('*');
    }
    //print the IP address
    Serial.println();
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
}

void setupServer(){
    //sen the index.html on empty address
    server.on("/", HTTP_GET, []{
        if(!server.authenticate(username, userpwd))
            return server.requestAuthentication();
        
        if(!handleReadFile("/index.html"))
            server.send(404, "text/plain", "Not Found");
    });
    //call the handlers of the requests
    server.on("/pcstate", HTTP_GET, getPcState);
    server.on("/hddstate", HTTP_GET, getHddState);
    server.on("/turnon", HTTP_POST, turnOn);
    server.on("/reset", HTTP_POST, reset);
    server.on("/kill", HTTP_POST, kill);
    
    server.onNotFound(handleNotFound);
    //start server
    server.begin();
    Serial.println("Server started");
}

//Streams the requested file if it exists and returns with true, returns with false otherwise
bool handleReadFile(String fileName){
    Serial.println("File read: "+fileName);
    if(SPIFFS.exists(fileName)){
        File file = SPIFFS.open(fileName, "r");
        server.streamFile(file, "html");
        file.close();
        return true;
    } else{
        return false;
    }
}
//reads the PC's power LED to decide if it is turned on
void sendPcState(){
    Serial.print("PC state: ");
    Serial.println(digitalRead(PC_PWR_LED));
    if(digitalRead(PC_PWR_LED) == HIGH)
        server.send(200, "text/plain", "On");
        
    else
        server.send(200, "text/plain", "Off");
}
//reads the PC's power LED to decide if it is turned on
void sendHddState(){
    Serial.print("HDD state: ");
    Serial.println(digitalRead(PC_HDD_LED));
    if(digitalRead(PC_HDD_LED) == HIGH)
        server.send(200, "text/plain", "On");
    else
        server.send(200, "text/plain", "Off");
}
//checks authentication, logs the requests and calls the appropriate function
void getPcState(){
    if(!server.authenticate(username, userpwd))
        return server.send(401, "text/plain", "Unauthorized");
    
    Serial.println("Get PC state");
    sendPcState();
}
//checks authentication, logs the requests and calls the appropriate function
void getHddState(){
    if(!server.authenticate(username, userpwd))
        return server.send(401, "text/plain", "Unauthorized");

    Serial.println("Get HDD state");
    sendHddState();
}
// turns on the PC by shortcircuiting the pins of the power switch
void turnOn(){
    if(!server.authenticate(username, userpwd))
        return server.send(401, "text/plain", "Unauthorized");

    Serial.println("Turn on request");
    digitalWrite(PC_PWR, HIGH);
    delay(500);
    digitalWrite(PC_PWR, LOW);
    
    delay(2000);
    sendPcState();
}
// resets the PC by shortcircuiting the pins of the power switch
void reset(){
    if(!server.authenticate(username, userpwd))
        return server.send(401, "text/plain", "Unauthorized");

    Serial.println("Reset request");
    digitalWrite(PC_RST, HIGH);
    delay(500);
    digitalWrite(PC_RST, LOW);
    
    delay(2000);
    sendPcState();
}

void kill(){
    if(!server.authenticate(username, userpwd))
        return server.send(401, "text/plain", "Unauthorized");

    Serial.println("Turn on request");
    digitalWrite(PC_PWR, HIGH);
    delay(4000);
    digitalWrite(PC_PWR, LOW);
    
    delay(2000);
    sendPcState();
}
// sends error 404 if the requested file can not be found
void handleNotFound(){
    if(!server.authenticate(username, userpwd))
        return server.send(401, "text/plain", "Unauthorized");

    if(!handleReadFile(server.uri()))
            server.send(404, "text/plain", "Not Found");
}
//initializes OTA, sets the handlers
void initOTA(){
    ArduinoOTA.setHostname(ota_user);
    ArduinoOTA.setPassword(ota_pwd);

  ArduinoOTA.onStart([]() {
    Serial.println("OTA start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nOTA End");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  Serial.println("OTA ready");
}
