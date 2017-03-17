#include "application.h"
#include "mqttClient.h"
#include "elapsedMillis.h"

#define GARADGET_SET_STATE_SUB "garadget/DEVICE_ID/set_state"
#define GARADGET_DOOR_STATE_PUB "garadget/DEVICE_ID/door_state"

// check mqtt connection every 5 seconds
#define MQTT_RECONNECT_INTERVAL 5000
elapsedMillis mqttReconnectTimer;

/*******************************************************************************
 * Function Name  : mqttInit
 * Description    : initialize the mqtt object and connect to broker
 * Return         : none
 *******************************************************************************/
void c_mqttClient::mqttInit()
{
  CallbackDelegate f_delegate;
  f_delegate = MakeDelegate(this, &c_mqttClient::mqttCallback);

  mqttServerIP[0] = 192;
  mqttServerIP[1] = 168;
  mqttServerIP[2] = 1;
  mqttServerIP[3] = 130;
  mqttServerPort = 1883;

  mqttCli.init(mqttServerIP, mqttServerPort, f_delegate);

  thereIsANewMessage = false;

  mqttConnect();
}

/*******************************************************************************
 * Function Name  : c_mqttClient
 * Description    : constructor
 * Return         : none
 *******************************************************************************/
c_mqttClient::c_mqttClient()
{
}

/*******************************************************************************
 * Function Name  : mqttCallback
 * Description    : this function gets called when an mqtt message is received
 * Return         : none
 *******************************************************************************/
void c_mqttClient::mqttCallback(char *topic, uint8_t *payload, unsigned int length)
{
  char p[length + 1];
  memcpy(p, payload, length);
  p[length] = NULL;
  String payloadStr(p);

  thereIsANewMessage = true;
  newMessage = payloadStr;

  #ifdef APPDEBUG
    Particle.publish("MQTT message received: ", payloadStr, 60, PRIVATE);
    Serial.printf("MQTT message received: " + payloadStr);
  #endif
}

/*******************************************************************************
 * Function Name  : mqttConnect
 * Description    : connects to the mqtt broker and subscribes to interesting topics
 * Return         : true if success, false otherwise
 *******************************************************************************/
bool c_mqttClient::mqttConnect()
{
  if (mqttCli.isConnected())
    return true;
  
  mqttCli.connect("garadget");

  if (mqttCli.isConnected()) {
    // we need to subscribe to topics that this device is interested in  
    mqttSubscribeToInterestingTopics();
    Particle.publish("MQTT connected", "Hurray!", PRIVATE);
  } else {
    Particle.publish("MQTT failed to connect", "Ouch!", PRIVATE);
    return false;
  }

  return true;

}


/*******************************************************************************
 * Function Name  : mqttSubscribeToInterestingTopics
 * Description    : subscribes to interesting topics
 * Return         : none
 *******************************************************************************/
void c_mqttClient::mqttSubscribeToInterestingTopics()
{
  String subscribeTopic = String(GARADGET_SET_STATE_SUB).replace("DEVICE_ID", System.deviceID());
  mqttCli.subscribe(subscribeTopic);
  #ifdef APPDEBUG
    Particle.publish("MQTT subscribed to ", subscribeTopic, PRIVATE);
    Serial.printf("MQTT subscribed to " + subscribeTopic);
  #endif
}

/*******************************************************************************
 * Function Name  : mqttPublishDoorState
 * Description    : publishes door status
 * Return         : none
 *******************************************************************************/
void c_mqttClient::mqttPublishDoorState(String state)
{
  String publishTopic = String(GARADGET_DOOR_STATE_PUB).replace("DEVICE_ID", System.deviceID());
  mqttPublish(publishTopic,state);
}

/*******************************************************************************
 * Function Name  : mqttPublish
 * Description    : publishes to topic
 * Return         : none
 *******************************************************************************/
void c_mqttClient::mqttPublish(String topic, String payload)
{
  mqttCli.publish(topic,payload);
  #ifdef APPDEBUG
    Particle.publish("MQTT published to " + topic, payload, PRIVATE);
    Serial.printf("MQTT published to " + topic + ": " + payload);
  #endif
}

/*******************************************************************************
 * Function Name  : mqttLoop
 * Description    : processes the mqtt events
 * Return         : the String message (mqtt payload) if there is any, NULL otherwise
 *******************************************************************************/
String c_mqttClient::mqttLoop()
{
  mqttReconnect();
  if (mqttCli.isConnected()) {
    mqttCli.loop();
  }

  if (thereIsANewMessage) {
    thereIsANewMessage = false;
    return newMessage;
  }

  return NULL;
}

/*******************************************************************************
 * Function Name  : mqttReconnect
 * Description    : reconnects to the mqtt broker if disconnected
 * Return         : true if success, false otherwise
 *******************************************************************************/
void c_mqttClient::mqttReconnect()
{

  // is time up? no, then come back later
  if (mqttReconnectTimer < MQTT_RECONNECT_INTERVAL)
  {
    return;
  }

  // time is up, reset timer
  mqttReconnectTimer = 0;

  #ifdef APPDEBUG
    Serial.printf("Checking mqtt connection");
  #endif

  // now try to reconnect
  if (! mqttCli.isConnected()) {
    mqttConnect();
    #ifdef APPDEBUG
      Serial.printf("Reconnecting to mqtt server");
    #endif
  }

}
