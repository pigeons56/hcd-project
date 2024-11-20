//Libraries
#include "Adafruit_seesaw.h"

//Constants
#define MIN_SOIL_MOISTURE 300

Adafruit_seesaw ss;
const int pumpPin = 9;

//Function Prototypes
bool needsWater();
void waterPlant();

void setup() {
  //Soil sensor setup
  ss.begin(0x36);

  //Water pump setup
  pinMode(pumpPin, OUTPUT);

  //Serial monitor
  Serial.begin(9600);
}

void loop() {
  waterPlant();
}

void waterPlant() {
  if (needsWater()) {
    digitalWrite(pumpPin, HIGH);
  } else {
    digitalWrite(pumpPin, LOW);
  }

}

bool needsWater() {
  //water based on soil moisture
  uint16_t capread = ss.touchRead(0);
  Serial.write(capread);
  if(capread < MIN_SOIL_MOISTURE) {
    Serial.write("SOIL IS DRY");
    return 1;
  } else {
    Serial.write("SOIL IS WET");
    return 0;
  }
}
  
