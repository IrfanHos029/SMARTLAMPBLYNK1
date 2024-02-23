#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#define BLYNK_TEMPLATE_ID "TMPL6S0qNZ3a4"
#define BLYNK_TEMPLATE_NAME "smartLamp"

#ifndef STASSID
#define STASSID "Wifi"
#define STAPSK "00000000"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;
const char* host = "OTA-LEDS";

#define BLYNK_FIRMWARE_VERSION        "0.1.0"

#define BLYNK_PRINT Serial
//#define BLYNK_DEBUG

#define APP_DEBUG

#define USE_NODE_MCU_BOARD
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager

#define TRIGGER_PIN 0

// wifimanager can run in a blocking mode or a non blocking mode
// Be sure to know how to process loops with no delay() if using non blocking
bool wm_nonblocking = false; // change to true to use non blocking

WiFiManager wm; // global wm instance
WiFiManagerParameter custom_field; // global param ( for non blocking w params )

#include "BlynkEdgent.h"
int led_pin = D7;
#define N_DIMMERS 3
int dimmer_pin[] = { D0, D0, 15 };
#define led1 D4
#define led2 D2
#define led3 D1

int stateLed1;
int stateLed2;
int stateLed3;

WidgetLED led(V3);

#define BLYNK_GREEN     "#23C48E"
#define BLYNK_BLUE      "#04C0F8"
#define BLYNK_YELLOW    "#ED9D00"
#define BLYNK_RED       "#D3435C"
#define BLYNK_DARK_BLUE "#5F7CD8"

BLYNK_CONNECTED() {
  // Turn LED on, so colors are visible
  led.on();
}

BLYNK_WRITE(V0)
{
  stateLed1 = param.asInt(); // assigning incoming value from pin V1 to a variable
  Serial.println(stateLed1);
  // process received value
}

BLYNK_WRITE(V1)
{
  stateLed2 = param.asInt(); // assigning incoming value from pin V1 to a variable

  // process received value
}

BLYNK_WRITE(V2)
{
  stateLed3 = param.asInt(); // assigning incoming value from pin V1 to a variable

  // process received value
}
void setup()
{
  Serial.begin(115200);
  delay(100);
  /* switch on led */
  pinMode(led_pin, OUTPUT);
  digitalWrite(led_pin, LOW);
  digitalWrite(led1, HIGH);
  digitalWrite(led2, HIGH);
  digitalWrite(led3, HIGH);
  pinMode(led1,OUTPUT);
  pinMode(led2,OUTPUT);
  pinMode(led3,OUTPUT);
  Serial.println("Booting");
  WiFi.mode(WIFI_STA);

  //WiFi.begin(ssid, password);
    bool res;
    // res = wm.autoConnect(); // auto generated AP name from chipid
    // res = wm.autoConnect("AutoConnectAP"); // anonymous ap
    res = wm.autoConnect("AutoConnectAP","password"); // password protected ap

    if(!res) {
        Serial.println("Failed to connect");
        // ESP.restart();
    } 
    else {
        //if you get here you have connected to the WiFi    
        Serial.println("connected...yeey :)");
    }
    while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    WiFi.begin(ssid, password);
    Serial.println("Retrying connection...");
    }
  /* switch off led */
  digitalWrite(led_pin, HIGH);

  /* configure dimmers, and OTA server events */
  analogWriteRange(1000);
  analogWrite(led_pin, 990);

  for (int i = 0; i < N_DIMMERS; i++) {
    pinMode(dimmer_pin[i], OUTPUT);
    analogWrite(dimmer_pin[i], 50);
  }

  ArduinoOTA.setHostname(host);
  ArduinoOTA.onStart([]() {  // switch off all the PWMs during upgrade
    for (int i = 0; i < N_DIMMERS; i++) {
      analogWrite(dimmer_pin[i], 0);
    }
    analogWrite(led_pin, 0);
  });

  ArduinoOTA.onEnd([]() {  // do a fancy thing with our board led at end
    for (int i = 0; i < 30; i++) {
      analogWrite(led_pin, (i * 100) % 1001);
      delay(50);
    }
  });

  ArduinoOTA.onError([](ota_error_t error) {
    (void)error;
    ESP.restart();
  });

  /* setup the OTA server */
  ArduinoOTA.begin();
  Serial.println("Ready");
  BlynkEdgent.begin();
}

void loop() {
   ArduinoOTA.handle();
  BlynkEdgent.run();

  if(stateLed1 == 1){ digitalWrite( led1,HIGH);}
  else{ digitalWrite( led1,LOW); }

  if(stateLed2 == 1){ digitalWrite( led2,HIGH);}
  else{ digitalWrite( led2,LOW); }

  if(stateLed3 == 1){ digitalWrite( led3,HIGH);}
  else{ digitalWrite( led3,LOW); }
}
