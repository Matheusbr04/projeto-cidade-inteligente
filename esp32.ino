// esp32_api.ino
// Requer Arduino core for ESP32 e ArduinoJson

#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>

const char* WIFI_SSID = "SEU-WIFI";
const char* WIFI_PASS = "SUA-SENHA";

WebServer server(80);

const int PIN_LDR = 32;
const int RELAY_PIN_26 = 26;
const int RELAY_PIN_27 = 27;

void handleSensor() {
  int raw = analogRead(PIN_LDR); // 0..4095
  StaticJsonDocument<64> doc;
  doc["ldr"] = raw;
  String out;
  serializeJson(doc, out);
  server.send(200, "application/json", out);
}

void handleRelay() {
  if (!server.hasArg("pin") || !server.hasArg("state")) {
    server.send(400, "text/plain", "Missing pin or state");
    return;
  }
  int pin = server.arg("pin").toInt();
  String state = server.arg("state");
  int outPin = -1;
  if (pin == 26) outPin = RELAY_PIN_26;
  else if (pin == 27) outPin = RELAY_PIN_27;
  else {
    server.send(400, "text/plain", "Invalid pin");
    return;
  }
  // Ajuste conforme seu módulo de relé: aqui assume-se ACTIVE LOW
  if (state == "on") digitalWrite(outPin, LOW);
  else digitalWrite(outPin, HIGH);

  String resp = String("{\"pin\":") + pin + ",\"state\":\"" + state + "\"}";
  server.send(200, "application/json", resp);
}

void setup() {
  Serial.begin(115200);
  delay(100);

  pinMode(RELAY_PIN_26, OUTPUT);
  pinMode(RELAY_PIN_27, OUTPUT);
  digitalWrite(RELAY_PIN_26, HIGH); // desligado (assumindo active LOW)
  digitalWrite(RELAY_PIN_27, HIGH);

  pinMode(PIN_LDR, INPUT);

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Conectando ao WiFi");
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 60) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.print("Conectado. IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println();
    Serial.println("Falha ao conectar ao WiFi.");
  }

  server.on("/sensor", HTTP_GET, handleSensor);
  server.on("/relay", HTTP_GET, handleRelay);

  server.begin();
  Serial.println("HTTP server iniciado");
}

void loop() {
  server.handleClient();
}