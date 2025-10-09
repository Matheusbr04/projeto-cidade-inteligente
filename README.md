# 🏠 Sistema de Iluminação Inteligente com ESP32

[![Arduino](https://img.shields.io/badge/Arduino-ESP32-blue.svg)](https://www.arduino.cc/)
[![MySQL](https://img.shields.io/badge/MySQL-8.0+-orange.svg)](https://www.mysql.com/)
[![PHP](https://img.shields.io/badge/PHP-7.4+-purple.svg)](https://www.php.net/)
[![License](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE)

Sistema completo de iluminação pública inteligente usando **ESP32**, **sensores IoT**, **MySQL** e **dashboard web** em tempo real.

## 🎯 Funcionalidades

### 🔧 Hardware
- **Detecção automática** de movimento (PIR)
- **Sensor de luminosidade** (LDR) 
- **Monitoramento de corrente** (ACS712)
- **Controle de relés** para lâmpadas
- **Sincronização NTP** para timestamps precisos

### 💡 Lógica Inteligente
- **Escuro + Presença** → Luz cheia por 20 segundos → Meia luz
- **Escuro + Sem presença** → Meia luz contínua
- **Dia** → Sistema desligado
- **Economia de energia** automática

### 📊 Monitoramento
- **Dashboard web** responsivo em tempo real
- **Gráficos** de consumo e estatísticas
- **Histórico** completo de operações
- **API REST** para integração


### Monitor Serial
```
🚀 ESP32 Iluminação Inteligente com NTP
✅ WiFi conectado! IP: 192.168.1.100  
✅ Tempo sincronizado! 📅 2024-09-21 20:27:35

===== STATUS ESP32 -> MySQL =====
🕐 Timestamp: 2024-09-21 20:27:35
🌙 LDR: 1650 (ESCURO)
👤 Presença: DETECTADA
⚡ Corrente: 0.456 A
🔋 Potência: 100.32 W
📊 Energia total: 2.456 kWh
💡 Estado: cheia
⏰ Luz cheia por mais: 15.2 s
================================

📤 Dados enviados para MySQL!
```

## 📋 Requisitos

### Hardware
- **ESP32** (qualquer modelo com WiFi)
- **Sensor PIR** (movimento)
- **LDR** + resistor 10kΩ (luminosidade)
- **ACS712** (sensor de corrente 20A)
- **2x Relés** (controle das lâmpadas)
- **Lâmpadas LED** (meia potência + potência total)

### Software
- **Arduino IDE** 1.8.19+
- **XAMPP** (Apache + MySQL + PHP)
- Navegador web moderno

### Conexões
```
ESP32          Dispositivo
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
GPIO 32    →   LDR (Analog)
GPIO 33    →   PIR (Digital)
GPIO 25    →   ACS712 (Analog)
GPIO 26    →   Relé Meia Luz
GPIO 27    →   Relé Luz Cheia
GND        →   GND Comum
3.3V       →   VCC Sensores
```

## 🛠️ Instalação

### 1. 📁 Estrutura de Arquivos
```
📁 seu-projeto/
├── 📄 esp32_codigo.ino          # Código principal ESP32
├── 📄 dashboard.html            # Interface web
├── 📄 salvar_dados.php          # API para salvar dados
├── 📄 consultar_dados.php       # API para consultar dados
├── 📄 database_schema.sql       # Estrutura do banco
└── 📄 README.md                 # Este arquivo
```

### 2. 🗄️ Configuração do Banco MySQL

1. **Instale e inicie XAMPP**
2. **Acesse phpMyAdmin:** `http://localhost/phpmyadmin`
3. **Execute o script SQL:**

```sql
CREATE DATABASE esp_test;
USE esp_test;

-- Tabela para dados de consumo
CREATE TABLE consumo_energia (
    cod_consumo_energia INT AUTO_INCREMENT PRIMARY KEY,
    consumo_energia FLOAT NOT NULL,
    data_registro TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    cod_poste INT NOT NULL,
    INDEX idx_poste_data (cod_poste, data_registro)
) ENGINE=InnoDB;

-- Tabela para operações da lâmpada
CREATE TABLE operacao_lampada (
    cod_operacao_lampada INT AUTO_INCREMENT PRIMARY KEY,
    inicio_operacao TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    fim_operacao TIMESTAMP NULL,
    cod_poste INT NOT NULL,
    estado VARCHAR(20) NOT NULL,
    INDEX idx_poste_inicio (cod_poste, inicio_operacao)
) ENGINE=InnoDB;

-- Tabela para mensagens/logs
CREATE TABLE mensagens (
    id INT AUTO_INCREMENT PRIMARY KEY,
    mensagem TEXT,
    data_hora TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    INDEX idx_data_hora (data_hora)
) ENGINE=InnoDB;
```

### 3. 🌐 Configuração Web

1. **Copie os arquivos PHP** para `C:\xampp\htdocs\`
2. **Configure a URL no código ESP32:**
```cpp
const char* serverName = "http://SEU_IP/salvar_dados.php";
```

### 4. 🔧 Upload do Código ESP32

1. **Configure WiFi no código:**
```cpp
const char* ssid     = "SEU_WIFI";
const char* password = "SUA_SENHA";
```

2. **Selecione a placa:** ESP32 Dev Module
3. **Upload** para o ESP32

## 🎮 Como Usar

### 1. 📊 Dashboard Web
```
http://localhost/dashboard.html
```
- **Cards** com status atual
- **Gráficos** de consumo em tempo real  
- **Auto-refresh** a cada 30 segundos

### 2. 🔍 API de Dados
```
http://localhost/consultar_dados.php
```
Retorna JSON com todos os dados:
```json
{
  "estadoAtual": "cheia",
  "consumoTotal": 2.456,
  "luminosidade": 1650,
  "presenca": 1,
  "historico": [...],
  "operacoes": [...],
  "status": "success"
}
```

### 3. 🧪 Teste Manual
```
http://localhost/salvar_dados.php?cod_poste=1&consumo=1.5&ldr=1800&presenca=1&estado=cheia
```

## ⚙️ Configuração Avançada

### 🕐 Timezone (Horário de Brasília)
```cpp
const long gmtOffset_sec = -3 * 3600;    // UTC-3
const char* ntpServer = "a.st1.ntp.br";  // Servidor brasileiro
```

### 🔧 Parâmetros de Sensibilidade
```cpp
#define LDR_THRESHOLD 2000        // Limite escuro/claro
#define TEMPO_LUZ_CHEIA 20000     // 20s luz cheia
#define FILTRO_RUIDO_CORRENTE 0.05 // Filtro ACS712
```

### 📡 Intervalo de Envio
```cpp
#define INTERVALO_ENVIO 5000      // 5s entre envios
```

## 📈 Monitoramento

### Dados Coletados
- ⚡ **Consumo energético** (kWh acumulado)
- 🌙 **Luminosidade ambiente** (valor ADC)
- 👤 **Detecção de presença** (0/1)
- 💡 **Estado da lâmpada** (desligado/meia/cheia)
- 🕐 **Timestamps** sincronizados via NTP

### Gráficos Disponíveis
- 📊 **Consumo por hora** (últimas 24h)
- 🍩 **Distribuição de estados** (pizza)
- 📋 **Histórico de operações** (tabela)

## 🔧 Solução de Problemas

### ❌ ESP32 não conecta WiFi
```cpp
// Verifique SSID e senha
const char* ssid = "nome_correto_wifi";
const char* password = "senha_correta";
```

### ❌ Dados não aparecem no dashboard
1. Teste a API: `http://localhost/consultar_dados.php`
2. Verifique XAMPP (Apache + MySQL rodando)
3. Confira logs do navegador (F12)

### ❌ Horário incorreto
```cpp
// Ajuste timezone conforme sua região
const long gmtOffset_sec = -3 * 3600;  // UTC-3 para Brasília
```

### ❌ Sensor não funciona
- Verifique conexões dos pinos
- Teste leituras no Serial Monitor
- Confirme alimentação 3.3V/5V

## 📊 Performance

- **📡 Latência:** ~2s (WiFi + MySQL)
- **💾 Armazenamento:** ~1MB/mês de dados
- **⚡ Consumo:** ~150mA (ESP32 + sensores)
- **📶 Range WiFi:** Até 50m em ambiente aberto

## 🛡️ Segurança

- **🔐 Validação** de dados no PHP
- **🚨 Logs de erro** automáticos  
- **🔄 Reconexão** automática WiFi
- **⏰ Timeout** em requisições HTTP

## 🤝 Contribuindo

1. **Fork** o projeto
2. **Crie** uma branch: `git checkout -b feature/nova-funcionalidade`
3. **Commit** suas mudanças: `git commit -m 'Add nova funcionalidade'`
4. **Push** para a branch: `git push origin feature/nova-funcionalidade`
5. **Abra** um Pull Request

### 💡 Ideias para Contribuir
- 📱 App mobile React Native
- 🤖 Integração com assistentes virtuais
- 🧠 Machine Learning para padrões de uso
- 📧 Sistema de alertas por email
- 🌦️ Integração com API de clima

## 📜 Changelog

### v1.0.0 (2024-09-21)
- ✅ Sistema básico funcionando
- ✅ Dashboard web responsivo
- ✅ Sincronização NTP
- ✅ API REST completa
- ✅ Documentação completa

### Próximas versões
- 📱 App mobile
- 🔔 Notificações push
- 📊 Analytics avançados
- 🌐 Multi-idiomas


## 👨‍💻 Autor

**Matheus Santos**
- 📧 Email: matheuseom04@gmail.com
- 🐙 GitHub: [@Matheusbr04](https://github.com/Matheusbr04)
- 💼 LinkedIn: [Matheus Santos](https://linkedin.com/in/matheus-santos-228582315)


### 📞 Suporte

Encontrou algum problema? 
- 🐛 [Abra uma issue](../../issues)
- 💬 [Discussions](../../discussions)
- ⭐ Dê uma estrela se o projeto foi útil!

---

<div align="center">

**🏠 Sistema de Iluminação Inteligente ESP32**

Feito com ❤️ para cidades mais inteligentes e sustentáveis

[![⭐ Stars](https://img.shields.io/github/stars/usuario/repositorio?style=social)](../../stargazers)
[![🍴 Forks](https://img.shields.io/github/forks/usuario/repositorio?style=social)](../../network/members)
[![📦 Issues](https://img.shields.io/github/issues/usuario/repositorio)](../../issues)

</div>