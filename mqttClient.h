#ifndef MQTTCLIENT_H
#define MQTTCLIENT_H

#include "application.h"
#include "global.h"
#include "MQTT.h"

class c_mqttClient {
 private:
    MQTT mqttCli;
    byte mqttServerIP[4];
    uint16_t mqttServerPort;
    bool thereIsANewMessage = false;
    String newMessage;

 public:
    c_mqttClient();
    void mqttCallback(char* topic, uint8_t* payload, unsigned int length);
    bool mqttConnect();
    void mqttSubscribeToInterestingTopics();
    void mqttInit();
    String mqttLoop();
    void mqttReconnect();
    void mqttPublishDoorState(String state);
    void mqttPublish(String topic, String payload);

};

#endif