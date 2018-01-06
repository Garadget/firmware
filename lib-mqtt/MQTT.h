/*
MQTT library for Particle Core, Photon, Arduino
This software is released under the MIT License.
Copyright (c) 2014 Hirotaka Niisato
Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, f_publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:
The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
Much of the code was inspired by Arduino Nicholas pubsubclient
sample code bearing this copyright.
//---------------------------------------------------------------------------
// Copyright (c) 2008-2012 Nicholas O'Leary
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, f_publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//---------------------------------------------------------------------------
*/

#ifndef __MQTT_H_
#define __MQTT_H_

#include "application.h"
#include "spark_wiring_string.h"
#include "spark_wiring_tcpclient.h"
#include "spark_wiring_usbserial.h"

// MQTT_MAX_PACKET_SIZE : Maximum packet size
// this size is total of [MQTT Header(Max:5byte) + Topic Name Length + Topic Name + Message ID(QoS1|2) + Payload]
#define MQTT_MAX_PACKET_SIZE 255

// MQTT_n_keepAlive : n_keepAlive interval in Seconds
#define MQTT_DEFAULT_KEEPALIVE 15
#define MQTT_RESPONSE_TIMEOUT 3
#define MQTT_RECONNECT_TIMEOUT 3

#define MQTTCONNECT     1 << 4  // Client request to connect to Server
#define MQTTCONNACK     2 << 4  // Connect Acknowledgment
#define MQTTPUBLISH     3 << 4  // f_publish message
#define MQTTPUBACK      4 << 4  // f_publish Acknowledgment
#define MQTTPUBREC      5 << 4  // f_publish Received (assured delivery part 1)
#define MQTTPUBREL      6 << 4  // f_publish Release (assured delivery part 2)
#define MQTTPUBCOMP     7 << 4  // f_publish Complete (assured delivery part 3)
#define MQTTSUBSCRIBE   8 << 4  // Client Subscribe request
#define MQTTSUBACK      9 << 4  // Subscribe Acknowledgment
#define MQTTUNSUBSCRIBE 10 << 4 // Client Unsubscribe request
#define MQTTUNSUBACK    11 << 4 // Unsubscribe Acknowledgment
#define MQTTPINGREQ     12 << 4 // PING Request
#define MQTTPINGRESP    13 << 4 // PING Response
#define MQTTDISCONNECT  14 << 4 // Client is Disconnecting
#define MQTTReserved    15 << 4 // Reserved


class MQTT {
/** types */
public:
typedef enum {
    QOS0 = 0,
    QOS1 = 1,
    QOS2 = 2,
} EMQTT_QOS;

typedef enum {
    CONN_ACCEPT = 0,
    CONN_UNACCEPTABLE_PROCOTOL = 1,
    CONN_ID_REJECT = 2,
    CONN_SERVER_UNAVAILALE = 3,
    CONN_BAD_USER_PASSWORD = 4,
    CONN_NOT_AUTHORIZED = 5
} EMQTT_CONNACK_RESPONSE;

typedef enum {
  MQTT_3_1 = 3,
  MQTT_3_1_1 = 4
} EMQTT_VER;

typedef enum {
  STATE_IDLE = 0,
  STATE_TIMEOUT,
  STATE_RETRY,
  STATE_REJECTED,
  STATE_CONNECTING,
  STATE_CONNECTED, // connected states from this
  STATE_PINGING
} EMQTT_STATE;

static const uint8_t S_CONN_3_1[9];
static const uint8_t S_CONN_3_1_1[7];

protected:
    TCPClient *o_client = NULL;
    uint8_t *s_buffer;
    EMQTT_VER n_version = MQTT_3_1;
    EMQTT_STATE n_state = STATE_IDLE;

    uint16_t n_nextMsgId;
    uint32_t n_timeout;

    String s_domain;
    uint8_t *a_ip = NULL;
    uint16_t n_port;
    int n_keepAlive;
    uint16_t n_maxPacketSize;

    void f_init(char* s_domain, uint8_t *a_ip, uint16_t n_port, int n_keepAlive, int n_maxPacketSize);
    bool f_onConnecting();
    void f_onConnected();
    bool f_onPinging();
    uint16_t f_onPublish(uint16_t n_len, uint8_t n_llen);
    uint16_t f_receive();
    void f_ping();
    void f_pingResponse();

  virtual void f_onConnectHanlder(uint8_t n_result) {};
  virtual void f_onReceiveHandler(char* s_topic, uint8_t* s_payload, unsigned int n_length) {};
//    virtual void f_onQosHanlder(unsigned int);

    bool f_timerTimeout();
    void f_timerReset(uint8_t n_time);

    uint16_t f_readPacket(uint8_t*);
    uint8_t f_readByte();
    bool f_write(uint8_t n_header, uint8_t* s_buf, uint16_t n_length);
    uint16_t f_writeString(const char* s_string, uint8_t* a_buf, uint16_t n_pos);
    void f_rotateProtocol();

public:
    bool f_connect(const char *id);
    bool f_connect(const char *id, const char *user, const char *pass);
    bool f_connect(const char *id, const char *user, const char *pass, const char* willTopic, EMQTT_QOS willQos, uint8_t willRetain, const char* willMessage, bool cleanSession);
    void f_disconnect();
    void f_clear();

    void f_setBroker(char* s_domain, uint16_t n_port);
    void f_setBroker(uint8_t *a_ip, uint16_t n_port);

    bool f_publish(const char *s_topic, const char* s_payload);
    bool f_publish(const char *s_topic, const char* s_payload, EMQTT_QOS qos, uint16_t *messageid = NULL);
    bool f_publish(const char *s_topic, const char* s_payload, EMQTT_QOS qos, bool dup, uint16_t *messageid = NULL);
    bool f_publish(const char *s_topic, const uint8_t *pyaload, unsigned int plength);
    bool f_publish(const char *s_topic, const uint8_t *a_payload, unsigned int plength, EMQTT_QOS qos, uint16_t *messageid = NULL);
    bool f_publish(const char *s_topic, const uint8_t *a_payload, unsigned int plength, EMQTT_QOS qos, bool dup, uint16_t *messageid = NULL);
    bool f_publish(const char *s_topic, const uint8_t *a_payload, unsigned int plength, bool retain);
    bool f_publish(const char *s_topic, const uint8_t *a_payload, unsigned int plength, bool retain, EMQTT_QOS qos, uint16_t *messageid = NULL);
    bool f_publish(const char *s_topic, const uint8_t *a_payload, unsigned int plength, bool retain, EMQTT_QOS qos, bool dup, uint16_t *messageid);
    bool f_publishRelease(uint16_t messageid);

    bool f_subscribe(const char *s_topic);
    bool f_subscribe(const char *s_topic, EMQTT_QOS);
    bool f_unsubscribe(const char *s_topic);
    bool f_process();
    bool f_isConnected();
};

#endif  // __MQTT_H_
