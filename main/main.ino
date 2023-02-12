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

#include "DHT.h"
#include <EEPROM.h>

//#include "WidgetLED.h"

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
// token : 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
#define APP_DEBUG
#include "BlynkEdgent.h"
//#define GREEN     "#23C48E"
//#define BLUE      "#04C0F8"
//#define YELLOW    "#ED9D00"
//#define RED       "#D3435C"
//#define DARK_BLUE "#5F7CD8"

WidgetLED led1(V10); //lampu tl
//WidgetLED led2(V5); //lampu kerja
//WidgetLED led3(V7); //lampu gudang
//WidgetLED led4(V8); //lampu teras
//WidgetLED led5(V6); //lampu status
//WidgetLED led6(V6); //lampu warning
// Uncomment your board, or configure a custom board in Settings.h
//#define USE_SPARKFUN_BLYNK_BOARD
#define USE_NODE_MCU_BOARD
//#define USE_WITTY_CLOUD_BOARD
//#define USE_WEMOS_D1_MINI


DHT dht(pin,dhtType);

BLYNK_WRITE(V1)//lampuTL
{
  int value = param.asInt();
  if(value){digitalWrite(lamp1,LOW); led1.on();}
  else{digitalWrite(lamp1,HIGH); led1.off();}
}

BLYNK_WRITE(V0)//lampu kerja
{
  int value = param.asInt();
  if(value){digitalWrite(lamp2,LOW); }//led2.on();}
  else{digitalWrite(lamp2,HIGH); }//led2.off();}
}

BLYNK_WRITE(V2)//lampu gudang
{
  int value = param.asInt();
  if(value){digitalWrite(lamp3,LOW);}// led3.on();}
  else{digitalWrite(lamp3,HIGH); }//led3.off();}
}

BLYNK_WRITE(V3)//lampu teras
{
  int value = param.asInt();
  if(value){digitalWrite(lamp4,LOW); }//led4.on()}
  else{digitalWrite(lamp4,HIGH); }//led4.off();}
}

BLYNK_WRITE(V4)//reset
{
  int value = param.asInt();
  if(value){ delay(3000); ESP.restart();}
  
}

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
