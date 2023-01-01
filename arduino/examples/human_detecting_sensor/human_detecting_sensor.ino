const int inPin = 5;
const int ledPin = 16;
int val = 0; // 読み取った値を格納する変数

void setup() {
  Serial.begin(115200);
  pinMode(inPin, INPUT);
  pinMode(ledPin, OUTPUT);
}

void loop() {
  val = digitalRead(inPin); // ピン読み取り
  Serial.println(val); // ログ出力

  if (val) {
    digitalWrite(ledPin, HIGH);
  } else {
    digitalWrite(ledPin, LOW);
  }
}
