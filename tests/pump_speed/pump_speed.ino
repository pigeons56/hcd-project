const int transistorPin = 9;
int count = 0;

void setup() {
  pinMode(transistorPin, OUTPUT);
  Serial.begin(9600);

  digitalWrite(transistorPin, HIGH);
}

void loop() {
  delay(1000);
  count++;
  Serial.print(count);
}
