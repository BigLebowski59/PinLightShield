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

#ifndef pls_h
#define pls_h

#include "Arduino.h"

// useful little functions
void CopyColor(int * destColor, int srcColor[3]);
unsigned long RGB2Long(byte red, byte green, byte blue);
void Long2RGB(unsigned long color, byte * red, byte * green, byte * blue);
void Long2RGB(unsigned long color, int * red, int * green, int * blue);
void Long2RGB(unsigned long color, int RGB[3]);
byte GetRed(unsigned long color);
byte GetGreen(unsigned long color);
byte GetBlue(unsigned long color);


// PinLightShield classes
class RGBStrip
{
  public:
    RGBStrip(int redpin, int greenpin, int bluepin, int brightness = 100);
    void LightStrip(unsigned long color);
    void LightStrip(int redval, int greenval, int blueval);
    void LightStrip(int color[3]);
    void SwitchOff();
    void MakeFlashes(unsigned long color, int flashes, int flashlength);
    void SetBrightness(int brightness);
    void RainbowColorChange(unsigned long CurrentMillis);
    void SetRainbowSpeed(int RainbowSpeed);
    void SetupMultiColorFlash(byte nrofcolors, unsigned long colors[5], int durations[5], boolean randsequence, int FlashDuration);
    void MultiColorFlash(unsigned long CurrentMillis, boolean * FlashActive);
    void SetupTwoColorFlash(int color1[3], int color2[3], int Col1Duration, int Col2Duration, int FlashDuration);
    void TwoColorFlash(unsigned long CurrentMillis, boolean * FlashActive);
    void SetupTwoColorFade(unsigned long fadecolorfrom, unsigned long fadecolorto, int fadestep, int fadespeed, int FadeDuration);
    void TwoColorFade(unsigned long CurrentMillis, boolean * FadeActive);
  private:
    int _redpin;
    int _greenpin;
    int _bluepin;
    int _brightness;	// range from 1 to 100
  // variables for RainbowColorChange()
    int _rainbowblue;	// remember the values of the 3 colors
    int _rainbowred;
    int _rainbowgreen;
    int _fadespeed_rainbow;
    unsigned long _startrainbow;	// remember the time the last rainbow cycle started
  // variables for MultiColorFlash
    byte _nrofcolors;
    unsigned long _colors[5];
    int _durations[5];
    boolean _randsequence;
    int _MultiFlashDuration;
    unsigned long _MultiFlashStartTime;
    unsigned long _LastMultiColorSwitch;
    byte _ActiveColorIndex;
  // variables for TwoColorFlash 
    int _Col1Duration;
    int _color1[3];
    int _Col2Duration;
    int _color2[3];
    int _FlashDuration;
    unsigned long _FlashStartTime;
    unsigned long _LastColorSwitch;
    boolean _Color1Active;
  // variables for TwoColorFade
    int _fadestep[3];
    int _fadedir[3];
    int _fadecolor[3];
    int _fadecolorfrom[3];
    int _fadecolorto[3];
    int _fadespeed;
    int _FadeDuration;
    unsigned long _LastFadeStep;
    unsigned long _FadeStartTime;
    void SwitchDir();
    boolean DetectColorLimit(int color, int fadecolorfrom, int fadecolorto, int colordir);
};

// This class implements a device that takes a 12V PWM signal (single LED, LED-Strip, Shaker Motor etc.)
class Std12VOutput
{
  public:
    Std12VOutput(int pin);
    void Output(int val);
    void OutputWithDelay(int val, int delaytime, unsigned long CurrentMillis, boolean *OutputActive);
    void MakeFlashes(int val, int flashes, int flashlength);
  private:
    int _pin;
    int _delaytime;	// used for LightLEDStripDelay to activate the signal for a certain time
    unsigned long _starttime;  // start of delayed activation
};

// This class implements a switch and the methods required to work with it
class Switch
{
  public:
    Switch(int pin, int switchwait = 0);
    boolean ReadSwitch();
    boolean ReadSwitchDelayed(unsigned long CurrentMillis);  // reads a switch but waits a bit before returning true
  private:
    int _pin;
    int _switchwait;
    int _closetime;
};

// This class implements an opto switch and the methods required to work with it
// Optos behave differently than normal switches as they are closed when not activated by
// a ball and are open when a ball is present
// This class behaves logically the same as the normal Switch class:
//	ball is present:	return true
//	no ball is present:	return false
class OptoSwitch
{
  public:
    OptoSwitch(int pin, int switchwait = 0);
    boolean ReadSwitch();
    boolean ReadSwitchDelayed(unsigned long CurrentMillis);  // reads a switch but waits a bit before returning true
  private:
    int _pin;
    int _switchwait;
    int _closetime;
};

// This class implements an Insert with two methods to read the state of the Insert
class Insert
{
  public:
    Insert(int pin, int FilterDelay, int InsertOnDelay = 0, int InsertOffDelay = 0);
    boolean InsertOn(unsigned long CurrentMillis);
    byte GetBlinkInsertState(unsigned long CurrentMillis);
  private:
    int _pin;
    int _filterdelay;		// needed because Inserts are not really constantly on
    int _insertondelay;		// time to wait until we call the Insert "ON" instead of "FLASHING"
    int _insertoffdelay;	// time to wait until we call the Insert "OFF" instead of "FLASHING"
    unsigned long _lastinserton;	// last time when Insert was ON
    unsigned long _lastinsertoff;	// last time when Insert was OFF
    byte _state;			// 0 = OFF, 1 = ON, 2 = FLASHING, 3 = UNDEFINED
};

// This class provides methods to get the state of Flashers, Coils, Motors and Shakers
class StdInput
{
  public:
    StdInput(int pin);
    boolean ReadInput();
  private:
    int _pin; 
};

#endif

