#include "UbidotsEsp32Mqtt.h"

// ============== CONFIGURAÇÕES WiFi ==============
const char *WIFI_SSID = "Inteli.Iot"; 
const char *WIFI_PASS = "%(Yk(sxGMtvFEs.3"; 

// ============== CONFIGURAÇÕES UBIDOTS ==============
const char *UBIDOTS_TOKEN = "BBUS-0DcpktUrltGEufTrUlqW0wHwOrL6Xq"; // Token do Ubidots
const char *DEVICE_LABEL = "esp32_pietro";         // Nome do device (igual ao API label)
const char *VARIABLE_LABEL = "dbm";                // Nome da variável
const char *CLIENT_ID = "esp32_pietro_client";     // ID único do cliente

Ubidots ubidots(UBIDOTS_TOKEN, CLIENT_ID); 
const int PUBLISH_FREQUENCY = 3000; 
unsigned long timer;
unsigned long last_publish = 0;
const unsigned long PUBLISH_INTERVAL = PUBLISH_FREQUENCY;


void callback(char *topic, byte *payload, unsigned int length){
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++){
    Serial.print((char)payload[i]);
  }
  Serial.println();
}
float value1;
void setup(){
  Serial.begin(115200);
  ubidots.setDebug(true); 
  ubidots.connectToWifi(WIFI_SSID, WIFI_PASS);
  ubidots.setCallback(callback);
  ubidots.setup();
  ubidots.reconnect();
  timer = millis();
}
void loop() {
  // Mantém a conexão MQTT ativa
  if (!ubidots.connected()) {
    ubidots.reconnect();
  }
  ubidots.loop();
  
  // Verifica conexão WiFi
  if (WiFi.status() == WL_CONNECTED) {
    int32_t dBm = WiFi.RSSI();
    
    // Publica a cada 3 segundos
    if (millis() - last_publish > PUBLISH_INTERVAL) {
      ubidots.add(VARIABLE_LABEL, dBm);
      ubidots.publish(DEVICE_LABEL);
      Serial.printf("📡 Sinal WiFi: %d dBm | Publicado no Ubidots!\n", dBm);
      last_publish = millis();
    }
  } else {
    Serial.println("❌ WiFi desconectado! Reconectando...");
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    delay(1000);
  }
}
