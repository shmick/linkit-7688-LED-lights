#include "FastLED.h"

// ColorWavesWithPalettes
// Animated shifting color waves, with several cross-fading color palettes.
// by Mark Kriegsman, August 2015
//
// Color palettes courtesy of cpt-city and its contributors:
//   http://soliton.vm.bytemark.co.uk/pub/cpt-city/
//
// Color palettes converted for FastLED using "PaletteKnife" v1:
//   http://fastled.io/tools/paletteknife/
//

#if FASTLED_VERSION < 3001000
#error "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define DATA_PIN    11
//#define CLK_PIN   4
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS    450
#define BRIGHTNESS  255

CRGB leds[NUM_LEDS];

// ten seconds per color palette makes a good demo
// 20-120 is better for deployment
#define SECONDS_PER_PALETTE 10

#define ControlPin 7 // On the 7688 Duo, Digital 7 = GPIO 0
int ControlPinVal = 0; // Set the default value to 0

void setup() {
  delay(3000); // 3 second delay for recovery

  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS)
  //.setCorrection(TypicalLEDStrip) // cpt-city palettes have different color balance
  .setDither(BRIGHTNESS < 255);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
}

// Forward declarations of an array of cpt-city gradient palettes, and
// a count of how many there are.  The actual color palette definitions
// are at the bottom of this file.
extern const TProgmemRGBGradientPalettePtr gGradientPalettes[];
extern const uint8_t gGradientPaletteCount;

// Current palette number from the 'playlist' of color palettes
uint8_t gCurrentPaletteNumber = 0;

CRGBPalette16 gCurrentPalette( CRGB::Black);
CRGBPalette16 gTargetPalette( gGradientPalettes[0] );


void loop()
{

  ControlPinVal = digitalRead(ControlPin);

  if (ControlPinVal == 0) {

    EVERY_N_SECONDS( SECONDS_PER_PALETTE ) {
      gCurrentPaletteNumber = addmod8( gCurrentPaletteNumber, 1, gGradientPaletteCount);
      gTargetPalette = gGradientPalettes[ gCurrentPaletteNumber ];
    }

    EVERY_N_MILLISECONDS(40) {
      nblendPaletteTowardPalette( gCurrentPalette, gTargetPalette, 16);
    }

    colorwaves( leds, NUM_LEDS, gCurrentPalette);

    FastLED.show();
    FastLED.delay(20);
  }

  else {
    fadeToBlack();
    FastLED.show();
  }

}// loop

void fadeToBlack()
{
  fadeToBlackBy(leds, NUM_LEDS, 1);
}

// This function draws color waves with an ever-changing,
// widely-varying set of parameters, using a color palette.
void colorwaves( CRGB* ledarray, uint16_t numleds, CRGBPalette16& palette)
{
  static uint16_t sPseudotime = 0;
  static uint16_t sLastMillis = 0;
  static uint16_t sHue16 = 0;

  uint8_t sat8 = beatsin88( 87, 220, 250);
  uint8_t brightdepth = beatsin88( 341, 96, 224);
  uint16_t brightnessthetainc16 = beatsin88( 203, (25 * 256), (40 * 256));
  uint8_t msmultiplier = beatsin88(147, 23, 60);

  uint16_t hue16 = sHue16;//gHue * 256;
  uint16_t hueinc16 = beatsin88(113, 300, 1500);

  uint16_t ms = millis();
  uint16_t deltams = ms - sLastMillis ;
  sLastMillis  = ms;
  sPseudotime += deltams * msmultiplier;
  sHue16 += deltams * beatsin88( 400, 5, 9);
  uint16_t brightnesstheta16 = sPseudotime;

  for ( uint16_t i = 0 ; i < numleds; i++) {
    hue16 += hueinc16;
    uint8_t hue8 = hue16 / 256;
    uint16_t h16_128 = hue16 >> 7;
    if ( h16_128 & 0x100) {
      hue8 = 255 - (h16_128 >> 1);
    } else {
      hue8 = h16_128 >> 1;
    }

    brightnesstheta16  += brightnessthetainc16;
    uint16_t b16 = sin16( brightnesstheta16  ) + 32768;

    uint16_t bri16 = (uint32_t)((uint32_t)b16 * (uint32_t)b16) / 65536;
    uint8_t bri8 = (uint32_t)(((uint32_t)bri16) * brightdepth) / 65536;
    bri8 += (255 - brightdepth);

    uint8_t index = hue8;
    //index = triwave8( index);
    index = scale8( index, 240);

    CRGB newcolor = ColorFromPalette( palette, index, bri8);

    uint16_t pixelnumber = i;
    pixelnumber = (numleds - 1) - pixelnumber;

    nblend( ledarray[pixelnumber], newcolor, 128);
  }
}

// Alternate rendering function just scrolls the current palette
// across the defined LED strip.
void palettetest( CRGB* ledarray, uint16_t numleds, const CRGBPalette16& gCurrentPalette)
{
  static uint8_t startindex = 0;
  startindex--;
  fill_palette( ledarray, numleds, startindex, (256 / NUM_LEDS) + 1, gCurrentPalette, 255, LINEARBLEND);
}





// Gradient Color Palette definitions for 33 different cpt-city color palettes.
//    956 bytes of PROGMEM for all of the palettes together,
//   +618 bytes of PROGMEM for gradient palette code (AVR).
//  1,494 bytes total for all 34 color palettes and associated code.

// Gradient palette "ib_jul01_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/ing/xmas/tn/ib_jul01.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 16 bytes of program space.

DEFINE_GRADIENT_PALETTE( ib_jul01_gp ) {
  0, 194,  1,  1,
  94,   1, 29, 18,
  132,  57, 131, 28,
  255, 113,  1,  1
};

// Gradient palette "rgi_15_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/ds/rgi/tn/rgi_15.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 36 bytes of program space.

DEFINE_GRADIENT_PALETTE( rgi_15_gp ) {
  0,   4,  1, 31,
  31,  55,  1, 16,
  63, 197,  3,  7,
  95,  59,  2, 17,
  127,   6,  2, 34,
  159,  39,  6, 33,
  191, 112, 13, 32,
  223,  56,  9, 35,
  255,  22,  6, 38
};

// Gradient palette "Analogous_1_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/nd/red/tn/Analogous_1.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 20 bytes of program space.

DEFINE_GRADIENT_PALETTE( Analogous_1_gp ) {
  0,   3,  0, 255,
  63,  23,  0, 255,
  127,  67,  0, 255,
  191, 142,  0, 45,
  255, 255,  0,  0
};

// Gradient palette "es_pinksplash_08_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/es/pink_splash/tn/es_pinksplash_08.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 20 bytes of program space.

DEFINE_GRADIENT_PALETTE( es_pinksplash_08_gp ) {
  0, 126, 11, 255,
  127, 197,  1, 22,
  175, 210, 157, 172,
  221, 157,  3, 112,
  255, 157,  3, 112
};

// Gradient palette "es_pinksplash_07_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/es/pink_splash/tn/es_pinksplash_07.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 28 bytes of program space.

DEFINE_GRADIENT_PALETTE( es_pinksplash_07_gp ) {
  0, 229,  1,  1,
  61, 242,  4, 63,
  101, 255, 12, 255,
  127, 249, 81, 252,
  153, 255, 11, 235,
  193, 244,  5, 68,
  255, 232,  1,  5
};

// Gradient palette "Coral_reef_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/nd/other/tn/Coral_reef.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 24 bytes of program space.

DEFINE_GRADIENT_PALETTE( Coral_reef_gp ) {
  0,  40, 199, 197,
  50,  10, 152, 155,
  96,   1, 111, 120,
  96,  43, 127, 162,
  139,  10, 73, 111,
  255,   1, 34, 71
};

// Gradient palette "es_ocean_breeze_036_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/es/ocean_breeze/tn/es_ocean_breeze_036.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 16 bytes of program space.

DEFINE_GRADIENT_PALETTE( es_ocean_breeze_036_gp ) {
  0,   1,  6,  7,
  89,   1, 99, 111,
  153, 144, 209, 255,
  255,   0, 73, 82
};

// Gradient palette "es_landscape_64_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/es/landscape/tn/es_landscape_64.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 36 bytes of program space.

DEFINE_GRADIENT_PALETTE( es_landscape_64_gp ) {
  0,   0,  0,  0,
  37,   2, 25,  1,
  76,  15, 115,  5,
  127,  79, 213,  1,
  128, 126, 211, 47,
  130, 188, 209, 247,
  153, 144, 182, 205,
  204,  59, 117, 250,
  255,   1, 37, 192
};

// Gradient palette "rainbowsherbet_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/ma/icecream/tn/rainbowsherbet.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 28 bytes of program space.

DEFINE_GRADIENT_PALETTE( rainbowsherbet_gp ) {
  0, 255, 33,  4,
  43, 255, 68, 25,
  86, 255,  7, 25,
  127, 255, 82, 103,
  170, 255, 255, 242,
  209,  42, 255, 22,
  255,  87, 255, 65
};

// Gradient palette "gr65_hult_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/hult/tn/gr65_hult.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 24 bytes of program space.

DEFINE_GRADIENT_PALETTE( gr65_hult_gp ) {
  0, 247, 176, 247,
  48, 255, 136, 255,
  89, 220, 29, 226,
  160,   7, 82, 178,
  216,   1, 124, 109,
  255,   1, 124, 109
};

// Gradient palette "GMT_drywet_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/gmt/tn/GMT_drywet.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 28 bytes of program space.

DEFINE_GRADIENT_PALETTE( GMT_drywet_gp ) {
  0,  47, 30,  2,
  42, 213, 147, 24,
  84, 103, 219, 52,
  127,   3, 219, 207,
  170,   1, 48, 214,
  212,   1,  1, 111,
  255,   1,  7, 33
};

// Gradient palette "ib15_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/ing/general/tn/ib15.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 24 bytes of program space.

DEFINE_GRADIENT_PALETTE( ib15_gp ) {
  0, 113, 91, 147,
  72, 157, 88, 78,
  89, 208, 85, 33,
  107, 255, 29, 11,
  141, 137, 31, 39,
  255,  59, 33, 89
};

// Gradient palette "es_emerald_dragon_08_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/es/emerald_dragon/tn/es_emerald_dragon_08.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 16 bytes of program space.

DEFINE_GRADIENT_PALETTE( es_emerald_dragon_08_gp ) {
  0,  97, 255,  1,
  101,  47, 133,  1,
  178,  13, 43,  1,
  255,   2, 10,  1
};

// Gradient palette "Colorfull_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/nd/atmospheric/tn/Colorfull.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 44 bytes of program space.

DEFINE_GRADIENT_PALETTE( Colorfull_gp ) {
  0,  10, 85,  5,
  25,  29, 109, 18,
  60,  59, 138, 42,
  93,  83, 99, 52,
  106, 110, 66, 64,
  109, 123, 49, 65,
  113, 139, 35, 66,
  116, 192, 117, 98,
  124, 255, 255, 137,
  168, 100, 180, 155,
  255,  22, 121, 174
};

// Gradient palette "Sunset_Real_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/nd/atmospheric/tn/Sunset_Real.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 28 bytes of program space.

DEFINE_GRADIENT_PALETTE( Sunset_Real_gp ) {
  0, 120,  0,  0,
  22, 179, 22,  0,
  51, 255, 104,  0,
  85, 167, 22, 18,
  135, 100,  0, 103,
  198,  16,  0, 130,
  255,   0,  0, 160
};

// Gradient palette "BlacK_Blue_Magenta_White_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/nd/basic/tn/BlacK_Blue_Magenta_White.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 28 bytes of program space.

DEFINE_GRADIENT_PALETTE( BlacK_Blue_Magenta_White_gp ) {
  0,   0,  0,  0,
  42,   0,  0, 45,
  84,   0,  0, 255,
  127,  42,  0, 255,
  170, 255,  0, 255,
  212, 255, 55, 255,
  255, 255, 255, 255
};

// Gradient palette "BlacK_Magenta_Red_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/nd/basic/tn/BlacK_Magenta_Red.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 20 bytes of program space.

DEFINE_GRADIENT_PALETTE( BlacK_Magenta_Red_gp ) {
  0,   0,  0,  0,
  63,  42,  0, 45,
  127, 255,  0, 255,
  191, 255,  0, 45,
  255, 255,  0,  0
};

// Gradient palette "BlacK_Red_Magenta_Yellow_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/nd/basic/tn/BlacK_Red_Magenta_Yellow.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 28 bytes of program space.

DEFINE_GRADIENT_PALETTE( BlacK_Red_Magenta_Yellow_gp ) {
  0,   0,  0,  0,
  42,  42,  0,  0,
  84, 255,  0,  0,
  127, 255,  0, 45,
  170, 255,  0, 255,
  212, 255, 55, 45,
  255, 255, 255,  0
};

// Gradient palette "Blue_Cyan_Yellow_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/nd/basic/tn/Blue_Cyan_Yellow.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 20 bytes of program space.

DEFINE_GRADIENT_PALETTE( Blue_Cyan_Yellow_gp ) {
  0,   0,  0, 255,
  63,   0, 55, 255,
  127,   0, 255, 255,
  191,  42, 255, 45,
  255, 255, 255,  0
};

// Added by Steve on Dec 5 2016

// Gradient palette "bhw1_sunconure_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/bhw/bhw1/tn/bhw1_sunconure.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 16 bytes of program space.

DEFINE_GRADIENT_PALETTE( bhw1_sunconure_gp ) {
  0,  20, 223, 13,
  160, 232, 65,  1,
  252, 232,  5,  1,
  255, 232,  5,  1
};



// Gradient palette "bhw1_w00t_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/bhw/bhw1/tn/bhw1_w00t.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 16 bytes of program space.

DEFINE_GRADIENT_PALETTE( bhw1_w00t_gp ) {
  0,   3, 13, 43,
  104,  78, 141, 240,
  188, 255,  0,  0,
  255,  28,  1,  1
};


// Gradient palette "bhw1_01_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/bhw/bhw1/tn/bhw1_01.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 12 bytes of program space.

DEFINE_GRADIENT_PALETTE( bhw1_01_gp ) {
  0, 227, 101,  3,
  117, 194, 18, 19,
  255,  92,  8, 192
};


// Gradient palette "bhw2_38_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/bhw/bhw2/tn/bhw2_38.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 32 bytes of program space.

DEFINE_GRADIENT_PALETTE( bhw2_38_gp ) {
  0, 104, 205, 212,
  28,  32, 184, 203,
  61,  39, 39, 168,
  96,  46,  1, 144,
  130,   8,  1, 22,
  163,  17,  3, 28,
  196,  24,  4, 31,
  255, 213,  9, 89
};

// Gradient palette "bhw3_45_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/bhw/bhw3/tn/bhw3_45.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 56 bytes of program space.

DEFINE_GRADIENT_PALETTE( bhw3_45_gp ) {
  0,  10, 26, 49,
  25,  68, 156, 255,
  40, 115, 217, 247,
  73,  46, 50, 106,
  91, 255, 252, 255,
  94, 252, 255, 201,
  109, 249, 248,  7,
  127, 255, 138,  1,
  142, 163, 44,  1,
  188, 252, 29, 23,
  206, 244, 85, 250,
  234, 229, 38, 69,
  249, 163, 17, 48,
  255, 163, 17, 48
};

// Gradient palette "bhw4_029_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/bhw/bhw4/tn/bhw4_029.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 24 bytes of program space.

DEFINE_GRADIENT_PALETTE( bhw4_029_gp ) {
  0,  28,  7, 75,
  43,  73, 22, 74,
  79, 177, 146, 197,
  122,  21, 72, 137,
  165,  15, 184, 75,
  255, 224, 205,  4
};

// Gradient palette "Alive_And_Kicking_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/colo/rphnick/tn/Alive_And_Kicking.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 40 bytes of program space.

DEFINE_GRADIENT_PALETTE( Alive_And_Kicking_gp ) {
  0, 192, 50, 207,
  51, 192, 50, 207,
  51,  87, 50, 207,
  102,  87, 50, 207,
  102,  37, 73, 207,
  153,  37, 73, 207,
  153,  37, 127, 207,
  204,  37, 127, 207,
  204,  37, 213, 140,
  255,  37, 213, 140
};

// Gradient palette "Optimus_Prime_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/colo/vredeling/tn/Optimus_Prime.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 40 bytes of program space.

DEFINE_GRADIENT_PALETTE( Optimus_Prime_gp ) {
  0,   5, 16, 18,
  51,   5, 16, 18,
  51,   8, 38, 71,
  102,   8, 38, 71,
  102, 194, 189, 151,
  153, 194, 189, 151,
  153, 167,  6,  2,
  204, 167,  6,  2,
  204,  53,  1,  1,
  255,  53,  1,  1
};

// Gradient palette "cw1_013_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/cw/1/tn/cw1-013.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 28 bytes of program space.

DEFINE_GRADIENT_PALETTE( cw1_013_gp ) {
  0,  20, 12, 13,
  0,  20, 12, 13,
  51, 186, 23,  9,
  101, 186, 23,  9,
  153, 247, 255,  9,
  204,  15, 46, 75,
  255,  84, 173, 255
};

// Gradient palette "cw1_002_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/cw/1/tn/cw1-002.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 16 bytes of program space.

DEFINE_GRADIENT_PALETTE( cw1_002_gp ) {
  0,   2, 29, 52,
  84,  73, 125, 160,
  168, 239, 77, 43,
  255, 190, 27, 22
};

// Gradient palette "pm3d07_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/pm/tn/pm3d07.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 24 bytes of program space.

DEFINE_GRADIENT_PALETTE( pm3d07_gp ) {
    0,   0,255,  0,
   84,   0, 32,  0,
   84, 255,255,  0,
  170, 135,149,  0,
  170, 255,  0,  0,
  255,  52,  0,  0};

// Gradient palette "a_rainbow_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/pj/1/tn/a-rainbow.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 96 bytes of program space.

DEFINE_GRADIENT_PALETTE( a_rainbow_gp ) {
    0, 237,  1, 21,
    4, 242,  1,  7,
   12, 244,  1,  1,
   28, 242, 29,  1,
   40, 239, 57,  1,
   53, 247, 95,  1,
   66, 247,178,  1,
   79, 242,217,  1,
   89, 188,246,  1,
  101, 137,248,  1,
  114,  35,147, 10,
  127,   4,127, 11,
  137,   4,127, 64,
  150,   1,156,186,
  163,   7, 66,255,
  173,   7, 45,255,
  183,  16, 13,255,
  193,  39, 13,255,
  206,  39,  1,153,
  219,  78,  1,156,
  225, 110,  1,125,
  229, 150,  1, 98,
  242, 150,  1, 34,
  255, 237,  1,  9};

  // Gradient palette "clownaround_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/pj/6/tn/clownaround.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 28 bytes of program space.

DEFINE_GRADIENT_PALETTE( clownaround_gp ) {
    0, 224, 10,  9,
   48, 201,139, 11,
   96, 237, 36,  1,
  155,  11,128, 29,
  193,  19,114,252,
  234,  33, 18,168,
  255,  33, 18,168};


// Single array of defined cpt-city color palettes.
// This will let us programmatically choose one based on
// a number, rather than having to activate each explicitly
// by name every time.
// Since it is const, this array could also be moved
// into PROGMEM to save SRAM, but for simplicity of illustration
// we'll keep it in a regular SRAM array.
//
// This list of color palettes acts as a "playlist"; you can
// add or delete, or re-arrange as you wish.
const TProgmemRGBGradientPalettePtr gGradientPalettes[] = {
  bhw1_sunconure_gp,
  bhw1_w00t_gp,
  bhw1_01_gp,
  bhw2_38_gp,
  bhw3_45_gp,
  bhw4_029_gp,
  Alive_And_Kicking_gp,
  Optimus_Prime_gp,
  cw1_013_gp,
  cw1_002_gp,
  pm3d07_gp,
  a_rainbow_gp,
  clownaround_gp,
  Sunset_Real_gp, // Keep
  es_ocean_breeze_036_gp, // Keep
  rgi_15_gp,// Keep
  Analogous_1_gp, // Keep
  es_pinksplash_08_gp, // Keep
  Coral_reef_gp, // Keep
  es_pinksplash_07_gp, // Keep
  es_landscape_64_gp, // Keep
  rainbowsherbet_gp, // Keep
  gr65_hult_gp, // Keep
  GMT_drywet_gp, // Keep
  ib_jul01_gp, // Keep
  ib15_gp, // Keep
  es_emerald_dragon_08_gp, // Keep
  Colorfull_gp, // Keep
  BlacK_Blue_Magenta_White_gp, // Keep
  BlacK_Magenta_Red_gp, // Keep
  BlacK_Red_Magenta_Yellow_gp, // Keep
  Blue_Cyan_Yellow_gp
}; // LAST

// Count of how many cpt-city gradients are defined:
const uint8_t gGradientPaletteCount =
  sizeof( gGradientPalettes) / sizeof( TProgmemRGBGradientPalettePtr );
