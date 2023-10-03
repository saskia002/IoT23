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
int punaneNuppVajutatud = 0;
int rohelineNuppVajutatud = 0;
int question = 0;

void setup() {
  pinMode(4, INPUT);  // D2 ehk punane
  pinMode(0, INPUT);  // D3 ehk roheline
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(STASSID, STAPSK);
  Serial.println("setup() done connecting to ssid '" STASSID "'");
}

void loop() {
  int punaneNupp = digitalRead(4);
  int rohelineNupp = digitalRead(0);

  if(punaneNupp == 0 && punaneNuppVajutatud == 1) {  punaneNuppVajutatud = 0; }
  if(rohelineNupp == 0 && rohelineNuppVajutatud == 1) {  rohelineNuppVajutatud = 0; }

  if (punaneNupp > 0 && punaneNuppVajutatud != 1) {
    //saadaVastus("1");
    punaneNuppVajutatud = 1;
    Serial.print("1");
  }

  if (rohelineNupp > 0 && rohelineNuppVajutatud != 1) {
    //saadaVastus("2");
    rohelineNuppVajutatud = 1;
    Serial.print("2");
  }



}

void saadaVastus(String vastus) {
  if ((WiFiMulti.run() == WL_CONNECTED)) {
    std::unique_ptr<BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);
    client->setInsecure();
    HTTPClient https;

    if (https.begin(*client, "https://script.google.com/macros/s/AKfycbwTzgGf5GyEz4tHNHJlFAmu4YrHnkpktr-oD-SO0bhDRIOEoHKyhtDWh5V3L5-0_Ny5Fw/exec?lamp="+ vastus)) {  // HTTPS
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
