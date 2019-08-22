//function to be called when the DOM is loaded. Sets the event listeners and initializes the web page
function onLoad(){
    console.log("Page loaded");
    document.getElementById("btnTurnon").addEventListener("click", onTurnonClick);
    document.getElementById("btnReset").addEventListener("click", onResetClick);
    document.getElementById("btnRefresh").addEventListener("click", onRefreshClick);
    document.getElementById("btnKill").addEventListener("click", onKillClick);

    document.getElementById("pcState").innerHTML = "Unknown";
    document.getElementById("pcState").style.color = "gray";

    document.getElementById("hddState").innerHTML = "Unknown";
    document.getElementById("hddState").style.color = "gray";

    getPcState();
    getHddState();
}
//sends two http requests to the server to get the info
function onRefreshClick(){
    console.log("Asking for refresh");
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = onGetPcState;
    xhttp.open("GET","pcstate",true);
    xhttp.send();

    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = onGetHddState;
    xhttp.open("GET","hddstate",true);
    xhttp.send();
}
//sends one http requests to the server to get the info
function getPcState(){
    console.log("getting PC state");
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = onGetPcState;
    xhttp.open("GET", "pcstate", true);
    xhttp.send();
}
//sends one http requests to the server to get the info
function getHddState(){
    console.log("getting HDD state");
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = onGetHddState;
    xhttp.open("GET", "hddstate", true);
    xhttp.send();
}
// handles the incoming PC state info
function onGetPcState(){
    console.log("PC state received");
    console.log(this.onreadyState);
    console.log(this.status);
    console.log(this.responseText);
    if(this.onreadyState == 4 && this.status == 200){
        console.log(this.responseText);
    }

    document.getElementById("pcState").innerHTML = this.responseText;
    if(this.responseText === "On"){
        document.getElementById("pcState").style.color = "green";
    }else if(this.responseText === "Off"){
        document.getElementById("pcState").style.color = "red";
    }else{
        document.getElementById("pcState").innerHTML = "Unknown";
        document.getElementById("pcState").style.color = "gray";
    }
}
// handles the incoming PC state info
function onGetHddState(){
    console.log("HDD state received");
    console.log(this.onreadyState);
    console.log(this.status);
    console.log(this.responseText);
    if(this.onreadyState == 4 && this.status == 200){
        console.log(this.responseText);
    }

    document.getElementById("hddState").innerHTML = this.responseText;
    if(this.responseText === "On"){
        document.getElementById("hddState").style.color = "green";
    }else if(this.responseText === "Off"){
        document.getElementById("hddState").style.color = "red";
    }else{
        document.getElementById("hddState").innerHTML = "Unknown";
        document.getElementById("hddState").style.color = "gray";
    }
}
//prepares and sends the turnon package
function onTurnonClick(){
    console.log("Turn on button pressed");
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange == onGetPcState;
    xhttp.open("POST", "turnon", true);
    xhttp.send();
}
//prepares and sends the reset package
function onResetClick(){
    console.log("Reset button pressed");
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange == onGetPcState;
    xhttp.open("POST", "reset", true);
    xhttp.send();
}

function onKillClick(){
    console.log("Kill button pressed");
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange == onGetPcState;
    xhttp.open("POST", "kill", true);
    xhttp.send();
}