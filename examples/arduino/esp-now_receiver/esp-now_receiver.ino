#include <ESP8266WiFi.h>
#include <espnow.h>

/* データの構造 */
typedef struct struct_message {
  char a[32];
  int b;
  float c;
  String d;
  bool e;
} struct_message;

/* Global variables */
struct_message myData; // データ

/* データ受信時のコールバック関数 */
void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("Char: ");
  Serial.println(myData.a);
  Serial.print("Int: ");
  Serial.println(myData.b);
  Serial.print("Float: ");
  Serial.println(myData.c);
  Serial.print("String: ");
  Serial.println(myData.d);
  Serial.print("Bool: ");
  Serial.println(myData.e);
  Serial.println();
}

void setup() {

  // データ転送レートを指定
  Serial.begin(115200);

  // Wi-Fi
  WiFi.mode(WIFI_STA);

  // MACアドレスを表示
  Serial.print("\n[setup] MAC Address: ");
  Serial.println(WiFi.macAddress());

  // ESP-NOWを初期化
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // ESP-NOWの設定
  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);  // 自分の役割を設定
  esp_now_register_recv_cb(OnDataRecv);       // 受信完了時のイベントを登録
}

void loop() {
  
}
