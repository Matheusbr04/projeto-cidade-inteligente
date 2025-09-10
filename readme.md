ESP8266 LED Status via Docker (v2)

Este projeto permite que um ESP8266 envie o status de um LED (ON / OFF) para um servidor web em PHP, que armazena os dados em um banco MySQL. Todo o ambiente backend pode ser executado via Docker, incluindo phpMyAdmin para gerenciar o banco visualmente.

📦 Estrutura do Projeto
esp8266-led-status/
├── docker-compose.yml       # Configuração dos containers Docker
├── sql/
│   └── init.sql             # Criação do banco e tabela
└── www/
    └── connect.php          # Script PHP que recebe os dados do ESP8266

🛠️ Requisitos

Docker e Docker Compose instalados

ESP8266 (ex: NodeMCU)

Arduino IDE com suporte ao ESP8266

🚀 Passo a Passo para Executar
1. Configurar o Docker

Na pasta do projeto (esp8266-led-status/), execute:

docker-compose up -d


Isso irá:

Subir um container MySQL com banco esp8266_db e usuário espuser

Subir um container PHP + Apache para servir o connect.php

Subir phpMyAdmin para administração visual (porta 8081)

2. Acessar Serviços

API do ESP8266: http://localhost:8080/connect.php

phpMyAdmin: http://localhost:8081

Usuário: espuser

Senha: esppass

⚠️ Se estiver rodando em outro computador na mesma rede, substitua localhost pelo IP da máquina que roda os containers.

3. Configurar o ESP8266

No arquivo ESP8266_LED_Status_Sender.ino:

const char* ssid = "SEU-WIFI";
const char* password = "SUA-SENHA";
const char* serverName = "http://192.168.0.X:8080/connect.php"; // IP da máquina Docker


Depois, faça upload para o ESP8266 via Arduino IDE.

4. Testar o Envio

O ESP8266 enviará o status do LED via HTTP POST:

POST /connect.php
Content-Type: application/x-www-form-urlencoded

ledStatus=ON


Você pode verificar os registros diretamente no phpMyAdmin ou via Serial Monitor do ESP8266.

⚙️ Estrutura do Banco de Dados

Tabela led_status:

Campo	Tipo	Detalhes
id	INT	AUTO_INCREMENT, PRIMARY KEY
status	VARCHAR(10)	ON / OFF
timestamp	DATETIME	Default CURRENT_TIMESTAMP
📝 Notas

Esta é a versão 2 do projeto.

Futuras melhorias podem incluir autenticação, integração LORA e controle bidirecional do LED.

Certifique-se de que a porta 8080 esteja liberada no firewall, caso acesse de outro dispositivo.