//Libraries
#include <RTClib.h>
#include "Adafruit_seesaw.h"

//Constants
#define WATER_INTERVAL_DAYS 7
#define MIN_SOIL_MOISTURE 300
#define PUMP_OPEN_COUNTS 1000
#define PLANT_LIGHT_ON_TIME 10
#define PLANT_LIGHT_OFF_TIME 16

RTC_DS3231 rtc; //Depends on what RTC we have
Adafruit_seesaw ss;
const int pumpPin = 1;
const int floatSwitchPin = 2;
const int ledPin = 3;
const int plantLightPin = 4;
const int plantLightOffPin = 5;
const int plantLightButtonPin = 6;

//Global vars
bool plantLightToggleOff = 0;
bool plantLightOn = 0;

//Function Prototypes
int getHour();
int getDay();
void setTime();
bool needsWater();
void waterPlant();
void readFloatSwitch();
void timePlantLight();
void turnPlantLightOn();
void turnPlantLightOff();
void plantLightToggle();

void setup() {
  // RTC Setup
  rtc.begin();
  setTime(); //Comment out after done ONCE

  //Soil sensor setup
  ss.begin(0x36);

  //Water pump setup
  pinMode(pumpPin, OUTPUT);

  //Float switch setup
  pinMode(floatSwitchPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);

  //Plant light setup
  pinMode(plantLightPin, OUTPUT);
  pinMode(plantLightOffPin, OUTPUT);
  pinMode(plantLightButtonPin, INPUT);
}

void loop() {
  //Plant light control
  plantLightToggle();
  if (plantLightToggleOff) {
    turnPlantLightOff(); //Plant light always off if toggled off
  } else {
    timePlantLight(); //Control plant light based on hour if not toggled off
  }

  //Water the plant according to time or soil moisture
  waterPlant();

  //Check water level
  readFloatSwitch();

  delay(100);
}

/*
 * Turns plant light on/off according to current hour.
*/
void timePlantLight() {
  static int hour = getHour();

  if (hour < PLANT_LIGHT_OFF_TIME && hour >= PLANT_LIGHT_ON_TIME) {
    turnPlantLightOn();
  } else {
    turnPlantLightOff();
  }
}

/*
 * Turns plant light on.
*/
void turnPlantLightOn() {
  if (!plantLightOn) {
      plantLightOn = 1;
      digitalWrite(plantLightPin, HIGH);
  }
}

/*
 * Turns plant light off.
*/
void turnPlantLightOff() {
  if (plantLightOn) {
    plantLightOn = 0;
    digitalWrite(plantLightPin, LOW);
  }
}

/*
 * Toggles plant light on/off according to button press.
*/
void plantLightToggle() {
  static int plantLightButtonState = digitalRead(plantLightButtonPin);

  if (plantLightButtonState == LOW) { //on press
    if (plantLightToggleOff) {
      plantLightToggleOff = 0;
    } else {
      plantLightToggleOff = 1;
    }
  }

  if (plantLightToggleOff) {
    digitalWrite(plantLightButtonPin, HIGH);
  }
}

/*
 * Reads the state of the float switch. If the float switch is
 * LOW (water level is high), turn off LED. If the float switch
 * is HIGH (water level is low), turn on LED.
*/
void readFloatSwitch() {
  static bool floatSwitchState = digitalRead(floatSwitchPin);

  if (floatSwitchState == LOW) {
    digitalWrite(ledPin, LOW);
  } else {
    digitalWrite(ledPin, HIGH);
  }
}

/*
 * Open pump depending on needsWater() value. Pump stays open for PUMP_OPEN_COUNTS.
*/
void waterPlant() {
  static bool pumpOpen = 0;
  static int count = 0;

  if (pumpOpen) {
    count++; //keep counting

    if (count > PUMP_OPEN_COUNTS) { //after pump is open for a certain duration
      count = 0; //reset counting
      pumpOpen = 0; //close pump
      digitalWrite(pumpPin, LOW); //physically close pump
    }
  } else {
      if (needsWater()) { //if water conditions are met
        pumpOpen = 1; //open pump
        digitalWrite(pumpPin, HIGH); //physically open pump
      }
  }

}

/*
 * Returns 1 if either soil moisture is below a minimum OR a certain
 * number of days have passed since watering. Otherwise return 0.
*/
bool needsWater() {
  static bool isWatered = 0;
  static bool isCountingDays = 0;
  static int hourNow = 0;
  static int dayNow = 0;
  static int daysPassed = 0;
  
  //water based on soil moisture
  uint16_t capread = ss.touchRead(0);
  if(capread < MIN_SOIL_MOISTURE) {
    isCountingDays = 0; //reset count
    return 1;
  } else {
    return 0;
  }

  //water based on time (if not watered already)
  if (!isCountingDays) {
    daysPassed = 0; //reset daysPassed
    hourNow = getHour(); //get hour right now when program was started/plant was watered
    dayNow = getDay(); //get day right now when program was started/plant was watered
    isCountingDays = 1; //start counting
  } else {
    if (getHour() == hourNow && getDay() != dayNow) { //if a day has passed
      daysPassed++;
    }

    if (daysPassed == WATER_INTERVAL_DAYS) { //if days equal to our watering interval passed
      isCountingDays = 0; //reset count
      return 1; 
    }
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
 * Returns the current day.
 */
int getDay() {
  DateTime now = rtc.now();
  return now.day();
}

/* 
 * One-time setup for the RTC. Time should save even when Arduino is not running.
 * Run ONCE.
 */
void setTime() {
  rtc.adjust(DateTime(2024, 11, 18, 4, 28, 0)); //yr, mon, day, hr, min, sec
}