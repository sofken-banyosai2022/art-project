#include <ESP8266WiFi.h>
#include <espnow.h>
#include <ArduinoOSCWiFi.h>

/* データの構造 */
typedef struct struct_message {
  int mode1;
} struct_message;

/* Global variables */
struct_message myData;                                          // データ
uint8_t broadcast_mac[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; // ブロードキャスト用MACアドレス

// Wi-Fi
const char* ssid = "*****";
const char* password = "*****";

// IPAddress
IPAddress ip(***, ***, *, *);
IPAddress gateway(***, ***, *, *);
IPAddress subnet(***, ***, ***, *);

// OSC
const char* host = "***.***.*.*";
const int port = *****;

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
  esp_now_add_peer(broadcast_mac, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);  // デバイスを登録
}

/* OSCのSetup */
void setupOSC() {

  // OSC受信
  OscWiFi.subscribe(port, "/lambda/msg",
    [](const OscMessage& m) {
      Serial.print(m.address()); Serial.print(" ");
      Serial.println(m.arg<int>(0));

      // 送信するデータを設定
      myData.mode1 = m.arg<int>(0);

      // ESP-NOWでデータを送信
      esp_now_send(broadcast_mac, (uint8_t *) &myData, sizeof(myData));
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
