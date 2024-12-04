//Libraries
#include "Adafruit_seesaw.h"
#include <I2C_RTC.h>

//Parameters
#define WATER_INTERVAL_DAYS 30
#define MIN_SOIL_MOISTURE 800
#define PUMP_OPEN_SECS 15
#define FLOAT_SENSOR_DELAY_MILLIS 150
#define START_HR 0//10
#define END_HR 24 //18

//Constants
const int floatSensorPin = 2;
const int ledPin = 7;
const int pumpPin = 9;

//Objects
Adafruit_seesaw ss;
static DS3231 RTC;

//Function Prototypes
void readFloatSensor();
bool needsWater();
void waterPlant();
unsigned long getMillisDiff(unsigned long start, unsigned long current);
int getHour();

void setup() {
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
  //Serial.println(hour);

  if (hour >= START_HR && hour < END_HR) { //only allow pumping/lights during daylight hours
    readFloatSensor();
    waterPlant();
  }
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
    Serial.println("START COUNTING");
    startMillis = millis();
    isCounting = 1; //start counting
  } else {
    currentMillis = millis();
    millisDiff = getMillisDiff(startMillis, currentMillis);
    if (millisDiff >= WATER_INTERVAL_DAYS) { //* 24 * 60 * 60 * 1000) { //if days equal to our watering interval passed
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