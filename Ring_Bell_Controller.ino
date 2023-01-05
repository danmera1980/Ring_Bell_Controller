#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#define WIFI_SSID "YOUR WIFI SSID"
#define WIFI_PASS "WOUR PASSSWORD"

#define MQTT_SERV "io.adafruit.com"
#define MQTT_PORT 1883
#define MQTT_NAME "YOUR USERNAME"
#define MQTT_PASS "YOUR KEY"

int relay = D2;

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, MQTT_SERV, MQTT_PORT, MQTT_NAME, MQTT_PASS);

Adafruit_MQTT_Subscribe bell = Adafruit_MQTT_Subscribe(&mqtt, MQTT_NAME "/f/bell");

void setup() {
  Serial.begin(9600);
  pinMode(relay, OUTPUT);

  Serial.print("\n\nConnecting to Wifi>");
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED){
    Serial.print(">");
    delay(50);
  }

  Serial.println("Connected!");

  mqtt.subscribe(&bell);

  Serial.println("Bell rang");
  digitalWrite(relay, HIGH);
  delay(800);
  digitalWrite(relay, LOW);
  delay(800);
  digitalWrite(relay, HIGH);
  delay(800);
  digitalWrite(relay, LOW);
}

void loop() {
  MQTT_connect();

  Adafruit_MQTT_Subscribe * subscription;
  while((subscription = mqtt.readSubscription(5000))){
    if(subscription == &bell){
      Serial.print("Bell: ");
      Serial.println((char*) bell.lastread);

      if(!strcmp((char*) bell.lastread, "ON")){
        Serial.println("Bell rang");
        digitalWrite(relay, HIGH);
        delay(800);
        digitalWrite(relay, LOW);
        delay(800);
        digitalWrite(relay, HIGH);
        delay(800);
        digitalWrite(relay, LOW);
      }
    }
  }
}

void MQTT_connect(){
  if (mqtt.connected() && mqtt.ping()){
    return;
  }

  int8_t ret;

  mqtt.disconnect();

  Serial.print("Connecting to MQTT... ");
  uint8_t retries = 3;
  while((ret = mqtt.connect()) != 0 ){
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds... ");
    mqtt.disconnect();
    delay(5000);
    retries--;
    if(retries == 0){
      ESP.reset();
    }
  }
  Serial.println("MQTT Connected!");
}
