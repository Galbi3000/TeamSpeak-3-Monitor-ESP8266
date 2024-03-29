#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "FreeSerif9pt7b.h"
#define TITLE_FONT FreeSerif9pt7b

#define OLED_RESET LED_BUILTIN  //4
Adafruit_SSD1306 display(OLED_RESET);

/*
#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif
*/

void scrollMessage(String message)
{
  if (scrollerMessage == "")
  {
    scrollerMessage = "          ";
    showScroller++;
  }
  scrollerMessage += message;
  scrollerMessage += "          ";
}

void drawDisplay()
{
  display.clearDisplay();
  display.setFont(&TITLE_FONT);
  display.setTextSize(1);
  if (numClients)
  {
    display.setCursor(0,12);
    display.println("TS3 Monitor");
    display.setCursor(0,34);
    display.print(numClients);
    display.print(" client");
    if(numClients > 1)
      display.print("s");
    else
      display.print("");
  }
  if (errorCount)
  {
    display.setCursor(0,12);
    display.println("TS3 Monitor");
    display.setCursor(80,34);
    display.print("E");
    display.print(errorCount);
  }
  display.setFont();
  display.setTextSize(2);
  if (showScroller)
  {
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

void updateDisplay()
{
  if (millis() - timeoutDisplay > displayRefresh)
  {
    drawDisplay();
    timeoutDisplay = millis();
  }
}
