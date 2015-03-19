#include <pls.h>
#include "WS2812_Definitions.h"

// this sketch is used to demonstrate the various light effects
// that are provided by the PLS library

// Color arrays
int black[3]  = { 0, 0, 0 };
int white[3]  = { 255, 255, 255 };
int red[3]    = { 255, 0, 0 };
int green[3]  = { 0, 255, 0 };
int blue[3]   = { 0, 0, 255 };
int yellow[3] = { 250, 250, 0 };
int dimWhite[3] = { 76, 76, 76 };
int orange[3] = { 255, 100, 0 };
// etc.

RGBStrip Strip(5, 6, 3);
unsigned long thecolors[5] = {NAVY, LIME, DARKRED, ORANGE, CYAN};
unsigned long tocolors[5] = {BLACK, DARKBLUE, TEAL, SEAGREEN, BROWN};
int thedurations[5] = {200, 100, 200, 100, 50};

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
int i, 
    j;
unsigned long CurrentMillis;
boolean Active = false;

// ====== Show lightning and Brightness ===========
Serial.println("Start with a color and slowly dim brightness to zero");
for (i=100; i>=0; i--)
  {
  Strip.LightStrip(255, 0, 0);  // light strip red
  Strip.SetBrightness(i);       // reduce brightness step by step
  delay(20);
  }

// ====== Show Rainbow color effect ==========
Serial.println("Restore brightness to 100%");
Strip.SetBrightness(100);

Serial.println("Start RainbowColorChange with speed 7");
Strip.SetRainbowSpeed(7);   // slow rainbow effect
CurrentMillis = millis();
do {
Strip.RainbowColorChange(millis());
} while (millis() - CurrentMillis < 5000);  // do this for 5 seconds

Strip.SwitchOff();
delay(2000);

Serial.println("Start RainbowColorChange with speed 1");
Strip.SetRainbowSpeed(1);   // repeat rainbow effect - just faster
CurrentMillis = millis();
do {
  Strip.RainbowColorChange(millis());
} while (millis() - CurrentMillis < 5000);  // do this for 5 seconds

Strip.SwitchOff();
delay(2000);

// ====== Show TwoColorFlash ==========
Serial.println("Start TwoColorFlash");
Strip.SetupTwoColorFlash(blue, yellow, 200, 200, 10000); 
do {
  Strip.TwoColorFlash(millis(), &Active);
} while (Active);

Strip.SwitchOff();
delay(2000);

// ====== Show MultiColorFlash ==========
Serial.println("Start MultiColorFlash");
Strip.SetupMultiColorFlash(5, thecolors, thedurations, false, 5000); 

do {
  Strip.MultiColorFlash(millis(), &Active);
} while (Active);

Strip.SwitchOff();
delay(2000);

// ====== Show TwoColorFade ==========
Serial.println("Start TwoColorFade");
for (i=0; i<=4; i++)
  for (j=0; j<=4; j++)
    {
    Strip.SetupTwoColorFade(thecolors[i], tocolors[j], 1, 1, 5000);
    do {
      Strip.TwoColorFade(millis(), &Active);
      } while (Active);
    }
}


