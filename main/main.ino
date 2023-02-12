
#include "DHT.h"
#include <EEPROM.h>

#define dhtType DHT11
#define pin D6
float suhu,kelembaban;
#define lamp1 D1
#define lamp2 D2
#define lamp3 D4
#define lamp4 D5

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

int valLamp1,valLamp2,valLamp3,valLamp4,Reset;
//display suhu : V10
//display kelem : v11

DHT dht(pin,dhtType);

BLYNK_CONNECTED()
  {
    led5.setColor(RED);
    led5.on();
  }
BLYNK_WRITE(V0)//lampu kerja lamp3
{
  valLamp1 = param.asInt();
  Serial.println("test");
}

BLYNK_WRITE(V1)//lampu tl lamp2
{
  valLamp2 = param.asInt();
}

BLYNK_WRITE(V2)//lampu teras lamp4
{
  valLamp3 = param.asInt();
}

BLYNK_WRITE(V3)//lampu gudang lamp1
{
  valLamp4 = param.asInt();
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
  digitalWrite(lamp1,HIGH);
  digitalWrite(lamp2,HIGH);
  digitalWrite(lamp3,HIGH);
  digitalWrite(lamp4,HIGH);
  delay(100);
  dht.begin();
  EEPROM.begin(512);
  BlynkEdgent.begin();
}

void loop() {
  getSensor();
  Run();
  
  Blynk.virtualWrite(V10,suhu);
  Blynk.virtualWrite(V11,kelembaban);
  Serial.println(suhu);
  Serial.println(kelembaban);
  BlynkEdgent.run();
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
    digitalWrite(lamp1,LOW);
  }
  else
  {
    led1.off();
    digitalWrite(lamp1,HIGH);
  }

  if(valLamp2)
  {
    led2.setColor(GREEN);
    led2.on();
    digitalWrite(lamp2,LOW);
  }
  else
  {
    led2.off();
    digitalWrite(lamp2,HIGH);
  }

  if(valLamp3)
  {
    led3.setColor(GREEN);
    led3.on();
    digitalWrite(lamp3,LOW);
  }
  else
  {
    led3.off();
    digitalWrite(lamp3,HIGH);
  }

  if(valLamp4)
  {
    led4.setColor(GREEN);
    led4.on();
    digitalWrite(lamp4,LOW);
  }
  else
  {
    led4.off();
    digitalWrite(lamp4,HIGH);
  }
}
