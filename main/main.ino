#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <EEPROM.h>

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
#define VIN 3.3 // V power voltage, 3.3v in case of NodeMCU
#define R 10000 // Voltage devider resistor value
const int Analog_Pin = 0; // Analog pin A0

int LDR_Val; // Analog value from the LDR
int Iluminance; //Lux value

// wifimanager can run in a blocking mode or a non blocking mode
// Be sure to know how to process loops with no delay() if using non blocking
//bool wm_nonblocking = false; // change to true to use non blocking

WiFiManager wm; // global wm instance
WiFiManagerParameter custom_field; // global param ( for non blocking w params )

#include "BlynkEdgent.h"

BlynkTimer timer;
BlynkTimer timer2;
int led_pin = D7;
#define N_DIMMERS 3
int dimmer_pin[] = { D0, D0, 15 };
#define led1 D4
#define led2 D2
#define led3 D1

int  stateLed1;
int  stateLed2;
int  stateLed3;
int  TIMER = 0;
bool stateWifi;
bool wm_nonblocking = false;

WidgetLED led(V5);

#define BLYNK_GREEN     "#23C48E"
#define BLYNK_BLUE      "#04C0F8"
#define BLYNK_YELLOW    "#ED9D00"
#define BLYNK_RED       "#D3435C"
#define BLYNK_DARK_BLUE "#5F7CD8"

BLYNK_CONNECTED() 
{
  // Turn LED on, so colors are visible
  led.on();
}

void myTimerEvent()
{
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V4, Iluminance);
  
}

void sendT(){
  Blynk.virtualWrite(V3, LDR_Val);
}

BLYNK_WRITE(V0)
{
  stateLed1 = param.asInt(); // assigning incoming value from pin V1 to a variable
  EEPROM.write(stateLed1,1);
  EEPROM.commit();
  Serial.println(stateLed1);
  // process received value
}

BLYNK_WRITE(V1)
{
  stateLed2 = param.asInt(); // assigning incoming value from pin V1 to a variable
  EEPROM.write(stateLed2,2);
  EEPROM.commit();
  // process received value
}

BLYNK_WRITE(V2)
{
  stateLed3 = param.asInt(); // assigning incoming value from pin V1 to a variable
  EEPROM.write(stateLed3,3);
  EEPROM.commit();
  // process received value
}
void setup()
{
  Serial.begin(115200);
  EEPROM.begin(512);
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
  //WiFi.mode(WIFI_STA);
  stateWifi = EEPROM.read(0);
  if(wm_nonblocking) wm.setConfigPortalBlocking(false);

  if(stateWifi){
    wm.setConfigPortalTimeout(60);
    bool stateWifi = wm.autoConnect("CONTROLLER", "00000000");
    if(!stateWifi){
      stateWifi = 0;
      delay(1000);
      EEPROM.write(0,stateWifi);
      EEPROM.commit();
//      for(int i =0; i < 5;i++)
//      {
//        digitalWrite(BUZZ,HIGH);
//        delay(50);
//        digitalWrite(BUZZ,LOW);
//        delay(50);
//      }
      ESP.restart();
    }
    else
    {
      stateLed1 = EEPROM.read(1);
      stateLed2 = EEPROM.read(2);
      stateLed3 = EEPROM.read(3);

      Blynk.virtualWrite(V0,stateLed1);
      Blynk.virtualWrite(V1,stateLed2);
      Blynk.virtualWrite(V2,stateLed3);
      
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
      timer.setInterval(1000L, myTimerEvent);
      timer2.setInterval(1000L, sendT);
    }
  }
  else
  {
    stateLed1 = EEPROM.read(1);
    stateLed2 = EEPROM.read(2);
    stateLed3 = EEPROM.read(3);
  }

}

void loop() {
  LDR_Val = analogRead(Analog_Pin);
  Iluminance = conversion(LDR_Val);
  
  if(stateWifi){
    ArduinoOTA.handle();
    BlynkEdgent.run();
    timer.run();
    timer2.run();
    stateWIFI();
  }
  else
  {
    kalkulasi();
  }
  
  
  if(stateLed1 == 1){ digitalWrite( led1,LOW);}
  else{ digitalWrite( led1,HIGH); }

  if(stateLed2 == 1){ digitalWrite( led2,LOW);}
  else{ digitalWrite( led2,HIGH); }

  if(stateLed3 == 1){ digitalWrite( led3,LOW);}
  else{ digitalWrite( led3,HIGH); }
}

void stateWIFI() {

  unsigned long tmr = millis();
  static int tmrWarning=0;
  const int delayWarning = 500;
  if(stateWifi){
    if (WiFi.status() != WL_CONNECTED) {
      if (tmr - tmrWarning > delayWarning) {
        tmrWarning = tmr;
        TIMER++;
        // if(TIMER <= 10)
        // {
        //   if(TIMER % 2){buzzer(1);}//digitalWrite(BUZZ,HIGH);}
        //   else{buzzer(0);}//digitalWrite(BUZZ,LOW);}
        // }
        if(TIMER >= 30){//rubah ini jika dirasa waktu tunda pergantian mode wifi saat disconnect kurang/lebih
          stateWifi = 0;
          EEPROM.write(0,stateWifi);
          EEPROM.commit();
          ESP.restart();
        }
        Serial.println(String() + "Timer:" + TIMER);
      }
    }
  }
}

int conversion(int raw_val){
  // Conversion rule
  float Vout = float(raw_val) * (VIN / float(1023));// Conversion analog to voltage
  float RLDR = (R * (VIN - Vout))/Vout; // Conversion voltage to resistance
  int lux = 500/(RLDR/1000); // Conversion resitance to lumen
  return lux;
}

void kalkulasi(){
  if(LDR_Val <= 500 ){
    stateLed2 = 1;
    stateLed3 = 1;
    EEPROM.write(2,stateLed2);
    EEPROM.write(3,stateLed3);
    EEPROM.commit();
  }
}
