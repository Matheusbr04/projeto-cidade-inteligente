# 💡 Automação de Iluminação Inteligente com ESP32 e InfluxDB

Este projeto demonstra como usar um microcontrolador **ESP32** para criar um **sistema de iluminação inteligente** que controla uma lâmpada halógena de 60 W em dois modos (Meia Luz e Luz Cheia) com base na **luminosidade ambiente** e na **detecção de movimento**.  
O consumo de energia em cada modo é registrado em tempo real em um banco de dados **InfluxDB**.

---

## 🚀 Funcionalidades

- **Controle Inteligente:** Desliga a lâmpada durante o dia (quando há luz ambiente suficiente).  
- **Modo Meia Luz (30 W):** Ativado automaticamente à noite quando **não há movimento**.  
- **Modo Luz Cheia (60 W):** Ativado à noite quando o **sensor de presença detecta movimento**.  
- **Monitoramento de Energia:** Envia o consumo de energia (**0 W**, **30 W** ou **60 W**) para o **InfluxDB** a cada 5 segundos.  
- **Plataforma:** Projetado para **ESP32** (compatível com ESP8266 com pequenos ajustes).

---

## 🛠️ Requisitos de Hardware

| Componente | Quantidade | Observações |
|-------------|-------------|--------------|
| **ESP32** | 1 | Microcontrolador principal |
| **Módulo Relé 2 Canais** | 1 | Deve suportar a tensão da lâmpada (220 V) |
| **LDR (Resistor Dependente de Luz)** | 1 | Para leitura da luminosidade ambiente |
| **Resistor 10 kΩ** | 1 | Para o divisor de tensão do LDR |
| **Sensor de Presença (PIR)** | 1 | Sensor com saída digital/relé |
| **Lâmpada Halógena 60 W (220 V)** | 1 | Lâmpada a ser controlada |
| **Componente Limitador** | 1 | Necessário para criar o modo Meia Luz (ex: resistor de alta potência, reator, ou segunda lâmpada em série) |

---

## 📌 Configuração de Hardware e Pinos

| Função | Pino ESP32 | Tipo | Observações |
|--------|-------------|------|--------------|
| **Luz Cheia** | GPIO 27 | OUTPUT | Relé 1 — bypass do limitador para 60 W |
| **Meia Luz** | GPIO 26 | OUTPUT | Relé 2 — liga a lâmpada através do limitador (30 W) |
| **LDR** | GPIO 32 | ADC Input | Conectado em um divisor de tensão (3.3 V → LDR → R 10 k → GND) |
| **Sensor PIR** | GPIO 34 | Input | Recebe sinal HIGH na detecção de movimento |

---

## 💻 Configuração do Software (Arduino IDE)

### 1. Instalação de Bibliotecas

Instale as bibliotecas abaixo pelo **Gerenciador de Bibliotecas**:

- `WiFiMulti` *(inclusa no core do ESP32/ESP8266)*
- `InfluxDbClient` *(por InfluxData)*
- `InfluxDbCloud` *(por InfluxData)*

---

### 2. Configuração de Credenciais

Antes de carregar o código, substitua as constantes pelas suas informações reais:

| Constante | Exemplo Atual | Descrição |
|------------|----------------|------------|
| `#define WIFI_SSID` | `"YOUR_WIFI_SSID"` | Nome da rede Wi-Fi |
| `#define WIFI_PASSWORD` | `"YOUR_WIFI_PASSWORD"` | Senha da rede Wi-Fi |
| `#define INFLUXDB_URL` | `"http://SEU_IP_AQUI:8086"` | URL do servidor InfluxDB |
| `#define INFLUXDB_TOKEN` | `"SEU_TOKEN_AQUI"` | Token de Escrita no InfluxDB |
| `#define INFLUXDB_ORG` | `"SUA_ORG_AQUI"` | Organização (ORG) no InfluxDB |
| `#define INFLUXDB_BUCKET` | `"SEU_BUCKET_AQUI"` | Nome do Bucket no InfluxDB |

---

### 3. Ajuste de Limiares (Thresholds)

Os valores que definem o que é “claro” ou “escuro” dependem da sua montagem do LDR.

```cpp
#define CLEAR_THRESHOLD 2000 // Se LDR > 2000, está claro e a lâmpada é desligada
```

> **Dica:**  
> Carregue o código, abra o *Monitor Serial* e observe os valores do LDR em diferentes condições de luz.  
> Ajuste o valor de `CLEAR_THRESHOLD` até encontrar o ponto ideal de transição.

---

## 📄 Código (main.ino)

O código completo do projeto está abaixo:

```cpp
#if defined(ESP32)
  #include <WiFiMulti.h>
  WiFiMulti wifiMulti;
  #define DEVICE "ESP32"
#elif defined(ESP8266)
  #include <ESP8266WiFiMulti.h>
  ESP8266WiFiMulti wifiMulti;
  #define DEVICE "ESP8266"
#endif

#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>

// ----------------------------------------------------------------------
// 1. DEFINIÇÕES GLOBAIS - ATUALIZE ESTES DADOS!
// ----------------------------------------------------------------------

#define WIFI_SSID "YOUR_WIFI_SSID" 
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"

#define INFLUXDB_URL "http://YOUR_INFLUXDB_IP:8086"
#define INFLUXDB_TOKEN "YOUR_INFLUXDB_TOKEN"
#define INFLUXDB_ORG "YOUR_INFLUXDB_ORG"
#define INFLUXDB_BUCKET "YOUR_BUCKET_NAME"

#define TZ_INFO "UTC-3" // Fuso horário

#define RELAY_FULL_LIGHT 27
#define RELAY_HALF_LIGHT 26
#define LDR_PIN          32
#define PIR_BUTTON_PIN   34

#define CLEAR_THRESHOLD  2000
#define POWER_FULL_W     60.0
#define POWER_HALF_W     30.0

#define RELAY_ON         LOW
#define RELAY_OFF        HIGH

InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);

// ----------------------------------------------------------------------
// 3. SETUP
// ----------------------------------------------------------------------

void setup() {
  Serial.begin(115200);
  
  pinMode(RELAY_FULL_LIGHT, OUTPUT);
  pinMode(RELAY_HALF_LIGHT, OUTPUT);
  digitalWrite(RELAY_FULL_LIGHT, RELAY_OFF);
  digitalWrite(RELAY_HALF_LIGHT, RELAY_OFF);

  pinMode(PIR_BUTTON_PIN, INPUT);

  WiFi.mode(WIFI_STA);
  wifiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting to WiFi");
  while (wifiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nWiFi Connected.");

  timeSync(TZ_INFO, "pool.ntp.org", "time.nis.gov");

  if (client.validateConnection()) {
    Serial.print("Connected to InfluxDB: ");
    Serial.println(client.getServerUrl());
  } else {
    Serial.print("InfluxDB connection failed: ");
    Serial.println(client.getLastErrorMessage());
  }
}

// ----------------------------------------------------------------------
// 4. LOOP - LÓGICA DO PROJETO
// ----------------------------------------------------------------------

void loop() {
  if (wifiMulti.run() != WL_CONNECTED) {
    Serial.println("WiFi connection lost. Reconnecting...");
    delay(5000);
    return;
  }

  int ldrValue = analogRead(LDR_PIN);
  bool motionDetected = (digitalRead(PIR_BUTTON_PIN) == HIGH);

  float currentPower = 0.0;
  String lightMode = "OFF";

  if (ldrValue > CLEAR_THRESHOLD) {
    lightMode = "OFF";
    digitalWrite(RELAY_FULL_LIGHT, RELAY_OFF);
    digitalWrite(RELAY_HALF_LIGHT, RELAY_OFF);
  } else {
    if (motionDetected) {
      lightMode = "FULL";
      digitalWrite(RELAY_FULL_LIGHT, RELAY_ON);
      digitalWrite(RELAY_HALF_LIGHT, RELAY_OFF);
      currentPower = POWER_FULL_W;
    } else {
      lightMode = "HALF";
      digitalWrite(RELAY_FULL_LIGHT, RELAY_OFF);
      digitalWrite(RELAY_HALF_LIGHT, RELAY_ON);
      currentPower = POWER_HALF_W;
    }
  }

  if (lightMode != "OFF") {
    Point powerData("power_consumption");
    powerData.addTag("device", DEVICE);
    powerData.addTag("mode", lightMode);
    powerData.addField("power_watts", currentPower);

    if (!client.writePoint(powerData)) {
      Serial.print("InfluxDB power data write failed: ");
      Serial.println(client.getLastErrorMessage());
    }
  }

  Point ldrData("ambient_light");
  ldrData.addTag("device", DEVICE);
  ldrData.addField("ldr_raw", ldrValue);
  client.writePoint(ldrData);

  Serial.printf("LDR: %d | Motion: %s | Light Mode: %s | Power: %.1fW\n", 
                ldrValue, motionDetected ? "YES" : "NO", lightMode.c_str(), currentPower);

  delay(5000); 
}
```

---

## 👨‍💻 Autor

**Matheus Santos**
- 📧 Email: matheuseom04@gmail.com
- 🐙 GitHub: [@Matheusbr04](https://github.com/Matheusbr04)
- 💼 LinkedIn: [Matheus Santos](https://linkedin.com/in/matheus-santos-228582315)

---

## 📊 Visualização no InfluxDB / Grafana

Você pode criar dashboards com:
- **Gráfico de potência (W)**
- **Status de operação (OFF / HALF / FULL)**
- **Luminosidade (LDR raw)**

Essas métricas permitem acompanhar o **comportamento automático da iluminação** e **otimizar o consumo de energia**.

---

## 🧩 Possíveis Extensões

- Adicionar **MQTT** para integração com Home Assistant.  
- Implementar **controle manual remoto** via painel web.  
- Enviar notificações quando o consumo ultrapassar limites definidos.

---

