// https://heltec.org/project/wifi-kit-32-v3/
// https://lastminuteengineers.com/oled-display-esp8266-tutorial/?utm_content=cmp-true
// https://javl.github.io/image2cpp/
// https://randomnerdtutorials.com/esp32-http-get-post-arduino/
// https://heltec.org/wifi_kit_install/
// https://github.com/Heltec-Aaron-Lee/WiFi_Kit_series/releases/download/0.0.9/package_heltec_esp32_index.json
// https://github.com/HelTecAutomation/Heltec_ESP32/blob/master/src/oled/API.md

#include <Arduino.h>
#include "heltec.h"
#include "WiFi.h"
#include <HTTPClient.h>
#include "image.h"

String baasUrl = "https://script.google.com/macros/s/AKfycbz7SKkblP1c-T4ai_i-GQgsLpUFaKN0q_-ZIMfQzkS3AyTk7LJ2luTmL7sg4V87lsjy/exec";
String action = "?action=";
String scriptUrl = baasUrl + action;

#define f1p 3
#define f1k 1
#define f1r 2
#define f2p 6
#define f2k 5
#define f2r 4
#define f2p_jalg 7
#define f2r_jalg 19

int nr;  // img

void logo(int nr) {
  Heltec.display->clear();

  if (nr == 1) { Heltec.display->drawXbm(0, 0, 128, 53, (const unsigned char *)img_1); }
  if (nr == 2) { Heltec.display->drawXbm(0, 0, 128, 53, (const unsigned char *)img_2); }
  if (nr == 3) { Heltec.display->drawXbm(0, 0, 128, 53, (const unsigned char *)img_3); }
  if (nr == 4) { Heltec.display->drawXbm(0, 0, 128, 53, (const unsigned char *)img_4); }
  if (nr == 5) { Heltec.display->drawXbm(0, 0, 128, 53, (const unsigned char *)img_5); }
  if (nr == 6) { Heltec.display->drawXbm(0, 0, 128, 53, (const unsigned char *)img_6); }
  if (nr == 7) { Heltec.display->drawXbm(0, 0, 128, 53, (const unsigned char *)img_7); }
  if (nr == 8) { Heltec.display->drawXbm(0, 0, 128, 53, (const unsigned char *)img_8); }
  if (nr == 9) { Heltec.display->drawXbm(0, 0, 128, 53, (const unsigned char *)img_9); }
  if (nr == 10) { Heltec.display->drawXbm(0, 0, 128, 53, (const unsigned char *)img_10); }

  Heltec.display->display();
}


void setup() {
  Serial.begin(115200);
  Heltec.begin(true, false, true);
  WiFi.disconnect(true);
  delay(1000);
  WiFi.mode(WIFI_STA);
  WiFi.setAutoConnect(true);
  WiFi.begin("Saskia", "");
  delay(100);

  Heltec.display->flipScreenVertically();

  byte count = 0;
  while (WiFi.status() != WL_CONNECTED && count < 10) {
    count++;
    delay(500);
    Heltec.display->clear();
    Heltec.display->drawString(0, 0, "Connecting...");
    Heltec.display->display();
  }

  if (WiFi.status() == WL_CONNECTED) {
    Heltec.display->clear();
    Heltec.display->drawString(0, 0, "Connected.");
    Heltec.display->display();
  } else {
    Heltec.display->clear();
    Heltec.display->drawString(0, 0, "Failed to connect.");
    Heltec.display->display();
  }
  Heltec.display->clear();
  Heltec.display->drawString(0, 10, "WiFi Setup done.");
  Heltec.display->display();

  pinMode(f1p, OUTPUT);
  pinMode(f1k, OUTPUT);
  pinMode(f1r, OUTPUT);
  pinMode(f2p, OUTPUT);
  pinMode(f2k, OUTPUT);
  pinMode(f2r, OUTPUT);
  pinMode(f2p_jalg, OUTPUT);
  pinMode(f2r_jalg, OUTPUT);
}


void loop() {
  logo(random(1, 11));
  setFoor();
  getStatusUpdates();
}

/* BUS LOGIC */

void setFoor() {
  setAllLow();

  // 1.foor 1 stykkel

  // foor2 jalakäia roheliseks
  digitalWrite(f2r_jalg, HIGH);

  // foor 2 peab olema punane samal ajal
  digitalWrite(f2p, HIGH);

  digitalWrite(f1r, HIGH);
  delay(5000);
  digitalWrite(f1r, LOW);

  for (int i = 0; i < 5; i++) {
    digitalWrite(f1k, HIGH);
    digitalWrite(f2r_jalg, HIGH);
    delay(500);
    digitalWrite(f1k, LOW);
    digitalWrite(f2r_jalg, LOW);
    delay(500);
  }

  digitalWrite(f1p, HIGH);

  //jalakäia punaseks
  digitalWrite(f2r_jalg, LOW);
  digitalWrite(f2p_jalg, HIGH);

  delay(5000);

  // 2. foor 2 tsykkel
  digitalWrite(f2p, LOW);
  digitalWrite(f2r, HIGH);
  delay(5000);
  digitalWrite(f2r, LOW);

  for (int i = 0; i < 5; i++) {
    digitalWrite(f2k, HIGH);
    delay(500);
    digitalWrite(f2k, LOW);
    delay(500);
  }

  digitalWrite(f2p_jalg, HIGH);
  digitalWrite(f2p, HIGH);
  digitalWrite(f1p, HIGH);
}


/* GOOGLE APPSCRIPT REQUESTS */

void getStatusUpdates() {
  HTTPClient https;
  https.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);

  if (https.begin(scriptUrl + "queryUpdates")) {
    int httpCode = https.GET();
    Serial.println(String(httpCode));
    String payload = https.getString();
    Serial.println(payload);
    https.end();

    // Remove brackets and quotes
    payload.remove(0, 1);
    payload.remove(payload.length() - 1, 1);

    // Tokenize the string
    int commaIndex = payload.indexOf(',');

    // Extract values
    String f1 = payload.substring(1, commaIndex - 1);
    payload.remove(0, commaIndex + 1);

    commaIndex = payload.indexOf(',');
    String f2 = payload.substring(1, commaIndex - 1);
    payload.remove(0, commaIndex + 1);

    commaIndex = payload.indexOf(',');
    String f2j = payload.substring(1, commaIndex - 1);
    payload.remove(0, commaIndex + 1);
    int pilt = payload.toInt();

    // Serial.println(f1);
    // Serial.println(f2);
    // Serial.println(f2j);
    // Serial.println(String(pilt));

    Heltec.display->clear();
    Heltec.display->drawString(0, 0, "API actions:");
    Heltec.display->drawString(10, 12, "f1: " + f1);
    Heltec.display->drawString(10, 22, "f2: " + f2);
    Heltec.display->drawString(10, 32, "f2_jalg: " + f2j);
    Heltec.display->drawString(10, 42, "pilt: " + String(pilt));
    Heltec.display->display();

    if (f1 == "error" || f2 == "error" || f2j == "error") {
      Heltec.display->clear();
      Heltec.display->drawString(0, 0, "Fooride Error:");
      Heltec.display->drawString(10, 12, "Mingi foor läks rikki :(");
      Heltec.display->display();
      setAllLow();
      for (int i = 0; i < 8; i++) {
        digitalWrite(f1k, HIGH);
        digitalWrite(f2k, HIGH);
        delay(500);
        digitalWrite(f1k, LOW);
        digitalWrite(f2k, LOW);
        delay(500);
      }
    } else {

      // foor 1 actions
      if (f1 != "") {
        Heltec.display->clear();
        Heltec.display->drawString(0, 0, "Foor 1:");
        if (f1 == "roheline") {
          Heltec.display->drawString(10, 12, "roheline tsükkel");
          setAllLow();
          digitalWrite(f2r_jalg, HIGH);
          digitalWrite(f1r, HIGH);
          digitalWrite(f2p, HIGH);
          Heltec.display->display();
          delay(8000);
        } else if (f1 == "kollane") {
          Heltec.display->drawString(10, 12, "kollane tsükkel");
          setAllLow();
          digitalWrite(f2p_jalg, HIGH);
          digitalWrite(f2p, HIGH);
          Heltec.display->display();
          for (int i = 0; i < 10; i++) {
            digitalWrite(f1k, HIGH);
            delay(500);
            digitalWrite(f1k, LOW);
            delay(500);
          }
        } else if (f1 == "punane") {
          Heltec.display->drawString(10, 12, "punane tsükkel");
          setAllLow();
          digitalWrite(f2r_jalg, HIGH);
          digitalWrite(f1p, HIGH);
          digitalWrite(f2p, HIGH);
          Heltec.display->display();
          delay(8000);
        }
      }

      // foor 2 actions
      if (f2 != "") {
        Heltec.display->clear();
        Heltec.display->drawString(0, 0, "Foor 2:");
        if (f2 == "roheline") {
          Heltec.display->drawString(10, 12, "roheline tsükkel");
          setAllLow();
          digitalWrite(f2p_jalg, HIGH);
          digitalWrite(f1p, HIGH);
          digitalWrite(f2r, HIGH);
          Heltec.display->display();
          delay(8000);
        } else if (f2 == "kollane") {
          Heltec.display->drawString(10, 12, "kollane tsükkel");
          setAllLow();
          digitalWrite(f2p_jalg, HIGH);
          digitalWrite(f1p, HIGH);
          Heltec.display->display();
          for (int i = 0; i < 8; i++) {
            digitalWrite(f2k, HIGH);
            delay(500);
            digitalWrite(f2k, LOW);
            delay(500);
          }
        } else if (f2 == "punane") {
          Heltec.display->drawString(10, 12, "punane tsükkel");
          setAllLow();
          digitalWrite(f2p_jalg, HIGH);
          digitalWrite(f1p, HIGH);
          digitalWrite(f2p, HIGH);
          Heltec.display->display();
          delay(8000);
        }
      }

      if (f2j != "") {
        Heltec.display->clear();
        Heltec.display->drawString(0, 0, "Foor 2 jalakäia:");
        if (f2 == "roheline") {
          Heltec.display->drawString(10, 12, "roheline tsükkel");
          setAllLow();
          digitalWrite(f2p_jalg, LOW);
          digitalWrite(f2r_jalg, HIGH);
          digitalWrite(f1p, HIGH);
          digitalWrite(f2p, HIGH);
          Heltec.display->display();
          delay(8000);
        } else if (f2 == "punane") {
          Heltec.display->drawString(10, 12, "punane tsükkel");
          setAllLow();
          digitalWrite(f2p_jalg, HIGH);
          digitalWrite(f2r_jalg, LOW);

          digitalWrite(f1r, HIGH);
          digitalWrite(f2p, HIGH);
          Heltec.display->display();
          delay(8000);
        }
      }
    }

    // pilt action
    if (pilt > 0 && pilt <= 10) {
      setAllLow();
      digitalWrite(f1p, HIGH);
      digitalWrite(f2p, HIGH);
      digitalWrite(f2p_jalg, HIGH);

      Heltec.display->clear();
      Heltec.display->drawString(0, 0, "Reklaami plakat:");
      Heltec.display->drawString(10, 12, "Näitan pilti nr: " + String(pilt));
      delay(1500);
      logo(pilt);
      Heltec.display->display();
      delay(8000);
    }

  }
}

void setAllLow() {
  digitalWrite(f2p_jalg, LOW);
  digitalWrite(f2r_jalg, LOW);
  digitalWrite(f1p, LOW);
  digitalWrite(f1k, LOW);
  digitalWrite(f1r, LOW);
  digitalWrite(f2p, LOW);
  digitalWrite(f2k, LOW);
  digitalWrite(f2r, LOW);
}
