#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>

#ifndef STASSID
#define STASSID "TLU"
#define STAPSK ""
#endif

ESP8266WiFiMulti WiFiMulti;

int punaneNupp   = 0;
int rohelineNupp = 0;
int punaneNuppVajutatud   = 0;
int rohelineNuppVajutatud = 0;

int question = 0;

void setup() {
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(STASSID, STAPSK);

  pinMode(14, INPUT);  // D5 ehk punane
  pinMode(12, INPUT);  // D3 ehk roheline
  pinMode(16, OUTPUT); // D0  ehk punane led
  pinMode(5, OUTPUT);  // D1 ehk roheline led
}

void loop() {
  punaneNupp = digitalRead(14);
  rohelineNupp = digitalRead(12);

  if (punaneNupp == LOW && punaneNuppVajutatud == 1) { punaneNuppVajutatud = 0; }
  if (rohelineNupp == LOW && rohelineNuppVajutatud == 1) { rohelineNuppVajutatud = 0; }

  if (punaneNupp == HIGH && punaneNuppVajutatud == 0) {
    //saadaVastus("1");
    punaneNuppVajutatud = 1;
    digitalWrite(16, HIGH);
    Serial.print("1");
  }

  if (rohelineNupp == HIGH && rohelineNuppVajutatud == 0) {
    //saadaVastus("2");
    rohelineNuppVajutatud = 1;
    digitalWrite(5, HIGH);
    Serial.print("2");
  }

  delay(500);
  digitalWrite(5, LOW);
  digitalWrite(16, LOW);
}

void saadaVastus(String vastus) {
  if ((WiFiMulti.run() == WL_CONNECTED)) {
    std::unique_ptr<BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);
    client->setInsecure();
    HTTPClient https;

    if (https.begin(*client, "https://script.google.com/macros/s/AKfycbwTzgGf5GyEz4tHNHJlFAmu4YrHnkpktr-oD-SO0bhDRIOEoHKyhtDWh5V3L5-0_Ny5Fw/exec?lamp=" + vastus)) {  // HTTPS
      int httpCode = https.GET();
      Serial.println(httpCode);

      https.end();
    } else {
      Serial.printf("[HTTPS] Unable to connect\n");
    }
  } else {
    Serial.println("error :(");
  }
}
