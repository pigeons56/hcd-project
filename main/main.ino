#define PLANT_LIGHT_ON_TIME 10
#define PLANT_LIGHT_OFF_TIME 16

const int plantLightPin = 4;
const int plantLightOffPin = 5;
const int plantLightButtonPin = 6;

bool plantLightToggleOff = 0;
bool plantLightOn = 0;

void timePlantLight();
void plantLightOn();
void plantLightOff();
void plantLightToggle();

void setup() {
  //Plant light setup
  pinMode(plantLightPin, OUTPUT);
  pinMode(plantLightOffPin, OUTPUT);
  pinMode(plantLightButtonPin, INPUT);

}

void loop() {
  //Plant light control
  plantLightToggle();
  if (plantLightToggleOff) {
    plantLightOff(); //Plant light always off if toggled off
  } else {
    timePlantLight(); //Control plant light based on hour if not toggled off
  }

}

/*
 * Turns plant light on/off according to current hour.
*/
void timePlantLight() {
  static int hour = getHour();

  if (hour < PLANT_LIGHT_OFF_TIME && hour >= PLANT_LIGHT_ON_TIME) {
    plantLightOn();
  } else {
    plantLightOff();
  }
}

/*
 * Turns plant light on.
*/
void plantLightOn() {
  if (!plantLightOn) {
      plantLightOn = 1;
      digitalWrite(plantLightPin, HIGH);
  }
}

/*
 * Turns plant light off.
*/
void plantLightOff() {
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