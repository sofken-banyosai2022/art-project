#include <ESP8266WiFi.h>
#include <espnow.h>

/* データの構造 */
typedef struct struct_message {
  int mode1;
} struct_message;

/* Global variables */
struct_message myData; // データ

/* ESP-NOWのSetup */
void setupEspNow() {

  // ESP-NOWを初期化
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // ESP-NOWの設定
  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);  // 自分の役割を設定
  esp_now_register_recv_cb(OnDataRecv);       // 受信完了時のイベントを登録
}

/* データ受信時のコールバック関数 */
void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.print("mode1: ");
  Serial.println(myData.mode1);
}

void setup() {

  // データ転送レートを指定
  Serial.begin(115200);

  // Wi-Fi
  WiFi.mode(WIFI_STA);

  // MACアドレスを表示
  Serial.print("\n[setup] MAC Address: ");
  Serial.println(WiFi.macAddress());

  setupEspNow(); // ESP-NOWのSetup
}

void loop() {
  
}
