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

// Refresh times for channel and client lists. (1000 = ~1 second)
#define channelRefresh 30000
#define clientRefresh 4000

// Structure definition for a TeamSpeak 3 channel
typedef struct
{
  int ID;
  int parentID;
  int channelAboveID;
  String channelName;
} ts3Channel;

// Structure definition for a TeamSpeak 3 client
typedef struct
{
  int ID;
  int channelID;
  String clientName;
} ts3Client;

// Variables and constants for the TeamSpeak 3 server telnet access
WiFiClient telnet;                            // The telnet client connection
const char* telnetHost = "192.168.0.110";     // The host address for the TeamSpeak 3 server
const int telnetPort = 10011;                 // The TeamSpeak 3 query server port
const char* queryLogin = "serveradmin";       // The login name for the query server
const char* queryPass = "querypass";          // The password for the query server
/* NOTE:
  The IP address I have here is the LAN IP address to my TS3 server that I run on a
  dedicated PC on my LAN. This is obviously not accessable from the internet as this IP!

  The login name used above is the default one for the admin of the server.
  The password associated with it is provided by the server the first time it is run.
  If you are using a hosted server you may not have access to this login name. You may need
  to be given a telnet access password for your server login. As I do not use a hosting
  service I do not know the specifics for this. Speak to a representative for your hosting
  service provider for further information about how they supply telnet access to the server.

  The telnetPort value above is the default telnet port number for TS3 servers. This might be
  different for hosted servers. Again, speak to your TS3 server hosting provider for details.
*/

String okResponse = "error id=0 msg=ok";      // The query server's success response
int loginOK, errorCount = 0;

String ts3ServerName;

ts3Channel channels[maxChannels]; // List of channels on the server
ts3Client clients[maxClients];    // List of clients connected to the server
String oldNames[maxClients];      // Old list of names for tracking people leaving/joining the server

int numChannels;          // The number of channels on the server
int numClients;           // The number of clients logged in to the server
int oldNumClients;        // For tracking people leaving/joining the server
int clientCount;          // The number of clients that are not Glabi!

const int statusLED = 2;  // The on board blue LED (currently not actually used!)
const int greenLED = 14;  // The green LED used to indicate users logged on to TS.

int statusLEDState;
int greenLEDState;

unsigned long timeoutChan, timeoutClient;

//
// OLED Display and message scroller variables and defines
//

// Refresh time for the OLED display. (1000 = ~1 second)
#define displayRefresh 250

// Scroller related variables. These are in Globals.h instead of OLEDDisplay.h for use in
// TeamSpeakFunctions.h
// The scroller system can queue one message while another is already being displayed.

String scrollerDisplay;   // Used for the section of the message shown on the display
String scrollerMessage;   // This is the next message to be displayed
String currentScroller;   // This is the current message being displayed when the scroller is active
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

