#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>

const char *SSID = "ssid"; //Mit Netzwerk-SSID ersetzen
const char *PASSWORD = "password"; //Mit Netzwerk-passwort ersetzen
const char *SERVERURL = "localhost:80";


void setup() {
  Serial.begin(115200); //Seriellen Monitor beginnen
      WiFi.begin(SSID, PASSWORD); //Mit WLAN-Netzwerk verbinden
      //Warten, bis mit WLAN verbunden wurde
      while (WiFi.status() != WL_CONNECTED)
      {
            delay(1000);
            Serial.println("Verbinde mit WLAN...");
      }
      Serial.println("Mit WLAN verbunden  ");


}

void loop() {
  long currTime = millis(); //Wie lang der ESP schon läuft in ms
  int BPM = 0; //Derzeitige Herzfrequenz
  int skinResistance = 0; //Derzeitiger Hautwiderstand

  //Daten senden per HTTP request (Die Daten werden später aus der URL geschlossen)
  HTTPClient http; 
  String url = String(SERVERURL) + "/bpm/" + String(BPM) + "/skinResistance/" + String(skinResistance)+"/currentTime/" + String(currTime); //Request aufbauen
  http.begin(url);
  int httpCode = http.GET();
}