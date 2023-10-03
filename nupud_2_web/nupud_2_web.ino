#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>

#ifndef STASSID
#define STASSID "TLU"
#define STAPSK ""
#endif

const uint8_t fingerprint[20] = {0xBB, 0xB9, 0x27, 0xFB, 0x7D, 0xF3, 0xA7, 0x1A, 0x57, 0xCC, 0x23, 0xF8, 0x42, 0xE9, 0x10, 0xBE, 0x59, 0x7E, 0x1F, 0xD4};

ESP8266WiFiMulti WiFiMulti;
int pressed = 0;
int pressed2 = 0;
int question = 0;

void setup() {
  pinMode(16, INPUT); 
  pinMode(5, INPUT); 
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(STASSID, STAPSK);
  Serial.println("setup() done connecting to ssid '" STASSID "'");
}

void loop() {
  int val = digitalRead(16);
  int val2 = digitalRead(5);

  if(val == 0 && pressed == 1) {  pressed = 0; }
  if(val2 == 0 && pressed2 == 1) {  pressed2 = 0; }

  if (val > 0 && pressed != 1) {
    saadaVastus("1");
    pressed = 1;
  }

  if (val2 > 0 && pressed2 != 1) {
    saadaVastus("2");
    pressed2 = 1;
  }

}

void saadaVastus(String vastus) {
  if ((WiFiMulti.run() == WL_CONNECTED)) {
    std::unique_ptr<BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);
    //client->setFingerprint(fingerprint);
    client->setInsecure();
    HTTPClient https;

    Serial.println(vastus);

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
