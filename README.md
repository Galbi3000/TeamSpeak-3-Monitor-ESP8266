# TeamSpeak-3-Monitor-ESP8266

This project is the Arduino IDE code for programming a NodeMCU ESP-12E (ESP8266) compatible device to monitor a TeamSpeak 3 server.

It creates a simple web page accessed through port 90 on the device's IP address (accessible through a browser with an address like: 192.168.1.44:90 ) that shows the simple layout of the server's channels and the clients logged in to them.
Connected to the device is an LED for indicating that the server has people logged in as well as an OLED display to show a count clients as well as the names of people who log in or out via a scrolling ticker-tape style message on the screen.

TODO: Add details on wiring the screen and LED to the NodeMCU device.

Here is a photo of my prototype device:
![image](https://github.com/Galbi3000/TeamSpeak-3-Monitor-ESP8266/blob/master/TS3Monitor.JPG)
