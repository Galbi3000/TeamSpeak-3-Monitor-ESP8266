/*
Note: This is not an actual source file. The whole file has been commented out to prevent errors.

TeamSpeakMonitor Revisions

Current version 2.33

2.33 - A missed issue where the scroller for clients logging in repeats for clarity but logging
       out did not, fixed now so that both have repeats.
     - Source code cleaned up to remove unused timer switches.

2.32 - Fixed a bug in the code for checking names logged out. It was using the wrong list
       of names to create the scroll message.
     - Fixed a bug in the code for checking names logged in. It was comparing names from the
       same list! Amazing how it still showed new names though!
     - Added a bit of clean up to the lists of names so that the list always contains empty
       strings after the last name.

2.31 - Added a line in the scroller routine to also reset the scrollerPos when setting 
       currentScroller to the next message.
     - Added commas between names in scroll message when more than one person logs in/out.

2.30 - Added scroller message indicating who logs in or out of the TeamSpeak 3 server.

2.20 - Moved global variables and functions into Globals.h
     - Moved the HTML Server functions to HTMLFunctions.h
     - Moved the TS3 Telnet functions to TEAMSPEAKFunctions.h
     - Moved the OLED Display functions to Display.h
     The above changes tidy the main file to just the setup() and loop() functions and
     makes it easier to find functions that may need changing.

2.15 - Moved the display refresh routine into a timer call-back to prevent visible
       freezes in the scrolling messages while the server is being queried.
       This turns out to be buggy so added optional compiler switch DISPLAY_ON_TIMER.

2.10 - Tweaked the display content using different fonts.
     - Added message scroller code ready for displaying who logged in/out.
     - Put display refresh (redraw) in own 'timer' for scroller speed control. 
       NOTE: See BUGS section at the end of this file for timer bug details.

2.00 - Added an OLED screen to the hardware side of the project. Currently using the
       Adafruit libraries which have ESP8266 support.
     - LED pin reassigned to prevent conflict with the OLED data pins.
     - Added code to display start-up progress to the OLED display.
     - Added code to display the number of connected clients on the TS server on the
       OLED screen. When no clients are connected the screen goes blank.

1.01 - Modified the Initial code for the NodeMCU device that the sketch is now used on.
     - Changed the TeamSpeak server web page to the root page.

1.00 - Initial version consisting of code copied from another sketch which provided the
       web server and LED control as well as the TeamSpeak code.
     - Removed the unrelated code for a test page controlling some LEDs.
       
TODO:

- Add a timeout for blanking the OLED screen when there are no clients on the TS server
  so it displays 0 clients for a while before blanking.

BUGS:

- Names with extended characters like é, è, ö, etc. do not display correctly.
  This seems to be an issue with the way the WiFi libraries process telnet data, perhaps telnet
  does not permit these characters!

- If the display redraw is on a timer then it stops working after a while! For now it's been
  disabled with #define DISPLAY_ON_TIMER 0. Setting this to 1 will re-enable the timer.

- Sometimes when a user logs in or out the name is not displayed in the scrolling message.
  Very intermittent! (Might be fixed in update 2.31, time will tell!)

*/
