/*
 *  This sketch is designed for an NodeMCU ESP-12E device and provides a simple web server 
 *  on port 90 which gives the user a web page on which they can see the channel list for 
 *  the TeamSpeak 3 server as well as any people logged in to the channels.
 *
 *  The sketch will light up an LED connected to GPIO 14 when there are users
 *  logged in to the TeamSpeak 3 server. It can optionally ignore certain usernames for
 *  the LED. For example I made it ignore all names I log in with on my TS3 server so that 
 *  it is not telling me I am logged in as I already know I am! See commented out section
 *  for more details on this feature.
 *  
 *  Finally an OLED display is connected to the NodeMCU board to display a count of users
 *  connected to the TeamSpeak 3 server as well as scroll the name of a user who has logged
 *  in or out of the TS3 server.
 *
 *  See Revisions tab for details of version changes.
 */

#define SERIAL_ON 0

#include "Globals.h"
#include "TeamSpeakFunctions.h"
#include "HTMLFunctions.h"
#include "OLEDDisplay.h"

void setup()
{
  // Initialize the OLED display and put the device name at the top of the display
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setRotation(2);
  display.clearDisplay();           // Clears the display buffer
  display.setTextSize(2);           // Set the text size to 2
  display.setTextColor(WHITE);      // Set the text colour to white. The OLED display used only has 2 colours, black (default back colour) and white.
  display.setFont(&TITLE_FONT);
  display.setTextSize(1);
  display.setCursor(0,12);
  display.println("TS3 Monitor");
  display.setFont();
  display.setCursor(0,16);
  display.display();                // Refresh the OLED display to show the changes made

  // Initialise the LEDs
  pinMode(statusLED, OUTPUT);
  pinMode(LED, OUTPUT);
  digitalWrite(statusLED, 1);
  digitalWrite(LED, 0);
  statusLEDState = 1;
  LEDState = 0;

  // Start the serial connection
  #if SERIAL_ON
  Serial.begin(115200);
  #endif

  // Connect to the WiFi router
  WiFi.begin(ssid, password);

  // Output connecting message to serial
  sprintln("");
  sprint("Connecting to WiFi");

  // Output connecting message to the OLED display
  display.setTextSize(1);
  display.println("Connecting to WiFi..");
  display.display();

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    sprint(".");
  }

  // Provide connection details to serial output
  sprintln("");
  sprint("Connected to ");
  sprintln(ssid);
  sprint("IP address: ");
  //sprintln(String(WiFi.localIP()));
  sprintln(WiFi.localIP().toString());

  // Display connection status on OLED screen
  display.println("Connected to ");
  display.println(ssid);
  display.display();

  // HTTP Server
  
  // Set up the web page handlers
  server.on("/", handleRoot);
  server.onNotFound(handleNotFound);
  
  // Here the list of headers to be recorded
  const char * headerkeys[] = {"User-Agent","Cookie"};
  size_t headerkeyssize = sizeof(headerkeys)/sizeof(char*);

  // Ask server to track these headers
  server.collectHeaders(headerkeys, headerkeyssize);

  server.begin();
  sprintln("HTTP server started");
  sprintln("");

  // Display the HTTP server status on the OLED screen
  display.println("HTTP server started.");
  display.display();

  // Indicate connecting to the TS3 server on the OLED screen
  display.println("Connecting to TS3.");
  display.display();

  // Telnet Client for TeamSpeak 3 ServerQuery
  loginServerQuery();

  timeoutChan = millis() + channelRefresh;   // Set the timeout for Channel list refresh so that it's run once at start
  timeoutClient = millis() + clientRefresh;  // Do the same for client list refreshing
  errorCount = 0;
  reconnectCount = 0;

  // Display a finished message on the OLED screen and wait 3 seconds to allow reading of the screen
  display.println("Finished setup.");
  display.display();
  delay(3000);

//  scrollerMessage = "          This is a test scroll message.          ";
//  showScroller = 3;
}

void loop()
{
  // HTTP first
  server.handleClient();  // Done, HTTP server handling is easy in loop()!

  // Refresh the channel and client lists from the TeamSpeak server (the functions handle the timing)
  refreshChannels();
  refreshClients();

  if (errorCount == 0)
    reconnectCount = 0;

  // If the number of clients has changed then see what names have been added/removed

  if (numClients < oldNumClients)       // Someone logged out
  {
    String message = "";
    for (int i = 0; i < oldNumClients; i++)
    {
      int found = 0;
      for (int j = 0; j < numClients; j++)
      {
        if (oldNames[i] == clients[j].clientName)
          found = 1;
      }
      if (!found)
      {
        if (message == "")
          message = oldNames[i];
        else
          message += ", " + oldNames[i];
      }
    }
    message += " logged out.";
    message += "........ " + message;
    scrollMessage(message);
  }
  else if (numClients > oldNumClients)  // Someone logged in
  {
    String message = "";
    for (int i = 0; i < numClients; i++)
    {
      int found = 0;
      for (int j = 0; j < oldNumClients; j++)
      {
        if (clients[i].clientName == oldNames[j])
          found = 1;
      }
      if (!found)
      {
        if (message == "")
          message = clients[i].clientName;
        else
          message += ", " + clients[i].clientName;
      }
    }
    message += " logged in.";
    message += "........ " + message;
    scrollMessage(message);
  }

  if (oldNumClients != numClients)
  {
    int i;
    for (i = 0; i < numClients; i++)
      oldNames[i] = clients[i].clientName;
    // Make sure the ends of the lists are empty, may cause issues if there are names still in place that shouldn't be there
    for (; i < maxClients; i++)
    {
      oldNames[i] = "";
      clients[i].clientName = "";
    }
    oldNumClients = numClients;
  }
  
  // Update the OLED display (the function handles timing)
  updateDisplay();

  if (errorCount >= 5)
  {
    sprintln("Attempting to regain communications with TeamSpeak Server.");
    loginOK = false;
    telnet.stop();
    loginServerQuery();
    errorCount = 0;
    reconnectCount += 1;
    scrollMessage("ERROR: Reconnecting to TS3 Server");

    if (WiFi.status() != WL_CONNECTED)
    {
      sprintln("Lost WiFi connection!");
      
      // Connect to the WiFi router
      WiFi.begin(ssid, password);
      
      // Wait for connection
      while (WiFi.status() != WL_CONNECTED)
      {
        // Output connecting message to serial
        sprintln("");
        sprint("Reconnecting to WiFi");
  
        delay(500);
        sprint(".");
      }  
      sprintln("");
    }
  }

  if (reconnectCount == 10)
  {
    sprintln("Tried reconnecting 10 times, rebooting!");
    ESP.restart();
  }

  int state = LEDState;
  if (clientCount == 0 && LEDState == 1)
    LEDState = 0;
  else if (clientCount > 0 && LEDState == 0)
    LEDState = 1;    
  if (state != LEDState)
    digitalWrite(LED, LEDState);
}
