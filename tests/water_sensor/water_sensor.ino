const int sensorPin = 2;
const int ledPin = 7;
bool sensorState;

void setup() {
  pinMode(sensorPin, INPUT);
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  sensorState = digitalRead(sensorPin);

  if (sensorState == LOW) {
    Serial.println("Sensor is LOW");
    digitalWrite(ledPin, HIGH);
  } else {
    Serial.println("Sensor is HIGH");
    digitalWrite(ledPin, LOW);
  }

  delay(500);
  
}
