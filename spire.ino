#include <OctoWS2811.h>

const int ledsPerStrip =  150;
const int numStrips = 4;
const int numPixels = ledsPerStrip * numStrips;

DMAMEM int displayMemory[ledsPerStrip * 6];
int drawingMemory[ledsPerStrip * 6];

const int config = WS2811_GRB | WS2811_800kHz;
OctoWS2811 leds(ledsPerStrip, displayMemory, drawingMemory, config);

int incoming_state = 0; // 0=none, 1=read RGB
int incoming_index = 0;
int incoming_red;
int incoming_green;
int incoming_blue;

void setup() {

  // turn on the LED
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);

  Serial.begin(9600);
  Serial1.begin(115200);

  leds.begin();
  leds.show();
}

#define RED    0xFF0000
#define GREEN  0x00FF00
#define BLUE   0x0000FF
#define YELLOW 0xFFFF00
#define PINK   0xFF1088
#define ORANGE 0xE05800
#define WHITE  0xFFFFFF
#define BLACK  0x000000
#define LHTGRY 0xAAAAAA // 0101010 cadence, check for bit errors
#define DRKGRY 0x555555 // "


void theDelay() {
  delay(1000);
}

void fadeToBlack(int microsec) {
  colorWipe(BLACK, microsec);
  delay(100);
}

void loop() {
  //int microsec = 2000000 / leds.numPixels();  // change them all in 2 seconds
  int microsec = 2000000 / numPixels;

//  colorWipe(YELLOW, microsec);
//  theDelay();    

  int avail = Serial1.available();

  while (avail > 0) {

    unsigned char c = Serial1.read();

    if (c == '$') {
      
      // begin RGB data
      incoming_state = 1;
      incoming_index = 0;

    } else {

      if (incoming_state == 1) {
  
        if (c >= '0' && c <= '9') {
          int n = c - '0';
          switch (incoming_index) {
          case  0: incoming_red   = n * 100; break;
          case  1: incoming_red   += n * 10; break;
          case  2: incoming_red   += n;      break;
          case  3: incoming_green = n * 100; break;
          case  4: incoming_green += n * 10; break;
          case  5: incoming_green += n;      break;
          case  6: incoming_blue  = n * 100; break;
          case  7: incoming_blue  += n * 10; break;
          case  8: incoming_blue  += n;      break;
          default: incoming_index = 0;  break;
          }

          incoming_index++;

          if (incoming_index >= 9) {
            unsigned int rgb = (incoming_red<<16) | (incoming_green<<8) | incoming_blue;

            // TODO: prevent too bright of a white!

            colorWipe(rgb, microsec);

            incoming_index = 0;
          }
        }
  
        
      }
    }
  }

}

void colorWipe(int color, int wait)
{
  Serial.println("colorWipe");

  for (int y=0; y < ledsPerStrip; y++) {

    for (int x=0; x < numStrips; x++) {
      leds.setPixel(y + (ledsPerStrip * x), color);
    }
    
    leds.show();
//    delayMicroseconds(10);
  }


  // There are a thousand microseconds in a millisecond, and a million microseconds in a second.
  // Currently, the largest value that will produce an accurate delay is 16383
  //delayMicroseconds(wait); // micro 

}
