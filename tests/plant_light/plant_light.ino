const int buttonPin = 12;
const int plantLightPin = 4;

bool toggleOff = 0;
int prevState = 0;
int buttonState = 0;

void setup() {
  pinMode(plantLightPin, OUTPUT);
  pinMode(buttonPin, INPUT);
  Serial.begin(9600);
}
void loop() {
  buttonState = digitalRead(buttonPin);

  if (prevState == HIGH && buttonState == LOW) { //pressed
    Serial.println("PRESSED");
    toggleOff != toggleOff;  
  } 

  if (toggleOff) {
    digitalWrite(plantLightPin, LOW);
  } else {
    digitalWrite(plantLightPin, HIGH); //HIGH is light on
  }

  prevState = buttonState;
}
