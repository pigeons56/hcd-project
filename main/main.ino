#include <Adafruit_seesaw.h>

Adafruit_seesaw ss;


void setup() {
  //Soil sensor setup
  ss.begin(0x36);

}

void loop() {
  // put your main code here, to run repeatedly:

}

int needsWater(){
  uint16_t capread = ss.touchRead(0);
  if(capread < 300) {
    return 1;
  } else {
    return 0;
  }
  
}
