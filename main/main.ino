/*************************************************************
  Blynk is a platform with iOS and Android apps to control
  ESP32, Arduino, Raspberry Pi and the likes over the Internet.
  You can easily build mobile and web interfaces for any
  projects by simply dragging and dropping widgets.

    Downloads, docs, tutorials: https://www.blynk.io
    Sketch generator:           https://examples.blynk.cc
    Blynk community:            https://community.blynk.cc
    Follow us:                  https://www.fb.com/blynkapp
                                https://twitter.com/blynk_app

  Blynk library is licensed under MIT license
 *************************************************************
  Blynk.Edgent implements:
  - Blynk.Inject - Dynamic WiFi credentials provisioning
  - Blynk.Air    - Over The Air firmware updates
  - Device state indication using a physical LED
  - Credentials reset using a physical Button
 *************************************************************/

/* Fill in information from your Blynk Template here */
/* Read more: https://bit.ly/BlynkInject */
#define BLYNK_TEMPLATE_ID "TMPLSDTZKbjy"
#define BLYNK_DEVICE_NAME "smart lamp"
//#define BLYNK_AUTH_TOKEN "zw7qBFJRKM_UVjMGz3qzJog-wpwoyT46"
#include "DHT.h"
#include <EEPROM.h>

#define dhtType DHT11
#define pin D2
float suhu,kelembaban;
#define lamp1 D1
#define lamp2 D2
#define lamp3 D3
#define lamp4 D4

#define BLYNK_FIRMWARE_VERSION        "0.1.0"

#define BLYNK_PRINT Serial
//#define BLYNK_DEBUG

#define APP_DEBUG

// Uncomment your board, or configure a custom board in Settings.h
//#define USE_SPARKFUN_BLYNK_BOARD
#define USE_NODE_MCU_BOARD
//#define USE_WITTY_CLOUD_BOARD
//#define USE_WEMOS_D1_MINI

#include "BlynkEdgent.h"
DHT dht(pin,dhtType);
//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
void setup()
{
  Serial.begin(115200);
  pinMode(lamp1,OUTPUT);
  pinMode(lamp2,OUTPUT);
  pinMode(lamp3,OUTPUT);
  pinMode(lamp4,OUTPUT);
  digitalWrite(lamp1,LOW);
  digitalWrite(lamp2,LOW);
  digitalWrite(lamp3,LOW);
  digitalWrite(lamp4,LOW);
  delay(100);
  dht.begin();
  EEPROM.begin(512);
  BlynkEdgent.begin();
}

void loop() {
  getSensor();
  BlynkEdgent.run();
}

void getSensor(){
  suhu = dht.readTemperature();
  kelembaban = dht.readHumidity();
  
}
