#include <DHT.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>

#include <Wire.h>

#define DHTPIN 4
#define DHTTYPE DHT11

WiFiMulti wifiMulti;
const char* ssid = "";
const char* password = "";

char buffer[500];
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  Serial.println(F("DHTxx test!"));

  wifiMulti.addAP(ssid, password);

  dht.begin();
}

void loop() {
  // Wait a few seconds between measurements.
  delay(2000);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));
  Serial.print(F("Heat index: "));
  Serial.print(hic);
  Serial.println(F("°C "));

  // WiFi
  if ((wifiMulti.run() == WL_CONNECTED)) {
    HTTPClient http;

    Serial.print("Connecting to website: ");
    http.begin("");  //HTTP connection

    // Sends data to our Node-Red instance with a buffer of 50, displaying our temperature and humidity values in the debug menu
    sprintf(buffer, "{\"temp\":%5.2f,\"humidity\":%5.2f}", t, h);
    int httpCode = http.POST(buffer);
    if (httpCode > 0) {
      Serial.printf("[HTTP] POST... code: %d\n", httpCode);
      if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        Serial.println(payload);
      }
    } else {
      Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
  }
}
