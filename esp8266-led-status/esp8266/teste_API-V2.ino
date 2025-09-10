#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const char* ssid = "SEU-WIFI";
const char* password = "SUA-SENHA";

// IP do seu servidor (Docker host) — use IP da máquina que roda o Docker
const char* serverName = "http://192.168.0.118:8080/connect.php";  // Atualize o IP!

void setup() {
  Serial.begin(115200);
  delay(1000);

  WiFi.begin(ssid, password);
  Serial.print("Conectando ao WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Conectado!");
  Serial.println("Endereço IP: ");
  Serial.println(WiFi.localIP());

  sendLEDStatus("ON");  // Altere para "OFF" se quiser testar outro valor
}

void loop() {
  // Seu código principal pode chamar sendLEDStatus("ON"/"OFF") conforme necessário
}

// Função para enviar o status do LED
void sendLEDStatus(const String& status) {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;

    http.begin(client, serverName);  // Forma atualizada!
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    String httpRequestData = "ledStatus=" + status;
    int httpResponseCode = http.POST(httpRequestData);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.print("Código de resposta: ");
      Serial.println(httpResponseCode);
      Serial.print("Resposta: ");
      Serial.println(response);
    } else {
      Serial.print("Erro no envio. Código: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  } else {
    Serial.println("WiFi desconectado!");
  }
}