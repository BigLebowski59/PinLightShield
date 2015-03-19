/* -----------------------------------------------------------
 pls.h  -  Library for PinLightShield
 Author: H.Koller, November 2014
 Released into the public domain.

 Change Log:
 Version 0: initial version
 Version 1:   added support for "unsigned long colors"
	      added more useful color conversion functions
	      added function SwitchOff
	      function LightStrip can now be used in 3 variants
	      added function MultiColorFlash
---------------------------------------------------------------*/

#include "Arduino.h"
#include "Pls.h"


// ------------ Function to copy one color into another one ---------
void CopyColor(int * destColor, int srcColor[3])
{
int i;

for (i=0; i<=2; i++)
  destColor[i] = srcColor[i];
}  

// ------------ Function to convert RGB color into an unsigend long value ---------
unsigned long RGB2Long(byte red, byte green, byte blue)
{
return ((unsigned long)red << 16) | ((unsigned long)green << 8) | blue;
}  

// ------------ Function to convert an unsigend long color into 3 values for RGB ---------
void Long2RGB(unsigned long color, byte * red, byte * green, byte * blue)
{
*red = byte(color >> 16);
*green = byte(color >> 8);
*blue = byte(color);
}  

void Long2RGB(unsigned long color, int * red, int * green, int * blue)
{
*red = byte(color >> 16);
*green = byte(color >> 8);
*blue = byte(color);
}  

void Long2RGB(unsigned long color, int RGB[3])
{
RGB[0] = byte(color >> 16);
RGB[1] = byte(color >> 8);
RGB[2] = byte(color);
}  

byte GetRed(unsigned long color)
{
return (color & 0xff0000) >> 16;
}

byte GetGreen(unsigned long color)
{
return (color & 0x00ff00) >> 8;
}

byte GetBlue(unsigned long color)
{
return (color & 0x0000ff);
}


// ===============================================================
// Implementation of class RGBStrip

// -----------  Constructor for RGBStrip --------------

RGBStrip::RGBStrip(int redpin, int greenpin, int bluepin, int brightness)
{
pinMode(redpin, OUTPUT);

_redpin = redpin;
pinMode(greenpin, OUTPUT);
_greenpin = greenpin;
pinMode(bluepin, OUTPUT);
_bluepin = bluepin;

_brightness = constrain(brightness, 0, 100);	// default value is full brightness

_rainbowred = 0;
_rainbowblue = 0;
_rainbowgreen = 0;
_startrainbow = 0;
_fadespeed_rainbow = 7;

SwitchOff();		// initially always switch it off
}

// ------------ Functions to light an RGBStrip  ---------------

void RGBStrip::LightStrip(unsigned long color)
{
analogWrite(_redpin, GetRed(color)*_brightness/100);
analogWrite(_greenpin, GetGreen(color)*_brightness/100);
analogWrite(_bluepin, GetBlue(color)*_brightness/100);
}

void RGBStrip::LightStrip(int redval, int greenval, int blueval)
{
analogWrite(_redpin, redval*_brightness/100);
analogWrite(_greenpin, greenval*_brightness/100);
analogWrite(_bluepin, blueval*_brightness/100);
}

void RGBStrip::LightStrip(int color[3])
{
analogWrite(_redpin, color[0]*_brightness/100);
analogWrite(_greenpin, color[1]*_brightness/100);
analogWrite(_bluepin, color[2]*_brightness/100);
}

void RGBStrip::SwitchOff()
{
LightStrip((unsigned long)0);
}


// -------- Function to flash the Strip a certain ---------
// -------- number of times with a certain length ---------
// Attention: uses delay() function so use only if nothing else can
//            happen during the call to this function

void RGBStrip::MakeFlashes(unsigned long color, int flashes, int flashlength) 
{
int i;

for (i=1; i<=flashes; i++) {
  LightStrip(color);
  delay(flashlength);
  if (i < flashes)        // switch off only between flashes, not after the last one
    { 
    LightStrip((unsigned long)0);
    delay(flashlength);
    }
  }
}

void RGBStrip::SetBrightness(int brightness)
{
if (brightness >= 0 && brightness <= 100)
  _brightness = brightness;
else
  _brightness = 100;
}

void RGBStrip::RainbowColorChange(unsigned long CurrentMillis)
{
int TimeExpired;
// rainbow color change
if (CurrentMillis - _startrainbow >= _fadespeed_rainbow*6*256)
  {
  _startrainbow = CurrentMillis;
  }
else
  {
  TimeExpired = CurrentMillis - _startrainbow;
  // fade from blue to violet
  if (TimeExpired < _fadespeed_rainbow*1*256)
    {
    _rainbowred = TimeExpired/_fadespeed_rainbow;
    }
  // fade from violet to red
  else if (TimeExpired < _fadespeed_rainbow*2*256)
    {
    _rainbowblue = 255 - (TimeExpired - _fadespeed_rainbow*1*256)/_fadespeed_rainbow;
    }
  // fade from red to yellow
  else if (TimeExpired < _fadespeed_rainbow*3*256)
    {
    _rainbowgreen = (TimeExpired - _fadespeed_rainbow*2*256)/_fadespeed_rainbow;
    }
  // fade from yellow to green
  else if (TimeExpired < _fadespeed_rainbow*4*256)
    {
    _rainbowred = 255 - (TimeExpired - _fadespeed_rainbow*3*256)/_fadespeed_rainbow;
    }
  // fade from green to teal
  else if (TimeExpired < _fadespeed_rainbow*5*256)
    {
    _rainbowblue = (TimeExpired - _fadespeed_rainbow*4*256)/_fadespeed_rainbow;
    }
  // fade from teal to blue
  else if (TimeExpired < _fadespeed_rainbow*6*256)
    {
    _rainbowgreen = 255 - (TimeExpired - _fadespeed_rainbow*5*256)/_fadespeed_rainbow;
    }
  } 
LightStrip(_rainbowred, _rainbowgreen, _rainbowblue);
} /* rainbowColorChange */

void RGBStrip::SetRainbowSpeed(int RainbowSpeed)
{
_fadespeed_rainbow = RainbowSpeed;
if (_fadespeed_rainbow <= 0)
  _fadespeed_rainbow = 1;
}

void RGBStrip::SetupMultiColorFlash(byte nrofcolors, unsigned long colors[5], int durations[5], boolean                                                randsequence, int FlashDuration)
{
int i;

if (nrofcolors > 5)
  _nrofcolors = 5;
else
  _nrofcolors = nrofcolors;

memcpy(_colors, colors, sizeof(_colors));
memcpy(_durations, durations, sizeof(_durations));
_randsequence = randsequence;
_ActiveColorIndex = 0;  // always start with first color in array
_MultiFlashDuration = FlashDuration;
_MultiFlashStartTime = 0;
}


void RGBStrip::MultiColorFlash(unsigned long CurrentMillis, boolean * FlashActive)
{
if (*FlashActive == false)   // signal detected for the first time
  {
  _MultiFlashStartTime = CurrentMillis;
  _LastMultiColorSwitch = CurrentMillis;
  *FlashActive = true;
  }
else              // we want to make sure, that this lasts for a while to really see some effect
  {
  if (CurrentMillis - _MultiFlashStartTime > _MultiFlashDuration)
    {
    *FlashActive = false;
    return;
    }
  }
  
if (CurrentMillis - _LastMultiColorSwitch >=    // determines the length of the flash               
    _durations[_ActiveColorIndex])               // (different for each color)
    {
    _LastMultiColorSwitch = CurrentMillis;     // store time of last color switch
    if (_randsequence)
       _ActiveColorIndex = random(0, _nrofcolors-1);
    else
       {
       _ActiveColorIndex++;                       // switch color
       if (_ActiveColorIndex > _nrofcolors-1)     // handle overrun
          _ActiveColorIndex = 0;
       }
    }

LightStrip(_colors[_ActiveColorIndex]);
}

void RGBStrip::SetupTwoColorFlash(int color1[3], int color2[3], 
                                  int Col1Duration, int Col2Duration, int FlashDuration)
{
CopyColor(_color1, color1);
CopyColor(_color2, color2);
_Col1Duration = Col1Duration;
_Col2Duration = Col2Duration;
_FlashDuration = FlashDuration;
_Color1Active = true;
_FlashStartTime = 0;
}

void RGBStrip::TwoColorFlash(unsigned long CurrentMillis, boolean * FlashActive)
{
if (*FlashActive == false)   // signal detected for the first time
  {
  _FlashStartTime = CurrentMillis;
  _LastColorSwitch = CurrentMillis;
  *FlashActive = true;
  }
else              // we want to make sure, that this lasts for a while to really see some effect
  {
  if (CurrentMillis - _FlashStartTime > _FlashDuration)
    {
    *FlashActive = false;
    return;
    }
  }
  
if (_Color1Active)
  {
  if (CurrentMillis - _LastColorSwitch >= _Col1Duration)    // determines the length of the flash (different for                                                             // each color)
    {
    _LastColorSwitch = CurrentMillis;
    _Color1Active = false;                    // switch color
    }
  }
else
  {
  if (CurrentMillis - _LastColorSwitch >= _Col2Duration)    // determines the length of the flash (different for                                                             // each color)
    {
    _LastColorSwitch = CurrentMillis;
    _Color1Active = true;                    // switch color
    }
  }

if (_Color1Active)
  LightStrip(_color1);
else
  LightStrip(_color2);
}

// ============= functions for TwoColorFade ================
void RGBStrip::SetupTwoColorFade(unsigned long fadecolorfrom, unsigned long fadecolorto, int fadestep, int fadespeed, int FadeDuration)
{
int coldiff[3];
int i;

Long2RGB(fadecolorfrom, _fadecolorfrom);
Long2RGB(fadecolorto, _fadecolorto);

_fadespeed = fadespeed;   // the speed of the color change
_FadeDuration = FadeDuration;

if (fadestep < 1)
  fadestep = 1;
if (fadestep > 5)
  fadestep = 5;

for (i=0; i<=2; i++)
  {
  coldiff[i] = _fadecolorfrom[i] - _fadecolorto[i];
  if (coldiff[i] < 0)
    _fadedir[i] = +1;
  else
    _fadedir[i] = -1;
  coldiff[i] = abs(coldiff[i]);
  }

if (coldiff[0] >= coldiff[1] && coldiff[0] >= coldiff[2])
  {
  _fadestep[0] = 100 * fadestep;
  _fadestep[1] = 100 * fadestep * coldiff[1]/coldiff[0];
  _fadestep[2] = 100 * fadestep * coldiff[2]/coldiff[0];
  }
else
  if (coldiff[1] >= coldiff[0] && coldiff[1] >= coldiff[2])
    {
    _fadestep[1] = 100 * fadestep;
    _fadestep[0] = 100 * fadestep * coldiff[0]/coldiff[1];
    _fadestep[2] = 100 * fadestep * coldiff[2]/coldiff[1];
    }
  else
    {
    _fadestep[2] = 100 * fadestep;
    _fadestep[1] = 100 * fadestep * coldiff[1]/coldiff[2];
    _fadestep[0] = 100 * fadestep * coldiff[0]/coldiff[2];
    }

for (i=0; i<=2; i++)
  _fadecolor[i] = _fadecolorfrom[i] * 100;
}

boolean RGBStrip::DetectColorLimit(int color, int fadecolorfrom, int fadecolorto, int colordir)
{
if (colordir == +1)
  {
  if (color > max(fadecolorfrom, fadecolorto))  // im ansteigen über´s Ziel geschossen
    {
    return true;
    }
  }
else
  {
  if (color < min(fadecolorfrom, fadecolorto))  // im ansteigen über´s Ziel geschossen
    {
    return true;
    }
  }
return false;
}

void RGBStrip::SwitchDir()
{
int i;
for (i=0; i<=2; i++)
  {
  if (_fadedir[i] == +1)
    _fadecolor[i] = max(_fadecolorfrom[i], _fadecolorto[i]) * 100;
  else
    _fadecolor[i] = min(_fadecolorfrom[i], _fadecolorto[i]) * 100;
  _fadedir[i] = -_fadedir[i];
  }
}

void RGBStrip::TwoColorFade(unsigned long CurrentMillis, boolean *FadeActive)
{
int i;

if (*FadeActive == false)   // signal detected for the first time
  {
  _FadeStartTime = CurrentMillis;
  _LastFadeStep = CurrentMillis;
  *FadeActive = true;
  }
else              // we want to make sure, that this lasts for a while to really see some effect
  {
  if (CurrentMillis - _FadeStartTime > _FadeDuration)
    {
    *FadeActive = false;
    return;
    }
  }

if (CurrentMillis - _LastFadeStep >= _fadespeed)    // determines the speed of the color change
  {
  _LastFadeStep = CurrentMillis;
  for (i=0; i<=2; i++)
    _fadecolor[i] = _fadecolor[i] + _fadedir[i] * _fadestep[i];

  // if one of the color components reaches the other end, all components are set to the other color
  // and we continue in the other direction
  if ((DetectColorLimit(_fadecolor[0], _fadecolorfrom[0] * 100, _fadecolorto[0] * 100, _fadedir[0])) ||  
      (DetectColorLimit(_fadecolor[1], _fadecolorfrom[1] * 100, _fadecolorto[1] * 100, _fadedir[1])) ||
      (DetectColorLimit(_fadecolor[2], _fadecolorfrom[2] * 100, _fadecolorto[2] * 100, _fadedir[2])))
    SwitchDir();
  }

LightStrip(_fadecolor[0]/100, _fadecolor[1]/100, _fadecolor[2]/100);
}

// --------- end of implementation of class RGBStrip ---------
// ===============================================================


// ===============================================================
// Implementation of class Std12VOutput

// -----------  Constructor for Std12VOutput--------------

Std12VOutput::Std12VOutput(int pin)
{
pinMode(pin, OUTPUT);
_pin = pin;
_delaytime = 0;
_starttime = 0;

Output(0);	// initially switch it off
}

// Function to send a signal to that device
void Std12VOutput::Output(int val)
{
val = constrain(val, 0, 255);
analogWrite(_pin, val);
}

/* Function to light an LED strip or an LED or shake a shaker motor
   for a specified time without using the delay() function
*/
void Std12VOutput::OutputWithDelay(int val, int delaytime, unsigned long CurrentMillis, boolean *OutputActive)
{
val = constrain(val, 0, 255);
if (_delaytime = 0)	// set initially and at the end of delayed 
  {
  _delaytime = delaytime;
  _starttime = CurrentMillis;
  }
if (CurrentMillis - _starttime > _delaytime)
  {
  Output(0);	// time over ==> switch off
  _delaytime = 0;
  _starttime = 0;
  *OutputActive = false;
  }
else
  {
  Output(val);
  *OutputActive = true;
  }
}

// Function to flash the Strip a certain number of times with a certain length
void Std12VOutput::MakeFlashes(int val, int flashes, int flashlength) 
{
int i;

for (i=1; i<=flashes; i++) 
  {
  Output(val);
  delay(flashlength);
  if (i < flashes)        // switch off only between flashes, not after the last one
    { 
    Output(0);
    delay(flashlength);
    }
  }
}

// --------- end of implementation of class Std12VOutput ---------
// ===============================================================


// ===============================================================
// Implementation of class Switch

// -----------  Constructor for Switch --------------

Switch::Switch(int pin, int switchwait)
{
pinMode(pin, INPUT);
_pin = pin;
_switchwait = switchwait; // how long do we wait in method ReadSwitchDelayed until we 
                          // return true (this is for switches like in the ball trough 
			  // where the ball rolls through and does not stay in that place)
_closetime = 0;
}

// ------------ Function to read the switch -------------

boolean Switch::ReadSwitch()
{
return digitalRead(_pin);  // returns true for a closed switch and false for an open one
}

// Function to read a switch but only return true if it was closed for a certain time
// This function is used e.g. for switches in the ball trough where we only want to
// report "switch closed" when the ball stays there for a while

boolean Switch::ReadSwitchDelayed(unsigned long CurrentMillis)
{
if (digitalRead(_pin) == HIGH)  // switch closed
  {
  if (_closetime == 0)		 // switch was open before
    _closetime = CurrentMillis;  // remember when switch was closed
  if (CurrentMillis - _closetime > _switchwait)  // was it closed long enough?
    {
    _closetime = 0;
    return true;    // switch was closed long enough to assume that the ball remains there
    }
  else
    return false;   // not yet closed long enough
  }
else
  {
  _closetime = 0;
  return false;
  }
}

// --------- end of implementation of class Switch ---------
// ===============================================================

// ===============================================================
// Implementation of class OptoSwitch

// -----------  Constructor for OptoSwitch --------------

OptoSwitch::OptoSwitch(int pin, int switchwait)
{
pinMode(pin, INPUT);
_pin = pin;
_switchwait = switchwait; // how long do we wait in method ReadSwitchDelayed until we 
                          // return true (this is for switches like in the ball trough 
			  // where the ball rolls through and does not stay in that place
_closetime = 0;
}

// Function to read an opto switch (returns true when the switch is open)
boolean OptoSwitch::ReadSwitch()
{
return !digitalRead(_pin);  // returns true for an open switch and false for a closed one
}

// Function to read an opto switch but only return true if it was open for a certain time
// This function is used e.g. for switches in the ball trough where we only want to
// report "switch closed" when the ball stays there for a while
boolean OptoSwitch::ReadSwitchDelayed(unsigned long CurrentMillis)
{
if (digitalRead(_pin) == LOW)  // switch open (usually means "ball present")
  {
  if (_closetime == 0)		 // switch was open before
    _closetime = CurrentMillis;  // remember when switch was closed
  if (CurrentMillis - _closetime > _switchwait)  // was it closed long enough?
    {
    _closetime = 0;
    return true;    // switch was closed long enough to assume that the ball remains there
    }
  else
    return false;   // not yet closed long enough
  }
else
  {
  _closetime = 0;
  return false;
  }
}
// --------- end of implementation of class Opto Switch ---------
// ===============================================================

/* ===============================================================
   Implementation of class Insert:

   For Inserts we need a few variables that define the delays we need to determine the state of the Insert
   _FilterDelay: this deals with the fact, that a lamp in the lamp matrix is not really constantly on 
                 when it´s "ON". It rather gets quick pulses (e.g. ~25ms for WPC) which make it seem to be "ON".
                 With this Delay we filter out those pulses so that the function InsertOn returns a constant
                 "true" when the lamp is ON.
   _InsertOnDelay: time to wait before we can say that an Insert changed from "Blinking" to "On" (value depends
                   on the frequency in which the insert flashes). If the value of this variable is
                    chosen too low the function GetBlinkInsertState may detect an ON-state when actually the
                    insert is still flashing.
   _InsertOffDelay: time to wait before we can say that an Insert changed from "Blinking" to "Off" (value
                    depends on the frequency in which the insert flashes). If the value of this variable is
                    chosen too low the function GetBlinkInsertState may detect an OFF-state when actually the
                    insert is still flashing.
*/ 

// -----------  Constructor for Insert --------------
Insert::Insert(int pin, int FilterDelay, int InsertOnDelay, int InsertOffDelay)
{
pinMode(pin, INPUT);
_pin = pin;
_filterdelay = FilterDelay;
_insertondelay = InsertOnDelay;
_insertoffdelay = InsertOffDelay;
_lastinserton = 0;
_lastinsertoff = 0;
_state = 0;
}

// ----------- Function to read Inserts that are only ON or OFF ----------
boolean Insert::InsertOn(unsigned long CurrentMillis)
{
boolean inserton;

if (digitalRead(_pin) == HIGH) 		// raising edge detected ==> Insert is ON
  {
  inserton = true;	
  _lastinserton = CurrentMillis;	// remember time of last HIGH
  }
else
  {
  if (CurrentMillis - _lastinserton > _filterdelay)	// no new HIGH for some time ==> Insert OFF again
    {
    inserton = false;
    _lastinsertoff = CurrentMillis;	// remember time of last LOW
    }
  else
    inserton = true;	// insert could still be on
  }
return inserton;
}

/* ----------- Function to read Inserts that can be ON, OFF or BLINKING --------
   Return Codes:
	0:	OFF
	1:	ON
	2:	BLINKING
	3:	UNCLEAR (not yet clear wether Insert is ON or BLINKING
*/
byte Insert::GetBlinkInsertState(unsigned long CurrentMillis)
{
if (InsertOn(CurrentMillis) == false)	// Insert is OFF
  {
  if (_state == 0)	// is already in OFF state
    return _state;	// nothing changes
  if (_state == 1)	// Insert was ON ==> neg. edge detected
    {
    _state = 0;		// we assume that from ON it only goes to OFF, not to FLASHING (for now!!!)
    return _state;
    }
  if (_state == 2)	// insert is FLASHING
    {
    if (CurrentMillis - _lastinserton > _insertoffdelay)	// wait long enough to be sure it´s OFF
      {
      _state = 0;	// insert is OFF
      return _state;
      }
    else
      return _state;	// state still FLASHING
    }
  if (_state == 3)	// state currently UNDEFINED
    return _state;	// can only be changed by a HIGH reading
  }
else						// Insert is ON
  {
  if (_state == 0)	// Insert was OFF ==> detected pos. edge
    {
    _state = 3;		// UNDEFINED, could have switched to ON or FLASHING
    return _state;
    }
  if (_state == 1)	// already HIGH
    return _state;	// nothing changes
  if (_state == 2)	// is in FLASHING mode
    if (CurrentMillis - _lastinsertoff > _insertondelay)	// wait long enough to be sure it´s switched to ON
      {
      _state = 1;	// it has switched to ON
      return _state;
      }
    else
      return _state;	// nothing changes for now
  if (_state == 3)	// state currently undefined
     {
     _state = 2;	// this must be the 2nd pos. edge ==> insert is now FLASHING
     return _state;
     }
  }
}


// --------- end of implementation of class Insert ---------
// ===============================================================

// ===============================================================
// Implementation of class StdInput

// -----------  Constructor for StdInput --------------

StdInput::StdInput(int pin)
{
pinMode(pin, INPUT);
_pin = pin;
}

// ------------ Function to read the switch -------------

boolean StdInput::ReadInput()
{
return digitalRead(_pin);  // returns true for an activated device, otherwise false
}
