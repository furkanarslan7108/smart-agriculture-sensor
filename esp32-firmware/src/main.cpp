#include <Arduino.h>
#include <ThingerESP32.h>
#include <SoilSensor.h>

// Thinger.io credentials
#define USERNAME "username"
#define DEVICE_ID "device_ID"
#define DEVICE_CREDENTIAL "device_credentials"

// Wi-Fi credentials
#define WIFI_SSID "ssid"
#define WIFI_PASSWORD "wifi_password"

DHT22 sensor(2);
ThingerESP32 thing(USERNAME, DEVICE_ID, DEVICE_CREDENTIAL);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  // Start wifi connection
  thing.add_wifi(WIFI_SSID, WIFI_PASSWORD);

  // Describe sensor data
  thing["soil_moisture"] >> [](pson &out) {
       int soilValue = analogRead(34); // Sensör pinini okuyun
       out = soilValue;
  };
}

void loop() {
  // put your main code here, to run repeatedly:
  thing.handle();
}

