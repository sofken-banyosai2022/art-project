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
struct_message myData;                                          // データ
uint8_t broadcast_mac[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; // ブロードキャスト用MACアドレス

unsigned long lastTime = 0;  
unsigned long timerDelay = 2000;                                // 2秒ごとに送信

/* データ送信時のコールバック関数 */
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {

  if (sendStatus == 0) {
    Serial.println("[ESP-NOW] Delivery success");
  } else{
    Serial.println("[ESP-NOW] Delivery fail");
  }
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
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);                   // 自分の役割を設定  
  esp_now_register_send_cb(OnDataSent);                             // 送信完了時のイベントを登録
  esp_now_add_peer(broadcast_mac, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);  // デバイスを登録
}

void loop() {

  // timerDelayごとに実行
  if ((millis() - lastTime) > timerDelay) {

    // 送信するデータを設定
    strcpy(myData.a, "THIS IS A CHAR");
    myData.b = random(1,20);
    myData.c = 1.2;
    myData.d = "Hello";
    myData.e = false;

    // ESP-NOWでデータを送信
    esp_now_send(broadcast_mac, (uint8_t *) &myData, sizeof(myData));

    lastTime = millis();
  }
}
