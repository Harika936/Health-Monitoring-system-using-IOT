#include <SoftwareSerial.h>
#include <Wire.h>
#include <LiquidCrystal.h>  
#define RELAY1 7
#define PULSE_PIN A0
#define TEMP_PIN A3

SoftwareSerial espSerial(8, 9); 
LiquidCrystal lcd(12, 11, 5, 4, 3, 2); 

const int PULSE_MIN = 60;   
const int PULSE_MAX = 120;  
const float TEMP_MAX = 40.0;   

void setup() {
  Serial.begin(115200);          
  espSerial.begin(115200);       
  lcd.begin(16, 2);  

  pinMode(RELAY1, OUTPUT);
  digitalWrite(RELAY1, LOW); 

  lcd.print("Smart Hospital");
  delay(2000);
  lcd.clear();

  Serial.println("Smart Hospital System Started");
  Serial.println("Component Readings:");
}

void loop() {

  int pulseRaw = analogRead(PULSE_PIN);
  int pulse = map(pulseRaw, 500, 1023, 50, 150);
  bool pulseAlert = (pulse < PULSE_MIN || pulse > PULSE_MAX);


  int tempRaw = analogRead(TEMP_PIN);
  float tempC = (tempRaw * 5.0 * 100.0) / 2300.0;
  bool tempAlert = (tempC > TEMP_MAX);


    lcd.setCursor(0, 0);  
    lcd.print("Pulse: ");
    lcd.print(pulse);
    lcd.print(" BPM ");
    lcd.setCursor(0, 1); 
    lcd.print("Temp: ");
    lcd.print(tempC, 1);
    lcd.print("C");



  Serial.println("---------------------");
  Serial.print("Pulse: ");
  Serial.print(pulse);
  Serial.print(" BPM (Alert: ");
  Serial.print(pulseAlert ? "YES" : "NO");
  Serial.println(")");

  Serial.print("Temperature: ");
  Serial.print(tempC, 1);
  Serial.print(" C (Alert: ");
  Serial.print(tempAlert ? "YES" : "NO");
  Serial.println(")");

  String data = String(pulse) + "," + String(tempC) + "," + String(pulseAlert) + "," + 
                String(tempAlert);
  espSerial.println(data);

  
  if (espSerial.available()) {
        String command = espSerial.readStringUntil('\n');
        if (command.startsWith("RELAY1,")) {
            int relayState = command.substring(7).toInt();
            digitalWrite(RELAY1, relayState);
            Serial.print("RELAY1 set to: ");
            Serial.println(relayState);
        }
    }

  delay(2000); 
}