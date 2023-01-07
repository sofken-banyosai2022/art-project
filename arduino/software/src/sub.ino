#include <ESP8266WiFi.h>
#include <espnow.h>

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
struct_message myData;                  // データ
const uint8_t unitNumber = UNIT_NUMBER; // ユニット番号
const uint8_t ledPin[3] = {12, 14, 13}; // LED Pin番号 (赤,緑,青)
uint8_t ledData[3] = {0, 0, 0};         // 現在のLED出力(0-255) (赤,緑,青)
uint8_t nextMac[] = NEXT_MAC;           // 次のユニット番号のMACアドレス
uint16_t lastSeqNo = 0;                 // 最終シーケンス番号
bool fDelayData = false;                // 関数delayDataの実行フラグ

/* ESP-NOWのSetup */
void setupEspNow() {

  // ESP-NOWを初期化
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    ESP.restart(); // リスタート
    return;
  }

  // ESP-NOWの設定
  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);                 // 自分の役割を設定
  esp_now_add_peer(nextMac, ESP_NOW_ROLE_COMBO, 1, NULL, 0); // デバイスを登録
  esp_now_register_send_cb(OnDataSent);                      // 送信完了時のイベントを登録
  esp_now_register_recv_cb(OnDataRecv);                      // 受信完了時のイベントを登録
}

/*　データ送信 */
void sendData(size_t numberLength) {

  // 自分宛てか判別
  for (size_t i = 0; i < numberLength; i++) {
    if (myData.number[i] == unitNumber || myData.number[i] == 200) { // ユニット番号, ブロードキャスト
      ledOn(myData.mode1, myData.color);  // LEDオン
      break;
    } else if (myData.number[i] == 100 || myData.number[i] == 101) { // デイジーチェーン
      fDelayData = true;                  // 指定秒数だけ遅延
      ledOn(myData.mode1, myData.color);  // LEDオン
    }
  }
}

/* ESP-NOWでデータを再送 */
void retry() {
  size_t numberLength;

  myData.retry++;
  Serial.print("retry: "); Serial.println(myData.retry); // ログ出力
  delayMicroseconds(16383);

  // number
  for (size_t i = 1; i < 10; i++) {
    if (!myData.number[i]) {
      numberLength = i;
      break;
    }
  }

  sendData(numberLength); // データ送信
}

/* データ送信時のコールバック関数 */
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {

  if (sendStatus == 0) {
    Serial.println("[ESP-NOW] Delivery success");
  } else{
    Serial.println("[ESP-NOW] Delivery fail");
    if (myData.retry < 10) retry(); // リトライ数が10以下の場合はデータを再送
  }
}

/* データ受信時のコールバック関数 */
void OnDataRecv(uint8_t *mac, uint8_t *incomingData, uint8_t len) {
  size_t i;
  size_t numberLength;

  // 送信情報
  memcpy(&myData, incomingData, sizeof(myData));

  if (lastSeqNo == myData.seqNo) return; // 前回と同じシーケンス番号の場合は処理を終了

  myData.retry = 0; // retry初期化
  lastSeqNo = myData.seqNo; // 最終シーケンス番号を更新
  Serial.print("seqNo: "); Serial.println(myData.seqNo); // シーケンス番号ログ出力

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

  sendData(numberLength); // データ送信
}

/* 指定秒数だけ遅延 */
void delayData() {
  delay(myData.mode2 * 10); // 遅延

  if (myData.number[i] == 100) {
    esp_now_send(nextMac, (uint8_t *) &myData, sizeof(myData)); // ESP-NOWでデータを送信
  } else if (myData.number[i] == 101) {
    ROUTE_101_ESP_NOW_SEND
  }

  fDelayData = false;       // フラグを下ろす
}

/* LEDオン */
void ledOn(uint8_t mode1, uint8_t color[3]) {
  if (mode1 == 1) { // 通常
    analogWrite(ledPin[0], color[0]);
    analogWrite(ledPin[1], color[1]);
    analogWrite(ledPin[2], color[2]);
  } else if (mode1 == 2) { // LED演出：フェードイン・アウト
    fade(color, 1000); 
  } else if (mode1 == 3) { // LED演出：ランダム
    analogWrite(ledPin[0], random(10, 246));
    analogWrite(ledPin[1], random(10, 246));
    analogWrite(ledPin[2], random(10, 246));
  }
}

/* LED演出：フェードイン・アウト */
void fade(uint8_t color[3], uint16_t delayTime) {
  float diff[3];
  float out[3] = {(float)ledData[0], (float)ledData[1], (float)ledData[2]};
  uint8_t step = 100;

  diff[0] = (float)(ledData[0] - color[0]) / step; // 現在値と次の値との差をstepで割った数 (R)
  diff[1] = (float)(ledData[1] - color[1]) / step; // 現在値と次の値との差をstepで割った数 (G)
  diff[2] = (float)(ledData[2] - color[2]) / step; // 現在値と次の値との差をstepで割った数 (B)

  // 各色の値を変更
  for (size_t diffNum = 0; diffNum < step; diffNum++) {
    analogWrite(ledPin[0], (uint8_t)(out[0] -= diff[0]));
    analogWrite(ledPin[1], (uint8_t)(out[1] -= diff[1]));
    analogWrite(ledPin[2], (uint8_t)(out[2] -= diff[2]));
    delayMicroseconds(delayTime);
  }

  ledData[0] = color[0]; // 現在値を保存 (R)
  ledData[1] = color[1]; // 現在値を保存 (G)
  ledData[2] = color[2]; // 現在値を保存 (B)
}

void setup() {

  // データ転送レートを指定
  Serial.begin(115200);

  // Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.disconnect(); // Wi-Fi切断

  // LED
  randomSeed(analogRead(0)); // ランダム初期化
  analogWriteRange(255);
  pinMode(ledPin[0], OUTPUT);
  pinMode(ledPin[1], OUTPUT);
  pinMode(ledPin[2], OUTPUT);

  // MACアドレスを表示
  Serial.print("\n[setup] MAC Address: ");
  Serial.println(WiFi.macAddress());

  setupEspNow(); // ESP-NOWのSetup
}

void loop() {

  // フラグが立っているか確認
  if (fDelayData) delayData();
}
