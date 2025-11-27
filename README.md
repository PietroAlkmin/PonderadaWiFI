# 📝 Relatório – Atividade Ponderada

### Coleta e Análise de Potência de Sinal WiFi (dBm) com ESP32 + MQTT + Ubidots

---

### 👨‍💻 Autor

**Pietro Alkmin** - Turma 17 - Inteli  
**Data:** Novembro de 2025

---

## 1. Introdução

Este relatório apresenta o desenvolvimento e execução de um sistema IoT projetado para medir a intensidade do sinal WiFi (em dBm) utilizando um **ESP32**. O objetivo da atividade foi coletar continuamente o valor do **RSSI** (Received Signal Strength Indicator), publicar esses dados em uma plataforma online via protocolo **MQTT** e monitorar em tempo real as variações do sinal através de um **dashboard**.

Além das coletas padrão, foi realizado um teste especial dentro do **elevador do Inteli**, que simula o efeito de uma **gaiola de Faraday** devido à blindagem metálica, causando atenuação significativa do sinal.

---

## 2. Desenvolvimento

### 2.1 Arquitetura da Solução

A solução implementada segue a seguinte arquitetura:

- **ESP32** conectado à rede WiFi do Inteli (`Inteli.Iot`)
- Leitura periódica do RSSI através da função `WiFi.RSSI()`
- Envio dos dados para a plataforma **Ubidots** via protocolo **MQTT**
- **Dashboard online** exibindo gráfico em tempo real do tipo **tempo × dBm**
- Testes em diferentes cenários físicos para análise do comportamento do sinal

**Fluxo geral do sistema:**

```
ESP32 → Leitura dBm → Publicação MQTT → Ubidots → Dashboard em tempo real
```

---

### 2.2 Configuração e Programação do ESP32

O código desenvolvido para o ESP32 realiza as seguintes tarefas:

1. **Conexão à rede WiFi** do Inteli
2. **Leitura periódica** da intensidade do sinal (RSSI)
3. **Publicação via MQTT** para o Ubidots a cada 3 segundos
4. **Impressão dos valores** via Serial Monitor para validação
5. **Reconexão automática** em caso de perda de conexão

**Código principal:**

```cpp
#include "UbidotsEsp32Mqtt.h"

// Configurações WiFi
const char *WIFI_SSID = "Inteli.Iot"; 
const char *WIFI_PASS = "%(Yk(sxGMtvFEs.3"; 

// Configurações Ubidots
const char *UBIDOTS_TOKEN = "BBUS-0DcpktUrltGEufTrUlqW0wHwOrL6Xq";
const char *DEVICE_LABEL = "esp32_pietro";
const char *VARIABLE_LABEL = "dbm";
const char *CLIENT_ID = "esp32_pietro_client";

Ubidots ubidots(UBIDOTS_TOKEN, CLIENT_ID);
const int PUBLISH_FREQUENCY = 3000; // Publica a cada 3 segundos

void setup(){
  Serial.begin(115200);
  ubidots.setDebug(true); 
  ubidots.connectToWifi(WIFI_SSID, WIFI_PASS);
  ubidots.setup();
  ubidots.reconnect();
}

void loop() {
  if (!ubidots.connected()) {
    ubidots.reconnect();
  }
  ubidots.loop();
  
  if (WiFi.status() == WL_CONNECTED) {
    int32_t dBm = WiFi.RSSI();
    
    if (millis() - last_publish > PUBLISH_INTERVAL) {
      ubidots.add(VARIABLE_LABEL, dBm);
      ubidots.publish(DEVICE_LABEL);
      Serial.printf("📡 Sinal WiFi: %d dBm | Publicado!\n", dBm);
      last_publish = millis();
    }
  }
}
```

---

### 2.3 Comunicação MQTT e Dashboard (Ubidots)

A comunicação MQTT foi configurada utilizando a plataforma **Ubidots**, que oferece:

- **Broker MQTT integrado** para recebimento de dados
- **Dashboard personalizável** com widgets em tempo real
- **Armazenamento automático** dos dados históricos
- **API REST** para consultas avançadas

**Configuração do Dashboard:**

1. Criação do device `esp32_pietro` no Ubidots
2. Configuração automática da variável `dbm`
3. Widget de gráfico de linha (Line Chart) configurado com:
   - **Eixo Y:** -100 a -30 dBm
   - **Atualização:** Tempo real
   - **Período de amostragem:** 3 segundos

O dashboard registrou corretamente todas as variações do sinal, incluindo os períodos de ausência de dados durante o teste no elevador.

---

### 2.4 Testes Realizados

Foram realizados testes em diferentes cenários para avaliar o comportamento do sinal WiFi:

#### Cenário 1 — Ambiente Normal (Sala de Aula)
- **Sinal:** Estável entre -35 e -45 dBm
- **Qualidade:** Excelente
- **Observações:** Pouca variação entre as amostras

#### Cenário 2 — Teste de Distância
- **Sinal:** Atenuação gradual conforme afastamento do roteador
- **Qualidade:** Boa a razoável (-50 a -70 dBm)
- **Observações:** Variação esperada com a distância

#### Cenário 3 — Interior do Elevador (Gaiola de Faraday)
- **Sinal:** Queda abrupta para -80 dBm ou inferior
- **Qualidade:** Muito fraca ou sem sinal
- **Observações:** 
  - Perda de conexão WiFi
  - Dashboard registrou "buraco" no gráfico
  - Confirmação do efeito de blindagem eletromagnética

#### Cenário 4 — Após Saída do Elevador
- **Sinal:** Recuperação imediata aos valores normais
- **Qualidade:** Retorno ao padrão excelente
- **Observações:**
  - Reconexão automática do ESP32
  - Dashboard voltou a receber dados normalmente
  - Sistema resiliente a interrupções

---

### 2.5 Análise dos Resultados

A análise dos gráficos e valores coletados permitiu observar:

- ✅ A intensidade do sinal WiFi diminui significativamente com **barreiras físicas metálicas**
- ✅ O **elevador** apresentou forte atenuação, confirmando o comportamento de uma **gaiola de Faraday**
- ✅ O **ESP32** manteve estabilidade de envio em ambientes abertos
- ✅ O protocolo **MQTT** se mostrou **resiliente** após quedas de conexão
- ✅ O **Ubidots** registrou de forma clara as interrupções e reconexões do sistema
- ✅ Os valores de RSSI variaram conforme esperado nos diferentes cenários

**Interpretação dos valores de RSSI:**

| RSSI (dBm) | Qualidade do Sinal |
|------------|-------------------|
| -30 a -50  | ⭐⭐⭐⭐⭐ Excelente |
| -50 a -60  | ⭐⭐⭐⭐ Bom |
| -60 a -70  | ⭐⭐⭐ Razoável |
| -70 a -80  | ⭐⭐ Fraco |
| -80 a -90  | ⭐ Muito Fraco |
| < -90      | ❌ Sem sinal |

---

## 3. Demonstração em Vídeo

🎥 **Link do vídeo:** [VIDEO AQUI](https://drive.google.com/file/d/1L6LjPOSOWzwga85METBlD3hM2yvsUky4/view?usp=sharing)

O vídeo demonstra:
- ✅ ESP32 conectado e funcionando
- ✅ Dashboard do Ubidots em tempo real
- ✅ Valores de RSSI sendo atualizados
- ✅ Teste no elevador (Gaiola de Faraday)
- ✅ Queda e recuperação do sinal no gráfico
- ✅ Análise dos resultados obtidos

**Duração:** 3-5 minutos

---

## 4. Conclusão

O experimento demonstrou, de forma prática, a relação entre **distância**, **barreiras físicas** e **intensidade do sinal WiFi**. Com o uso do **ESP32**, **MQTT** e **Ubidots**, foi possível monitorar em tempo real o comportamento do RSSI em diferentes ambientes.

O teste no **elevador** mostrou, de maneira evidente, o **bloqueio do sinal** causado pela blindagem metálica, simulando uma **gaiola de Faraday**. Esse comportamento ficou claramente refletido nos gráficos da plataforma, que registraram a interrupção temporária de dados.

A atividade cumpriu seu objetivo ao integrar conceitos de:
- 🔹 **IoT** (Internet of Things)
- 🔹 **Redes sem fio** e análise de radiofrequência
- 🔹 **Comunicação MQTT**
- 🔹 **Dashboards em tempo real**
- 🔹 **Análise de dados** e interpretação de resultados

---

## 📦 Estrutura do Repositório

```
PonderadaWiFI/
├── esp32_wifi_mqtt/
│   └── esp32_wifi_mqtt.ino    # Código completo do ESP32
├── dashboard/
│   └── index.html              # Dashboard alternativa (HTML local)
├── MQTT_CONFIG.md              # Guia de configuração MQTT
├── TESTE_MOSQUITTO.md          # Guia para testes com Mosquitto local
├── GUIA_RAPIDO.md              # Referência rápida
└── README.md                   # Este documento
```

---

## 🛠️ Requisitos e Tecnologias

### Hardware
- **ESP32** (qualquer modelo compatível com Arduino IDE)
- **Cabo USB** para programação e alimentação
- **Powerbank** (opcional, para testes móveis)

### Software
- **Arduino IDE** (versão 2.0 ou superior)
- **Bibliotecas:**
  - `WiFi.h` (inclusa no ESP32 core)
  - `UbidotsEsp32Mqtt.h` (biblioteca Ubidots para ESP32)

### Plataforma
- **Ubidots** (plataforma IoT para dashboard e MQTT)
- Conta gratuita disponível em: https://ubidots.com

---

## 🚀 Como Reproduzir

### Passo 1: Instalar a Arduino IDE e Configurar o ESP32

1. Baixe e instale a Arduino IDE: https://www.arduino.cc/en/software
2. Adicione suporte ao ESP32:
   - `File` → `Preferences` → "Additional Board Manager URLs"
   - Adicione: `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`
   - `Tools` → `Board` → `Boards Manager` → Procure "ESP32" e instale

### Passo 2: Instalar a Biblioteca Ubidots

1. `Sketch` → `Include Library` → `Manage Libraries`
2. Procure: **"Ubidots ESP32 MQTT"**
3. Instale a biblioteca

### Passo 3: Criar Conta no Ubidots

1. Acesse: https://ubidots.com
2. Crie uma conta gratuita
3. Vá em **"Devices"** → **"+"** → **"Blank Device"**
4. Crie um device com label: `esp32_pietro` (ou personalizado)
5. Copie o **Token** em: Perfil → **"API Credentials"**

### Passo 4: Configurar e Fazer Upload do Código

1. Abra `esp32_wifi_mqtt/esp32_wifi_mqtt.ino`
2. Configure:
   - WiFi SSID e senha
   - Token do Ubidots
   - Device label (mesmo criado no Ubidots)
3. Conecte o ESP32 via USB
4. Selecione: `Tools` → `Board` → `ESP32 Dev Module`
5. Selecione a porta: `Tools` → `Port`
6. Clique em **Upload**

### Passo 5: Verificar no Serial Monitor

1. `Tools` → `Serial Monitor` (115200 baud)
2. Verifique a conexão WiFi
3. Confirme que os valores de RSSI estão sendo publicados

### Passo 6: Criar Dashboard no Ubidots

1. Vá em **"Data"** → **"Dashboards"** → **"+"**
2. Adicione widget: **"Line Chart"**
3. Selecione device: `esp32_pietro`
4. Selecione variável: `dbm`
5. Configure eixo Y: -100 a -30
6. Salve e visualize os dados em tempo real!

### Passo 7: Realizar Teste no Elevador

1. Mantenha ESP32 ligado (use powerbank se necessário)
2. Deixe dashboard aberta no laptop/celular
3. Entre no elevador
4. Aguarde 5+ segundos com portas fechadas
5. Saia e observe a recuperação do sinal
6. **Filme tudo!**



---

## 📚 Referências

- [ESP32 Arduino Core Documentation](https://docs.espressif.com/projects/arduino-esp32/)
- [Ubidots ESP32 MQTT Library](https://github.com/ubidots/ubidots-mqtt-esp)
- [MQTT Protocol Specification](https://mqtt.org/)
- [Conceito de Gaiola de Faraday](https://pt.wikipedia.org/wiki/Gaiola_de_Faraday)

---

## 👨‍💻 Autor

**Pietro Alkmin**  
Turma 17 - Inteli  
Novembro de 2025

---

## 📄 Licença

Este projeto foi desenvolvido para fins educacionais no Instituto de Tecnologia e Liderança (Inteli).

---
