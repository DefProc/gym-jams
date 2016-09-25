#include <SPI.h>
#include <RFM69.h>
#include "../../common/common.h"

#define IS_RFM69HW false

bool game_running = true;
uint8_t which_game = 1;
uint8_t max_games = 3;
uint16_t red_score = 0;
uint16_t blue_score = 0;

void setup() {
  Serial.begin(BAUD);
  Serial.println(F("start games_master"));

  radio.initialize(FREQUENCY,GATEWAYID,NETWORKID);
  #ifdef IS_RFM69HW
    radio.setHighPower();
  #endif
  radio.encrypt(KEY);

  if (game_running == true) {
    Serial.println("started game");
  }
}

void loop() {
  checkIncoming();

  // check for serial incomming and parse as required
  while (Serial.available()) {
    char sue = Serial.read();
    if (sue == 'h' || sue == 'H') {
      Serial.print(F("games_master"));
      Serial.print(F("H    - help (print this message)"));
      Serial.print(F("S    - start game"));
      Serial.print(F("X    - stop game"));
      Serial.println();
    } else if (sue == 'S' || sue == 's') {
      // start the current game
      Serial.println("STARTED GAME\n");
      Serial.println("RED\t\t\tBLUE");
      Serial.println("---\t\t\t----");
      red_score = 0;
      blue_score = 0;
      game_running = true;
    } else if (sue == 'X' || sue == 'x') {
      game_running = false;
      // stop the current game
      Serial.println("END OF GAME");
      Serial.println("Final Scores:");
      Serial.print("RED: ");
      Serial.print(red_score);
      Serial.print("BLUE: ");
      Serial.print(blue_score);
      Serial.println();
    }
  }

  // report any new scores

}

bool sendMessage(uint8_t _target, e_Message _message, e_Side _side, long _value) {
  // build and send the message
  myPacket.the_message = _message;
  myPacket.the_side = _side;
  myPacket.the_value = _value;
  // if broadcast send it without ack
  if (_target == BROADCAST) {
    radio.send(BROADCAST, (const void*)(&myPacket), sizeof(myPacket));
    return true;
  }
  // if targeted, send with ack
  bool ack_success = radio.sendWithRetry(_target, (const void*)(&myPacket), sizeof(myPacket), 5);
  return ack_success;
}

void checkIncoming() {
  // check any incoming radio messages
  // *** We need to call this fast enough so as humans don't notice ***
  if (radio.receiveDone()) {
    // send ACK if requested (and it's directed at us only)
    if (radio.ACKRequested() && radio.TARGETID == GATEWAYID) {
      //uint8_t theNodeID = radio.SENDERID;
      radio.sendACK();
    }
    // check if we're getting hit data
    if (radio.DATALEN = sizeof(t_MessageFormat)) {
      yourPacket = *(t_MessageFormat*)radio.DATA;
      // check the incoming message is a button,
      // if so:
      //     * increment the score (if Game1)
      //     * notify the relavent display (if Game1)
      //     * change posession (if Game2)
      // use the_side and radio.SENDERID to get button number
      uint8_t received_button = radio.SENDERID;
      if (received_button > B_RED_NODE_MIN && received_button < B_BLUE_NODE_MAX) {
        // it's a button in normal range
        if (game_running == true) {
          sendMessage(received_button , OK, NONE, 0);
          if (team(received_button) == RED) {
            red_score++;
            Serial.write('r');
          } else if (team(received_button) == BLUE) {
            blue_score++;
            Serial.write('b');
          }
        }
      }
    }
  }
}

e_Side team(uint8_t _nodeID) {
  if (_nodeID > B_BLUE_NODE_MIN && _nodeID < B_BLUE_NODE_MAX) {
    return BLUE;
  } else if (_nodeID > B_RED_NODE_MIN && _nodeID < B_RED_NODE_MAX) {
    return RED;
  } else {
    return NONE;
  }
}
