#include <ESP8266WiFi.h>
#include <espnow.h>

/* データの構造 */
typedef struct struct_message {
  int mode1;      // 送信モードを指定
  int number[10]; // 送信するユニットを指定
  int color[3];   // 送信する色を指定
} struct_message;

/* Global variables */
struct_message myData;    // データ
const int unitNumber = 1; // ユニット番号
const int LED_R = 12;     // 赤色LED Pin番号
const int LED_G = 14;     // 緑色LED Pin番号
const int LED_B = 13;     // 青色LED Pin番号

/* ESP-NOWのSetup */
void setupEspNow() {

  // ESP-NOWを初期化
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    ESP.restart(); // リスタート
    return;
  }

  // ESP-NOWの設定
  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);  // 自分の役割を設定
  esp_now_register_recv_cb(OnDataRecv);       // 受信完了時のイベントを登録
}

/* LED設定 */
void setLED(int numberLength) {
  for (size_t i = 0; i < numberLength; i++) {
    if (myData.number[i] == 100 || myData.number[i] == unitNumber) {
      analogWrite(LED_R, myData.color[0]);
      analogWrite(LED_G, myData.color[1]);
      analogWrite(LED_B, myData.color[2]);
      break;
    }
  }
}

/* データ受信時のコールバック関数 */
void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
  size_t i;
  int numberLength;
  memcpy(&myData, incomingData, sizeof(myData));

  // ログ出力
  // mode1
  Serial.print("mode1: "); Serial.println(myData.mode1);

  // number
  for (i = 1; i < 10; i++) {
    if (!myData.number[i]) {
      numberLength = i;
      break;
    }
  }

  Serial.print("numberLength: "); Serial.println(numberLength);
  Serial.print("number:");

  for (i = 0; i < numberLength; i++) {
    Serial.print(" "); Serial.print(myData.number[i]);
  }

  // color
  Serial.print("\ncolor:");

  for (i = 0; i < 3; i++) {
    Serial.print(" "); Serial.print(myData.color[i]);
  }

  Serial.println("");

  // LEDのModeを選択
  switch (myData.mode1) {
    case 1: setLED(numberLength); break; // LED設定
    default: Serial.println("default");
  }
}

void setup() {

  // データ転送レートを指定
  Serial.begin(115200);

  // Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.disconnect(); // Wi-Fi切断

  // LED
  analogWriteRange(255);
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);

  // MACアドレスを表示
  Serial.print("\n[setup] MAC Address: ");
  Serial.println(WiFi.macAddress());

  setupEspNow(); // ESP-NOWのSetup
}

void loop() {
  
}
