#include <Arduino.h>
#include "ESPBASE.h"
#include "31850OneWire.h"
#include "MAX31850Temp.h"

#define TEMP_PIN 4
#define RELAY1_PIN 12
#define RELAY2_PIN 13
byte relay1state = 0;
byte relay2state = 0;
String RelayTopic;
String StatusTopic;
String ValueTopic;
String sChipID;

ESPBASE Esp;
MAX31850* mytemp = new MAX31850(TEMP_PIN);

void setup() 
{
    Serial.begin(115200);
    char cChipID[10];
    sprintf(cChipID,"%08X",ESP.getChipId());
    sChipID = String(cChipID);

    Esp.initialize();
    RelayTopic = String(DEVICE_TYPE) + "/" + config.DeviceName + "/command";
    StatusTopic = String(DEVICE_TYPE) + "/" + config.DeviceName + "/status";
    ValueTopic = String(DEVICE_TYPE) + "/" + config.DeviceName + "/value";
    mqttSubscribe();

    pinMode(RELAY1_PIN,OUTPUT);
    pinMode(RELAY2_PIN,OUTPUT);
    digitalWrite(RELAY1_PIN,LOW);
    digitalWrite(RELAY2_PIN,LOW);

    Serial.println("Done with setup");
}

void sendStatus()
{
  String message = "Woodstove,1,";
  if(relay1state == 1)
    message = message + "on";
  else
    message = message + "off";
  message = message + ":Woodstove,2,";
  if(relay2state == 1)
    message = message + "on";
  else
    message = message + "off";
  Esp.mqttSend(StatusTopic,"",message);
}

void loop() 
{
    static int count = 0;
    if(TempTime >= config.TemperatureDelay and config.TemperatureDelay > 0)
    {
        TempTime = 0;
        float stovetemp = mytemp->getTemp();
        Serial.println(String(stovetemp));
        if((int) stovetemp >= config.OnTemperature && relay1state == 0)
        {
          relay1state = 1;
          digitalWrite(RELAY1_PIN,HIGH);
          sendStatus();
        }
        if((int) stovetemp < config.OffTemperature && relay1state == 1)
        {
          relay1state = 0;
          digitalWrite(RELAY1_PIN,LOW);
          sendStatus();
        }
        count++;
        if(count > 6)
        {
            count = 0;
            String topic = String(DEVICE_TYPE) + "/" + config.DeviceName + "/value";
            String message = "StoveTemp:" + String(stovetemp);
            Esp.mqttSend(topic,"",message);
        }
    }
    Esp.loop();
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
  if (s_topic == RelayTopic || s_topic == "AllLights" || s_topic == "computer/timer/event") 
  {
    if(s_payload == "signal")
    {
      Esp.mqttSend(StatusTopic,sChipID," WiFi: " + getSignalString());
    }
    if(s_payload == "TOGGLE")
    {
      if(relay1state == 1)
      {
        s_payload = "OFF";
      }
      else
      {
        s_payload = "ON";
      }
    }
    if(s_payload == "TOGGLE_1")
    {
      if(relay1state == 1)
      {
        s_payload = "OFF_1";
      }
      else
      {
        s_payload = "ON_1";
      }
    }
    if(s_payload == "TOGGLE_2")
    {
      if(relay2state == 1)
      {
        s_payload = "OFF_2";
      }
      else
      {
        s_payload = "ON_2";
      }
    }
    if(s_payload == "ON_1" || s_payload == "ON")
    {
      relay1state = 1;
      digitalWrite(RELAY1_PIN,HIGH);
    }
    if(s_payload == "OFF_1" || s_payload == "OFF")
    {
      relay1state = 0;
      digitalWrite(RELAY1_PIN,LOW);
    }
    if(s_payload == "ON_2" || s_payload == "ON")
    {
      relay2state = 1;
      digitalWrite(RELAY2_PIN,HIGH);
    }
    if(s_payload == "OFF_2" || s_payload == "OFF")
    {
      relay2state = 0;
      digitalWrite(RELAY2_PIN,LOW);
    }
    sendStatus();
  }
}

void mqttSubscribe()
{
    if(Esp.mqttClient->connected())
    {
        if(Esp.mqttClient->subscribe(RelayTopic.c_str()))
        {
            Serial.println("Subscribed to " + RelayTopic);
            Esp.mqttSend(StatusTopic,"","Subscribed to " + RelayTopic);
            Esp.mqttSend(StatusTopic,verstr,","+Esp.MyIP()+" start");
        }
    }
}