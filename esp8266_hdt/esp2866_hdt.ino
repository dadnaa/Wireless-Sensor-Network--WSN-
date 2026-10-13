#include <ESP8266WiFi.h>
#include <espnow.h>
#include <DHT11.h>
#include <user_interface.h>

#define DHTPIN D2
DHT11 dht11(DHTPIN);

uint8_t sinkAddress[] = {0x84, 0x0D, 0x8E, 0xA5, 0x37, 0x7B};;

struct SensorData {
  uint32_t nodeId;
  float temperature;
  float humidity;
  uint32_t counter;
};

SensorData dataToSend;
uint32_t packetCounter = 0;
unsigned long previousMillis = 0;

void onDataSent(uint8_t *mac, uint8_t status) {
  Serial.print("Envoi : ");
  Serial.println(status == 0 ? "SUCCES" : "ECHEC");
}

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  wifi_set_channel(1);

  Serial.println("\nWSN - NOEUD CAPTEUR");
  Serial.print("MAC du capteur : ");
  Serial.println(WiFi.macAddress());

  if (esp_now_init() != 0) {
    Serial.println("Erreur initialisation ESP-NOW");
    return;
  }

  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);

  if (esp_now_add_peer(sinkAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0) != 0) {
    Serial.println("Erreur ajout du sink");
    return;
  }

  esp_now_register_send_cb(onDataSent);
  Serial.println("Noeud capteur pret.");
}

void loop() {
  if (millis() - previousMillis >= 5000) {
    previousMillis = millis();

    int temperature = 0;
    int humidity = 0;

    int resultRead = dht11.readTemperatureHumidity(temperature, humidity);

    if (resultRead != 0) {
      Serial.println("Erreur lecture DHT11");
      return;
    }

    dataToSend.nodeId = 1;
    dataToSend.temperature = temperature;
    dataToSend.humidity = humidity;
    dataToSend.counter = ++packetCounter;

    int resultSend = esp_now_send(
      sinkAddress,
      (uint8_t *)&dataToSend,
      sizeof(dataToSend)
    );

    Serial.printf("T=%d C, H=%d %%\n", temperature, humidity);

    if (resultSend != 0) {
      Serial.println("Erreur lors de l'envoi");
    }
  }
}