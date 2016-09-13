/****************************************

****************************************/

#include <FastLED.h>
#define NUM_LEDS 16
#define DATA_PIN 6

const unsigned long sampleWindow = 50; // Sample window width in mS (50 mS = 20Hz)
unsigned int sample;

CRGB leds[NUM_LEDS];

void setup()
{
   Serial.begin(115200);
   Serial.println(F("Starting sound_leveler"));
   FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);

   fill_solid(leds, NUM_LEDS, CRGB::Red);
   FastLED.show();
   delay(250);
   fill_solid(leds, NUM_LEDS, CRGB::Green);
   FastLED.show();
   delay(250);
   fill_solid(leds, NUM_LEDS, CRGB::Blue);
   FastLED.show();
   delay(250);
   fill_solid(leds, NUM_LEDS, CRGB::Black);
   FastLED.show();
   delay(30);
}


void loop()
{
   unsigned long startMillis= millis();  // Start of sample window
   unsigned int peakToPeak = 0;   // peak-to-peak level

   unsigned int signalMax = 0;
   unsigned int signalMin = 1024;

   // collect data for 50 mS
   while (millis() - startMillis < sampleWindow)
   {
      sample = analogRead(0);
      if (sample < 1024)  // toss out spurious readings
      {
         if (sample > signalMax)
         {
            signalMax = sample;  // save just the max levels
         }
         else if (sample < signalMin)
         {
            signalMin = sample;  // save just the min levels
         }
      }
   }
   peakToPeak = signalMax - signalMin;  // max - min = peak-peak amplitude
   //float volts = (peakToPeak * 5.0) / 1024.0;  // convert to volts

   //Serial.println(volts);
   Serial.print(peakToPeak);
   int light_leds = map(peakToPeak, 15, 100, 0, NUM_LEDS);
   light_leds = constrain(light_leds, 1, NUM_LEDS);
   light_leds = light_leds - 1;
   Serial.print(F(" - "));
   Serial.print(light_leds);
   Serial.println();
   fill_solid(leds, NUM_LEDS, CRGB::Black);
   fill_solid(leds, light_leds-1, CRGB::Red);
   FastLED.show();
}
