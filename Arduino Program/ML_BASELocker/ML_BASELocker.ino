#include <WiFi.h>
#include <Arduino.h>
#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif

#include <Firebase_ESP_Client.h>
//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"


const int ledPins[] = { 27, 13, 14, 15, 16, 17, 18, 19 };
const int relayPins[] = { 32, 12, 25, 26, 33, 4, 23, 26 };
const int IR = 35;
long oneMinutesTimer[] = { millis(), millis(), millis(), millis(), millis(), millis(), millis(), millis() };
long oneDayTimer[] = { millis(), millis(), millis(), millis(), millis(), millis(), millis(), millis() };

// Insert your network credentials
#define WIFI_SSID "unity"
#define WIFI_PASSWORD "dwl686168"

// Insert Firebase project API Key
#define API_KEY "AIzaSyDjF4fYgrQEyrIkMuL6xYqDpfMFldh9FvM"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://baselocker-b865d-default-rtdb.firebaseio.com"

//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
int intValue;
float floatValue;
bool signupOK = false;

String RecStatus = "";
String PrevStatus = "";

#define ON HIGH
#define OFF LOW


String getValue(String data, char separator, int index) {
  int found = 0;
  int strIndex[] = { 0, 0 };
  int maxIndex = data.length();
  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("ok");
    signupOK = true;
  } else {
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback;  //see addons/TokenHelper.h

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  pinMode(IR, INPUT_PULLUP);

  for (int i = 0; i < sizeof(ledPins) / sizeof(ledPins[0]); i++) {
    pinMode(ledPins[i], OUTPUT);
    pinMode(relayPins[i], OUTPUT);
  }

  Serial.println("setup complete");
}

void loop() {

  //status IR = Door Closed (1), Door Opened(0)

  if (Firebase.ready() && signupOK && micros() - sendDataPrevMillis > 1000000 || sendDataPrevMillis == 0) {
    sendDataPrevMillis = micros();
    Serial.println("Singed Up");
    if (Firebase.RTDB.getString(&fbdo, "/locker/BINUS%20ASO/Main%20Hallway/A/status")) {

      Serial.println("membaca");
      if (fbdo.dataType() == "string") {
        String nilai = fbdo.stringData();
        String RecStatus = nilai;
        Serial.println(RecStatus);

        for (int i = 0; i < sizeof(ledPins) / sizeof(ledPins[0]); i++) {
          if (RecStatus.charAt(i) == '1') {
            if (PrevStatus.charAt(i) == '0') {
              oneMinutesTimer[i] = micros();
              oneDayTimer[i] = micros();
            }
            digitalWrite(relayPins[i], HIGH);
            digitalWrite(ledPins[i], LOW);
            Serial.println("unlocking Relay");
            delay(500);

            int statusIR = digitalRead(IR);
            // int statusIR = Serial.parseInt();
            if (statusIR == 1) {

              Serial.println(statusIR);

              if ((micros() - oneMinutesTimer[i]) >= 15000000) {
                char temp[RecStatus.length() + 1];
                strcpy(temp, RecStatus.c_str());
                temp[i] = '0';
                RecStatus = temp;
                if (Firebase.RTDB.setString(&fbdo, "/locker/BINUS%20ASO/Main%20Hallway/A/status", RecStatus)) {
                  Serial.println("PASSED");
                  Serial.println("PATH: " + fbdo.dataPath());
                  Serial.println("TYPE: " + fbdo.dataType());
                } else {
                  Serial.println("FAILED");
                  Serial.println("REASON: " + fbdo.errorReason());
                }
                break;
              }
              unsigned long remainingTime = 15000000 - (micros() - oneMinutesTimer[i]);  // Calculate the remaining time in milliseconds

              Serial.print("Time Remaining :");  // Display the remaining time in seconds on the LCD

              Serial.println(remainingTime / 1000000);  // Display the remaining time in seconds on the LCD
            }
            if (statusIR == 0) {
              char temp[RecStatus.length() + 1];
              strcpy(temp, RecStatus.c_str());
              temp[i] = '2';
              RecStatus = temp;
              if (Firebase.RTDB.setString(&fbdo, "/locker/BINUS%20ASO/Main%20Hallway/A/status", RecStatus)) {
                oneMinutesTimer[i] = micros();
                Serial.println("PASSED");
                Serial.println("PATH: " + fbdo.dataPath());
                Serial.println("TYPE: " + fbdo.dataType());
              } else {
                Serial.println("FAILED");
                Serial.println("REASON: " + fbdo.errorReason());
              }
              break;
            }
          }
          if (RecStatus.charAt(i) == '2') {


            digitalWrite(relayPins[i], LOW);
            digitalWrite(ledPins[i], LOW);
            Serial.println("locking Relay");
            char temp[RecStatus.length() + 1];
            strcpy(temp, RecStatus.c_str());
            temp[i] = '4';
            RecStatus = temp;
            if (Firebase.RTDB.setString(&fbdo, "/locker/BINUS%20ASO/Main%20Hallway/A/status", RecStatus)) {
              Serial.println("PASSED");
              Serial.println("PATH: " + fbdo.dataPath());
              Serial.println("TYPE: " + fbdo.dataType());
            } else {
              Serial.println("FAILED");
              Serial.println("REASON: " + fbdo.errorReason());
            }
          }
          if (RecStatus.charAt(i) == '3') {
            digitalWrite(relayPins[i], HIGH);
            digitalWrite(ledPins[i], LOW);
            delay(500);
            Serial.println("Unlocking Relay");
            int statusIR = digitalRead(IR);
            // int statusIR = Serial.parseInt();

            if (statusIR == 1) {
              Serial.println(statusIR);
              digitalWrite(relayPins[i], HIGH);
              digitalWrite(ledPins[i], LOW);
              Serial.println("unlocking Relay");
            }
            if (statusIR == 0) {
              Serial.println(statusIR);
              digitalWrite(relayPins[i], LOW);
              digitalWrite(ledPins[i], LOW);
              Serial.println("locking Relay");
              char temp[RecStatus.length() + 1];
              strcpy(temp, RecStatus.c_str());
              temp[i] = '4';
              RecStatus = temp;
              if (Firebase.RTDB.setString(&fbdo, "/locker/BINUS%20ASO/Main%20Hallway/A/status", RecStatus)) {
                Serial.println("PASSED");
                Serial.println("PATH: " + fbdo.dataPath());
                Serial.println("TYPE: " + fbdo.dataType());
              } else {
                Serial.println("FAILED");
                Serial.println("REASON: " + fbdo.errorReason());
              }
            }
          }

          if (RecStatus.charAt(i) == '4') {
            digitalWrite(relayPins[i], LOW);
            digitalWrite(ledPins[i], LOW);
            Serial.println("locking Relay");
          }
          if (RecStatus.charAt(i) == '4' || RecStatus.charAt(i) == '3') {

            Serial.println("Menunggu");
            if ((micros() - oneDayTimer[i]) >= 86400000000) {
              char temp[RecStatus.length() + 1];
              strcpy(temp, RecStatus.c_str());
              temp[i] = '0';
              RecStatus = temp;
              if (Firebase.RTDB.setString(&fbdo, "/locker/BINUS%20ASO/Main%20Hallway/A/status", RecStatus)) {
                oneMinutesTimer[i] = micros();
                Serial.println("PASSED");
                Serial.println("PATH: " + fbdo.dataPath());
                Serial.println("TYPE: " + fbdo.dataType());
              } else {
                Serial.println("FAILED");
                Serial.println("REASON: " + fbdo.errorReason());
              }
              break;
            }
            unsigned long remainingTime = (86400000000 - (micros() - oneDayTimer[i]))/ 1000000;  // Calculate the remaining time in milliseconds

            unsigned long hours = remainingTime / 3600;
            unsigned long minutes = (remainingTime % 3600) / 60;
            unsigned long seconds = remainingTime % 60;

            // Format the time with leading zeros if needed
            Serial.print(String(hours) + ":" + String(minutes / 10) + String(minutes % 10) + ":" + String(seconds / 10) + String(seconds % 10));
          }

          if (RecStatus.charAt(i) == '0') {
            digitalWrite(relayPins[i], LOW);
            digitalWrite(ledPins[i], HIGH);
            Serial.println("locking Relay");
            oneDayTimer[i] = micros();
            oneMinutesTimer[i] = micros();
          }
          PrevStatus = RecStatus;
        }
      }
    } else {
      Serial.println("gagal membaca");
      Serial.println(fbdo.errorReason());
    }
    delay(1000);
  } else {
    Serial.println("gagal auth");
    Serial.println(fbdo.errorReason());
  }
  delay(1000);
}
