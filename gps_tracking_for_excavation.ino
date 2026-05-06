#include <WiFi.h>
#include <HTTPClient.h>
#include <TinyGPSPlus.h>
#include <HardwareSerial.h>

// WiFi Credentials
const char* WIFI_SSID = "F54";
const char* WIFI_PASSWORD = "87654321";

// Firebase Function
const char* FIREBASE_URL = "https://us-central1-gpstracking-acf06.cloudfunctions.net/updateLocation";

const char* LORRY_ID = "-OnDW4_zB5zYU0JLTQJp";

// GPS configuration
#define GPS_BAUD 9600
#define GPS_RX_PIN 16
#define GPS_TX_PIN 17

// Objects
TinyGPSPlus gps;
HardwareSerial gpsSerial(2);

unsigned long lastUpdate = 0;
const int interval = 10000;

void setup() {

  Serial.begin(115200);

  gpsSerial.begin(GPS_BAUD, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);

  Serial.println("GPS Started");

  connectWiFi();
}

void loop() {

  while (gpsSerial.available()) {
    gps.encode(gpsSerial.read());
  }

  if (gps.location.isUpdated()) {

    if (millis() - lastUpdate > interval) {

      sendLocation();

      lastUpdate = millis();
    }
  }

}

void connectWiFi() {

  Serial.print("Connecting WiFi");

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi Connected");

}

void sendLocation() {

  if (WiFi.status() != WL_CONNECTED) return;

  double lat = gps.location.lat();
  double lon = gps.location.lng();
  double speed = gps.speed.kmph();

  String url = String(FIREBASE_URL);
  url += "?lorryId=" + String(LORRY_ID);
  url += "&lat=" + String(lat,6);
  url += "&lng=" + String(lon,6);
  url += "&speed=" + String(speed,2);

  Serial.println("Sending Location");
  Serial.println(url);

  HTTPClient http;

  http.begin(url);

  int httpCode = http.GET();

  if (httpCode > 0) {

    Serial.print("Server Response: ");
    Serial.println(httpCode);

  } else {

    Serial.println("Error sending data");

  }

  http.end();
} 
