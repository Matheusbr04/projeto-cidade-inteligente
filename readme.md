# 📡 Projeto ESP8266 + PHP + MySQL (XAMPP)

Este projeto demonstra como enviar mensagens do ESP8266 para um banco de dados MySQL usando um servidor local (XAMPP) e PHP.

---

## 1. 📋 Criar Banco de Dados e Tabela no MySQL

1. Acesse o phpMyAdmin:
http://localhost/phpmyadmin

markdown
Copiar código

2. Crie um banco de dados chamado:
esp_test

pgsql
Copiar código

3. Execute a seguinte query SQL para criar a tabela `mensagens`:

```sql
CREATE TABLE mensagens (
    id INT AUTO_INCREMENT PRIMARY KEY,
    mensagem TEXT,
    data_hora TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);
2. 📝 Criar o Script PHP
Crie um arquivo chamado salvar_mensagem.php.

Salve o arquivo no seguinte diretório:

makefile
Copiar código
C:\xampp\htdocs\
Conteúdo do arquivo salvar_mensagem.php:

php
Copiar código
<?php
$servername = "localhost";
$username = "root";
$password = "";
$dbname = "esp_test";

// Pega o valor da URL
$mensagem = $_GET['mensagem'] ?? 'Sem mensagem';

// Conecta ao MySQL
$conn = new mysqli($servername, $username, $password, $dbname);

// Checa conexão
if ($conn->connect_error) {
    die("Falha na conexão: " . $conn->connect_error);
}

// Insere no banco
$sql = "INSERT INTO mensagens (mensagem) VALUES ('$mensagem')";

if ($conn->query($sql) === TRUE) {
    echo "Mensagem salva com sucesso";
} else {
    echo "Erro: " . $conn->error;
}

$conn->close();
?>
3. 📡 Código do ESP8266 (Arduino IDE)
Código de exemplo para envio de mensagem ao servidor:

cpp
Copiar código
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
🧪 Testar PHP manualmente
Você pode testar manualmente o script PHP pelo navegador:

perl
Copiar código
http://localhost/salvar_mensagem.php?mensagem=Hello%20World
Se tudo estiver funcionando corretamente, você verá a mensagem Mensagem salva com sucesso e o conteúdo será armazenado no banco de dados.

✅ Requisitos
XAMPP instalado e executando (Apache + MySQL)

phpMyAdmin acessível em localhost

ESP8266 (NodeMCU, por exemplo)

Arduino IDE com suporte ao ESP8266

Conexão Wi-Fi ativa

📂 Estrutura Final do Projeto
yaml
Copiar código
📁 C:\xampp\htdocs\
  └── salvar_mensagem.php

📁 Banco de Dados: esp_test
  └── Tabela: mensagens (id, mensagem, data_hora)

🔌 ESP8266
  └── Envia GET request para o script PHP a cada 10 segundos
🔒 Observações de Segurança
Este é um projeto básico para fins de aprendizado. Em produção:

Nunca insira dados diretamente no SQL sem sanitização (use prepared statements)

Implemente autenticação/autorização

Utilize HTTPS para segurança na comunicação

🚀 Resultado Esperado
O ESP8266 envia uma mensagem para o servidor PHP, que salva essa mensagem no banco de dados MySQL. A cada 10 segundos, uma nova entrada é adicionada com a mensagem "Hello World" e o timestamp atual.

go
Copiar código
