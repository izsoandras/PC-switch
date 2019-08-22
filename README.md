# PC-switch
Simple gadget to turn on my PC remotely. I could have used my motherboards Wake on LAN service, but it is disabled on my student hostel's network.

A webserver is running on the ESP8266 with a very simple REST API. After authentication the client side offers a few buttons. These buttons send http requests to the proper URI address, and the ESP performs the required action.
The available options:
- **Turn on:** imitates the pressing of the power button
- **Reset:** imitates the pressing of the reset button
- **Refresh:** asks the server for the current state of the PC (does not work at this time)

The webpage also displays the current state of the PC.

The circuit is fairly simple. You can plug the board on the place of the front panel header of the mother board. Since I used long pin headers, the wires running to the chassis's front panel can be plugged in aswell
I used two FETs paralell to the power and reset button, to imitate the press of those buttons. There are also LEDs on the same pins as the FETs' gates for debug purposes.
The PC's state could be read from the Power and HDD LEDs' state.

The current problem is that, I did not measure the LEDs' active state, just assumed it's high active. Unfortunately my motherboard is more complicated than that.
If the PC os off, the + pin of the LED is on 5V, and the - pin is on 3V. If it's turned on the pins are on 3V and 0.1V respectively.
