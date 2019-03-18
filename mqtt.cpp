#include "main.h"
#include "mqtt.h"
using namespace std::placeholders;



#define MQTT_DBG
#ifdef MQTT_DBG
  #define MQTT_DbgMsg DbgMsg("MQTT: "); DbgMsg
#else
  #define MQTT_DbgMsg
#endif







void class_mqtt::callback(char* topic, uint8_t* payload, unsigned int length)
{
  if(strcmp(MQTT_TOPIC_CMD, topic) == 0)
  {
    switch(payload[0])
    {
      case MQTT_CMD_OPEN:
        MQTT_DbgMsg("cmd open\n");
      break;
      case MQTT_CMD_CLOSE:
        MQTT_DbgMsg("cmd close\n");
      break;
      case MQTT_CMD_STOP:
        MQTT_DbgMsg("cmd stop\n");
      break;
      default:
        MQTT_DbgMsg("unknown cmd\n");
        break;
    }
  }
  //topic received that wasnt subscriped to
  else
  {
    //MQTT_DbgMsg("MQTT: unknown topic\n");
  }
  
  MQTT_DbgMsg("Msg: (topic: %s | msg: %.*s)\n", topic, length, payload);
}

class_mqtt::class_mqtt()
{ }

void class_mqtt::onEvent(EVENT_TYPE t, unsigned int d)
{}

void class_mqtt::TaskInit(void)
{
  MQTTState = STATE_MQTT_STOPPED;
  MQTTClient.setClient(espClient);

  MQTTClient.setCallback( std::bind(&class_mqtt::callback, this, _1, _2, _3) );

  MQTTClient.setServer(mqttServer, mqttPort);
  //MQTTClient.connect("ESP8266Client", mqttUser, mqttPassword);

  MQTT_DbgMsg("Initialized\n");
}

void class_mqtt::TaskRun(void)
{  
  switch(MQTTState)
  {
    case STATE_MQTT_INIT:
      if(WiFi.isConnected())
      {    
        MQTTClient.connect("ESP8266Client_93675");
        MQTTState = STATE_MQTT_CONNECTING;
      }
    break;
    case STATE_MQTT_CONNECTING:
      if(MQTTClient.connected())
      {
        MQTT_DbgMsg("MQTT connected\n");
        //MQTTClient.subscribe("#");
        MQTTClient.subscribe(MQTT_TOPIC_CMD);
        MQTTState = STATE_MQTT_CONNECTED;
      }
      else
      {
        MQTTClient.connect("ESP8266Client_93675");
        //MQTT_DbgMsg("mqtt connect fail (%d)\n", MQTTClient.state());
      }
    break;
    case STATE_MQTT_CONNECTED:
      MQTTClient.publish("esp/test", "Yehaaaa");
      MQTTState = STATE_MQTT_STOPPED;
    break;
    case STATE_MQTT_STOPPED:
    break;
    default:
      MQTTState = STATE_MQTT_STOPPED;
  }
  
  MQTTClient.loop();
  //MQTT_DbgMsg("MQTT stat: %i\n", MQTTClient.state());
  

  
  //MQTTClient.subscribe("room_id/win_id/cmd");
  //MQTTClient.publish("room_id/win_id/position", "Hello from ESP8266");
  //MQTTClient.publish("room_id/win_id/stateChange", "Hello from ESP8266");
}
