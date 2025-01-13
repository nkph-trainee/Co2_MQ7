// กำหนดค่าคอนฟิก Blynk
#define BLYNK_TEMPLATE_ID "TMPL6pkA_CiM0"
#define BLYNK_TEMPLATE_NAME "CoHDT22"
#define BLYNK_AUTH_TOKEN "rScQ1GJggy4IMW5R7InpCcqy303hxXXa"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>
#include <ESP8266HTTPClient.h>

// Pin Configuration
#define DHTPIN D4         // Pin เชื่อมต่อ DHT22
#define DHTTYPE DHT22     // ระบุเซ็นเซอร์ DHT22
#define MQ7PIN A0         // Pin เชื่อมต่อ MQ-7

DHT dht(DHTPIN, DHTTYPE);

// WiFi Credentials
char ssid[] = "Tang8822";       // ชื่อ WiFi
char pass[] = "88882222";   // รหัสผ่าน WiFi

// HTTP Server Base URL
const char* serverURL = "http://npmh.moph.go.th/api/itwork/api/iot/temporature";

WiFiClient client;  // สร้าง WiFiClient

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, pass);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  dht.begin();

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected!");
}

void loop() {
  Blynk.run();

  static unsigned long lastSendTime = 0;
  if (millis() - lastSendTime > 60000) { // ทุก 5 นาที (300,000 ms)
    lastSendTime = millis();

    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();
    int coConcentration = analogRead(MQ7PIN);

    if (isnan(temperature) || isnan(humidity)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }

    // ส่งข้อมูลผ่าน Blynk
    Blynk.virtualWrite(V1, temperature);
    Blynk.virtualWrite(V2, humidity);
    Blynk.virtualWrite(V3, coConcentration);

    // สร้าง Query String สำหรับ GET Request
    String queryString = "?coConcentration=" + String(coConcentration) +
                         "&humidity=" + String(humidity) +
                         "&temperature=" + String(temperature);

    // สร้าง URL แบบเต็ม
    String fullURL = String(serverURL) + queryString;

    // ส่งข้อมูลผ่าน HTTPClient ด้วย GET Method
    HTTPClient http;
    http.begin(client, fullURL); // ใช้ WiFiClient พร้อม URL

    int httpResponseCode = http.GET(); // ใช้ GET Method

    if (httpResponseCode > 0) {
      Serial.println("GET Response: " + String(httpResponseCode));
      Serial.println("Response Body: " + http.getString());
    } else {
      Serial.println("Error on sending GET: " + http.errorToString(httpResponseCode));
    }
    http.end();
  }
}