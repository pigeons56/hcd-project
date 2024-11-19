#include <Adafruit_seesaw.h>

#define WATER_INTERVAL_DAYS 7
#define MIN_SOIL_MOISTURE 300

Adafruit_seesaw ss;

bool needsWater();


void setup() {
  //Soil sensor setup
  ss.begin(0x36);

}

void loop() {
  // put your main code here, to run repeatedly:

}

/*
 * Returns 1 if either soil moisture is below a minimum OR a certain
 * number of days have passed since watering. Otherwise return 0.
*/
bool needsWater(){
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
