#include <EEPROM.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include <SPI.h>
#include <RFM69.h>
#include "../../common/common.h"

#define IS_RFM69HW false

#define NODEID_LOCATION 123
#define TEAMID_LOCATION 46

uint8_t nodeID = 31;
uint8_t buttonID = 1;
e_Side team = RED;

bool radioCalled = true;
bool makeBuzz = true;
unsigned long lastButton = 0UL;

void setup() {
  // set up ports for minimal leakage current
  DDRD &= B00000011;       // set Arduino pins 2 to 7 as inputs, leaves 0 & 1 (RX & TX) as is
  DDRB = B00000000;        // set pins 8 to 13 as inputs
  PORTD |= B11111100;      // enable pullups on pins 2 to 7
  PORTB |= B11111111;      // enable pullups on pins 8 to 13

  pinMode(B_LED, OUTPUT);
  digitalWrite(B_LED, LOW);
  pinMode(B_PIEZO, OUTPUT);
  digitalWrite(B_PIEZO, LOW);
  pinMode(B_BUTTON, INPUT);
  digitalWrite(B_LED, HIGH);
  delay(1000);
  digitalWrite(B_LED, LOW);

  Serial.begin(BAUD);
  pinMode(B_BUTTON, INPUT_PULLUP);

  // check the eeprom for the stored settings
  byte a_team;
  EEPROM.get(TEAMID_LOCATION, a_team);
  if (e_Side(a_team) == RED || e_Side(a_team) == BLUE) {
    team = e_Side(a_team);
  } else {
    team = RED;
  }

  uint8_t a_button;
  EEPROM.get(NODEID_LOCATION, a_button);
  if (a_button < (B_BLUE_NODE_MAX - B_BLUE_NODE_MIN)) {
    buttonID = a_button;
  } else {
    buttonID = 9;
  }

  if (team == RED) {
    nodeID = buttonID + B_RED_NODE_MIN;
  } else {
    nodeID = buttonID + B_BLUE_NODE_MIN;
  }

  radio.initialize(FREQUENCY,nodeID,NETWORKID);
  #ifdef IS_RFM69HW
    radio.setHighPower();
  #endif
  radio.encrypt(KEY);
  // send the radio to sleep straight away (outgoing only)
  radio.sleep();

  // report the current setup
  Serial.println(F("start button"));
  Serial.print("buttonID: ");
  Serial.print(buttonID);
  Serial.print("nodeID: ");
  Serial.print(nodeID);
  if (team == RED) { Serial.print("team: RED"); }
  else { Serial.print("team: BLUE"); }
  Serial.println();

  // setup some initial states
  yourPacket.the_message = OK;
  yourPacket.the_side = NONE;
  yourPacket.the_value = 0;

  attachInterrupt(B_INT, buttonCheck, FALLING);
}

void loop() {
  if (Serial.available()) {
    char sue = Serial.read();
    if (sue == 'r' || sue == 'R') {
      //is a red node
      unsigned int button_number = Serial.parseInt();
      if (button_number < (B_RED_NODE_MAX-B_RED_NODE_MIN)) {
        // it's in range, set the id
        buttonID = button_number;
        team = RED;
        nodeID = buttonID + B_RED_NODE_MIN;
        EEPROM.put(TEAMID_LOCATION, uint8_t(team));
        EEPROM.put(NODEID_LOCATION, buttonID);
        radio.initialize(FREQUENCY, nodeID, NETWORKID);
        Serial.print("buttonID: ");
        Serial.print(buttonID);
        Serial.print("nodeID: ");
        Serial.print(nodeID);
        Serial.print("team: RED");
        Serial.println();
      }
    } else if (sue == 'b' || sue == 'B') {
      //is a blue node
      unsigned int button_number = Serial.parseInt();
      if (button_number < (B_BLUE_NODE_MAX-B_BLUE_NODE_MIN)) {
        // it's in range, set the id
        buttonID = button_number;
        team = BLUE;
        nodeID = buttonID + B_BLUE_NODE_MIN;
        EEPROM.put(TEAMID_LOCATION, uint8_t(team));
        EEPROM.put(NODEID_LOCATION, buttonID);
        radio.initialize(FREQUENCY, nodeID, NETWORKID);
        Serial.print("buttonID: ");
        Serial.print(buttonID);
        Serial.print("nodeID: ");
        Serial.print(nodeID);
        Serial.print("team: BLUE");
        Serial.println();
      }
    } else if (sue == 'p' || sue == 'P') {
      // serial pseudo button press
      Serial.println("pressed");
      lastButton = millis();
      radioCalled = false;
    }
  }

  if (radioCalled == false) {
    //make a radio call
    myPacket.the_message = PRESS;
    myPacket.the_side = team;
    myPacket.the_value = buttonID;
    detachInterrupt(B_INT);
    radio.sendWithRetry(GATEWAYID, (const void*)(&myPacket), sizeof(myPacket), 5);
    radioCalled = true;
    unsigned long reply_time = millis();
    boolean replied = false;
    t_MessageFormat yourPacket;
    while (millis() - reply_time <= WAIT_FOR_REPLY) {
      if (radio.receiveDone()) {
        if (radio.DATALEN = sizeof(yourPacket)) {
          yourPacket = *(t_MessageFormat*)radio.DATA;
          replied = true;
          if (radio.ACKRequested()) {
            radio.sendACK();
          }
          break;
        }
      }
    }
    if (replied == true && yourPacket.the_message == OK) {
      makeBuzz = true;
      //soundThePiezo();
    }
    radio.sleep();
    attachInterrupt(B_INT, buttonCheck, FALLING);
  }

  if (makeBuzz == true) {
    soundThePiezo();
    makeBuzz = false;
  }
}

static bool waitForAck()
{
  unsigned long now = millis();
  while (millis() - now <= ACK_TIME)
    if (radio.ACKReceived(GATEWAYID))
      return true;
  return false;
}

void buttonCheck()
{
  // interrupt function for button
  // check for debounce timeout
  if (millis() - lastButton >= DBOUNCE_TMOUT) {
    lastButton = millis();
    radioCalled = false;
    Serial.println("button pressed");
  }
}

void soundThePiezo() {
  tone(B_PIEZO, 523, 1000);
}
