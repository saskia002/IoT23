#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include "certs.h"

#ifndef STASSID
#define STASSID "TLU"
#define STAPSK ""
#endif

// https://docs.google.com/spreadsheets/d/1AOGNGkq4ZolAH_v6hzdiCDFYPg5B-xu7vjTqIEWlXr0/export?format=csv

ESP8266WiFiMulti WiFiMulti;

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(STASSID, STAPSK);
  Serial.println("setup() done connecting to ssid '" STASSID "'");
}

// const uint8_t finger[20] = {0x9D, 0x80, 0x9B, 0xCF, 0x63, 0xAA, 0x86, 0x29, 0xE9, 0x3C, 0x78, 0x9A, 0xEA, 0xDA, 0x15, 0x56, 0x7E, 0xBF, 0x56, 0xD8};

void loop() {
  int nr = analogRead(A0);
  Serial.println(nr);

  // wait for WiFi connection
  if ((WiFiMulti.run() == WL_CONNECTED)) {
    std::unique_ptr<BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);
    client->setInsecure();
    HTTPClient https;

    Serial.print("[HTTPS] begin...\n");
    if (https.begin(*client, "https://docs.google.com/spreadsheets/d/1AOGNGkq4ZolAH_v6hzdiCDFYPg5B-xu7vjTqIEWlXr0/gviz/tq?tqx=out:csv&sheet=Leht1")) {  // HTTPS

      Serial.print("[HTTPS] GET...\n");
      int httpCode = https.GET();

      if (httpCode > 0) {
        Serial.printf("[HTTPS] GET... code: %d\n", httpCode);

        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          String payload = https.getString();
          payload.trim();
          payload.toLowerCase();
          payload.replace("\"", "");
          Serial.println(payload);
        }
      } else {
        Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
      }

      https.end();
    } else {
      Serial.printf("[HTTPS] Unable to connect\n");
    }
  }

  delay(5000);
}
