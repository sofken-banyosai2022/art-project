/**
   ArduinoOSC
   https://github.com/hideakitai/ArduinoOSC
*/

#define ARDUINOOSC_DEBUGLOG_ENABLE // Debug Loggerを有効
#include <ArduinoOSCWiFi.h>

/* Global variables */
const char* ssid = "*****";
const char* password = "*****";

// IPAddress
IPAddress ip(***, ***, *, *);
IPAddress gateway(***, ***, *, *);
IPAddress subnet(***, ***, ***, *);

// ArduinoOSC
const char* host = "***.***.*.*";
const int port = *****;

int i;
float f;
String s;

void setup() {

  // Wi-Fi
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  WiFi.config(ip, gateway, subnet);
  Serial.print("\n[setup] connecting");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  
  Serial.print("\n[setup] connected Wi-Fi:");
  Serial.println(WiFi.localIP());

  // OSC受信
  //  OscWiFi.subscribe(port, "/lambda/msg",
  //    [](const OscMessage& m) {
  //      Serial.print(m.remoteIP()); Serial.print(" ");
  //      Serial.print(m.remotePort()); Serial.print(" ");
  //      Serial.print(m.size()); Serial.print(" ");
  //      Serial.print(m.address()); Serial.print(" ");
  //      Serial.print(m.arg<int>(0)); Serial.print(" ");
  //      Serial.print(m.arg<float>(1)); Serial.print(" ");
  //      Serial.print(m.arg<String>(2)); Serial.println();
  //    }
  //  );

  // 毎秒30回でOSC送信
  // OscWiFi.publish(host, port, "/publish/value", 1);

  // 毎秒1回でOSC送信
  // OscWiFi.publish(host, port, "/publish/value", 1)
  //   ->setFrameRate(1);

  // 1回だけOSC送信
  OscWiFi.send(host, port, "/publish/value", 1);
  Serial.println("[OSC] send data");
}

void loop() {
    OscWiFi.update(); // 自動的にOSCを送受信
}
