#include <ESP8266WiFi.h>
#include <espnow.h>
#include <ArduinoOSCWiFi.h>

/* データの構造 */
typedef struct struct_message {
  uint16_t seqNo;     // 送信シーケンス番号
  uint8_t retry;      // 送信リトライ数
  uint8_t mode1;      // 送信モードを指定
  uint8_t number[10]; // 送信するユニットを指定
  uint8_t color[3];   // 送信する色を指定
  uint8_t mode2;      // 遅延時間を指定
} struct_message;

/* Global variables */
struct_message myData;                                         // データ
uint8_t main2Mac[] = MAIN2_MAC;                                // main2のMACアドレス
uint16_t lastSeqNo = 0;                                        // 最終シーケンス番号

// Wi-Fi
const char* ssid = WiFi_SSID;
const char* password = WiFi_PASSWORD;

// IPAddress
IPAddress ip(IP_ADDRESS_IP);
IPAddress gateway(IP_ADDRESS_GATEWAY);
IPAddress subnet(IP_ADDRESS_SUBNET);

// OSC
const uint16_t port = OSC_PORT;

/* ESP-NOWでデータを再送 */
void retry() {
  myData.retry++;
  Serial.print("retry: "); Serial.println(myData.retry); // ログ出力
  delayMicroseconds(16383);
  esp_now_send(main2Mac, (uint8_t *) &myData, sizeof(myData)); // ESP-NOWでデータを送信
}

/* データ送信時のコールバック関数 */
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {

  if (sendStatus == 0) {
    Serial.println("[ESP-NOW] Delivery success");
  } else {
    Serial.println("[ESP-NOW] Delivery fail");
    if (myData.retry < 10) retry(); // リトライ数が10以下の場合はデータを再送
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
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);                   // 自分の役割を設定  
  esp_now_register_send_cb(OnDataSent);                             // 送信完了時のイベントを登録
  esp_now_add_peer(main2Mac, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);       // デバイスを登録
}

/* OSCのSetup */
void setupOSC() {

  // OSC受信
  OscWiFi.subscribe(port, "/of/esp",
    [](const OscMessage& m) {
      size_t i;
      size_t numberLength;

      // 送信情報
      myData.retry = 0; // retry初期化
      myData.seqNo = ++lastSeqNo; // シーケンス番号
      Serial.print("address: "); Serial.println(m.address()); // addressログ出力
      Serial.print("seqNo: "); Serial.println(myData.seqNo); // シーケンス番号ログ出力

      // 送信するデータを設定
      // mode1
      myData.mode1 = m.arg<int>(0); 
      Serial.print("mode1: "); Serial.println(m.arg<int>(0)); // ログ出力

      // number
      numberLength = m.arg<int>(1);
      memset(myData.number, 0, sizeof(myData.number));               // myData.number配列を初期化
      Serial.print("numberLength: "); Serial.println(m.arg<int>(1)); // ログ出力
      Serial.print("number:");

      for (i = 0; i < numberLength; i++) {
        myData.number[i] =  m.arg<int>(2 + i);
        Serial.print(" "); Serial.print(m.arg<int>(2 + i)); // ログ出力
      }

      // color    
      Serial.print("\ncolor:");
  
      for (i = 0; i < 3; i++) {
        myData.color[i] =  m.arg<int>(2 + numberLength + i);
        Serial.print(" "); Serial.print(m.arg<int>(2 + numberLength + i)); // ログ出力
      }

      // mode2
      myData.mode2 = m.arg<int>(5 + numberLength);
      Serial.print("\nmode2: "); Serial.println(m.arg<int>(5 + numberLength)); // ログ出力

      // ESP-NOWでデータを送信
      esp_now_send(main2Mac, (uint8_t *) &myData, sizeof(myData));
    }
  );
}

void setup() {

  // データ転送レートを指定
  Serial.begin(115200);

  // Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  WiFi.config(ip, gateway, subnet);
  Serial.print("\n[setup] connecting");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  
  Serial.print("\n[setup] connected Wi-Fi: ");
  Serial.println(WiFi.localIP());             // IPアドレスを表示
  Serial.print("[setup] MAC Address: ");
  Serial.println(WiFi.macAddress());          // MACアドレスを表示

  setupOSC();    // OSCのSetup
  setupEspNow(); // ESP-NOWのSetup
}

void loop() {

  // 自動的にOSCを送受信
  OscWiFi.update();
}
