#include <Arduino.h>
#include <PubSubClient.h>
#include "Gsender.h"
#include "ESPBASE.h"
#include "MQ135.h"

long lastReconnectAttempt = 0;
String StatusTopic;
String sChipID;
#define ANALOGPIN A0
MQ135 gasSensor = MQ135(ANALOGPIN);
long lastGas = 0;
long gasTime = 20000;
extern float vRZERO;
ESPBASE Esp;

void setup() {
  Serial.begin(115200);
  char cChipID[10];
  sprintf(cChipID,"%08X",ESP.getChipId());
  sChipID = String(cChipID);

  Esp.initialize();
  StatusTopic = String(DEVICE_TYPE) + "/" + config.DeviceName + "/status";
  customWatchdog = millis();
  Esp.mqttSend(String(DEVICE_TYPE) + "/" + config.DeviceName + "/startup",verstr,","+String(WiFi.localIP())+","+sChipID);
  vRZERO=config.vRZERO;
  Serial.println("Done with setup");
/*  test mail
  Gsender *gsender = Gsender::Instance();    // Getting pointer to class instance
  String subject = "Subject is optional!";
  if(gsender->Subject(subject)->Send("5303133307@tmomail.net", "Setup test")) {
      Serial.println("Message send.");
  } else {
      Serial.print("Error sending message: ");
      Serial.println(gsender->getError());
  } */
}

void gasLoop()
{
  long tm = millis();
  if(tm > lastGas + gasTime)
  {
//    float rz = vRZERO;
    lastGas = tm;
    float rzero = gasSensor.getRZero(); //this to get the rzero value, uncomment this to get ppm value
    float ppm = gasSensor.getPPM(); // this to get ppm value, uncomment this to get rzero value
    Serial.println(rzero); // this to display the rzero value continuously, uncomment this to get ppm value
    Serial.println(ppm); // this to display the ppm value continuously, uncomment this to get rzero value
    Esp.mqttSend(String(DEVICE_TYPE) + '/' + config.DeviceName + "/value","rvalue:",String(rzero));
    Esp.mqttSend(String(DEVICE_TYPE) + '/' + config.DeviceName + "/value","ppm:",String(ppm));
//    if(rz != vRZERO)
//      Esp.mqttSend(String(DEVICE_TYPE) + '/' + config.DeviceName + "/log","newRZERO:",String(vRZERO));
  }
}

void loop() {
  Esp.loop();
  gasLoop();
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

void sendStatus()
{
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  char c_payload[length];
  memcpy(c_payload, payload, length);
  c_payload[length] = '\0';
  
  String s_topic = String(topic);
  String s_payload = String(c_payload);
  Serial.print(s_topic + ":" + s_payload);
}

void mqttSubscribe()
{
    if (Esp.mqttClient->connected()) 
    {
        //subscribe to topics here
    }
}


