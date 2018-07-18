#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "main.h"
#include "mqtt.h"
#include "observer.h"

#define MQTT_DBG
#ifdef MQTT_DBG
  #define MQTT_DbgMsg DbgMsg
#else
  #define MQTT_DbgMsg
#endif

using namespace std::placeholders;

//void callback(char*, byte*, unsigned int);

//WiFiClient espClient;
//PubSubClient MQTTClient(espClient);

/*
const char* mqttServer = "test.mosquitto.org";
const int mqttPort = 1883;
const char* mqttUser = "YourMqttUser";
const char* mqttPassword = "YourMqttUserPassword";
*/
const char* mqttServer = "test.mosquitto.org";
const int mqttPort = 1883;
const char* mqttUser = "YourMqttUser";
const char* mqttPassword = "YourMqttUserPassword";


cMQTT ins_mqtt;


void cMQTT::callback(char* topic, uint8_t* payload, unsigned int length)
{ 
  MQTT_DbgMsg("Msg: (topic: %s | msg: %.*s)\n", topic, length, payload);
  //MQTT_DbgMsg("-----------------------");
}

  
void cMQTT::InitMQTT(void)
{
  MQTTState = ST_MQTT_INIT;
  MQTTClient.setClient(espClient);
  

  f3 = std::bind(&cMQTT::callback, this, _1, _2, _3);
  MQTTClient.setCallback(f3);
  
  MQTTClient.setServer(mqttServer, mqttPort);
  //MQTTClient.connect("ESP8266Client", mqttUser, mqttPassword);  
}
    
void cMQTT::MQTTTask(void)
{
  switch(MQTTState)
  {
    case ST_MQTT_INIT:
      MQTTClient.connect("ESP8266Client_93675");
      MQTTState = ST_MQTT_CONNECTING;
    break;
    case ST_MQTT_CONNECTING:
      if(MQTTClient.connected())
      {
        MQTT_DbgMsg("MQTT connected\n");
        //MQTTClient.subscribe("esp/test");
        MQTTClient.subscribe("#");
        MQTTState = ST_MQTT_CONNECTED;
      }
      else
      {
        MQTT_DbgMsg("mqtt fail (%u)\n", MQTTClient.state());
      }
    break;
    case ST_MQTT_CONNECTED:
      MQTTClient.publish("esp/test", "Yehaaaa");
      MQTTState = ST_MQTT_STOPPED;
    break;
    case ST_MQTT_STOPPED:
    break;
    default:
      MQTTState = ST_MQTT_STOPPED;
  }
  
  MQTTClient.loop();
  MQTT_DbgMsg("MQTT stat: %i\n", MQTTClient.state());
  
  
  //MQTTClient.publish("esp/test", "Hello from ESP8266");
  //MQTTClient.subscribe("esp/test");
  
  //MQTTClient.subscribe("room_id/win_id/cmd");
  //MQTTClient.publish("room_id/win_id/position", "Hello from ESP8266");
  //MQTTClient.publish("room_id/win_id/stateChange", "Hello from ESP8266");
}




