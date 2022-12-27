#include <ESP8266WiFi.h>
#include <espnow.h>

/* データの構造 */
typedef struct struct_message {
  int mode1;      // 送信モードを指定
  int number[10]; // 送信するユニットを指定
  int color[3];   // 送信する色を指定
  int mode2;      // 遅延時間を指定
} struct_message;

/* Global variables */
struct_message myData;    // データ
uint8_t broadcastMac[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; // ブロードキャスト用MACアドレス
uint8_t sub1Mac[] = SUB1_MAC;                                  // sub1のMACアドレス

/* データ送信時のコールバック関数 */
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {

  if (sendStatus == 0) {
    Serial.println("[ESP-NOW] Delivery success");
  } else{
    Serial.println("[ESP-NOW] Delivery fail");
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

  // mode2
  Serial.print("\nmode2: "); Serial.println(myData.mode2);

  // 送信先を判別
  if (myData.number[0] == 100) { // デイジーチェーン
    esp_now_send(sub1Mac, (uint8_t *) &myData, sizeof(myData));      // ESP-NOWでデータを送信
  } else { // ユニット番号, ブロードキャスト
    esp_now_send(broadcastMac, (uint8_t *) &myData, sizeof(myData)); // ESP-NOWでデータを送信 (broadcast)
  }
}

/* ESP-NOWのSetup */
void setupEspNow() {

  // ESP-NOWを初期化
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    ESP.restart(); // リスタート
    return;
  }

  // ESP-NOWの設定
  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);                       // 自分の役割を設定
  esp_now_add_peer(sub1Mac, ESP_NOW_ROLE_COMBO, 1, NULL, 0);       // デバイスを登録
  esp_now_register_send_cb(OnDataSent);                            // 送信完了時のイベントを登録
  esp_now_register_recv_cb(OnDataRecv);                            // 受信完了時のイベントを登録
}

void setup() {

  // データ転送レートを指定
  Serial.begin(115200);

  // Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.disconnect(); // Wi-Fi切断

  // MACアドレスを表示
  Serial.print("\n[setup] MAC Address: ");
  Serial.println(WiFi.macAddress());

  setupEspNow(); // ESP-NOWのSetup
}

void loop() {
  
}
