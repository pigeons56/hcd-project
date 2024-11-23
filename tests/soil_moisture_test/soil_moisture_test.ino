//Libraries
#include "Adafruit_seesaw.h"

//Constants
#define MIN_SOIL_MOISTURE 800

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
int reading;

void loop() {
  reading = digitalRead(pumpPin);

  Serial.println(reading);

  waterPlant();
}

void waterPlant() {
  if (needsWater()) {
    Serial.println("PUMP ON");
    digitalWrite(pumpPin, HIGH);
  } else {
    Serial.println("PUMP OFF");
    digitalWrite(pumpPin, LOW);
  }

}

bool needsWater() {
  //water based on soil moisture
  uint16_t capread = ss.touchRead(0);
  Serial.println(capread);
  if(capread < MIN_SOIL_MOISTURE) {
    Serial.println("SOIL IS DRY");
    return 1;
  } else {
    Serial.println("SOIL IS WET");
    return 0;
  }
}
  
