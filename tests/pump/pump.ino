const int buttonPin = 2;
const int transistorPin = 9;
int buttonState;

void setup() {
  pinMode(buttonPin, INPUT);
  pinMode(transistorPin, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  buttonState = digitalRead(buttonPin);

  if (buttonState == LOW) {
    Serial.print("pressed");
    digitalWrite(transistorPin, HIGH);
  } else {
    Serial.print("not");
    digitalWrite(transistorPin, LOW);
  }
}
