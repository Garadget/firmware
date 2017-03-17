mqtt crash course

this subscribes to the state changes:
pi@pi3:~ $ mosquitto_sub -h 192.168.1.130 -t garadget/2a003f0009473433373738/door_state
initialized
closing
closed


This closes the door:
pi@pi3:~ $ mosquitto_pub -h 192.168.1.130 -t garadget/2a003f0009473433373738/set_state -m close

This opens the door:
pi@pi3:~ $ mosquitto_pub -h 192.168.1.130 -t garadget/2a003f0009473433373738/set_state -m open


at boot time c_door::f_init()  calls init:
  o_mqttClient.mqttInit();


at every loop we check for new mqtt messages received, and if there are any new messages, we call f_receiveState()
    // mqttLoop returns the mqtt payload received if there is any, else NULL
    String temp_state = o_mqttClient.mqttLoop();
    if ( temp_state != NULL ) {
      f_receiveState(temp_state);
    }


if there is an event on the door on c_door::f_publishEvent(doorState n_event)
we call the publish function on mqtt 

  // here comes the MQTT publish
  o_mqttClient.mqttPublishDoorState(s_event);


during each call of mqttLoop(), there is a call to mqttReconnect()
this is so every MQTT_RECONNECT_INTERVAL (5 seconds) the mqtt tries
reconnect if it's not connected.


Source: https://www.codeproject.com/kb/cpp/fastdelegate.aspx
