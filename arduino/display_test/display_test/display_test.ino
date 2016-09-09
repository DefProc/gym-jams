#include "FastLED.h"
#include <SdFat.h>
#include <TimerOne.h>
SdFat sd;
SdFile myFile;

#define BUFFPIXEL 1

// How many leds in your strip?
#define NUM_LEDS 90

// How many numbered images on the SD card?
#define NUM_IMAGES 5

// For led chips like Neopixels, which have a data line, ground, and power, you just
// need to define DATA_PIN.  For led chipsets that are SPI based (four wires - data, clock,
// ground, and power), like the LPD8806, define both DATA_PIN and CLOCK_PIN
#define DATA_PIN 6

// Error messages stored in flash.
#define error(msg) sd.errorHalt(F(msg))

// Define the array of leds
CRGB leds[NUM_LEDS];

// timer frequency for the ADC
const unsigned char PS_16 = (1 << ADPS2);
const unsigned char PS_128 = (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

//System Setup
volatile boolean
  changeImage = false; // should we change the image to the next one?
boolean
  singleGraphic = false, // single BMP file
  setupActive = false; // set brightness, playback mode, etc.
int
  offsetX = 0, // for translating images x pixels
  offsetY = 0; // for translating images y pixels
uint8_t
  analogThreshold = 70, // above what value should we take a reading as activated
  currentImage = 0; // which image are we looking at
uint32_t
  impactRepeat = 500UL; // debounce timeout for the sensor
volatile uint32_t
  lastImpact = 0UL; // when did we last see a hit

void setup() {
  Serial.begin(9600);
  Serial.println("starting nerf_test");

  // set up the ADC sampling speed
  ADCSRA &= ~PS_128;  // remove bits set by Arduino library
  ADCSRA |= PS_16;    // set the prescaler to 16 (1MHz)

  // initialize the LED string
  FastLED.addLeds<WS2812,DATA_PIN,RGB>(leds,NUM_LEDS);
  FastLED.setBrightness(84);

  // start the SD card
  if (!sd.begin(4, SPI_HALF_SPEED)) {
    sd.initErrorHalt();
  } else {
    Serial.println(F("SD began"));
    sd.ls(LS_R);
  }

  // show the colour wipe, so we know the display's working
  colour_wipe();

  // draw the invader logo
  bmpDraw("invader.bmp", 0, 0);
  delay(2500);

  // Start the fast interrupt for the analog read
  pinMode(A0, INPUT);
  Timer1.initialize(10000); // run at 100Hz
  Timer1.attachInterrupt(checkImpact);

  // and start watching INT1 for too
  pinMode(3, INPUT);
  attachInterrupt(INT1, piezoInt, FALLING);
}

void fadeall() { for(int i = 0; i < NUM_LEDS; i++) { leds[i].nscale8(250); } }

void loop() {
  if (changeImage == true) {
    char filename_buffer[13];
    sprintf(filename_buffer, "%04d.bmp", currentImage+1);
    if (sd.exists(filename_buffer)) {
      Serial.print("Displaying ");
      Serial.println(filename_buffer);
      bmpDraw(filename_buffer, 0, 0);
      changeImage = false;
    } else {
      Serial.print(filename_buffer);
      Serial.println(" doesn't exist");
    }
    currentImage++;
    currentImage = currentImage % (NUM_IMAGES);
  }
}

void colour_wipe() {
  static uint8_t hue = 0;
  Serial.print("x");
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
    delay(10);
  }
  Serial.print("x");

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
    delay(10);
  }
}

void checkImpact() {
  if (analogRead(A0) >= analogThreshold && millis() - lastImpact >= impactRepeat) {
    changeImage = true;
    lastImpact = millis();
  }
}

void piezoInt() {
  if (millis() - lastImpact >= impactRepeat) {
    changeImage = true;
    lastImpact = millis();
    Serial.println(F("interrupt"));
  }
}

// This function opens a Windows Bitmap (BMP) file and
// displays it at the given coordinates.  It's sped up
// by reading many pixels worth of data at a time
// (rather than pixel by pixel).  Increasing the buffer
// size takes more of the Arduino's precious RAM but
// makes loading a little faster.  20 pixels seems a
// good balance.

void bmpDraw(char *filename, uint8_t x, uint8_t y) {

  int  bmpWidth, bmpHeight;   // W+H in pixels
  uint8_t  bmpDepth;              // Bit depth (currently must be 24)
  uint32_t bmpImageoffset;        // Start of image data in file
  uint32_t  rowSize;               // Not always = bmpWidth; may have padding
  uint8_t  sdbuffer[3*BUFFPIXEL]; // pixel buffer (R+G+B per pixel)
  uint8_t  buffidx = sizeof(sdbuffer); // Current position in sdbuffer
  boolean  goodBmp = false;       // Set to true on valid header parse
  boolean  flip    = true;        // BMP is stored bottom-to-top
  int  w, h, row, col;
  uint8_t  r, g, b;
  uint32_t pos = 0;
  const uint8_t  gridWidth = 9;
  const uint8_t  gridHeight = 9;

  if((x >= gridWidth) || (y >= gridHeight)) {
    Serial.print(F("Abort."));
    return;
  }

  Serial.println();

  if (!myFile.isOpen())
  {
    Serial.print(F("Loading image '"));
    Serial.print(filename);
    Serial.println('\'');
    // Open requested file on SD card
    if (!myFile.open(filename, O_READ)) {
      Serial.println(F("File open failed"));
      //sdErrorMessage();
      return;
    }
  }
  else myFile.rewind();

  //if (debugMode == true)
  //{
   // printFreeRAM();
  //}

  // Parse BMP header
  if(read16(myFile) == 0x4D42) { // BMP signature
    Serial.print(F("File size: ")); Serial.println(read32(myFile));
    (void)read32(myFile); // Read & ignore creator bytes
    bmpImageoffset = read32(myFile); // Start of image data
    Serial.print(F("Image Offset: ")); Serial.println(bmpImageoffset, DEC);
    // Read DIB header
    Serial.print(F("Header size: ")); Serial.println(read32(myFile));
    bmpWidth  = read32(myFile);
    bmpHeight = read32(myFile);
    if(read16(myFile) == 1) { // # planes -- must be '1'
      bmpDepth = read16(myFile); // bits per pixel
      Serial.print(F("Bit Depth: ")); Serial.println(bmpDepth);
      if((bmpDepth == 24) && (read32(myFile) == 0)) { // 0 = uncompressed

        goodBmp = true; // Supported BMP format -- proceed!
        Serial.print(F("Image size: "));
        Serial.print(bmpWidth);
        Serial.print('x');
        Serial.println(bmpHeight);

        Serial.print(F("Image offset: "));
        Serial.print(offsetX);
        Serial.print(F(", "));
        Serial.println(offsetY);

        // image smaller than 16x16?
        if ((bmpWidth < 9 && bmpWidth > -9) || (bmpHeight < 9 && bmpHeight > -9))
        {
          clearStripBuffer();
        }

        // BMP rows are padded (if needed) to 4-byte boundary
        rowSize = (bmpWidth * 3 + 3) & ~3;
        Serial.print(F("Row size: "));
        Serial.println(rowSize);


        // If bmpHeight is negative, image is in top-down order.
        // This is not canon but has been observed in the wild.
        if(bmpHeight < 0) {
          bmpHeight = -bmpHeight;
          flip      = false;
        }

        // initialize our pixel index
        byte index = 0; // a byte is perfect for a 16x16 grid

        // Crop area to be loaded
        w = bmpWidth;
        h = bmpHeight;
        if((x+w-1) >= gridWidth)  w = gridWidth - x;
        if((y+h-1) >= gridHeight) h = gridHeight - y;

        for (row=0; row<h; row++) { // For each scanline...

          // Seek to start of scan line.  It might seem labor-
          // intensive to be doing this on every line, but this
          // method covers a lot of gritty details like cropping
          // and scanline padding.  Also, the seek only takes
          // place if the file position actually needs to change
          // (avoids a lot of cluster math in SD library).

          if(flip) // Bitmap is stored bottom-to-top order (normal BMP)
            pos = (bmpImageoffset + (offsetX * -3) + (bmpHeight - 1 - (row + offsetY)) * rowSize);
          else     // Bitmap is stored top-to-bottom
            pos = bmpImageoffset + row * rowSize;
          if(myFile.curPosition() != pos) { // Need seek?
            myFile.seekSet(pos);
            buffidx = sizeof(sdbuffer); // Force buffer reload
          }

          for (col=0; col<w; col++) { // For each pixel...
            // Time to read more pixel data?
            if (buffidx >= sizeof(sdbuffer)) { // Indeed
              myFile.read(sdbuffer, sizeof(sdbuffer));
              buffidx = 0; // Set index to beginning
            }

            // push to LED buffer
            b = sdbuffer[buffidx++];
            r = sdbuffer[buffidx++];
            g = sdbuffer[buffidx++];

            // offsetY is beyond bmpHeight
            if (row >= bmpHeight - offsetY)
            {
              // black pixel
              leds[getIndex(col, row)] = CRGB::Black;
              //led[getIndex(col, row)] strip.Color(0, 0, 0));
            }
            // offsetY is negative
            else if (row < offsetY * -1)
            {
              // black pixel
              leds[getIndex(col, row)] = CRGB::Black;
              //strip.setPixelColor(getIndex(col, row), strip.Color(0, 0, 0));
            }
            // offserX is beyond bmpWidth
            else if (col >= bmpWidth + offsetX)
            {
              // black pixel
              leds[getIndex(col, row)] = CRGB::Black;
              //strip.setPixelColor(getIndex(col, row), strip.Color(0, 0, 0));
            }
            // offsetX is positive
            else if (col < offsetX)
            {
              // black pixel
              leds[getIndex(col, row)] = CRGB::Black;
              //strip.setPixelColor(getIndex(col, row), strip.Color(0, 0, 0));
            }
            // all good
            else {
              leds[getIndex(col+x, row)].setRGB(r, g, b);
              //strip.setPixelColor(getIndex(col+x, row), strip.Color(r, g, b));
            }
            // paint pixel color
          } // end pixel
        } // end scanline
      } // end goodBmp
    }
  }
  FastLED.show();
  // NOTE: strip.show() halts all interrupts, including the system clock.
  // Each call results in about 6825 microseconds lost to the void.
  if (singleGraphic == false || setupActive == true)
  {
    Serial.println(F("Closing Image..."));
    myFile.close();
  }
  if(!goodBmp) Serial.println(F("Format unrecognized."));
}

// These read 16- and 32-bit types from the SD card file.
// BMP data is stored little-endian, Arduino is little-endian too.
// May need to reverse subscript order if porting elsewhere.

uint16_t read16(SdFile& f) {
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read(); // MSB
  return result;
}

uint32_t read32(SdFile& f) {
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}

void clearStripBuffer()
{
  for (int i=0; i<NUM_LEDS; i++)
  {
    leds[i] = CRGB::Black;
  }
}

byte getIndex(byte x, byte y)
{
  byte index;
  if (y % 2 == 0)
  {
    // even lines are the right way round
    // (0,2,4,6,8)
    index = y * 10 + x;
  } else {
    // odd number lines are reversed direction
    // (1,3,5,7,9)
    index = (y * 10 + 9) - x - 1;
  }
  return index;
}
