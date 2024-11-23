const int buttonPin = 12;
const int plantLightPin = 4;

bool toggleOff = 0;

void setup() {
  pinMode(plantLightPin, OUTPUT);
  pinMode(buttonPin, INPUT);
  Serial.begin(9600);
}
void loop() {
  int buttonState = digitalRead(buttonPin);

  if (buttonState == LOW) { //pressed
    Serial.println("PRESSED");
    toggleOff != toggleOff;  
  } 

  if (toggleOff) {
    digitalWrite(plantLightPin, LOW);
  } else {
    digitalWrite(plantLightPin, HIGH); //HIGH is light on
  }
}
