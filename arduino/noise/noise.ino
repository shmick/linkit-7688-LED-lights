/* Title: inoise8_pal_demo.ino

   By: Andrew Tuline

   Date: August, 2016

   This short sketch demonstrates some of the functions of FastLED, including:

   Perlin noise
   Palettes
   Palette blending
   Alternatives to blocking delays
   Beats (and not the Dr. Dre kind, but rather the sinewave kind)

   Refer to the FastLED noise.h and lib8tion.h routines for more information on these functions.


   Recommendations for high performance routines:

    Don't use blocking delays, especially if you plan to use buttons for input.
    Keep loops to a minimum, and don't use nested loops.
    Don't use floating point math. It's slow. Use 8 bit where possible.
    Let high school and not elementary school math do the work for you, i.e. don't just count pixels; use sine waves or other math functions instead.
    FastLED math functions are faster than built in math functions.

*/


#include "FastLED.h"

#if FASTLED_VERSION < 3001000
#error "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define LED_PIN     11
#define CLK_PIN     99
#define BRIGHTNESS  128
#define LED_TYPE    WS2812      // Only use the LED_PIN for WS2812's
#define COLOR_ORDER GRB

#define NUM_LEDS 450

// Adding a control pin to allow the lights to be toggled on and off
// through setting a GPIO pin on the LinkIt 7688 Duo

#define ControlPin 7 // On the 7688 Duo, Digital 7 = GPIO 0
int ControlPinVal = 0; // Set the default value to 0

// Setup a millis() timer for Serial output
unsigned long previousMillis = 0;        // will store last time LED was updated
const long interval = 1000;           // interval at which to blink (milliseconds)

struct CRGB leds[NUM_LEDS];

static uint16_t dist;         // A random number for our noise generator.
uint16_t scale = 16;          // Wouldn't recommend changing this on the fly, or the animation will be really blocky.
uint8_t maxChanges = 32;      // Value for blending between palettes.


// FastLED provides several 'preset' palettes: RainbowColors_p, RainbowStripeColors_p,
// OceanColors_p, CloudColors_p, LavaColors_p, ForestColors_p, and PartyColors_p.

CRGBPalette16 currentPalette(CRGB::Black);
//CRGBPalette16 targetPalette(OceanColors_p);
// CRGBPalette16 targetPalette ( CRGB::Black, CRGB::Red, CRGB::Green, CRGB::Blue, CRGB::Cyan, CRGB::White, CRGB::OldLace, CRGB::Purple, CRGB::Magenta, CRGB::Yellow, CRGB::OrangeRed, CRGB::DeepPink, CRGB::Orange, CRGB::Red, CRGB::Green, CRGB::Blue  );
CRGBPalette16 targetPalette(ForestColors_p);


void setup() {
  Serial1.begin(57600);
  delay(3000);

  LEDS.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  //  LEDS.addLeds<LED_TYPE,LED_PIN,CLK_PIN, COLOR_ORDER>(leds,NUM_LEDS);

  LEDS.setBrightness(BRIGHTNESS);
  dist = random16(12345);          // A semi-random number for our noise generator
} // setup()


void loop() {
  // ControlPinVal defaults to 0, which means the lights should come on
  // A python script will set ControlPin to high (1) to turn the lights off

  unsigned long currentMillis = millis();
  ControlPinVal = digitalRead(ControlPin);

  if (ControlPinVal == 0) {

    EVERY_N_MILLISECONDS(10) {
      nblendPaletteTowardPalette(currentPalette, targetPalette, maxChanges);  // Blend towards the target palette
      fillnoise8();                                                           // Update the LED array with noise at the new location
    }

    EVERY_N_SECONDS(2) {             // Change the target palette to a random one every 5 seconds.
      targetPalette = CRGBPalette16(CHSV(random8(), 255, random8(128, 255)), CHSV(random8(), 255, random8(128, 255)), CHSV(random8(), 192, random8(128, 255)), CHSV(random8(), 255, random8(128, 255)));
    }
  }
  else {
    AllOff();
  }

  LEDS.show();                    // Display the LED's at every loop cycle.

  // Use a millis() timer to output serial data without blocking

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    Serial1.print("ControlPin");
    Serial1.print("\t");
    Serial1.println(ControlPinVal);
    Serial1.println();
  }

} // loop()

// This function turns all of the pixels off and will only
// be run when ControlPinVal = 1
// This value is toggled via a python script
void AllOff()
{
  fill_solid(leds, NUM_LEDS, CRGB::Black);
}


void fillnoise8() {
  for (int i = 0; i < NUM_LEDS; i++) {                                     // Just ONE loop to fill up the LED array as all of the pixels change.
    uint8_t index = inoise8(i * scale, dist + i * scale) % 255;            // Get a value from the noise function. I'm using both x and y axis.
    leds[i] = ColorFromPalette(currentPalette, index, 255, LINEARBLEND);   // With that value, look up the 8 bit colour palette value and assign it to the current LED.
  }
  dist += beatsin8(10, 1, 4);                                              // Moving along the distance (that random number we started out with). Vary it a bit with a sine wave.
  // In some sketches, I've used millis() instead of an incremented counter. Works a treat.
} // fillnoise8()
