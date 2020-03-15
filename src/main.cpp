#include <Arduino.h>
#include "ESPBASE.h"
//#include "31850OneWire.h"
#include "MAX31850Temp.h"
#include "fanspeedcontrol.h"

#define TEMP_PIN D4
//#define RELAY1_PIN 12
//#define RELAY2_PIN 13
#define MINSPEED 120
#define fanpin1 D1
#define fanpin2 D2

#define MAXSPEED 1024
byte relay1state = 0;
byte relay2state = 0;
String CommandTopic;
String StatusTopic;
String ValueTopic;
String sChipID;
int insideTemp = -20;
int insideTempTimeout = 0;
unsigned int fanspeed = 0;
//dimmerLamp* fandimmer;
FanSpeedControler* fanspeed1;
FanSpeedControler* fanspeed2;
ESPBASE Esp;
MAX31850* mytemp = new MAX31850(TEMP_PIN);
float stovetemp = 0.0;

void setup() 
{
    Serial.begin(115200);
    char cChipID[10];
    sprintf(cChipID,"%08X",ESP.getChipId());
    sChipID = String(cChipID);

    Esp.initialize();
    CommandTopic = String(DEVICE_TYPE) + "/" + config.DeviceName + "/command";
    StatusTopic = String(DEVICE_TYPE) + "/" + config.DeviceName + "/status";
    ValueTopic = String(DEVICE_TYPE) + "/" + config.DeviceName + "/value";
    mqttSubscribe();
    fanspeed1 = new FanSpeedControler(fanpin1,MAXSPEED,MINSPEED);
    fanspeed2 = new FanSpeedControler(fanpin2,MAXSPEED,MINSPEED);
    Serial.println("Done with setup");
}

void sendStatus()
{
  Esp.mqttSend(StatusTopic,"fanspeed1",fanspeed1->getDebugStr() + " stovetemp:" + String(stovetemp) + " it:" + String(insideTemp));
  Esp.mqttSend(StatusTopic,"fanspeed2",fanspeed2->getDebugStr());
}

void loop() 
{
  static int count = 0;
  if(TempTime >= config.TemperatureDelay and config.TemperatureDelay > 0)
  {
    TempTime = 0;
    // commented for testing
    stovetemp = mytemp->getTemp();
    Serial.println(String(stovetemp));
    /**********
     * Set the fan speed here
     * ***************/
    fanspeed = MINSPEED;
    if(insideTemp == -20)
    {
      if(stovetemp > 70)
        fanspeed = 200;
      if(stovetemp > 80)
        fanspeed = 400;
      if(stovetemp > 90)
        fanspeed = 600;
      if(stovetemp > 100)
        fanspeed = 800;
      if(stovetemp > 120)
        fanspeed = MAXSPEED;
    }
    else
    {
      int dif = (int)stovetemp - insideTemp;
      if(insideTemp < 65)
      {
        fanspeed = dif * 50;
      }
      else
        if(insideTemp < 75)
        {
          fanspeed = dif * 10;
        }
    }
    if(fanspeed > MAXSPEED)
      fanspeed = MAXSPEED;
    if(fanspeed < MINSPEED)
      fanspeed = MINSPEED;
    fanspeed1->setSpeed(fanspeed);
    fanspeed2->setSpeed(fanspeed);
    count++;
    if(count > 6)
    {
      count = 0;
      String topic = String(DEVICE_TYPE) + "/" + config.DeviceName + "/value";
      String message = "StoveTemp:" + String(stovetemp) + " Fanspeed:" + String(fanspeed);
      Esp.mqttSend(topic,"",message);
      }
  }
  Esp.loop();
}

String getValue(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

String getSignalString()
{
  String signalstring = "";
  byte available_networks = WiFi.scanNetworks();
  signalstring = signalstring + sChipID + ":";
 
  for (int network = 0; network < available_networks; network++) {
    String sSSID = WiFi.SSID(network);
    if(network > 0)
      signalstring = signalstring + ",";
    signalstring = signalstring + WiFi.SSID(network) + "=" + String(WiFi.RSSI(network));
  }
  return signalstring;    
}

void mqttCallback(char* topic, byte* payload, unsigned int length) 
{
  char c_payload[length];
  memcpy(c_payload, payload, length);
  c_payload[length] = '\0';
  
  String s_topic = String(topic);
  String s_payload = String(c_payload);
  if(s_topic == "Temperature/test1temp/value") // maybe put this in the config ?
  {
    String stemp = getValue(s_payload,':',1);
    Serial.println(stemp);
    insideTemp=stemp.toFloat();
    insideTempTimeout = 600; // house temperature timeout set to 10 minutes.  Maybe put this in the config?
  }
//  if(s_topic == "EDWoodstove/woodstove/value") // maybe put this in the config ?
//  {
//    Serial.println("Got woodstove temp");
//    String stemp = getValue(s_payload,':',1);
//    Serial.println(stemp);
//    stovetemp = stemp.toFloat(); // house temperature timeout set to 10 minutes.  Maybe put this in the config?
//  }
  if (s_topic == CommandTopic) 
  {
    if(s_payload == "signal")
    {
      Esp.mqttSend(StatusTopic,sChipID," WiFi: " + getSignalString());
    }
    if(s_payload == "status")
    {
      sendStatus();
    }
  }
}

void mqttSubscribe()
{
    if(Esp.mqttClient->connected())
    {
        if(Esp.mqttClient->subscribe(CommandTopic.c_str()))
        {
            Serial.println("Subscribed to " + CommandTopic);
            Esp.mqttSend(StatusTopic,"","Subscribed to " + CommandTopic);
            Esp.mqttSend(StatusTopic,verstr,","+Esp.MyIP()+" start");
        }
        String temptopic = "Temperature/test1temp/value";
        if(Esp.mqttClient->subscribe(temptopic.c_str()))
        {
            Serial.println("Subscribed to " + temptopic);
            Esp.mqttSend(StatusTopic,"","Subscribed to " + temptopic);
        }
        // just for testing
        String wstemptopic = "EDWoodstove/woodstove/value";
        if(Esp.mqttClient->subscribe(wstemptopic.c_str()))
        {
            Serial.println("Subscribed to " + wstemptopic);
            Esp.mqttSend(StatusTopic,"","Subscribed to " + wstemptopic);
        }

    }
}

void mainTick()
{
  insideTempTimeout--;
  if(insideTempTimeout <= 0)
  {
    insideTempTimeout = 0;
    insideTemp = -20;
  }
  fanspeed1->tick();
  fanspeed2->tick();
//  Esp.mqttSend(StatusTopic,"fanspeed1",fanspeed1->getDebugStr() + " stovetemp:" + String(stovetemp) + " it:" + String(insideTemp));
//  Esp.mqttSend(StatusTopic,"fanspeed2",fanspeed2->getDebugStr());
//  Serial.println(insideTempTimeout);
}