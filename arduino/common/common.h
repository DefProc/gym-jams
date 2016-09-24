/*
  Some common settings for all the gym jams component programs
  */


#define BAUD 115200

// RFM69 assignments
#define NETWORKID   100
#define GATEWAYID   1
#define BROADCAST   255
#define FREQUENCY   RF69_868MHZ
#define KEY         "changemechangeme"
#define ACK_TIME    30  // # of ms to wait for an ack
#define REQUESTACK true
RFM69 radio;

// Button pin assignments
#define B_BUTTON 3
#define B_INT INT1
#define B_LED 9
#define B_PIEZO 6
#define DBOUNCE_TMOUT 200
#define WAIT_FOR_REPLY 2000

// set the possible nodeIDs for the button on each team
#define B_RED_NODE_MIN 30
#define B_RED_NODE_MAX 40
#define B_BLUE_NODE_MIN 40
#define B_BLUE_NODE_MAX 50

// floating robots
#define ROBOT_NODE 21
#define R_LED_PIN 6

// Display pinouts
#define D_LED_PIN 6
#define NODE_MIN 1
#define NODE_MAX 16
#define NODE_LOC 1
#define IMAGE_LOC 5
#define IMAGE_DEFAULT 5
#define IMAGE_MAX 200

// message enum
enum e_Message {
  OK,
  PRESS,
  SHOW,
  START,
  STOP,
  ANIMATE,
};

// side enum
enum e_Side {
  NONE,
  RED,
  BLUE,
  SCORE,
};

// the message format struct
typedef struct {
  e_Message the_message;
  e_Side    the_side;
  long      the_value;
}t_MessageFormat;

t_MessageFormat myPacket;
t_MessageFormat yourPacket;
