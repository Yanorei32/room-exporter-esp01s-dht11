#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <DHT.h>

const char* ssid     = "YOUR_SSID";
const char* password = "YOUR_KEY";
/ // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

#define DHTTYPE DHT11
#define DHTPIN  2

DHT dht(DHTPIN, DHTTYPE, 11);

WiFiServer server(80);

void setup(void) {
  Serial.begin(115200);
  dht.begin();

  Serial.printf("Working to connect %s", ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("DHT Weather Exporter");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  server.begin();
  Serial.println("Server started");
}

unsigned long previousMillis = 0;
const unsigned long interval = 1000;

void loop(void) {
  WiFiClient c = server.available();
  if (!c) return;

  while (!c.available()) delay(1);
  c.flush();

  unsigned long currentMillis = millis();
  
  if (currentMillis - previousMillis < interval) {
    c.println("HTTP/1.1 429 Too Many Requests");
    c.println("Content-Type: plain/text");
    return;
  }
  
  previousMillis = currentMillis;

  c.println("HTTP/1.1 200 OK");
  c.println("Content-Type: palin/text");
  c.println("");
  c.printf("esp8266_millis %lu\n", currentMillis);
  
  float h = dht.readHumidity();
  if (!isnan(h)) {
    c.printf("room_humidity_percent %f\n", h);
  }

  float t = dht.readTemperature();
  if (!isnan(t)) {
    c.printf("room_temperature_celsius %f\n", t);
  }
  
  delay(1);
}

