#include <RTClib.h>

RTC_DS3231 rtc; //Depends on what RTC we have

int getHour();
int getDay();
void setTime();

void setup() {
  // RTC Setup
  rtc.begin();
  setTime(); //Comment out after done ONCE

}

void loop() {
  // put your main code here, to run repeatedly:

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
