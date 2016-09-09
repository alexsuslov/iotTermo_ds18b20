/*
Dalas 18b20 to mqtt
 */
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <PubSubClient.h>
#include "config.h"

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress insideThermometer;

float t; //Temperature
int error;
char tmpMSG[64];

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  Serial.println("\n Starting ds18b20 sensor");
  pinMode(POWER_PIN, OUTPUT);
  digitalWrite(POWER_PIN, 1);
  sensors.begin();
  Serial.print("sensors conunter is: ");
  Serial.println(sensors.getDeviceCount(), DEC);

  // report parasite power requirements
  Serial.print("Parasite power is: ");
  if (sensors.isParasitePowerMode()) Serial.println("ON");
  else Serial.println("OFF");

  digitalWrite(POWER_PIN, 0);
}


//////////////////////////////////////////
void data_get(){
  digitalWrite(POWER_PIN, 1);
  sensors.requestTemperatures();
  t = sensors.getTempCByIndex(0);
  digitalWrite(POWER_PIN, 0);
  if (isnan(t)) {
    error = 1; // 1 temperature error
    return;
  }
}


//////////////////////////////////////////
void data_display(){
  if (error){
    return;
  }

  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.println(" *C ");

}

///////////////////////////////////////////
void wifi_connect(){
  if (error){
    return;
  }
  if (WiFi.status() == WL_CONNECTED){
    return;
  }

  WiFi.begin(ACCESS_POINT, ACCESS_POINT_KEY);
  Serial.print("Connecting");
  int i = 50;
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    if (!i--){
      error = 2; // 2 wifi error
      return;
    }
  }
  Serial.println();
  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());
}


void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void mqtt_connect(){
  if (error || client.connected()){
    return;
  }
  Serial.println("connected mqtt");

  client.setServer(MQTT_HOST, MQTT_PORT);
  client.setCallback(mqtt_callback);

  int i = 60;
  while (!client.connected()) {
    if (client.connect("dht22_v", MQTT_USER, MQTT_PASS)){
      Serial.println("mqtt connected");
    } else {
      delay(500);
      Serial.print(".");
      if (!i--){
        error = 3; // 3 mqtt error
        return;
      }
    }
  }
}

void mqtt_pub(){
  if (error || !client.connected()){
    return;
  }

  Serial.println("publish mqtt");
  dtostrf(t, 2, 2, tmpMSG);
  client.publish(MQTT_TOPIC, tmpMSG);
}

// tasks
void loop() {
  error = 0;
  data_get();
  data_display();
  wifi_connect();
  mqtt_connect();
  mqtt_pub();

  if (error) {
    Serial.print("Failed: ");
    Serial.println(error);
    return;
  }
  delay( TIMEOUT * 1000);
}