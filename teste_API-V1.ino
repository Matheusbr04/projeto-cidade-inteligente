#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const char* ssid = "SEU-WIFI";  // Seu SSID
const char* password = "SUA-SENHA";  // Sua senha
const char* serverName = "http://192.168.0.100:8080/connect.php";  // IP do servidor onde está o PHP

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverName);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    String httpRequestData = "ledStatus=ON";  // Exemplo de dado que o ESP vai enviar
    int httpResponseCode = http.POST(httpRequestData);

    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);

    http.end();
  }
}

void loop() {
}