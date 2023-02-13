
#include "DHT.h"
#include <EEPROM.h>
#include <NTPClient.h>
//#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#define dhtType DHT11
#define pin D6
float suhu,kelembaban;
#define lamp1 D1
#define lamp2 D2
#define lamp3 D4
#define lamp4 D5
#define ind1 D7
#define ind2 D0

#define BLYNK_FIRMWARE_VERSION        "0.1.0"

#define BLYNK_PRINT Serial
//#define BLYNK_DEBUG
// token : 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
#define APP_DEBUG
#include "BlynkEdgent.h"
#define GREEN     "#23C48E"
#define BLUE      "#04C0F8"
#define YELLOW    "#ED9D00"
#define RED       "#D3435C"
#define DARK_BLUE "#5F7CD8"

WidgetLED led1(V5); //lampu kerja
WidgetLED led2(V6); //lampu tl
WidgetLED led3(V7); //lampu teras
WidgetLED led4(V8); //lampu gudang
WidgetLED led5(V9); //lampu status
WidgetLED led6(V12); //lampu warning


#define USE_NODE_MCU_BOARD

const long utcOffsetInSeconds = 25200;
WiFiUDP ntpUDP;
NTPClient Clock(ntpUDP, "asia.pool.ntp.org", utcOffsetInSeconds);

int jam,menit,detik;
int jamStart,menStart,detStart;
int jamStop,menStop,detStop;
int valLamp1,valLamp2,valLamp3,valLamp4,Reset;
//display suhu : V10
//display kelem : v11
int valIdx[4];
DHT dht(pin,dhtType);

BLYNK_CONNECTED()
{
    led5.setColor(RED);
    led5.on();

}

BLYNK_WRITE(V18)
{
  
  TimeInputParam t(param);

  // Process start time

  if (t.hasStartTime())
  {
    Serial.println(String("Start: ") +
                   t.getStartHour() + ":" +
                   t.getStartMinute() + ":" +
                   t.getStartSecond());
    jamStart = t.getStartHour();
    menStart = t.getStartMinute();
    detStart = t.getStartSecond();
  }
  

  if (t.hasStopTime())
  {
    Serial.println(String("Stop: ") +
                   t.getStopHour() + ":" +
                   t.getStopMinute() + ":" +
                   t.getStopSecond());

     jamStop = t.getStopHour();
     menStop = t.getStopMinute();
     detStop = t.getStopSecond();
  }

//  Serial.println(String("Time zone: ") + t.getTZ());
//
//  for (int i = 1; i <= 7; i++) {
//    if (t.isWeekdaySelected(i)) {
//      Serial.println(String("Day ") + i + " is selected");
//    }
//  }
//
//  Serial.println();
  }
  
BLYNK_WRITE(V0)//lampu kerja lamp3
{
  valLamp1 = param.asInt();
  EEPROM.write(valIdx[0],valLamp1);
  EEPROM.commit();
  //Serial.println("test");
}

BLYNK_WRITE(V1)//lampu tl lamp2
{
  valLamp2 = param.asInt();
  EEPROM.write(valIdx[1],valLamp2);
  EEPROM.commit();
}

BLYNK_WRITE(V2)//lampu teras lamp4
{
  valLamp3 = param.asInt();
  EEPROM.write(valIdx[2],valLamp3);
  EEPROM.commit();
}

BLYNK_WRITE(V3)//lampu gudang lamp1
{
  valLamp4 = param.asInt();
  EEPROM.write(valIdx[3],valLamp4);
  EEPROM.commit();
}


BLYNK_WRITE(V4)//reset
{
  int value = param.asInt();
  if(value){ delay(3000); ESP.restart();}
  
}

void setup()
{
  Serial.begin(115200);
  BlynkEdgent.begin();
  Clock.begin();
  dht.begin();
  EEPROM.begin(4);
  //inialisasi();
  pinMode(lamp1,OUTPUT);
  pinMode(lamp2,OUTPUT);
  pinMode(lamp3,OUTPUT);
  pinMode(lamp4,OUTPUT);
  pinMode(ind1,OUTPUT);
  pinMode(ind2,OUTPUT);
  delay(100);
  
}

void loop() {
  getSensor();
  Run();
  digitalWrite(lamp1,valIdx[0]);
  digitalWrite(lamp2,valIdx[1]);
  digitalWrite(lamp3,valIdx[2]);
  digitalWrite(lamp4,valIdx[3]);
  Blynk.virtualWrite(V10,suhu);
  Blynk.virtualWrite(V11,kelembaban);
//  Serial.println(suhu);
//  Serial.println(kelembaban);
  BlynkEdgent.run();
}

void inialisasi(){
//  for(int i = 0; i <=3; i++)
//  {
//    valIdx[i] = EEPROM.read(i);
//    //Serial.println(valIdx[i]);
//    delay(100);
//  }

  Blynk.virtualWrite(V0,valIdx[0]);
  Blynk.virtualWrite(V1,valIdx[1]);
  Blynk.virtualWrite(V2,valIdx[2]);
  Blynk.virtualWrite(V3,valIdx[3]);

  digitalWrite(lamp1,valIdx[0]);
  digitalWrite(lamp2,valIdx[1]);
  digitalWrite(lamp3,valIdx[2]);
  digitalWrite(lamp4,valIdx[3]);
  
}
void getSensor(){
  suhu = dht.readTemperature();
  kelembaban = dht.readHumidity();
  
}

void Run(){
  if(valLamp1)
  {
    led1.setColor(GREEN);
    led1.on();
    valIdx[0] = 0;
  }
  else
  {
    led1.off();
    valIdx[0] = 1;
  }

  if(valLamp2)
  {
    led2.setColor(GREEN);
    led2.on();
    valIdx[1] = 0;
  }
  else
  {
    led2.off();
    valIdx[1] = 1;
  }

  if(valLamp3)
  {
    led3.setColor(GREEN);
    led3.on();
    valIdx[2] = 0;
  }
  else
  {
    led3.off();
    valIdx[2] = 1;
  }

  if(valLamp4)
  {
    led4.setColor(GREEN);
    led4.on();
    valIdx[3] = 0;
  }
  else
  {
    led4.off();
    valIdx[3] = 1;
  }
}
