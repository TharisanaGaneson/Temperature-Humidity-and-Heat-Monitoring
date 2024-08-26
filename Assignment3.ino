#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266WebServer.h>
#include <UniversalTelegramBot.h>
#include <EEPROM.h>
#include "DHT.h"
#include "Assignment3Interface.h"

// Sensor and Telegram configuration
#define DHTTYPE DHT22  // DHT sensor type: change to DHT11 if using DHT11
const int DHTPin = 5;   // GPIO pin where DHT sensor is connected
DHT dht(DHTPin, DHTTYPE);  // Initialize DHT sensor with pin and type

// Wi-Fi credentials
const char* ssid = "tharisana";
const char* password = "tharisana1234";

// Telegram Bot Token and Chat ID
String botToken = "6920395243:AAHHbAL-tZo-KfgwLwiJx1REi3Pof079R04";
String chatID = "1770612402";

// Create instances for server and secure client
ESP8266WebServer server(80);       // Web server running on port 80
WiFiClientSecure client;           // Secure client for HTTPS communication
UniversalTelegramBot bot(botToken, client);  // Telegram bot instance

// EEPROM address for storing min, max, avg data
#define EEPROM_SIZE 1024
#define EEPROM_ADDR_TEMP 0
#define EEPROM_ADDR_HUMIDITY 100
#define EEPROM_ADDR_HEAT_INDEX 200

unsigned long lastCalculationTime = 0;

// Function prototypes
void handleRoot();
void handleDHT11Temperature();
void handleDHT11Humidity();
void handleDHT11HeatIndex();
void handleTemperatureStats();
void handleHumidityStats();
void handleHeatIndexStats();
void sendTelegramMessage(String message);
void calculateAndStoreStats();
void retrieveAndDisplayStats();
void processSensorData(float value, String dataType, String endpoint);

// Min, Max, Avg variables for each parameter
float minTemp = 1000, maxTemp = -1000, sumTemp = 0;
float minHumidity = 1000, maxHumidity = -1000, sumHumidity = 0;
float minHeatIndex = 1000, maxHeatIndex = -1000, sumHeatIndex = 0;
int dataCount = 0;

void setup(void) {
  Serial.begin(115200);  // Initialize serial communication for debugging
  delay(500);  // Small delay for stability

  dht.begin();  // Initialize DHT sensor
  delay(500);  // Small delay for stability

  EEPROM.begin(EEPROM_SIZE);  // Initialize EEPROM

  // Connect to Wi-Fi network
  WiFi.begin(ssid, password);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nConnected to: " + String(ssid));
  Serial.println("IP address: " + WiFi.localIP().toString());

  // Setup web server routes
  server.on("/", handleRoot);  // Main page route
  server.on("/readTemperature", handleDHT11Temperature);  // Temperature endpoint
  server.on("/readHumidity", handleDHT11Humidity);  // Humidity endpoint
  server.on("/readHeatIndex", handleDHT11HeatIndex);  // Heat index endpoint
  server.on("/getTemperatureStats", handleTemperatureStats);  // Temperature stats endpoint
  server.on("/getHumidityStats", handleHumidityStats);  // Humidity stats endpoint
  server.on("/getHeatIndexStats", handleHeatIndexStats);  // Heat index stats endpoint

  server.begin();  // Start the web server
  Serial.println("HTTP server started");

  client.setInsecure();  // Allow insecure connections for Telegram API (no certificate validation)
}

void loop(void) {
  server.handleClient();  // Handle incoming HTTP requests

  // Calculate and store stats every minute
  if (millis() - lastCalculationTime > 60000) {
    calculateAndStoreStats();
    retrieveAndDisplayStats();
    lastCalculationTime = millis();
  }
}

void handleRoot() {
  retrieveAndDisplayStats();  // Display the latest stats on the root page
  server.send(200, "text/html", MAIN_page);  // Send the HTML page from the external file
}

void handleDHT11Temperature() {
  float temperature = dht.readTemperature();
  processSensorData(temperature, "Temperature", "/readTemperature");
}

void handleDHT11Humidity() {
  float humidity = dht.readHumidity();
  processSensorData(humidity, "Humidity", "/readHumidity");
}

void handleDHT11HeatIndex() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  float heatIndex = dht.computeHeatIndex(temperature, humidity, false);  // Calculate heat index in Celsius
  processSensorData(heatIndex, "Heat Index", "/readHeatIndex");
}

void processSensorData(float value, String dataType, String endpoint) {
  if (isnan(value)) {
    Serial.println("Failed to read from " + dataType + " sensor!");
    server.send(500, "text/plain", "Sensor error");
  } else {
    Serial.println(dataType + ": " + String(value));
    server.send(200, "text/plain", String(value));

    // Update min, max, sum values
    if (dataType == "Temperature") {
      minTemp = min(minTemp, value);
      maxTemp = max(maxTemp, value);
      sumTemp += value;
    } else if (dataType == "Humidity") {
      minHumidity = min(minHumidity, value);
      maxHumidity = max(maxHumidity, value);
      sumHumidity += value;
    } else if (dataType == "Heat Index") {
      minHeatIndex = min(minHeatIndex, value);
      maxHeatIndex = max(maxHeatIndex, value);
      sumHeatIndex += value;
    }

    dataCount++;  // Increment data count

    // Send a Telegram message if temperature exceeds 30°C
    if (dataType == "Temperature" && value >= 30.0) {
      sendTelegramMessage("Warning! " + dataType + " = " + String(value) + "°C");
    }
  }
}

void sendTelegramMessage(String message) {
  if (client.connect("api.telegram.org", 443)) {
    String url = "/bot" + botToken + "/sendMessage?chat_id=" + chatID + "&text=" + message;
    client.print("GET " + url + " HTTP/1.1\r\nHost: api.telegram.org\r\nConnection: close\r\n\r\n");

    while (client.connected()) {
      if (client.readStringUntil('\n') == "\r") {
        break;
      }
    }
    client.stop();
  }
}

// Calculate min, max, avg and store in EEPROM
void calculateAndStoreStats() {
  if (dataCount > 0) {
    float avgTemp = sumTemp / dataCount;
    float avgHumidity = sumHumidity / dataCount;
    float avgHeatIndex = sumHeatIndex / dataCount;

    EEPROM.put(EEPROM_ADDR_TEMP, minTemp);
    EEPROM.put(EEPROM_ADDR_TEMP + sizeof(float), maxTemp);
    EEPROM.put(EEPROM_ADDR_TEMP + 2 * sizeof(float), avgTemp);

    EEPROM.put(EEPROM_ADDR_HUMIDITY, minHumidity);
    EEPROM.put(EEPROM_ADDR_HUMIDITY + sizeof(float), maxHumidity);
    EEPROM.put(EEPROM_ADDR_HUMIDITY + 2 * sizeof(float), avgHumidity);

    EEPROM.put(EEPROM_ADDR_HEAT_INDEX, minHeatIndex);
    EEPROM.put(EEPROM_ADDR_HEAT_INDEX + sizeof(float), maxHeatIndex);
    EEPROM.put(EEPROM_ADDR_HEAT_INDEX + 2 * sizeof(float), avgHeatIndex);

    EEPROM.commit();

    // Reset variables for next cycle
    minTemp = 1000; maxTemp = -1000; sumTemp = 0;
    minHumidity = 1000; maxHumidity = -1000; sumHumidity = 0;
    minHeatIndex = 1000; maxHeatIndex = -1000; sumHeatIndex = 0;
    dataCount = 0;
  }
}

// Retrieve stats from EEPROM and display in Serial
void retrieveAndDisplayStats() {
  float minTemp, maxTemp, avgTemp;
  float minHumidity, maxHumidity, avgHumidity;
  float minHeatIndex, maxHeatIndex, avgHeatIndex;

  EEPROM.get(EEPROM_ADDR_TEMP, minTemp);
  EEPROM.get(EEPROM_ADDR_TEMP + sizeof(float), maxTemp);
  EEPROM.get(EEPROM_ADDR_TEMP + 2 * sizeof(float), avgTemp);

  EEPROM.get(EEPROM_ADDR_HUMIDITY, minHumidity);
  EEPROM.get(EEPROM_ADDR_HUMIDITY + sizeof(float), maxHumidity);
  EEPROM.get(EEPROM_ADDR_HUMIDITY + 2 * sizeof(float), avgHumidity);

  EEPROM.get(EEPROM_ADDR_HEAT_INDEX, minHeatIndex);
  EEPROM.get(EEPROM_ADDR_HEAT_INDEX + sizeof(float), maxHeatIndex);
  EEPROM.get(EEPROM_ADDR_HEAT_INDEX + 2 * sizeof(float), avgHeatIndex);

  // Print the retrieved data to Serial Monitor
  Serial.println("Latest Statistics:");
  Serial.println("Temperature: Min = " + String(minTemp) + "°C, Max = " + String(maxTemp) + "°C, Avg = " + String(avgTemp) + "°C");
  Serial.println("Humidity: Min = " + String(minHumidity) + "%, Max = " + String(maxHumidity) + "%, Avg = " + String(avgHumidity) + "%");
  Serial.println("Heat Index: Min = " + String(minHeatIndex) + "°C, Max = " + String(maxHeatIndex) + "°C, Avg = " + String(avgHeatIndex) + "°C");
}

// Handlers for retrieving stats via web requests
void handleTemperatureStats() {
  sendStatResponse(EEPROM_ADDR_TEMP);
}

void handleHumidityStats() {
  sendStatResponse(EEPROM_ADDR_HUMIDITY);
}

void handleHeatIndexStats() {
  sendStatResponse(EEPROM_ADDR_HEAT_INDEX);
}

void sendStatResponse(int address) {
  float minVal, maxVal, avgVal;
  EEPROM.get(address, minVal);
  EEPROM.get(address + sizeof(float), maxVal);
  EEPROM.get(address + 2 * sizeof(float), avgVal);

  String jsonResponse = "{\"min\":" + String(minVal) + ",\"max\":" + String(maxVal) + ",\"avg\":" + String(avgVal) + "}";
  server.send(200, "application/json", jsonResponse);
}
