#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const char* ssid = "Loki124";
const char* password = "00000000";
const char* thingspeak_api_key = "PNWMUJXVA20AMXM8";  // <-- Replace with your ThingSpeak Write API Key

WiFiClient client;
unsigned long lastUploadTime = 0;
const unsigned long uploadInterval = 5000; // ThingSpeak's minimum interval: 15 sec

// Alert cooldown
unsigned long lastAlert = 0;
const unsigned long ALERT_COOLDOWN = 60000; // 1-minute cooldown

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi...");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected to WiFi.");
  Serial.println("Waiting for sensor data...");
}

void loop() {
  if (Serial.available()) {
    String data = Serial.readStringUntil('\n');
    Serial.println("\n[Raw Data Received]: " + data);

    int pulse, pulseAlert, tempAlert;
    float tempC;

    // Expecting: pulse,tempC,pulseAlert,tempAlert
    int parsed = sscanf(data.c_str(), "%d,%f,%d,%d", 
                        &pulse, &tempC, &pulseAlert, &tempAlert);

    if (parsed == 4 && millis() - lastUploadTime > uploadInterval) {
      String url = "http://api.thingspeak.com/update?api_key=" + String(thingspeak_api_key) +
                   "&field1=" + String(pulse) +
                   "&field2=" + String(tempC);

      HTTPClient http;
      http.begin(client, url);
      int httpCode = http.GET();

      if (httpCode > 0) {
        Serial.println("[✓] Data sent to ThingSpeak");
      } else {
        Serial.println("[X] Failed to send data to ThingSpeak");
      }

      http.end();
      lastUploadTime = millis();
    }

    // Alert logic
    String alertMessage = "";
    bool alertTriggered = false;
    unsigned long currentTime = millis();

    if (pulseAlert) {
      alertMessage += "Abnormal Pulse (" + String(pulse) + " BPM)\n";
      alertTriggered = true;
    }

    if (tempAlert) {
      alertMessage += "High Temp (" + String(tempC) + " °C)\n";
      alertTriggered = true;
    }

    if (alertTriggered && (currentTime - lastAlert > ALERT_COOLDOWN)) {
      Serial.println("[!] ALERT:");
      Serial.println(alertMessage);
      lastAlert = currentTime;
    }
  }
}