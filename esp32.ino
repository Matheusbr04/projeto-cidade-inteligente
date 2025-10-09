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

// WiFi AP SSID
#define WIFI_SSID "YOUR_WIFI_SSID" 
// WiFi password
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"

// InfluxDB (ATUALIZE COM SEUS VALORES REAIS)
#define INFLUXDB_URL "http://YOUR_INFLUXDB_IP:8086" // Exemplo: "http://192.168.1.10:8086"
#define INFLUXDB_TOKEN "YOUR_INFLUXDB_TOKEN"
#define INFLUXDB_ORG "YOUR_INFLUXDB_ORG"
#define INFLUXDB_BUCKET "YOUR_BUCKET_NAME"

// Time zone info
#define TZ_INFO "UTC-3" // Ajuste conforme seu fuso horário

// PIN DEFINITIONS
#define RELAY_FULL_LIGHT 27 // Pino para o modo CHEIO (Relé 1)
#define RELAY_HALF_LIGHT 26 // Pino para o modo MEIA LUZ (Relé 2)
#define LDR_PIN          32 // Pino ADC para o LDR
#define PIR_BUTTON_PIN   34 // Pino Digital para o Sensor de Presença

// LDR THRESHOLDS (Ajuste esses valores de 0 a 4095 conforme seu ambiente)
#define CLEAR_THRESHOLD  2000 // Se LDR > 2000, está CLARO (desliga a lâmpada)

// POWER CONSUMPTION (Valores para a lâmpada Halógena 60W 220V)
#define POWER_FULL_W     60.0 // Consumo em Watts (Modo Cheio)
#define POWER_HALF_W     30.0 // Consumo estimado em Watts (Modo Meia Luz - 50%)

// RELAY LOGIC: A maioria dos módulos de relé são "Active LOW"
#define RELAY_ON         LOW
#define RELAY_OFF        HIGH

// ----------------------------------------------------------------------
// 2. INFLUXDB CLIENT
// ----------------------------------------------------------------------

InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);

// ----------------------------------------------------------------------
// 3. SETUP
// ----------------------------------------------------------------------

void setup() {
  Serial.begin(115200);
  
  // Setup GPIOs para os Relés (Saída)
  pinMode(RELAY_FULL_LIGHT, OUTPUT);
  pinMode(RELAY_HALF_LIGHT, OUTPUT);
  digitalWrite(RELAY_FULL_LIGHT, RELAY_OFF);
  digitalWrite(RELAY_HALF_LIGHT, RELAY_OFF);

  // Setup GPIO para o Sensor PIR (Entrada)
  pinMode(PIR_BUTTON_PIN, INPUT);

  // Setup wifi
  WiFi.mode(WIFI_STA);
  wifiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting to wifi");
  while (wifiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nWiFi Connected.");

  // Accurate time is necessary for InfluxDB
  timeSync(TZ_INFO, "pool.ntp.org", "time.nis.gov");

  // Check server connection
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
  // 4.1. Garante a Conexão Wi-Fi
  if (wifiMulti.run() != WL_CONNECTED) {
    Serial.println("WiFi connection lost. Reconnecting...");
    delay(5000);
    return;
  }

  // 4.2. LEITURA DOS SENSORES
  int ldrValue = analogRead(LDR_PIN);
  bool motionDetected = (digitalRead(PIR_BUTTON_PIN) == HIGH);

  float currentPower = 0.0;
  String lightMode = "OFF";

  // 4.3. LÓGICA DE CONTROLE DA LUZ
  if (ldrValue > CLEAR_THRESHOLD) {
    
    // A) MUITO CLARO (Dia): Desliga tudo
    lightMode = "OFF";
    digitalWrite(RELAY_FULL_LIGHT, RELAY_OFF);
    digitalWrite(RELAY_HALF_LIGHT, RELAY_OFF);

  } else {
    
    // B) ESCURO O SUFICIENTE (Noite): Ativa algum modo
    if (motionDetected) {
      // Sensor de Presença ATIVADO: Modo Cheio
      lightMode = "FULL";
      digitalWrite(RELAY_FULL_LIGHT, RELAY_ON);
      digitalWrite(RELAY_HALF_LIGHT, RELAY_OFF);
      currentPower = POWER_FULL_W;

    } else {
      // Sensor de Presença DESATIVADO: Modo Meia Luz
      lightMode = "HALF";
      digitalWrite(RELAY_FULL_LIGHT, RELAY_OFF);
      digitalWrite(RELAY_HALF_LIGHT, RELAY_ON);
      currentPower = POWER_HALF_W;
    }
  }

  // 4.4. LOGICA DE ENVIO PARA O INFLUXDB (Somente se a lâmpada estiver ligada)
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
  
  // Envia a leitura do LDR (útil para monitorar e ajustar o threshold)
  Point ldrData("ambient_light");
  ldrData.addTag("device", DEVICE);
  ldrData.addField("ldr_raw", ldrValue);
  client.writePoint(ldrData);

  // 4.5. Monitoramento Serial e Delay
  Serial.printf("LDR: %d | Motion: %s | Light Mode: %s | Power: %.1fW\n", 
                ldrValue, motionDetected ? "YES" : "NO", lightMode.c_str(), currentPower);

  // Intervalo de checagem e log
  delay(5000); 
}