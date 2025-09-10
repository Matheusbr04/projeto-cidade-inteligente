#include <ESP8266WiFi.h>

// === CONFIGURAÇÃO DO WIFI ===
const char* ssid = "SEU-WIFI";
const char* password = "SUA-SENHA";
const char* host = "192.168.0.100"; // IP do servidor local com XAMPP

// === CONFIGURAÇÃO DO LDR E LED ===
const int ldrPin = A0;        // LDR conectado à entrada analógica A0
const int ledPin = D1;        // LED conectado à porta digital D1 (GPIO5)
const int darkThreshold = 400; // Valor abaixo do qual consideramos "escuro"

bool ledLigado = false;       // Estado anterior do LED (para evitar envios repetidos)

void setup() {
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH);  // Garante LED apagado no início

  Serial.begin(115200);
  delay(1000);

  // Conectar ao Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Conectando ao Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConectado ao Wi-Fi");
}

void loop() {
  int lightLevel = analogRead(ldrPin);
  Serial.print("Nível de luz: ");
  Serial.println(lightLevel);

  if (lightLevel < darkThreshold) {
    // Está escuro
    digitalWrite(ledPin, LOW);

    // Se o LED ainda não estava ligado, envia uma vez
    if (!ledLigado) {
      ledLigado = true;
      enviarMensagem();
    }
  } else {
    // Está claro
    digitalWrite(ledPin, HIGH);
    ledLigado = false;
  }

  delay(500);
}

void enviarMensagem() {
  WiFiClient client;

  if (client.connect(host, 80)) {
    String url = "/salvar_mensagem.php?mensagem=LED%20ligado";

    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n\r\n");

    Serial.println("Mensagem enviada: LED ligado");
  } else {
    Serial.println("Falha na conexão com o servidor");
  }
}
