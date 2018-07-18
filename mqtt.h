#ifndef _MQTT_h
#define _MQTT_h

#if defined(ARDUINO) && ARDUINO >= 100
  #include "arduino.h"
#else
  #include "WProgram.h"
#endif

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define MQTT_TOPIC_CMD "/living_room/door_win/cmd"
#define MQTT_TOPIC_POS "/living_room/door_win/pos"
#define MQTT_TOPIC_STA "/living_room/door_win/state"


typedef enum MQTT_STATE
{
  ST_MQTT_INIT = 0,
  ST_MQTT_CONNECTING = 1,
  ST_MQTT_CONNECTED = 2,
  ST_MQTT_STOPPED = 3
} MQTT_STATE;

class cMQTT
{
  private:
    WiFiClient espClient;
    PubSubClient MQTTClient;
    char MQTTState;
    
    void callback(char*, uint8_t*, unsigned int);
    std::function<void(char*, uint8_t*, unsigned int)> f3;
    
  public:
    void InitMQTT(void);
    void MQTTTask(void);
};

extern cMQTT ins_mqtt;

//void MQTTTask(void);
//void InitMQTT(void);

#endif

