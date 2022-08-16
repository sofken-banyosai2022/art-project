const int ledpin = 5;

void setup() {

  Serial.begin(115200);
  pinMode(ledpin, OUTPUT);

}

void loop() {

  digitalWrite(ledpin, HIGH);
  Serial.println("ON");
  delay(500);

  digitalWrite(ledpin, LOW);
  Serial.println("OFF");
  delay(500);

}
