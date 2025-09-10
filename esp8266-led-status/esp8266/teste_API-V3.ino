#include <ESP8266WiFi.h>

const char* ssid = "SEU_SSID";
const char* password = "SUA_SENHA_WIFI";
const char* host = "192.168.1.100"; // IP do seu PC com XAMPP

void setup() {
  Serial.begin(115200);
  delay(1000);

  WiFi.begin(ssid, password);
  Serial.print("Conectando ao Wi-Fi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConectado ao Wi-Fi");
}

void loop() {
  WiFiClient client;

  if (client.connect(host, 80)) {
    String url = "/salvar_mensagem.php?mensagem=Hello%20World";

    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n\r\n");

    Serial.println("Mensagem enviada: Hello World");
  } else {
    Serial.println("Falha na conexão com o servidor");
  }

  delay(10000); // Espera 10 segundos
}
