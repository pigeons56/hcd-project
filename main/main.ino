#define PUMP_OPEN_COUNTS 1000

const int pumpPin = 1;

void waterPlant();

void setup() {
  //Water pump setup
  pinMode(pumpPin, OUTPUT);

}

void loop() {

  waterPlant();

}

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