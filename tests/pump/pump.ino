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
    Serial.println("pressed");
    delay(200);
    digitalWrite(transistorPin, HIGH);
  } else {
    Serial.println("not");
    delay(200);
    digitalWrite(transistorPin, LOW);
  }
  
}
