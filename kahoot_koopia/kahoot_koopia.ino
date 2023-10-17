#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>

#ifndef STASSID
#define STASSID "Saskia"
#define STAPSK ""
#endif

ESP8266WiFiMulti WiFiMulti;

int punaneNupp   = 0;
int rohelineNupp = 0;
int punaneNuppVajutatud   = 0;
int rohelineNuppVajutatud = 0;

String macAddr = "";

String baasUrl = "https://script.google.com/macros/s/AKfycbxXlIVdFPoFxEnNnYntlZyky0raguYJWBVmIvUfrj5El-1XvFZNaDIUB9K4Zd5XAq3umA/exec";
String action = "?action=";
String scriptUrl = baasUrl + action;

void setup() {
  Serial.begin(115200);
  Serial.flush();
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(STASSID, STAPSK);

  pinMode(14, INPUT);  // D5 ehk punane
  pinMode(12, INPUT);  // D3 ehk roheline
  pinMode(16, OUTPUT); // D0  ehk punane led
  pinMode(5, OUTPUT);  // D1 ehk roheline led

  Serial.println("Connected to WiFi Network");
  macAddr = WiFi.BSSIDstr();
  Serial.println("Mac addr: " + macAddr);

  saadaPultiMacAddr();
}

void loop() {
  punaneNupp = digitalRead(14);
  rohelineNupp = digitalRead(12);

  if (punaneNupp == LOW && punaneNuppVajutatud == 1) { punaneNuppVajutatud = 0; }
  if (rohelineNupp == LOW && rohelineNuppVajutatud == 1) { rohelineNuppVajutatud = 0; }

  if (punaneNupp == HIGH && punaneNuppVajutatud == 0) {
    saadaVastus("1");
    punaneNuppVajutatud = 1;
  }

  if (rohelineNupp == HIGH && rohelineNuppVajutatud == 0) {
    saadaVastus("2");
    rohelineNuppVajutatud = 1;
  }

}

void saadaVastus(String vastus) {
  if (WiFiMulti.run() == WL_CONNECTED) {
    digitalWrite(5, HIGH);
    digitalWrite(16, HIGH);
    std::unique_ptr<BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);
    HTTPClient https;

    client->setInsecure();
    https.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);

    if (https.begin(*client, scriptUrl + "salvestaNupp&nuppNr=" + vastus + "&macAddr=" + macAddr)) {
      int httpCode = https.GET();        
      String payload = https.getString();
      Serial.println(payload);
      https.end();

      if (payload == "false") {
        digitalWrite(5, LOW);
        pulsateLed(16);
      } else if (payload == "true") {
        digitalWrite(16, LOW);
        pulsateLed(5);
      } else if (payload != "true" || payload != "false") {
        digitalWrite(16, LOW);
        pulsateTwoLeds(5, 16);
      }

    } 

  }
}

void saadaPultiMacAddr() {
  if (WiFiMulti.run() == WL_CONNECTED) {
    digitalWrite(5, HIGH);
    digitalWrite(16, HIGH);
    std::unique_ptr<BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);
    HTTPClient https;

    client->setInsecure();
    https.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);

    if (https.begin(*client, scriptUrl + "salvestaPult&macAddr=" + macAddr)) {
      int httpCode = https.GET();        
      String payload = https.getString();
      Serial.println(payload);
      https.end();
      if (payload != "saved") {
        digitalWrite(5, LOW);
      }
      if (payload == "saved") {
        digitalWrite(16, LOW);
      }

      delay(1500);
      digitalWrite(16, LOW);
      digitalWrite(5, LOW);
      
    } 
  }
}

void pulsateLed(int ledPin) {
    for (int i = 0; i < 4; i++) {
    easeLed(ledPin); 
    delay(125);      
  }
}

void easeLed(int ledPin) {
  int bright = 0;
  int time = 50;
  int steps = 20;

  for (int i=0; i<=steps; i++) {
    bright = i*int(255/steps);
    analogWrite(ledPin, bright);
    delay(time);
  }

  bright = 255;
  for (int i=steps; i>=0; i--) {
    bright = i*int(255/steps);
    analogWrite(ledPin, bright);
    delay(time);
  }
}

void pulsateTwoLeds(int ledPin, int ledPin2) {
    for (int i = 0; i < 4; i++) {
    easeTwoLeds(ledPin, ledPin2); 
    delay(125);      
  }
}

void easeTwoLeds(int ledPin, int ledPin2) {
  int bright = 0;
  int time = 50;
  int steps = 20;

  for (int i=0; i<=steps; i++) {
    bright = i*int(255/steps);
    analogWrite(ledPin, bright);
    analogWrite(ledPin2, bright);
    delay(time);
  }

  bright = 255;
  for (int i=steps; i>=0; i--) {
    bright = i*int(255/steps);
    analogWrite(ledPin, bright);
    analogWrite(ledPin2, bright);
    delay(time);
  }
}

