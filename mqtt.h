#ifndef _MQTT_h
#define _MQTT_h

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "class_defs.h"
#include "blinds.h"


#define MQTT_TOPIC_CMD  "/living_room/door_win/cmd"
#define MQTT_TOPIC_POS  "/living_room/door_win/pos"
#define MQTT_TOPIC_STA  "/living_room/door_win/state"

#define MQTT_CMD_OPEN   '0'
#define MQTT_CMD_CLOSE  '1'
#define MQTT_CMD_STOP   '2'

typedef enum MQTT_STATE
{
  STATE_MQTT_INIT = 0,
  STATE_MQTT_CONNECTING = 1,
  STATE_MQTT_CONNECTED = 2,
  STATE_MQTT_STOPPED = 3
} MQTT_STATE;

class class_mqtt : public Task, public Observer
{
  private:
    WiFiClient espClient;
    PubSubClient MQTTClient;
    char MQTTState;
    const char* mqttServer = "test.mosquitto.org";
    const int mqttPort = 1883;
    const char* mqttUser = "YourMqttUser";
    const char* mqttPassword = "YourMqttUserPassword";
    
    void callback(char*, uint8_t*, unsigned int);

  public:
    class_mqtt();
    void TaskInit(void);
    void TaskRun(void);
    void onEvent(EVENT_TYPE, unsigned int);
};




#endif
