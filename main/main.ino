const int floatSwitchPin = 2;
const int ledPin = 3;

void setup() {
  pinMode(floatSwitchPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);

}

void loop() {
  // put your main code here, to run repeatedly:

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