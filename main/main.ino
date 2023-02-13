
#include "DHT.h"
//#include <EEPROM.h>
#include <NTPClient.h>
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

WidgetLED led1(V6); //lampu kerja
WidgetLED led2(V7); //lampu tl
WidgetLED led3(V8); //lampu teras
WidgetLED led4(V9); //lampu gudang
WidgetLED led5(V13); //lampu status
WidgetLED led6(V11); //lampu clock
WidgetLED led7(V10); //led status mode
WidgetLED led8(V12); //status error sensor

#define USE_NODE_MCU_BOARD

const long utcOffsetInSeconds = 25200;
WiFiUDP ntpUDP;
NTPClient Clock(ntpUDP, "asia.pool.ntp.org", utcOffsetInSeconds);

int jam,menit,detik;
int jamStart,menStart,detStart;
int jamStop,menStop,detStop;
int valLamp1,valLamp2,valLamp3,valLamp4,Reset;
int stateM;
int tgr=0;
//display suhu : V10
//display kelem : v11
int valIdx[4];
DHT dht(pin,dhtType);

BLYNK_CONNECTED()
{
    led5.setColor(GREEN);
    led5.on();

}

BLYNK_WRITE(V5)
{
  stateM = param.asInt();
  EEPROM.write(54,stateM);
  EEPROM.commit();
}

BLYNK_WRITE(V14)
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

  }
  
BLYNK_WRITE(V1)//lampu kerja 
{
  valLamp1 = param.asInt();
  EEPROM.write(50,valLamp1);
  EEPROM.commit();
  //Serial.println("test");
}

BLYNK_WRITE(V2)//lampu tl 
{
  valLamp2 = param.asInt();
  EEPROM.write(51,valLamp2);
  EEPROM.commit();
}

BLYNK_WRITE(V3)//lampu teras 
{
  valLamp3 = param.asInt();
  EEPROM.write(52,valLamp3);
  EEPROM.commit();
}

BLYNK_WRITE(V4)//lampu gudang
{
  valLamp4 = param.asInt();
  EEPROM.write(53,valLamp4);
  EEPROM.commit();
}


BLYNK_WRITE(V0)//reset
{
  int value = param.asInt();
  if(value){ delay(5000); ESP.restart();}
  
}

void setup()
{
  Serial.begin(115200);
  BlynkEdgent.begin();
  Clock.begin();
  dht.begin();
 // EEPROM.begin(4);
  tgr=1;
  digitalWrite(lamp1,HIGH);
  digitalWrite(lamp2,HIGH);
  digitalWrite(lamp3,HIGH);
  digitalWrite(lamp4,HIGH);
  pinMode(lamp1,OUTPUT);
  pinMode(lamp2,OUTPUT);
  pinMode(lamp3,OUTPUT);
  pinMode(lamp4,OUTPUT);
  pinMode(ind1,OUTPUT);
  pinMode(ind2,OUTPUT);
  delay(500);
  
}

void loop() {
  inialisasi();
  getSensor();
  getClock(); 
  Run();
  show();
  Blynk.virtualWrite(V16,suhu);
  Blynk.virtualWrite(V17,kelembaban);
  
  BlynkEdgent.run();
}

void show(){
  Serial.print("JAM:");
  Serial.print(jam);
  Serial.print(":");
  Serial.print(menit);
  Serial.print(":");
  Serial.println(detik);

  Serial.print("ALARM:");
  Serial.print(jamStart);
  Serial.print(":");
  Serial.print(menStart);
  Serial.print(":");
  Serial.println(detStart);

  Serial.print("ALARM:");
  Serial.print(jamStop);
  Serial.print(":");
  Serial.print(menStop);
  Serial.print(":");
  Serial.println(detStop);

  Serial.print("suhu:");
  Serial.print(suhu);
  Serial.print("lembab:");
  Serial.println(kelembaban);
}
void alarm()
{
  if(jam == jamStart && menit == menStart && detik == detik)
  {
    valIdx[0] = 0;
    valIdx[1] = 0;
    valIdx[2] = 0;
    valIdx[3] = 0;
  digitalWrite(lamp1,valIdx[0]);
  digitalWrite(lamp2,valIdx[1]);
  digitalWrite(lamp3,valIdx[2]);
  digitalWrite(lamp4,valIdx[3]);
    Blynk.virtualWrite(V1,1);
    Blynk.virtualWrite(V2,1);
    Blynk.virtualWrite(V3,1);
    Blynk.virtualWrite(V4,1);
    led1.setColor(RED);
    led1.on();
    led2.setColor(RED);
    led2.on();
    led3.setColor(RED);
    led3.on();
    led4.setColor(RED);
    led4.on();
    Serial.println("ON");
  }

  else if(jam == jamStop && menit == menStop && detik == detStop)
  {
    valIdx[0] = 1;
    valIdx[1] = 1;
    valIdx[2] = 1;
    valIdx[3] = 1;
  digitalWrite(lamp1,valIdx[0]);
  digitalWrite(lamp2,valIdx[1]);
  digitalWrite(lamp3,valIdx[2]);
  digitalWrite(lamp4,valIdx[3]);
    Blynk.virtualWrite(V1,0);
    Blynk.virtualWrite(V2,0);
    Blynk.virtualWrite(V3,0);
    Blynk.virtualWrite(V4,0);
    Serial.println("OFF");
    led1.off();
    led2.off();
    led3.off();
    led4.off();
  }

  
}


void getClock()
{
  Clock.update();
  jam = Clock.getHours();
  menit = Clock.getMinutes();
  detik = Clock.getSeconds();

  if(detik % 2)
  {
    digitalWrite(ind2,HIGH);
    led6.setColor(YELLOW);
    led6.on();
  }
  else
  {
    led6.off();
    digitalWrite(ind2,LOW);
  }
}
void inialisasi()
{
  if(tgr){
  for(int i = 50; i <=54; i++)
  {
    valIdx[i-50] = EEPROM.read(i);
    //Serial.println(valIdx[i]);
    delay(500);
  }
  
  valLamp1 = valIdx[0];
  valLamp2 = valIdx[1];
  valLamp3 = valIdx[2];
  valLamp4 = valIdx[3];
  Blynk.virtualWrite(V1,valIdx[0]);
  Blynk.virtualWrite(V2,valIdx[1]);
  Blynk.virtualWrite(V3,valIdx[2]);
  Blynk.virtualWrite(V4,valIdx[3]);

//  digitalWrite(lamp1,valIdx[0]);
//  digitalWrite(lamp2,valIdx[1]);
//  digitalWrite(lamp3,valIdx[2]);
//  digitalWrite(lamp4,valIdx[3]);
  stateM = valIdx[4];

  Serial.println("EPPROM: ");
  Serial.print("1: ");
  Serial.println(valIdx[0]);
  Serial.print("2: ");
  Serial.println(valIdx[1]);
  Serial.print("3: ");
  Serial.println(valIdx[2]);
  Serial.print("4: ");
  Serial.println(valIdx[3]);
  Serial.print("5: ");
  Serial.println(valIdx[4]);
  tgr=0;
  }
  else{
    return;
  }
}
void getSensor(){
  suhu = dht.readTemperature();
  kelembaban = dht.readHumidity();
  if (isnan(suhu) || isnan(kelembaban))
  {
    Serial.println("masalah pada sensor suhu");
    led8.setColor(YELLOW);
    led8.on();
  }
  else{
    led8.off();
  }
}

void Run()
{
  if(stateM == 0){
    led7.off();
  if(valLamp1)
  {
    led1.setColor(RED);
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
    led2.setColor(RED);
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
    led3.setColor(RED);
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
    led4.setColor(RED);
    led4.on();
    valIdx[3] = 0;
  }
  else
  {
    led4.off();
    valIdx[3] = 1;
  }

  digitalWrite(lamp1,valIdx[0]);
  digitalWrite(lamp2,valIdx[1]);
  digitalWrite(lamp3,valIdx[2]);
  digitalWrite(lamp4,valIdx[3]);
  }

  else if(stateM == 1)
  {
    led7.setColor(RED);
    led7.on();
    alarm();
  }

}
