💡 ESP8266 com LDR e Envio de Status via PHP (v4)

Na versão 4 deste projeto, o ESP8266 usa um sensor LDR para detectar quando o ambiente está escuro. Quando isso acontece, ele:

- **Liga o LED**
- **Envia uma mensagem para um servidor local com XAMPP** (PHP + MySQL)

---

## ✅ Requisitos

### 🔧 Hardware
- ESP8266 (NodeMCU, Wemos D1, etc.)
- LDR (Sensor de luminosidade)
- Resistor de 10kΩ para o divisor de tensão do LDR
- LED + resistor (ex: 220Ω)
- Cabos jumper

### 🧰 Software
- [XAMPP](https://www.apachefriends.org/)
- Apache e MySQL ativos
- Acesso ao `http://localhost/phpmyadmin`
- Arduino IDE com suporte ao ESP8266

---

## 🔌 Esquema de Ligações

| Componente | Pino ESP8266 |
|------------|--------------|
| LDR (com resistor) | A0 |
| LED (com resistor) | D1 (GPIO5) |

> O LDR deve estar em um **divisor de tensão** com um resistor de 10kΩ entre o pino A0 e o GND.

---

## 🧠 Lógica do Projeto

1. O ESP8266 lê o valor do LDR (luminosidade).
2. Se o valor for **abaixo de 400**, considera "escuro":
   - Liga o LED.
   - Envia uma única mensagem HTTP GET para o servidor PHP.
3. Se estiver claro, desliga o LED e reseta o estado.

---

## 📋 Banco de Dados MySQL

Acesse [http://localhost/phpmyadmin](http://localhost/phpmyadmin) e crie o banco:

```sql
CREATE DATABASE esp_test;

USE esp_test;

CREATE TABLE mensagens (
    id INT AUTO_INCREMENT PRIMARY KEY,
    mensagem TEXT,
    data_hora TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);
📝 Script PHP (salvar_mensagem.php)
Salve esse arquivo em:

makefile
Copiar código
C:\xampp\htdocs\salvar_mensagem.php
php
Copiar código
<?php
$servername = "localhost";
$username = "root";
$password = "";
$dbname = "esp_test";

$mensagem = $_GET['mensagem'] ?? 'Sem mensagem';

$conn = new mysqli($servername, $username, $password, $dbname);

if ($conn->connect_error) {
    die("Falha na conexão: " . $conn->connect_error);
}

$sql = "INSERT INTO mensagens (mensagem) VALUES ('$mensagem')";

if ($conn->query($sql) === TRUE) {
    echo "Mensagem salva com sucesso";
} else {
    echo "Erro: " . $conn->error;
}

$conn->close();
?>
📡 Código do ESP8266
cpp
Copiar código
#include <ESP8266WiFi.h>

// === CONFIGURAÇÃO DO WIFI ===
const char* ssid = "SEU-WIFI";
const char* password = "SUA-SENHA";
const char* host = "192.168.0.100"; // IP do PC com XAMPP

// === CONFIGURAÇÃO DO LDR E LED ===
const int ldrPin = A0;       // LDR no pino analógico
const int ledPin = D1;       // LED no pino digital D1 (GPIO5)
const int darkThreshold = 400; // Abaixo disso é considerado escuro

bool ledLigado = false;      // Evita envios repetidos

void setup() {
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH); // LED apagado inicialmente

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
  int lightLevel = analogRead(ldrPin);
  Serial.print("Nível de luz: ");
  Serial.println(lightLevel);

  if (lightLevel < darkThreshold) {
    digitalWrite(ledPin, LOW); // Liga o LED
    if (!ledLigado) {
      ledLigado = true;
      enviarMensagem();
    }
  } else {
    digitalWrite(ledPin, HIGH); // Desliga o LED
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
🔍 Testar Manualmente (sem ESP)
Abra no navegador:

perl
Copiar código
http://localhost/salvar_mensagem.php?mensagem=Teste%20manual