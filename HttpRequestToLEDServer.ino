/**
 * Code to check Flask api and see if my resume site has had any visits
 * Turns LED on for a second if there was a visitor since the last check
 * Based on BasicHTTPClient.ino
 * Additional code by Tom Hastings 
 * Blog post: https://www.circusscientist.com/2021/08/18/i-made-an-led-indicator-for-my-portfolio-site/
*/

#define led D4 //built in LED on my D1 mini
#include <Arduino.h>
#include <EEPROM.h>

int eepromAddr = 0;
byte visits;

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>

#include <WiFiClient.h>

ESP8266WiFiMulti WiFiMulti;

void setup() {
  pinMode(led, OUTPUT);
  digitalWrite(led, HIGH); //HIGH is off for D1 mini

  visits = EEPROM.read(eepromAddr); 
  delay(100); //wait for EEPROM.read
  Serial.begin(115200);
  // Serial.setDebugOutput(true);
  Serial.println("");
  Serial.print("saved visits: ");
  Serial.println(visits);
  Serial.println();
  Serial.println();
  Serial.println();

  for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP("YOUR-SSID", "PASSWORD");

}

void loop() {
  // wait for WiFi connection
  if ((WiFiMulti.run() == WL_CONNECTED)) {
    WiFiClient client;

    HTTPClient http;

    Serial.print("[HTTP] begin...\n");
    if (http.begin(client, "http://flask_service.com")) {  // HTTP

      Serial.print("[HTTP] GET...\n");
      // start connection and send HTTP header
      int httpCode = http.GET();

      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTP] GET... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          String payload = http.getString();
          String pressed = "pressed";
          if(payload.indexOf(pressed) > -1){
            Serial.print("got it: ");
            Serial.println(payload);
            digitalWrite(led, LOW); //LOW is on for LED_BUILTIN
            visits = visits + 1;
            Serial.print("saving visitor number: ");
            Serial.println(visits);
            EEPROM.write(eepromAddr, visits);
            delay(5000); 
          } else {
            Serial.print("not yet: ");
            Serial.println(payload);
            digitalWrite(led, HIGH);
            delay(10); 
          }
          
        }
      } else {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }

      http.end();
    } else {
      Serial.printf("[HTTP} Unable to connect\n");
    }
  }

  delay(10000);
  
}