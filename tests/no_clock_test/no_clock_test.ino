//Libraries
#include "Adafruit_seesaw.h"

//Parameters
#define MIN_SOIL_MOISTURE 1000
#define PUMP_OPEN_SECS 10

//Constants
const int buttonPin = 12;
const int plantLightPin = 4;
const int floatSensorPin = 2;
const int ledPin = 7;
const int pumpPin = 9;

//Objects
Adafruit_seesaw ss;

//Function Prototypes
void toggleLight();
void readFloatSensor();
bool needsWater();
void waterPlant();

//counter
int secondsPassed = 0;

void setup() {
  //Plant light setup
  pinMode(plantLightPin, OUTPUT);
  pinMode(buttonPin, INPUT);

  //float sensor & button setup
  pinMode(floatSensorPin, INPUT);
  pinMode(ledPin, OUTPUT);

  //Soil sensor setup
  ss.begin(0x36);

  //Water pump setup
  pinMode(pumpPin, OUTPUT);

  //Serial monitor setup
  Serial.begin(9600);
}
void loop() {
  toggleLight();
  readFloatSensor();
  waterPlant();
  
  //Clock subsitute
  delay(1000);
  if (secondsPassed > 59) {
    secondsPassed = 0;
  } else {
    secondsPassed++;
  }
}

void toggleLight() {
  static bool prevState = HIGH;
  static bool toggleOff = 0;
  bool buttonState = digitalRead(buttonPin);

  if (prevState == HIGH && buttonState == LOW) { //pressed
    //Serial.println("PRESSED");
    toggleOff != toggleOff;  
  } 

  if (toggleOff) {
    digitalWrite(plantLightPin, LOW);
  } else {
    digitalWrite(plantLightPin, HIGH); //HIGH is light on
  }

  prevState = buttonState;
}

void readFloatSensor() {
  bool floatSensorState = digitalRead(floatSensorPin);

  if (floatSensorState == LOW) {
    //Serial.println("Sensor is LOW");
    digitalWrite(ledPin, HIGH);
  } else {
    //Serial.println("Sensor is HIGH");
    digitalWrite(ledPin, LOW);
  }
}

void waterPlant() {
  static bool pumpOpen = 0;
  static int startSec = 0;
  static int currentSec = 0;
  static int secDiff = 0;
  static int totSecsPassed = 0;

  if (pumpOpen) {
    currentSec = secondsPassed;

    //Calculate how many secs have passed
    if (currentSec >= startSec) { //did not pass the min
      secDiff = currentSec - startSec;
    } else { //did pass the min
      secDiff = 60 - startSec + currentSec;
    }

    totSecsPassed += secDiff;
    startSec = currentSec;

    if (totSecsPassed >= PUMP_OPEN_SECS) { //after pump is open for a certain duration
      pumpOpen = 0; //close pump
      digitalWrite(pumpPin, LOW); //physically close pump
    }
  } else {
      if (needsWater()) { //if water conditions are met
        pumpOpen = 1; //open pump
        startSec = secondsPassed; //get start time
        digitalWrite(pumpPin, HIGH); //physically open pump
      }
  }

}

bool needsWater() {
  //water based on soil moisture
  uint16_t capread = ss.touchRead(0);
  Serial.println(capread);
  if(capread < MIN_SOIL_MOISTURE) {
    //Serial.println("SOIL IS DRY");
    return 1;
  } else {
    //Serial.println("SOIL IS WET");
    return 0;
  }
}