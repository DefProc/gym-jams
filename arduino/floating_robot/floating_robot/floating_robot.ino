#include <SPI.h>
#include <RFM69.h>
#include "../../common/common.h"

#define IS_RFM69HW false

uint8_t nodeID = ROBOT_NODE;

void setup() {
  // set up ports for minimal leakage current
  DDRD &= B00000011;       // set Arduino pins 2 to 7 as inputs, leaves 0 & 1 (RX & TX) as is
  DDRB = B00000000;        // set pins 8 to 13 as inputs
  PORTD |= B11111100;      // enable pullups on pins 2 to 7
  PORTB |= B11111111;      // enable pullups on pins 8 to 13

  Serial.begin(BAUD);
  Serial.println(F("start floating_robot"));

  radio.initialize(FREQUENCY,nodeID,NETWORKID);
  #ifdef IS_RFM69HW
    radio.setHighPower();
  #endif
  radio.encrypt(KEY);
}

void loop() {

}
