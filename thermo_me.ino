/*
  ThermoMe  -- case is important, emphasizes the person-specific nature
  write as thermo_me in code in Ruby-style underscore-case that will class-ify
  in to the correct "ThermoMe".

  TODO:

  * Pulse all LEDS at same brightness.
  * Pulse all LEDs with each at different brightness..
    * ..with equal dimest-to-brightest time.
    * ..with proportional (unequal dimest-to-brightest time, but same cycle).
  * Differtial brightness between LEDs that can move/fade from end-to-end.
  * Ability to turn pulsing on and off easily.
  * Automatically adjust maximum pulse brightness based on ambient light.
  * Trigger button press, all with debouncing, hopefully using interrupts:
    * quick press.
    * long press.
    * double quick-press?
  * Adjust button presses, all with debouncing, hopefully using interrupts:
    * two (or more) button using one pin with resistors/analogRead
    * quick press.
    * long press.
    * double quick-press?
  * Combined presses of Trigger and Adjust buttons:
    * ..all with debouncing.
    * ..hopefully using interrupts.
    * quick press.
    * long press.
    * double quick-press?
  * Read temperature sensor and adjust LEDs accordingly.
    * ..when temperature is in range.
    * ..when temperature is above range.
    * ..when temperature is below range.
  * Set temp range to current.
  * Adjust temperature range using Adjust buttons.
  * Demo mode.


*/

// void setup() {
//   pinMode(1, OUTPUT);
// }
//
// void loop() {
//   digitalWrite(1, !digitalRead(1));
//   delay(100);
// }



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/// Fast Adressable Bitbang LED Library
/// Copyright (c)2015, 2017 Dan Truong
///
/// This is the simplest exmple to use the library.
///
/// This example is for an Arduino Uno board with a LED strip connected to
/// port D6. Targetting any other board requires you to change something.
/// The program sends an array of pixels to display on the strip.
/// "strip" represents the hardware: LED types and port configuration,
/// "pixels" represents the data sent to the LEDs: a series of colors.
///
/// Wiring:
///
/// The LED strip DI (data input) line should be on port D6 (Digital pin 6 on
/// Arduino Uno). If you need to change the port, change all declarations below
/// from, for example from "ws2812b<D,6> myWs2812" to "ws2812b<B,4> myWs2812"
/// if you wanted to use port B4.
/// The LED power (GND) and (+5V) should be connected on the Arduino Uno's GND
/// and +5V.
///
/// Visual results:
///
/// If the hardware you use matches this program you will see the LEDs blink
/// repeatedly red, green, blue, white, in that order.
///
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#include <FAB_LED.h>

// Declare the LED protocol and the port
// ws2812b<D,0>  strip;
// ws2812<D,2>  strip;
// sk6812<D,2>  strip;
ws2812<D,4>  strip;

// How many pixels to control
const uint8_t numPixels = 3;

// How bright the LEDs will be (max 255)
const uint8_t maxBrightness = 16;

// The pixel array to display
// grb  pixels[numPixels] = {};
// rgbw  pixels[numPixels] = {};
rgbw  pixels[numPixels] = {};


////////////////////////////////////////////////////////////////////////////////
// Sets the array to specified color
////////////////////////////////////////////////////////////////////////////////
void updateColors(char r, char g, char b, char w)
{
  for(int i = 0; i < numPixels; i++)
  {
    pixels[i].r = r;
    pixels[i].g = g;
    pixels[i].b = b;
    pixels[i].w = w;
  }
}

////////////////////////////////////////////////////////////////////////////////
// This method is automatically called once when the board boots.
////////////////////////////////////////////////////////////////////////////////
void setup()
{
  // Turn off the LEDs
  strip.clear(2 * numPixels);

  pinMode(A5, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  Serial.begin(9600);
}

uint16_t brightness;

////////////////////////////////////////////////////////////////////////////////
/// @brief This method is automatically called repeatedly after setup() has run.
/// It is the main loop.
////////////////////////////////////////////////////////////////////////////////
void loop()
{
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));

  // Write the pixel array red
  updateColors(maxBrightness, 0 , 0, 0);
  // Display the pixels on the LED strip
  strip.sendPixels(numPixels, pixels);
  // Wait 0.1 seconds
  delay(100);

  // Write the pixel array green
  updateColors(0, maxBrightness, 0, 0);
  // Display the pixels on the LED strip
  strip.sendPixels(numPixels, pixels);
  // Wait 0.1 seconds
  delay(100);

  // Write the pixel array blue
  updateColors(0, 0, maxBrightness, 0);
  // Display the pixels on the LED strip
  strip.sendPixels(numPixels, pixels);
  // Wait 0.1 seconds
  delay(100);

  // Write the pixel array white
  updateColors( maxBrightness, maxBrightness, maxBrightness, 0);
  // Display the pixels on the LED strip
  strip.sendPixels(numPixels, pixels);
  // Wait 0.1 seconds
  delay(100);

  // Write the pixel array warm white
  updateColors( 0, 0, 0, maxBrightness);
  // Display the pixels on the LED strip
  strip.sendPixels(numPixels, pixels);
  // Wait 0.1 seconds
  delay(100);

  // Turn off the LEDs
  strip.clear(numPixels);
  delay(500);

  brightness = analogRead(A5);
  Serial.println(brightness);
}
