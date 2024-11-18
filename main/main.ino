void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}

int needsWater(){
  int value = analogRead(sensorPin);
  if(value < 300)
    return 1;
  else
    return 0;
  //backup water if not watered in 48 hours? need a way to track time
}
