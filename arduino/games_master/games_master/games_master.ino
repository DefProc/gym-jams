#include <SPI.h>
#include <RFM69.h>
#include "../../common/common.h"

#define IS_RFM69HW false

enum e_GameStates {
  STARTED,
  STOPPED
};
e_GameStates gameState;
bool game_running = false;
uint8_t which_game = 1;
uint8_t max_games = 3;
t_MessageFormat yourPacket;

void setup() {
  Serial.begin(BAUD);
  Serial.println(F("start games_master"));

  radio.initialize(FREQUENCY,GATEWAYID,NETWORKID);
  #ifdef IS_RFM69HW
    radio.setHighPower();
  #endif
  radio.encrypt(KEY);
}

void loop() {
  // check for serial incomming and parse as required
  while (Serial.available()) {
    char sue = Serial.read();
    if (sue == 'h' || sue == 'H') {
      Serial.print(F("games_master"));
      Serial.print(F("H    - help (print this message)"));
      Serial.print(F("S    - start game"));
      Serial.print(F("X    - stop game"));
      Serial.print(F("G<n> - select game"));
      Serial.print(F("G1   - Button Tag"));
      Serial.print(F("G2   - Robot Rugby"));
      Serial.print(F("G3   - Something Else"));
      Serial.print(F("O    - OVO speaking mode ON"));
      Serial.print(F("Q    - OVO speaking mode OFF"));
      Serial.println();
    } else if (sue == 'S' || sue == 's') {
      // start the current game
      sendMessage(BROADCAST, START, NONE, 0);
      // call the animating boxes to start

    } else if (sue == 'X' || sue == 'x') {
      // stop the current game
      sendMessage(BROADCAST, STOP, NONE, 0);
    } else if (sue == 'O' || sue == 'o') {
      // set OVO speaking mode on
      sendMessage(ROBOT_NODE, ANIMATE, NONE, 0);
    } else if (sue == 'Q' || sue == 'q') {
      // set OVO speaking mode off
      sendMessage(ROBOT_NODE, STOP, NONE, 0);
    } else if (sue == 'G' || sue == 'g') {
      // start the current game
      int number = Serial.parseInt();
      if (number <= max_games) {
        which_game = number;
      }
      Serial.print(F("PLAY GAME "));
      Serial.println(which_game);
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
      //
    }
  }
}
