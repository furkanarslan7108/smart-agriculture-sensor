#include <WiFi.h>
#include <ThingerESP32.h>
#include <DHT.h>

// WiFi credentials
#define WIFI_SSID "Zyxel_FB81_5G"
#define WIFI_PASS "LJ3TRRUTUA"

// Thinger.io credentials
#define USERNAME "furkanarslan2003"
#define DEVICE_ID "esp32"
#define DEVICE_CREDENTIAL "xUzgAUA4RgMME46R"

// Sensor pins
#define DHT_PIN 4
#define SOIL_SENSOR_PIN A0  // Analog pin for capacitive soil sensor

// DHT sensor type
#define DHT_TYPE DHT22

// Initialize objects
ThingerESP32 thing(USERNAME, DEVICE_ID, DEVICE_CREDENTIAL);
DHT dht(DHT_PIN, DHT_TYPE);

// Variables to store sensor readings
float temperature = 0;
float humidity = 0;
int soilMoisture = 0;
int soilMoisturePercent = 0;

// Calibration values for soil sensor (adjust based on your sensor)
const int DRY_SOIL = 3000;    // Value in air (completely dry)
const int WET_SOIL = 1000;    // Value in water (completely wet)

void setup() {
  Serial.begin(115200);
  
  // Initialize DHT sensor
  dht.begin();
  
  // Configure WiFi
  thing.add_wifi(WIFI_SSID, WIFI_PASS);
  
  // Define Thinger.io resources
  thing["dht"] >> [](pson& out){
    out["temperature"] = temperature;
    out["humidity"] = humidity;
  };
  
  thing["soil"] >> [](pson& out){
    out["raw"] = soilMoisture;
    out["percentage"] = soilMoisturePercent;
  };
  
  // Combined resource for all sensors
  thing["sensors"] >> [](pson& out){
    out["temperature"] = temperature;
    out["humidity"] = humidity;
    out["soil_raw"] = soilMoisture;
    out["soil_percent"] = soilMoisturePercent;
  };
  
  Serial.println("ESP32 Sensor Monitor Starting...");
}

void loop() {
  thing.handle();
  
  // Read sensors every 5 seconds
  static unsigned long lastReading = 0;
  if (millis() - lastReading > 5000) {
    lastReading = millis();
    
    // Read DHT22 sensor
    temperature = dht.readTemperature();
    humidity = dht.readHumidity();
    
    // Read capacitive soil moisture sensor
    soilMoisture = analogRead(SOIL_SENSOR_PIN);
    
    // Convert to percentage (0% = completely dry, 100% = completely wet)
    soilMoisturePercent = map(soilMoisture, DRY_SOIL, WET_SOIL, 0, 100);
    soilMoisturePercent = constrain(soilMoisturePercent, 0, 100);
    
    // Check if DHT readings are valid
    if (isnan(temperature) || isnan(humidity)) {
      Serial.println("Failed to read from DHT sensor!");
      temperature = 0;
      humidity = 0;
    }
    
    // Print readings to Serial
    Serial.println("=== Sensor Readings ===");
    Serial.printf("Temperature: %.2f°C\n", temperature);
    Serial.printf("Humidity: %.2f%%\n", humidity);
    Serial.printf("Soil Moisture Raw: %d\n", soilMoisture);
    Serial.printf("Soil Moisture: %d%%\n", soilMoisturePercent);
    Serial.println("========================");
    
    // Send data to Thinger.io (automatic with defined resources)
    // You can also manually call specific resources:
    // thing.call_endpoint("your_endpoint_id");
  }
  
  // Small delay to prevent overwhelming the system
  delay(100);
}