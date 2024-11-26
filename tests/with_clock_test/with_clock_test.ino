//Libraries
#include "Adafruit_seesaw.h"

//Parameters
#define WATER_INTERVAL_DAYS 30
#define MIN_SOIL_MOISTURE 500
#define PUMP_OPEN_SECS 5
#define FLOAT_SENSOR_DELAY_MILLIS 150

//Constants
const int buttonPin = 13;
const int plantLightPin = 4;
const int floatSensorPin = 2;
const int ledPin = 7;
const int pumpPin = 9;

//Objects
Adafruit_seesaw ss;
RTC_DS3231 rtc;

//Function Prototypes
void toggleLight();
void readFloatSensor();
bool needsWater();
void waterPlant();
unsigned long getMillisDiff(unsigned long start, unsigned long current);
void setTime();
int getHour();

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

  // RTC Setup
  rtc.begin();
  setTime(); //Comment out after done ONCE

  //Serial monitor setup
  Serial.begin(9600);
}
void loop() {
  toggleLight();
  readFloatSensor();
  waterPlant();
  
}

/*
 * Toggle plant light on and off according to button press.
*/
void toggleLight() {
  static bool prevState = HIGH;
  static bool toggleOff = 0;
  bool buttonState = digitalRead(buttonPin);
  static unsigned long startMillis = 0;
  static unsigned long currentMillis = 0;
  static unsigned long millisDiff = 0;

  currentMillis = millis();
  millisDiff = getMillisDiff(startMillis, currentMillis);

  if (prevState == HIGH && buttonState == LOW //pressed
      && millisDiff >= FLOAT_SENSOR_DELAY_MILLIS ) { //delay has passed
    Serial.println("PRESSED");
    startMillis = millis();
    toggleOff = !toggleOff;  
  } 

  if (toggleOff) {
    //Serial.println("OFF");
    digitalWrite(plantLightPin, LOW);
  } else {
    //Serial.println("ON");
    digitalWrite(plantLightPin, HIGH); //HIGH is light on
  }

  prevState = buttonState;
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
    if (millisDiff >= WATER_INTERVAL_DAYS * 24 * 60 * 60 * 1000) { //if days equal to our watering interval passed
      isCounting = 0; //reset count
      Serial.println("TIME MET");
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
  DateTime now = rtc.now();
  return now.hour();
}

/* 
 * One-time setup for the RTC. Time should save even when Arduino is not running.
 * Run ONCE.
 */
void setTime() {
  rtc.adjust(DateTime(2024, 11, 18, 4, 28, 0)); //yr, mon, day, hr, min, sec
}