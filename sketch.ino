#include <WiFi.h>
#include <WebServer.h>
#include <DHTesp.h>
#include <LiquidCrystal_I2C.h>
#include <PubSubClient.h> 

const int PINO_DHT = 15;
const int PINO_POTENCIOMETRO = 34;
const int PINO_LED_VERDE = 2;
const int PINO_LED_VERMELHO = 4;

LiquidCrystal_I2C lcd(0x27, 16, 2);
DHTesp dht;
WebServer server(80); 

const char* ssid = "Wokwi-GUEST";
const char* password = "";

// aqui esta configurando o mqtt professor 
const char* mqtt_server = "broker.hivemq.com";

WiFiClient espClient;
PubSubClient client(espClient);

float temp_celsius = 0;
float hidratacao = 0;
String classificacao = "ANALISANDO";

// Função para manter o MQTT conectado
void reconectarMQTT() {
  while (!client.connected()) {
    String clientId = "OrionBeacon-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      // Conectou!
    } else {
      delay(2000); 
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(PINO_LED_VERDE, OUTPUT);
  pinMode(PINO_LED_VERMELHO, OUTPUT);
  
  dht.setup(PINO_DHT, DHTesp::DHT22);
  lcd.init();
  lcd.backlight();
  
  WiFi.begin(ssid, password);
  Serial.print("Conectando Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi Conectado!");

  // Configura o servidor MQTT
  client.setServer(mqtt_server, 1883);

  // Mantém a API REST para o postman/documentação
  server.on("/api/status", HTTP_GET, []() {
    String json = "{\"area\": \"Cratera A-17\", \"temperaturaC\": " + String(temp_celsius, 1) + ", \"indiceHidratacao\": " + String(hidratacao, 2) + ", \"classificacao\": \"" + classificacao + "\"}";
    server.send(200, "application/json", json);
  });
  server.begin();
}

void loop() {
  server.handleClient();
  
  // Garante que o MQTT está sempre online
  if (!client.connected()) {
    reconectarMQTT();
  }
  client.loop();
  
  static unsigned long lastTime = 0;
  if (millis() - lastTime > 2000) {
    lastTime = millis();
    
    // 1. Lê os Sensores
    temp_celsius = dht.getTemperature();
    hidratacao = analogRead(PINO_POTENCIOMETRO) / 4095.0;

    
    if (temp_celsius >= 5 && temp_celsius <= 30 && hidratacao > 0.7) {
      classificacao = "ALTA: H2O LIQ";
      digitalWrite(PINO_LED_VERDE, HIGH); digitalWrite(PINO_LED_VERMELHO, LOW);
    } 
    else if (temp_celsius < 0 && hidratacao > 0.7) {
      classificacao = "ALTA: GELO";
      digitalWrite(PINO_LED_VERDE, HIGH); digitalWrite(PINO_LED_VERMELHO, LOW);
    } 
    else {
      classificacao = "BAIXA: SECO";
      digitalWrite(PINO_LED_VERDE, LOW); digitalWrite(PINO_LED_VERMELHO, HIGH);
    }

  
    lcd.clear();
    lcd.setCursor(0, 0); lcd.print("T:"); lcd.print(temp_celsius, 1); lcd.print("C H:"); lcd.print(hidratacao, 2);
    lcd.setCursor(0, 1); lcd.print("St: "); lcd.print(classificacao);
    
    
    Serial.println("Telemetria -> Temp: " + String(temp_celsius, 1) + "C | Hidr: " + String(hidratacao, 2) + " | Status: " + classificacao);

    
    
    /
    String payload = "{\"temp\":\"" + String(temp_celsius, 1) + "\", \"hidra\":\"" + String(hidratacao, 2) + "\", \"status\":\"" + classificacao + "\"}";
    client.publish("fiap/orion/gianolli", payload.c_str());

    
    String payloadTemp = "{\"temperaturaC\": " + String(temp_celsius, 1) + "}";
    client.publish("fiap/orion/gianolli/temperatura", payloadTemp.c_str());
    

    String payloadHidra = "{\"indiceHidratacao\": " + String(hidratacao, 2) + "}";
    client.publish("fiap/orion/gianolli/hidratacao", payloadHidra.c_str());
  }
}