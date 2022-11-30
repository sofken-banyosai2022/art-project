#include <ESP8266WiFi.h>
#include <espnow.h>
#include <ArduinoOSCWiFi.h>

/* データの構造 */
typedef struct struct_message {
  int mode1;      // 送信モードを指定
  int number[10]; // 送信するユニットを指定
  int color[3];   // 送信する色を指定
} struct_message;

/* Global variables */
struct_message myData;                                          // データ
uint8_t broadcast_mac[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; // ブロードキャスト用MACアドレス
uint8_t main2_mac[] = MAIN2_MAC;          // main2のMACアドレス

// Wi-Fi
const char* ssid = WiFi_SSID;
const char* password = WiFi_PASSWORD;

// IPAddress
IPAddress ip(IP_ADDRESS_IP);
IPAddress gateway(IP_ADDRESS_GATEWAY);
IPAddress subnet(IP_ADDRESS_SUBNET);

// OSC
const int port = OSC_PORT;

/* データ送信時のコールバック関数 */
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {

  if (sendStatus == 0) {
    Serial.println("[ESP-NOW] Delivery success");
  } else{
    Serial.println("[ESP-NOW] Delivery fail");
  }
}

/* ESP-NOWのSetup */
void setupEspNow() {

  // ESP-NOWを初期化
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // ESP-NOWの設定
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);                   // 自分の役割を設定  
  esp_now_register_send_cb(OnDataSent);                             // 送信完了時のイベントを登録
  esp_now_add_peer(main2_mac, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);      // デバイスを登録
}

/* OSCのSetup */
void setupOSC() {

  // OSC受信
  OscWiFi.subscribe(port, "/of/esp",
    [](const OscMessage& m) {
      size_t i;
      size_t numberLength;

      // address
      Serial.print("address: "); Serial.println(m.address()); // ログ出力

      // 送信するデータを設定
      // mode1
      myData.mode1 = m.arg<int>(0); 
      Serial.print("mode1: "); Serial.println(m.arg<int>(0)); // ログ出力

      // number
      numberLength = m.arg<int>(1);
      memset(myData.number, 0, sizeof(myData.number)); // myData.number配列を初期化
      Serial.print("numberLength: "); Serial.println(m.arg<int>(1)); // ログ出力

      for (i = 0; i < numberLength; i++) {
        myData.number[i] =  m.arg<int>(2 + i);
        Serial.print("number: "); Serial.println(m.arg<int>(2 + i)); // ログ出力
      }

      // color      
      for (i = 0; i < 3; i++) {
        myData.color[i] =  m.arg<int>(2 + numberLength + i);
        Serial.print("color: "); Serial.println(m.arg<int>(2 + numberLength + i)); // ログ出力
      }

      // ESP-NOWでデータを送信
      esp_now_send(main2_mac, (uint8_t *) &myData, sizeof(myData));
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
