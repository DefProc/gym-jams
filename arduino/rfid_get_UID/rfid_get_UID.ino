/**
 *  @title:  StrongLink SL018/SL030 RFID reader demo
 *  @author: marc@marcboon.com
 *  @see:    http://www.stronglink.cn/english/sl018.htm
 *  @see:    http://www.stronglink.cn/english/sl030.htm
 *
 *  Arduino to SL018/SL030 wiring:
 *  A4/SDA     2     3
 *  A5/SCL     3     4
 *  5V         4     -
 *  GND        5     6
 *  3V3        -     1
 */

#include <Wire.h>
#include <SL018.h>

SL018 rfid;
bool tagRead = false;

void setup()
{
  Wire.begin();
  Serial.begin(115200);

  // prompt for tag
  Serial.println("Show me your tag");
}

void loop()
{
  // start seek mode
  rfid.seekTag();
  // wait until tag detected
  if(rfid.available() && tagRead == false) {
    // print tag id
    Serial.println(rfid.getTagString());
    delay(100);
    tagRead = true;
  } else if (!rfid.available()) {
    tagRead = false;
  }
}

