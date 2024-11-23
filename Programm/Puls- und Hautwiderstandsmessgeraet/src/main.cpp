#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <math.h>

//WLAN und HTTP-Konstanten
const char *SSID = "SSID"; //Mit Netzwerk-SSID ersetzen
const char *PASSWORD = "PASSWORD"; //Mit Netzwerk-Passwort ersetzen
const char *SERVERURL = "http://0.0.0.0:8000"; //Mit Messlaptop-IP im öokalen Netzwerk ersetzen
const int ESP_ID = 1; //ID des ESPs
//Herzfrequenz-Konstanten
const float THRESH_FACTOR = 0.7; //Schwelle (80% der Differenz), über der ein Herzschlag registriert wird
const int FREQ_MIN_DIFF = 10; //Minimale Differenz des Minimums und Maximums des Wertebereichs bei der Herzfrequenzmessung 
const int FREQ_SAMPLES = 250; //Anzahl der Messungen in einer Herzfrequenz-Messreihe
//Widerstandsberechnungs-Konstanten
const float KNOWN_RESISTANCE = 68000; //Widerstand in der Schaltung
const float VIN = 3.3; //Spannung in der Widerstandsmessung
//Herzfrequenz-Variablen
int freqSampleBuffer[FREQ_SAMPLES]; //Derzeitige Herzfrequenz-Messreihe mit der länge oben
int freqBufferIndex = 0; //Nummer der letzten Herzfrequenz-Messung
long lastTime = 0; //Timestamp der letzten Herzfrequenz-Messreihen-Auswertung 
int pulse = 0; //Derzeitige Herzfrequenz in BPM
//Widerstandsberechnungs-Variablen
int skinResistance = 0; //Derzeitiger Hautwiderstand in Ohm
//Pins
#define FREQUENCY_MEASUREMENT_PIN 32 //Herzfrequenzsensor-Pin
#define RESISTANCE_MEASUREMENT_PIN 33 //Hautwiderstandssensor-Pin


void setup() {
  Serial.begin(115200); //Seriellen Monitor beginnen
  float currTime = millis(); //Timestamp des setups registrieren
  // --WLAN-Verbindung--
  WiFi.begin(SSID, PASSWORD); //Mit WLAN-Netzwerk verbinden
  // Warten, bis mit WLAN verbunden wurde
  while (WiFi.status() != WL_CONNECTED)
  {
        delay(500);
        Serial.print("Verbinde mit WLAN, SSID: ");
        Serial.println(SSID);
  }
  Serial.println("Mit WLAN verbunden!");
  HTTPClient http; 
  String url = String(SERVERURL) + "/connected/" + String(ESP_ID) + "/time/" + String(currTime); //Verbindungsankündigung-Reqeust
  http.begin(url); 
  http.GET(); //Request absenden

}

void loop() {
  //Zeit
  float currTime = millis(); //Wie lang der ESP bereits läuft

  //-- HERZFREQUENZMESSUNG --
  freqSampleBuffer[freqBufferIndex] = analogRead(FREQUENCY_MEASUREMENT_PIN); //Messung in Messreihe eintragen

  //Messreihe alle 5 Sekunden in BPM anzeigen
  if(millis()-lastTime > 500 && freqBufferIndex == FREQ_SAMPLES-1) {
    lastTime = millis(); //Timestamp der letzten Messung 
    //Analoger Eingang grenzwerte
    int maxValue = 0;
    int minValue = 4096;
    // Calculate maximum and minimum
    for(int i = 0; i < FREQ_SAMPLES; i++) {
      maxValue = max(freqSampleBuffer[i], maxValue);
      minValue = min(freqSampleBuffer[i], minValue); 
    }
  
    float diff = max(maxValue - minValue, FREQ_MIN_DIFF); //Wertebereich ausrechnen
    float threshold = diff * THRESH_FACTOR + minValue; //Herzschlagschwelle für den derzeitigen Wertebereich ausrechnen
    int nHeartbeats = 0; //Menge der Herzschläge in dem derzeitigen Buffer
    int tHeartbeats = 0; //Keine Ahnung
    int lastHeartbeat = 0; //Timestamp des letzten Herzschlags
    //Herzschläge zählen
    for(int i = 1; i < FREQ_SAMPLES; i++) {
      if(freqSampleBuffer[(freqBufferIndex+i+1)%FREQ_SAMPLES] >= threshold 
          && freqSampleBuffer[(freqBufferIndex+i)%FREQ_SAMPLES] < threshold) {
        //Überprüfen, ob der letzte Herzschlag zu kurz her oder zu lang her ist
        if(lastHeartbeat && i-lastHeartbeat > 5 && i-lastHeartbeat < 150) {
          tHeartbeats += i-lastHeartbeat; //???
          nHeartbeats++; //Herzschläge inkrementieren
        }
        //Timestamp des letzten Herzschlags implementieren
        lastHeartbeat = i;
      }
    }
    float bpm = (60000.0 * nHeartbeats / (tHeartbeats * 20) )/2; //BPM berechnen; Schwarze Magie
    //Ergebnisse Ausgeben wenn genug Herzschläge gefunden werden
    if(nHeartbeats > 3) {
       pulse  = bpm; //Puls ausgeben wenn genug Herzschläge gefunden werden
    }
  }
  freqBufferIndex = (freqBufferIndex+1) % FREQ_SAMPLES; //Index inkrementieren
  //-- HAUTWIDERSTANDSMESSUNG --

  float rawResistance = analogRead(RESISTANCE_MEASUREMENT_PIN);
  float vOut = 0; //Zwischenvariable in der Widerstandsberechnung
  float resistanceBuffer = 0; //Zwischencariable in der Widerstandsberechnung
  int resistanceCalculated = 0; //Berechneter Widerstans

  if (rawResistance > 0 && rawResistance < 4095)
  {
    //Widerstand berechnen
    resistanceBuffer = rawResistance * VIN; 
    vOut = (resistanceBuffer)/4096.0;
    resistanceBuffer = (VIN/vOut) -1;
    resistanceCalculated = KNOWN_RESISTANCE * resistanceBuffer;
    skinResistance = resistanceCalculated;
  }else{
    skinResistance = 0; //0 schicken wenn die Messung zu niedrig ist
    if (rawResistance > 4095){
      skinResistance = 2000000; //2M schicken wenn die Messung zu hoch ist
    }
  }
  //Ergebnisse an Seriellen Monitor ausgeben
  Serial.print("Resistance: ");
  Serial.print(skinResistance);
  Serial.print("; Pulse: ");
  Serial.print(pulse);
  Serial.println();
  //-- Daten Senden --
  //Daten senden per HTTP request (Die Daten werden später auf dem Server aus der URL geschlossen)
  HTTPClient http; 
  String url = String(SERVERURL) + "/bpm/" + String(pulse) + "/skinResistance/" + String(skinResistance)+"/currentTime/" + String(currTime) + "/clientID/" + String(ESP_ID); //Request aufbauen
  http.begin(url); 
  http.GET(); //Request absenden
  delay(20); //Alle 20ms Messungen machen & Senden
}