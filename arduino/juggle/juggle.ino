/* Juggle

  By: Originally by Mark Kriegsman

  Modified By: Andrew Tuline

  Date: February 2015

  Juggle just moves some balls back and forth. A single ball could be a Cylon effect. I've added several variables to this simple routine.

*/

#include "FastLED.h"

#if FASTLED_VERSION < 3001000
#error "Requires FastLED 3.1 or later; check github for latest code."
#endif

// Fixed definitions cannot change on the fly.
#define LED_DT 11                                             // Data pin to connect to the strip.
#define LED_TYPE WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS    450

// Initialize changeable global variables.
uint8_t max_bright = 255;                                     // Overall brightness definition. It can be changed on the fly.

struct CRGB leds[NUM_LEDS];                                   // Initialize our LED array.

// Routine specific variables
uint8_t    numdots =   6;                                     // Number of dots in use.
uint8_t   faderate =   4;                                     // How long should the trails be. Very low value = longer trails.
uint8_t     hueinc =  16;                                     // Incremental change in hue between each dot.
uint8_t    thishue =   0;                                     // Starting hue.
uint8_t     curhue =   0;                                     // The current hue
uint8_t    thissat = 255;                                     // Saturation of the colour.
uint8_t thisbright = 255;                                     // How bright should the LED/display be.
uint8_t   basebeat =   5;                                     // Higher = faster movement.

// Adding a control pin to allow the lights to be toggled on and off
// through setting a GPIO pin on the LinkIt 7688 Duo
#define ControlPin 7 // On the 7688 Duo, Digital 7 = GPIO 0
int ControlPinVal = 0; // Set the default value to 0

void setup() {
  delay(1000);                                                // Power-up safety delay or something like that.
  LEDS.addLeds<LED_TYPE, LED_DT, COLOR_ORDER>(leds, NUM_LEDS);    // Use this for WS2812B
  FastLED.setBrightness(max_bright);
} // setup()

void loop() {

  ControlPinVal = digitalRead(ControlPin);

  if (ControlPinVal == 0) {
    ChangeMe();
    juggle();
  }

  else {
    AllOff();
  }

  FastLED.show();

} // loop()

void AllOff()
{
  fill_solid(leds, NUM_LEDS, CRGB::Black);
}

void juggle() {                                               // Several colored dots, weaving in and out of sync with each other
  curhue = thishue;                                           // Reset the hue values.
  fadeToBlackBy(leds, NUM_LEDS, faderate);
  for ( int i = 0; i < numdots; i++) {
    leds[beatsin16(basebeat + i + numdots, 0, NUM_LEDS)] += CHSV(curhue, thissat, thisbright); //beat16 is a FastLED 3.1 function
    curhue += hueinc;
  }
} // juggle()


void ChangeMe() {                                             // A time (rather than loop) based demo sequencer. This gives us full control over the length of each sequence.
  uint8_t secondHand = (millis() / 1000) % 60;                // IMPORTANT!!! Change '30' to a different value to change duration of the loop.
  static uint8_t lastSecond = 99;                             // Static variable, means it's only defined once. This is our 'debounce' variable.
  if (lastSecond != secondHand) {                             // Debounce to make sure we're not repeating an assignment.
    lastSecond = secondHand;
    switch (secondHand) {
      case  0: numdots = 1; basebeat = 20; hueinc = 16; faderate = 2; thishue = 0; break;                  // You can change values here, one at a time , or altogether.
      case 10: numdots = 4; basebeat = 10; hueinc = 16; faderate = 8; thishue = 128; break;
      case 20: numdots = 8; basebeat =  3; hueinc =  0; faderate = 8; thishue = random8(); break;         // Only gets called once, and not continuously for the next several seconds. Therefore, no rainbows.
      case 30: break;
    }
  }
} // ChangeMe()
