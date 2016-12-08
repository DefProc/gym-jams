#include <SPI.h>
#include <RFM69.h>
#include "../../common/common.h"

bool game_running = true;
uint8_t which_game = 1;
uint8_t max_games = 3;

void setup() {
  Serial.begin(BAUD);
  Serial.println(F("start games_master"));

  radio.initialize(FREQUENCY,GATEWAYID,NETWORKID);
  radio.encrypt(KEY);
}

void loop() {
  // check for serial incomming and parse as required
  while (Serial.available()) {
    char sue = Serial.read();
    if (sue == 'h' || sue == 'H') {
      Serial.println(F("games_master"));
      Serial.println(F("H    - help (print this message)"));
      Serial.println(F("B    - Blue"));
      Serial.println(F("R    - Red"));
      Serial.println(F("X    - Off"));
      Serial.println();
    } else if (sue == 'b' | sue == 'B') {
      // blue mode
      if(sendMessage(ROBOT_NODE, SHOW, BLUE, 0)) { Serial.println("got ACK"); }
    } else if (sue == 'r' | sue == 'R') {
      // red mode
      if(sendMessage(ROBOT_NODE, SHOW, RED, 0)) { Serial.println("got ACK"); }
    } else if (sue == 'x' | sue == 'X') {
      // off mode
      if(sendMessage(ROBOT_NODE, STOP, NONE, 0)) { Serial.println("got ACK"); }
    }
  }
  checkIncoming();
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
  } else {
    // if targeted, send with ack
    bool ack_success = radio.sendWithRetry(_target, (const void*)(&myPacket), sizeof(myPacket), 5);
    return ack_success;
  }
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
        Serial.print("b");
        if (team(received_button ) == BLUE) {
          Serial.print(" BLUE ");
          Serial.print(received_button -B_BLUE_NODE_MIN);
        } else if (team(received_button ) == RED){
          Serial.print(" RED ");
          Serial.print(received_button -B_RED_NODE_MIN);
        } else {
          Serial.print(received_button );
        }
        Serial.println();
        if (game_running == true && which_game == 1) {
          // send the OK message so the button buzzes
          // for game 1
          if (team(received_button) == RED) {
            // buttons 31-34 to boxes 101–104
            sendMessage(received_button +70, SHOW, team(received_button ), 0);
          } else if (team(received_button) == BLUE) {
            // buttons 41-44 do boxes (11)1–(11)4
            sendMessage(received_button +70, SHOW, team(received_button ), 0);
          }
          Serial.print("sent SHOW to ");
          Serial.print(received_button + 70);
          Serial.println();
          sendMessage(received_button , OK, NONE, 0);
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
