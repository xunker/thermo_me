/*
  ThermoMe  -- case is important, emphasizes the person-specific nature
  write as thermo_me in code in Ruby-style underscore-case that will class-ify
  in to the correct "ThermoMe".

  TODO:

  * Pulse all LEDS at same brightness.
  * Pulse all LEDs with each at different brightness..
    * ..with equal dimest-to-brightest time.
    * DONE ..with proportional (unequal dimest-to-brightest time, but same cycle).
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

#define LED_STRIP_PIN 4

// Declare the LED protocol and the port
// sk6812<D,LED_STRIP_PIN>  strip;
// sk6812b<D,LED_STRIP_PIN>  strip;
// ws2812<D,LED_STRIP_PIN>  strip;
ws2812b<D,LED_STRIP_PIN>  strip;

// How many pixels to control
#define NUM_PIXELS 5


#define BRIGHTNESS_FLOOR 0
#define BRIGHTNESS_CEILING 255
// How bright the LEDs will be (max 255)
uint8_t maxBrightness = BRIGHTNESS_CEILING;

// The pixel array to display
grb  pixels[NUM_PIXELS] = {};

void updatePixels(char r, char g, char b)
{
  for(int i = 0; i < NUM_PIXELS; i++)
  {
    pixels[i].r = g;
    pixels[i].g = r;
    pixels[i].b = b;
  }
}

void updatePixel(uint8_t pixel, char r, char g, char b) {
  pixels[pixel].r = g;
  pixels[pixel].g = r;
  pixels[pixel].b = b;
}

boolean pulseDirection = HIGH; // HIGH = inc, LOW = dec
byte pixelPulseState[NUM_PIXELS]; // store pulse brightness value per pixel
byte pixelPulseMax[NUM_PIXELS]; // store max brightness value per pixel

void setup()
{
  // Turn off the LEDs
  strip.clear(2 * NUM_PIXELS);

  pinMode(A5, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  // updateMaxBrightness();


  for(int i = 0; i < NUM_PIXELS; i++) {
    pixelPulseState[i] = 0;
    // pixelPulseMax[i] = 128;
  }

  pixelPulseMax[0] = BRIGHTNESS_CEILING/4;
  pixelPulseMax[1] = BRIGHTNESS_CEILING;
  pixelPulseMax[2] = BRIGHTNESS_CEILING/2;

  Serial.begin(9600);
}

void loop()
{
  // digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));

  // constantTimePulse();

  fadeBetweenPixels();

  // updateMaxBrightness();
}

void updateMaxBrightness() {
  uint16_t brightness = analogRead(A5);
  // Serial.print("raw brightness: ");
  // Serial.print(brightness);
  maxBrightness = 255 - map(brightness, 0, 1023, 0, 255);
  // Serial.print(", maxBrightness: ");
  // Serial.println(maxBrightness);

  // analogWrite(LED_BUILTIN, maxBrightness);
}

#define MIN_POSITION 0
#define MAX_POSITION 255
#define PIXEL_WIDTH (MAX_POSITION - MIN_POSITION) / NUM_PIXELS
#define LIGHT_WIDTH PIXEL_WIDTH*0.9
uint8_t position = (MIN_POSITION + (PIXEL_WIDTH/2));
boolean fadeDirection = HIGH; // HIGH = inc, LOW = dec

#define BRIGHTNESS_UPDATE_COUNTER 255
uint8_t brightnessUpdateCounter = BRIGHTNESS_UPDATE_COUNTER;

void fadeBetweenPixels() {
  /* as though a light source is moving between the pixels */
  if (fadeDirection == HIGH) {
    position++;
    if (position >= MAX_POSITION-(LIGHT_WIDTH/2)) {
      fadeDirection = LOW;
    }
  } else {
    position--;
    if (position <= MIN_POSITION+(LIGHT_WIDTH/2)) {
      fadeDirection = HIGH;
    }
  }

  byte light_lower = position - (LIGHT_WIDTH/2);
  byte light_upper = position + (LIGHT_WIDTH/2);
  // Serial.print("light_lower: ");
  // Serial.print(light_lower);
  // Serial.print(" light_upper: ");
  // Serial.print(light_upper);
  // Serial.println("");



  for(byte i = 0; i < NUM_PIXELS; i++) {
    byte pixel_lower = PIXEL_WIDTH*i;
    byte pixel_upper = pixel_lower+PIXEL_WIDTH;

    byte brightness = 0;

    if ((light_lower > pixel_lower) && (light_lower <= pixel_upper)) {
      brightness = pixel_upper-light_lower;
      if (light_upper < pixel_upper) { // if light is "narrower" than pixel
        brightness -= pixel_upper - light_upper;
      }
    } else if ((light_upper > pixel_lower) && (light_lower <= pixel_lower)) {
      brightness = light_upper-pixel_lower;
      if (light_lower > pixel_lower) { // if light is "narrower" than pixel
        brightness -= light_lower - pixel_lower;
      }
    }

    // Serial.print(i);
    // Serial.print(" - ");
    // Serial.print(brightness);
    // Serial.print(" pixel_lower: ");
    // Serial.print(pixel_lower);
    // Serial.print(" pixel_upper: ");
    // Serial.print(pixel_upper);
    // Serial.println("");

    byte rgb_values[NUM_PIXELS] = { 0, 0, 0 };

    // (0..10).map{|pos|  n = 3; n.times.map{ |nn| 3.times.map { |i| (pos-((255/(n-1))*(i)))+((255/3)*nn) }.map(&:abs)}}

    //
    // for(byte rgb_a = 0; rgb_a < 3; rgb_a++) {
    //   rgb_values[rgb_a] = (
    //     abs(position-((255/(NUM_PIXELS-1))*(rgb_a)))
    //   ) + ((255/3)*i);
    // }


    rgb_values[i] = map(brightness, 0, 255, BRIGHTNESS_FLOOR, maxBrightness);
    // if (i == 0) {
    //   rgb_values[1] = brightness/2;
    //   rgb_values[2] = brightness/2;
    // } else if (i == 1) {
    //   rgb_values[0] = brightness/2;
    //   rgb_values[2] = brightness/2;
    // } else if (i == 2) {
    //   rgb_values[0] = brightness/2;
    //   rgb_values[1] = brightness/2;
    // }
    // Serial.print(i);
    // Serial.print(" ");
    // Serial.print(rgb_values[0]);
    // Serial.print(" ");
    // Serial.print(rgb_values[1]);
    // Serial.print(" ");
    // Serial.print(rgb_values[2]);
    // Serial.println("");

    /*

      Hey stupid! Don't try to set the hue of the LEDs by `position`, set it by
      how far in or out of the temperature range it is!
    */

    // updatePixel(i, rgb_values[0], rgb_values[1], rgb_values[2]);
    updatePixel(i, 0, rgb_values[i], 0);
    // updatePixel(i, brightness, 0, 0);
  }

  strip.sendPixels(NUM_PIXELS, pixels);

  brightnessUpdateCounter--;
  if (brightnessUpdateCounter <= 0) {
    updateMaxBrightness();
    brightnessUpdateCounter = BRIGHTNESS_UPDATE_COUNTER;
  }
  delay(1);
  // Serial.println("-----");
}

void constantTimePulse() {
  /* constant-time pulsing with different brightness */
  boolean finishedPulsePhase = true;
  for(int i = 0; i < NUM_PIXELS; i++) {
    if (pulseDirection == HIGH) {
      if (pixelPulseState[i] < pixelPulseMax[i]) {
        finishedPulsePhase = false;
        pixelPulseState[i]++;
      }
    } else {
      if (pixelPulseState[i] > BRIGHTNESS_FLOOR) {
        finishedPulsePhase = false;
        pixelPulseState[i]--;
      }
    }
    // Serial.print("Pixel ");
    // Serial.print(i);
    // Serial.print(", state: ");
    // Serial.print(pixelPulseState[i]);
    // Serial.println("");
    switch (i) {
      case 0:
        updatePixel(i, pixelPulseState[i], 0, 0);
        break;
      case 1:
        updatePixel(i, 0, pixelPulseState[i], 0);
        break;
      case 2:
        updatePixel(i, 0, 0, pixelPulseState[i]);
        break;
      default:
      break;
    }
  }
  // Serial.print("pulseDirection: ");
  // Serial.print(pulseDirection);
  // Serial.print(", finishedPulsePhase: ");
  // Serial.print(finishedPulsePhase);
  // Serial.println("");

  if (finishedPulsePhase) {
    pulseDirection = !pulseDirection;
  }

  strip.sendPixels(NUM_PIXELS, pixels);
  delay(10);

}
