//Libraries
#include "Adafruit_seesaw.h"
#include <I2C_RTC.h>

//Parameters
#define WATER_INTERVAL_MILLIS 4 * 24 * 60 * 60 * 1000 //days converted to millis
#define PUMP_OPEN_MILLIS 150 * 1000 //seconds converted to millis
#define FLOAT_SENSOR_DELAY_MILLIS 150
#define START_HR 10
#define END_HR 18
#define WATER_LIMIT_MILLIS 1 * 24 * 60 * 60 * 1000 //days converted to millis
#define MIN_SOIL_MOISTURE_WAIT_MILLIS 4 * 24 * 60 * 60 * 1000 //days converted to millis

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
void printTime();
void printStatusMsg(String msg);
void getMinSoilMoisture();

//Global variables
uint16_t minSoilMoisture = 400;
int minSoilMoistureSet = 0;

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
  Serial.begin(115200);

  printStatusMsg("DEPLOYED");
}

void loop() {
  //First figure out min soil moisture
  while (!minSoilMoistureSet) {
    getMinSoilMoisture();
  }
  
  //After that, start normal operations:
  int hour = RTC.getHours();

  if (hour >= START_HR && hour < END_HR) { //only allow pumping/lights during daylight hours
    readFloatSensor();
    waterPlant();
  }
}

/* 
 * Occurs first when Arduino is powered. Waits a certain number of days (MIN_SOIL_MOISTURE_WAIT_MILLIS), 
 *then reads soil moisture (this is 'dry' soil and is the minimum soil moisture).
*/
void getMinSoilMoisture() {
  static unsigned long startMillis = millis();
  static unsigned long currentMillis = 0;
  static unsigned long millisDiff = 0;

  currentMillis = millis();
  millisDiff = getMillisDiff(startMillis, currentMillis);

  if (millisDiff >= MIN_SOIL_MOISTURE_WAIT_MILLIS) { //wait a number of days
    minSoilMoisture = ss.touchRead(0);
    minSoilMoistureSet = 1;
    Serial.print(minSoilMoisture);
    printStatusMsg(" Min soil moisture recorded");
  }
}

/*
 * Reads state of float sensor. LOW is when water level is lower than sensor.
*/
void readFloatSensor() {
  bool floatSensorState = digitalRead(floatSensorPin);

  if (floatSensorState == LOW) {
    digitalWrite(ledPin, HIGH);
  } else {
    digitalWrite(ledPin, LOW);
  }
}

/*
 * If plant needs watering (determined by needsWater()), open 
 * pump for a certain amount of seconds (PUMP_OPEN_SECS)
*/
void waterPlant() {
  static bool pumpOpen = 0;
  static unsigned long pumpStartMillis = 0;
  static unsigned long dayCountStartMillis = 0;
  static unsigned long currentMillis = 0;
  static unsigned long millisDiff = 0;
  static bool isWateredToday = 0;

  if (!isWateredToday) {
    if (pumpOpen) {
      currentMillis = millis();
      millisDiff = getMillisDiff(pumpStartMillis, currentMillis);

      if (millisDiff >= PUMP_OPEN_MILLIS) { //after pump is open for a certain duration
        pumpOpen = 0; //close pump
        isWateredToday = 1; //don't water again today
        dayCountStartMillis = millis(); //start counting
        digitalWrite(pumpPin, LOW); //physically close pump

        printStatusMsg("Pump CLOSED");
      }
    } else {
      if (needsWater()) { //if water conditions are met
        pumpOpen = 1; //open pump
        pumpStartMillis = millis(); //get pump open start time
        digitalWrite(pumpPin, HIGH); //physically open pump

        printStatusMsg("Pump OPENED");
      }
    }
  } else { //count for a day passing
    currentMillis = millis();
    millisDiff = getMillisDiff(dayCountStartMillis, currentMillis);
    if (millisDiff >= WATER_LIMIT_MILLIS) {
      isWateredToday = 0;
      printStatusMsg("Water limit reset");
    }
  }
}

/*
 * Returns 0 if plant doesn't need water, 1 if plant needs water.
 * Needs water is determined by:
 *  1. Soil moisture goes below minimum soil moisture (MIN_SOIL_MOISTURE)
 *  2. Days without being watered passes set interval (WATER_INTERVAL_MILLIS)
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
  if(capread < minSoilMoisture) {
    isCounting = 0; //reset count
    retVal = 1;

    Serial.print(capread);
    printStatusMsg(" Needs water (SOIL condition)");
  } else {
    retVal = 0;
  }

  //water based on time (if not watered already)
  if (!isCounting) {
    startMillis = millis();
    isCounting = 1; //start counting
  } else {
    currentMillis = millis();
    millisDiff = getMillisDiff(startMillis, currentMillis);
    if (millisDiff >= WATER_INTERVAL_MILLIS) { //if days equal to our watering interval passed
      isCounting = 0; //reset count
      retVal = 1; 

      printStatusMsg("Needs water (TIME condition)");
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
 * Print current real time to Serial Monitor. Used for status/error messages.
*/
void printTime() {
  Serial.print(RTC.getHours());
  Serial.print(":");
  Serial.print(RTC.getMinutes());
  Serial.print(":");
  Serial.println(RTC.getSeconds());
}

/*
 * Print status message
*/
void printStatusMsg(String msg) {
  Serial.print(msg);
  Serial.print(": ");
  printTime();
}
