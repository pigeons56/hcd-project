//Libraries
#include "Adafruit_seesaw.h"
#include <I2C_RTC.h>

//Parameters
#define WATER_INTERVAL_DAYS 1
#define MIN_SOIL_MOISTURE 1000
#define PUMP_OPEN_SECS 30
#define FLOAT_SENSOR_DELAY_MILLIS 150
#define START_HR 10
#define END_HR 18

//Constants
const int buttonPin = 13;
const int plantLightPin = 4;
const int floatSensorPin = 2;
const int ledPin = 7;
const int pumpPin = 9;

//Objects
Adafruit_seesaw ss;
static DS3231 RTC;

//Function Prototypes
bool togglePlantLight();
void readFloatSensor();
bool needsWater();
void waterPlant();
unsigned long getMillisDiff(unsigned long start, unsigned long current);
void controlPlantLight();
int getHour();

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

  // RTC Setup
  RTC.begin();

  //Serial monitor setup
  Serial.begin(9600);
}

void loop() {
  int hour = getHour();

  if (hour >= START_HR && hour < END_HR) { //only allow pumping/lights during daylight hours
    controlPlantLight();
    readFloatSensor();
    waterPlant();
  }
}

/*
 * Physically turns plant light on/off according to toggle
 * or defined hour interval. Toggle control overrides scheduled
 * turning on/off according to hour interval.
*/
void controlPlantLight() {
  bool toggleOff = togglePlantLight();
  
  if (toggleOff) {
    //Serial.println("OFF");
    digitalWrite(plantLightPin, LOW);
  } else if (!toggleOff) {
    //Serial.println("ON");
    digitalWrite(plantLightPin, HIGH); //HIGH is light on
  }
}

/*
 * Toggle plant light on and off according to button press.
*/
bool togglePlantLight() {
  static bool prevState = HIGH;
  static bool toggleOff = 0;
  static unsigned long startMillis = 0;
  static unsigned long currentMillis = 0;
  static unsigned long millisDiff = 0;
  bool buttonState = digitalRead(buttonPin);

  currentMillis = millis();
  millisDiff = getMillisDiff(startMillis, currentMillis);
  //Serial.println(buttonState);

  if (prevState == HIGH && buttonState == LOW //pressed
      && millisDiff >= FLOAT_SENSOR_DELAY_MILLIS ) { //delay has passed
    Serial.println("PRESSED");
    startMillis = millis();
    toggleOff = !toggleOff;  
  } 

  prevState = buttonState;

  return toggleOff;
}

/*
 * Reads state of float sensor. LOW is when 
*/
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

/*
 * If plant needs watering (determined by needsWater()), open 
 * pump for a certain amount of seconds (PUMP_OPEN_SECS)
*/
void waterPlant() {
  static bool pumpOpen = 0;
  static unsigned long startMillis = 0;
  static unsigned long currentMillis = 0;
  static unsigned long millisDiff = 0;

  if (pumpOpen) {
    currentMillis = millis();
    millisDiff = getMillisDiff(startMillis, currentMillis);
    

    if (millisDiff >= PUMP_OPEN_SECS * 1000) { //after pump is open for a certain duration
      pumpOpen = 0; //close pump
      digitalWrite(pumpPin, LOW); //physically close pump
      Serial.println("CLOSE PUMP----------------------------------------------------");
    }
  } else {
      if (needsWater()) { //if water conditions are met
        pumpOpen = 1; //open pump
        startMillis = millis(); //get start time
        Serial.println("OPEN PUMP************");
        digitalWrite(pumpPin, HIGH); //physically open pump
      }
  }

}

/*
 * Returns 0 if plant doesn't need water, 1 if plant needs water.
 * Needs water is determined by:
 *  1. Soil moisture goes below minimum soil moisture (MIN_SOIL_MOISTURE)
 *  2. Days without being watered passes set interval (WATER_INTERVAL_DAYS)
*/
bool needsWater() {
  static bool isWatered = 0;
  static bool isCounting = 0;
  static unsigned long startMillis = 0;
  static unsigned long currentMillis = 0;
  static unsigned long millisDiff = 0;
  bool retVal = 0;

  //water based on soil moisture
  uint16_t capread = ss.touchRead(0);
  Serial.println(capread);
  if(capread < MIN_SOIL_MOISTURE) {
    isCounting = 0; //reset count
    retVal = 1;
  } else {
    retVal = 0;
  }

  //water based on time (if not watered already)
  if (!isCounting) {
    //Serial.println("START COUNTING");
    startMillis = millis();
    isCounting = 1; //start counting
  } else {
    currentMillis = millis();
    millisDiff = getMillisDiff(startMillis, currentMillis);
    if (millisDiff >= WATER_INTERVAL_DAYS * 24 * 60 * 60 * 1000) { //if days equal to our watering interval passed
      isCounting = 0; //reset count
      //Serial.println("TIME MET");
      retVal = 1; 
    }
  }

  return retVal;
}

/*
 * Calculates the difference between start and current milliseconds.
*/
unsigned long getMillisDiff(unsigned long start, unsigned long current) {
  if (current >= start) {
    return current - start;
  } else { //overflow occurred, millis() reset
    return 4294967295 - start + current;
  }
}

/*
 * Returns the current hour.
 */
int getHour() {
  return RTC.getHours();
}