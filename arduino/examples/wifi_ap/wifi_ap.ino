#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

/* Global variables */
const char *ssid = "*****";
const char *password = "*****";

ESP8266WebServer server(80);

void handleRoot() {

  /* http://192.168.4.1 */
  server.send(200, "text/html", "<h1>You are connected</h1>");
}

void setup() {
  Serial.begin(115200);

  // Wi-Fi
  Serial.print("\nConfiguring access point...");

  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();

  Serial.print("AP IP address: ");
  Serial.println(myIP);

  server.on("/", handleRoot);
  server.begin();

  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}
