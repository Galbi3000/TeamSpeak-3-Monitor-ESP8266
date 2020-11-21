// GLOBALS.H
//
// All global variables.
//

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiClient.h>

//
// WiFi variables
//

const char* ssid     = "your-ssid";
const char* password = "your-password";

//
// HTML Server variables
//

ESP8266WebServer server(90);                  // The web server using port 90
String content;

//
// TeamSpeak3 Telnet variables and defines
//

// Support up to 128 channels and 32 clients. 32 clients is the max for a free TeamSpeak server.
// These can be increased as long as microcontroler has enough RAM!
#define maxChannels 128
#define maxClients 32

// Refresh times for channel and client lists. (1000 = 1 second)
#define channelRefresh 30000
#define clientRefresh 4000

typedef struct
{
  int ID;
  int parentID;
  int channelAboveID;
  String channelName;
} ts3Channel;

typedef struct
{
  int ID;
  int channelID;
  String clientName;
} ts3Client;

WiFiClient telnet;                            // The telnet client connection
const char* telnetHost = "192.168.0.110";     // The host address for the TeamSpeak 3 server
const int telnetPort = 10011;                 // The TeamSpeak 3 query server port
const char* queryLogin = "serveradmin";       // The login name for the query server
//const char* queryLogin = "ts3mon";       // The login name for the query server

//const char* queryPass = "5WSyp+Pd";           // Old serveradmin password
//const char* queryPass = "oLqw8jM4";           // Password for ts3mon
const char* queryPass = "ftGu61dx";             // New serveradmin password

String okResponse = "error id=0 msg=ok";      // The query server's success response
int loginOK, errorCount = 0, reconnectCount = 0;

String ts3ServerName;

ts3Channel channels[maxChannels]; // List of channels on the server
ts3Client clients[maxClients];    // List of clients connected to the server
String oldNames[maxClients];      // Old list of names for tracking people leaving/joining the server

int numChannels;          // The number of channels on the server
int numClients;           // The number of clients logged in to the server
int oldNumClients;        // For tracking people leaving/joining the server
int clientCount;          // The number of clients that are not Glabi!

const int statusLED = 2;  // The on board blue LED
const int LED = 14;       // The LED used to indicate users logged on to TS.

int statusLEDState;
int LEDState;

unsigned long timeoutChan, timeoutClient;

//
// OLED Display and message scroller variables and defines
//

// Refresh time for the OLED scroller display. (1000 = 1 second)
#define displayRefresh 200

String scrollerDisplay, scrollerMessage, currentScroller;
int showScroller = 0; // Set this to the number of times a message should scroll past the screen.
int scrollerPos = 0;  // Position in the message for the scroller display.
unsigned long timeoutDisplay;

//
// Global Functions
//

void sprint(String str)
{
  #if SERIAL_ON
  Serial.print(str);
  #endif  
}

void sprintln(String str)
{
  #if SERIAL_ON
  Serial.println(str);
  #endif  
}
