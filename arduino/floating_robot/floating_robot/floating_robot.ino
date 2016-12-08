//include libraries

#include "FastLED.h"
#include <SPI.h>
#include <RFM69.h>
#include "../../common/common.h"

//set the data pins for LEDs and length of LED string
#define NUM_LEDS 30
#define DATA_PIN 6

//Create new FastLED object called leds lenght of num_leds
CRGB leds[NUM_LEDS];

//set the node ID to = robot_node from constants file
uint8_t nodeID = ROBOT_NODE;
bool changeImage = true;
const unsigned long sampleWindow = 50; // Sample window width in mS (50 mS = 20Hz)
unsigned int sample;


void setup() {
  Serial.begin(BAUD); //from constants file (115200)
  Serial.println(F("start floating_robot"));

  //start the radio & add the encryption key (constants in the constants file)
  radio.initialize(FREQUENCY,nodeID,NETWORKID);
  radio.encrypt(KEY);

  // initialize the LED string
  FastLED.addLeds<WS2812,DATA_PIN,GRB>(leds,NUM_LEDS); //(ws2812 = name of chip in the LEDs on iPixels, set as greenRedBlue because that is how iPixels are organised
  FastLED.setBrightness(84); // we have reduced the brightness from 255 to 84
  colourWipe(1); //does colourwipe to show that this has started. If we set it to 10, it would be slower, 10 = slowest, 1 = fastest.
  delay(1000); //waits
  fill_solid(leds, NUM_LEDS, CRGB::Black); // sets all leds to black
  FastLED.show(); //sends to leds

  // just some starting values to force the starting state:
  yourPacket.the_message = STOP; //
  yourPacket.the_side = NONE;
  yourPacket.the_value =0;
}


void fadeall() { for(int i = 0; i < NUM_LEDS; i++) { leds[i].nscale8(250); } }

void loop() {
  checkIncoming();

  //if were need to do some animation then go into animation state
  if (yourPacket.the_message != STOP) {
    changeImage = true;
  }

  if (changeImage == true) { //animate something
    if (yourPacket.the_message == ANIMATE) { 
      // choose the talking colour
      CRGB colour = CRGB::Green; 
      if (yourPacket.the_side == BLUE) { 
        colour = CRGB::Blue; // turn red
      } else if (yourPacket.the_side == RED){ 
        colour = CRGB::Red; //turn blue
      }
      // start the talking function
      speakToMe(colour);
      //changeImage = true; // keep running the animation until told not to
    } else if (yourPacket.the_message == STOP) { //turns leds to black / off
      clearStripBuffer();
      FastLED.show();
      changeImage = false;
    } else if (yourPacket.the_message == SHOW) { //show a certain colour colourwipe if a side has won
      if (yourPacket.the_side == NONE) {
        // run the rainbow wipe
        colourWipe(uint8_t(yourPacket.the_value));
      } else {
        // show the requested colour from the_side
        CRGB colour = CRGB::Blue; //sets colour to blue if told to do so form the master
        if (yourPacket.the_side == RED) {
          colour = CRGB::Red; //sets colour to red if told to do so form the master
        }
        theatreChase(colour); //does theatre chase with either red or blue depending on the colour value it is given
      }
    }
  }
}

void speakToMe(CRGB _colour) {
  // flash the leds in response to sound input
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
  // map sound level to the led brightness
  int light_bright = map(peakToPeak, 15, 100, 10, 255);
  fill_solid(leds, NUM_LEDS, _colour);
  FastLED.setBrightness(light_bright);
  FastLED.show();
}

void theatreChase(CRGB _colour) {
  for (int i=0; i<NUM_LEDS; i=i+3) {
    clearStripBuffer();
    for (int q=0; q<3; q++) {
      leds[i+q] = _colour;
    }
    FastLED.show();
    delay(50);
  }
}

void colourWipe(uint8_t _speed) {
  if (_speed<1) { _speed = 1;}
  if (_speed>10) { _speed = 10;}
  static uint8_t hue = 0;
  // First slide the led in one direction
  for(int i = 0; i < NUM_LEDS; i++) {
    // Set the i'th led to red
    leds[i] = CHSV(hue++, 255, 255);
    // Show the leds
    FastLED.show();
    //now that we've shown the leds, reset the i'th led to black
    // leds[i] = CRGB::Black;
    fadeall();
    // Wait a little bit before we loop around and do it again
    delay(_speed*10);
  }

  // Now go in the other direction.
  for(int i = (NUM_LEDS)-1; i >= 0; i--) {
    // Set the i'th led to red
    leds[i] = CHSV(hue++, 255, 255);
    // Show the leds
    FastLED.show();
    // now that we've shown the leds, reset the i'th led to black
    // leds[i] = CRGB::Black;
    fadeall();
    // Wait a little bit before we loop around and do it again
    delay(_speed*10);
  }
}

void clearStripBuffer() {
  for (int i=0; i<NUM_LEDS; i++)
  {
    leds[i] = CRGB::Black;
  }
}

void checkIncoming() {
  // check any incoming radio messages
  // *** We need to call this fast enough so as humans don't notice ***
  if (radio.receiveDone()) {
    // send ACK if requested (and it's directed at us only)
    if (radio.ACKRequested() && radio.TARGETID == nodeID) {
      //uint8_t theNodeID = radio.SENDERID;
      radio.sendACK();
    }
    // check if we're getting hit data
    if (radio.DATALEN = sizeof(t_MessageFormat)) {
      yourPacket = *(t_MessageFormat*)radio.DATA;
    }
  }
}
