#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

/*
 * The OLED display used in the project is the 0.96 inch 128x64 display with 4 pin 
 * SPI interface. Any 4 pin SPI model can be substituted. The SPI interface is wired
 * to the default SPI pins of the ESP8266 with the CLK on GPIO 4.
 */

// The following header file is a free bitmap font for the Adafruit GFX routines used for the
// title on the display.
#include "FreeSerif9pt7b.h"
#define TITLE_FONT FreeSerif9pt7b

#define OLED_RESET LED_BUILTIN  //4
Adafruit_SSD1306 display(OLED_RESET);

// The 128x64 display is used for this project, the following lines will force a compile error
// if the display height is not 64!
#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

// The following are C specific includes and variables for the timer interrupt display redraw.
#if DISPLAY_ON_TIMER
extern "C" {
#include "user_interface.h"
}
os_timer_t displayTimer;
#endif

/*
 * DISPLAY FUNCTIONS
 * 
 * The rest of this file contains custom functions for the display.
 * 
 */

// Initialize the interrupt timer for the display redraw
void initDisplayTimer(void)
{
#if DISPLAY_ON_TIMER
  os_timer_setfn(&displayTimer, timerCallback, NULL);   // Set the timer callback function
  os_timer_arm(&displayTimer, displayRefresh, true);    // Set the timer to a quarter of a second
#endif
}

// scrollMessage - Sets a message to be scrolled across the bottom of the OLED display.
void scrollMessage(String message)
{
  // If the scroller system is not currently displaying a message
  if (scrollerMessage == "")
  {
    scrollerMessage = "          ";   // Add leading spaces to ensure the message scrolls in from the right.
    showScroller++;                   // Enable the scroller system to display the message.
  }
  scrollerMessage += message;         // Add the actual message to the scroller!
  scrollerMessage += "          ";    // Add closing spaces to ensure the message scrolls fully off the display.
}

// drawDisplay - Redraws the OLED display. Most lines should be self explainatory!
void drawDisplay()
{
  display.clearDisplay();
  display.setFont(&TITLE_FONT); // TITLE_FONT is actually used for the title, client count and error count indicator!
  display.setTextSize(1);

  // If there is anyone logged in to the TS3 server then display a count of clients
  if (numClients)
  {
    display.setCursor(0,12);
    display.println("TS3 Monitor");
//    display.setCursor(0,34);
    display.print(numClients);
    display.print(" client");
    if(numClients > 1)
      display.print("s");
    else
      display.print("");
  }

  // If there is an error communicating with the TS3 server then display the number
  // of errors to the right of the display on the same line as the client count.
  if (errorCount)
  {
    // Display the title in case the display is 'off' when there are no users connected
    display.setCursor(0,12);
    display.println("TS3 Monitor");
    // Now display the error count as E1, E2, etc. 
    display.setCursor(80,34);   // Possition the text right of the client count
    display.print("E");
    display.print(errorCount);
  }
  display.setFont();            // Reset to the default fixed width font
  display.setTextSize(2);

  // Display the scroller if it is active
  if (showScroller)
  {
    // If there is no message being scrolled then set currentScroller to the queued message
    if (currentScroller == "")
    {
      currentScroller = scrollerMessage;
      scrollerMessage = "";
      scrollerPos = 0;
    }
    
    scrollerDisplay = currentScroller.substring(scrollerPos, scrollerPos + 10);
    scrollerPos++;
    if (scrollerPos == currentScroller.length()-10)
    {
      showScroller--;
      scrollerPos = 0;
      if (scrollerMessage != "")
      {
        currentScroller = scrollerMessage;
        scrollerMessage = "";
      }
      if (!showScroller)
      {
        currentScroller = "";
      }
    }
    display.setTextSize(2);
    display.setCursor(0,46);
    display.print(scrollerDisplay);
  }
  display.display();
}

#if DISPLAY_ON_TIMER

// Display refresh timer callback
void timerCallback(void *pArg)
{
  drawDisplay();
}

void updateDisplay()
{
  // Do nothing when display is on interrupt timer
}

#else

void updateDisplay()
{
  if (millis() - timeoutDisplay > displayRefresh)
  {
    drawDisplay();
    timeoutDisplay = millis();
  }
}

#endif

