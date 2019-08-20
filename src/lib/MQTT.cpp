#include "MQTT.h"

#define MQTTQOS0_HEADER_MASK        (0 << 1)
#define MQTTQOS1_HEADER_MASK        (1 << 1)
#define MQTTQOS2_HEADER_MASK        (2 << 1)

#define DUP_FLAG_OFF_MASK           (0 << 3)
#define DUP_FLAG_ON_MASK            (1 << 3)

const uint8_t MQTT::S_CONN_3_1[] = {0x00,0x06,'M','Q','I','s','d','p', 0x03};
const uint8_t MQTT::S_CONN_3_1_1[] = {0x00,0x04,'M','Q','T','T', 0x04};

void MQTT::f_init(char* s_domain, uint8_t *a_ip, uint16_t n_port, int n_keepAlive, int n_maxPacketSize) {
  if (a_ip != NULL)
    this->a_ip = a_ip;
  if (s_domain != NULL)
    this->s_domain = s_domain;
  this->n_port = n_port;
  this->n_keepAlive = n_keepAlive;
  // if n_maxPacketSize is over MQTT_MAX_PACKET_SIZE.
  this->n_maxPacketSize = (n_maxPacketSize <= MQTT_MAX_PACKET_SIZE ? MQTT_MAX_PACKET_SIZE : n_maxPacketSize);
  s_buffer = new uint8_t[this->n_maxPacketSize];
  o_client = new TCPClient();
}

void MQTT::f_setBroker(char* s_domain, uint16_t n_port) {
  if (f_isConnected())
    f_disconnect();

  this->s_domain = s_domain;
  a_ip = NULL;
  this->n_port = n_port;
}

void MQTT::f_setBroker(uint8_t *a_ip, uint16_t n_port) {
  if (f_isConnected())
    f_disconnect();

  this->s_domain = "";
  this->a_ip = a_ip;
  this->n_port = n_port;
}

bool MQTT::f_connect(const char *s_id) {
  return f_connect(s_id, NULL, NULL, 0, QOS0, 0, 0, true);
}

bool MQTT::f_connect(const char *s_id, const char *user, const char *pass) {
  return f_connect(s_id, user, pass, 0, QOS0, 0, 0, true);
}

bool MQTT::f_connect(const char *s_id, const char *user, const char *pass, const char* willTopic, EMQTT_QOS willQos, uint8_t willRetain, const char* willMessage, bool cleanSession) {
  if (f_isConnected() || n_state != STATE_IDLE || !f_timerTimeout())
    return false;

  int n_result = 0;
  if (a_ip == NULL) {
    Log.info("MQTT - connecting to %s:%d (version %u)", this->s_domain.c_str(), n_port, n_version);
    n_result = o_client->connect(this->s_domain.c_str(), n_port);
  }
  else {
    Log.info("MQTT - connecting to %d.%d.%d.%d:%d (version %u)", a_ip[0], a_ip[1], a_ip[2], a_ip[3], n_port, n_version);
    n_result = o_client->connect(a_ip, n_port);
  }

  if (!n_result) {
    Log.error("MQTT - connection failed");
    n_state = STATE_REJECTED;
    return false;
  }

  n_nextMsgId = 1;
  const uint8_t* p_conn = MQTT::S_CONN_3_1_1;
  if (n_version == MQTT_3_1)
    p_conn = MQTT::S_CONN_3_1;
  uint16_t n_conn = p_conn[1] + 3;

  // Leave room in the buffer for header and variable length field
  uint16_t n_length = 5;
  for (uint8_t j = 0; j < n_conn; j++)
    s_buffer[n_length++] = p_conn[j];

  uint8_t v = willTopic
    ? 0x06 | (willQos << 3) | (willRetain << 5)
    : 0x02;

  if (!cleanSession)
    v = v & 0xfd;

  if (user != NULL) {
    v = v|0x80;
    if (pass != NULL)
      v = v|(0x80>>1);
  }

  s_buffer[n_length++] = v;
  s_buffer[n_length++] = ((n_keepAlive) >> 8);
  s_buffer[n_length++] = ((n_keepAlive) & 0xFF);
  n_length = f_writeString(s_id, s_buffer, n_length);
  if (willTopic) {
    n_length = f_writeString(willTopic, s_buffer, n_length);
    n_length = f_writeString(willMessage, s_buffer, n_length);
  }

  if (user != NULL) {
    n_length = f_writeString(user, s_buffer, n_length);
    if (pass != NULL)
      n_length = f_writeString(pass, s_buffer, n_length);
  }

  f_write(MQTTCONNECT, s_buffer, n_length - 5);
  f_timerReset(MQTT_RESPONSE_TIMEOUT);
  n_state = STATE_CONNECTING;
  return true;
}

bool MQTT::f_onConnecting() {
  if (!o_client->available()) {
    if (f_timerTimeout()) {
      Log.error("MQTT - connection timeout");
      n_state = STATE_TIMEOUT;
      f_rotateProtocol();
    }
    return false;
  }

  uint8_t llen;
  uint16_t len = f_readPacket(&llen);

  if (len == 4) {
    if (s_buffer[3] == CONN_ACCEPT) {
      Log.info("MQTT - connect success");
      n_state = STATE_CONNECTED;
      f_timerReset(n_keepAlive);
    }
    else {
      Log.error("MQTT - connect fail. code = [%d]", s_buffer[3]);
      if (s_buffer[3] == CONN_UNACCEPTABLE_PROCOTOL) {
        n_state = STATE_RETRY;
        f_rotateProtocol();
      }
    }
    f_onConnectHanlder(s_buffer[3]);
    return (s_buffer[3] == CONN_ACCEPT);
  }

  Log.error("MQTT - invalid response");
  n_state = STATE_REJECTED;
  return false;
}

void MQTT::f_onConnected() {
  if (f_timerTimeout())
    f_ping();
}

void MQTT::f_ping() {
  s_buffer[0] = MQTTPINGREQ;
  s_buffer[1] = 0;
  o_client->write(s_buffer, 2);
  n_state = STATE_PINGING;
  f_timerReset(MQTT_RESPONSE_TIMEOUT);
  Log.info("MQTT - pinging broker");
}

bool MQTT::f_onPinging() {
  if (!f_timerTimeout())
    return true;
  Log.warn("MQTT - no response to ping");
  n_state = STATE_TIMEOUT;
  return false;
}

bool MQTT::f_timerTimeout() {
  return (millis() > n_timeout);
}

void MQTT::f_timerReset(uint8_t n_time) {
  n_timeout = millis() + n_time * 1000UL;
}

void MQTT::f_rotateProtocol() {
  switch(n_version) {
    case MQTT_3_1:
      n_version = MQTT_3_1_1;
      break;
    case MQTT_3_1_1:
      n_version = MQTT_3_1;
      break;
  }
}

uint8_t MQTT::f_readByte() {
    while (!o_client->available()) {
      // @todo: non-breaking
    }
    return o_client->read();
}

uint16_t MQTT::f_receive() {
  if (!o_client->available())
    return 0;

  uint8_t n_llen;
  uint16_t n_len = f_readPacket(&n_llen);
  if (!n_len)
    return 0;

  f_timerReset(n_keepAlive);
  n_state = STATE_CONNECTED;

  uint8_t n_type = s_buffer[0] & 0xF0;
  Log.info("MQTT - incomming message %u", n_type);
  switch (n_type) {
    case MQTTPUBLISH:
      return f_onPublish(n_len, n_llen);

    case MQTTPUBCOMP:
    case MQTTSUBACK:
      // @todo
      return 1;

    case MQTTPINGREQ:
      f_pingResponse();
      return 1;

    case MQTTPINGRESP:
      n_state = STATE_CONNECTED;
      return 1;

    default:
      Log.warn("MQTT - unknown message type %u", n_type);
  }
  return 0;
}

uint16_t MQTT::f_onPublish(uint16_t n_len, uint8_t n_llen) {
  uint16_t msgId = 0;
  uint8_t *a_payload;

  uint16_t tl = (s_buffer[n_llen + 1] << 8) + s_buffer[n_llen + 2];
  char s_topic[tl + 1];
  for (uint16_t i = 0; i < tl; i++)
    s_topic[i] = s_buffer[n_llen + 3 + i];
  s_topic[tl] = 0;
  // msgId only present for QOS>0
  if ((s_buffer[0] & 0x06) == MQTTQOS1_HEADER_MASK) {
    msgId = (s_buffer[n_llen + 3 + tl] << 8) + s_buffer[n_llen + 3 + tl + 1];
    a_payload = s_buffer + n_llen + 3 + tl + 2;
    f_onReceiveHandler(s_topic, a_payload, n_len - n_llen - 3 - tl - 2);

    s_buffer[0] = MQTTPUBACK;
    s_buffer[1] = 2;
    s_buffer[2] = (msgId >> 8);
    s_buffer[3] = (msgId & 0xFF);
    o_client->write(s_buffer, 4);
  }
  else {
    a_payload = s_buffer + n_llen + 3 + tl;
    f_onReceiveHandler(s_topic, a_payload, n_len - n_llen - 3 - tl);
  }
  return n_len;
}

void MQTT::f_pingResponse() {
  s_buffer[0] = MQTTPINGRESP;
  s_buffer[1] = 0;
  o_client->write(s_buffer, 2);
  Log.info("MQTT - responded to incoming ping");
}

uint16_t MQTT::f_readPacket(uint8_t* lengthLength) {
  uint16_t len = 0;
  s_buffer[len++] = f_readByte();
  bool isPublish = (s_buffer[0] & 0xF0) == MQTTPUBLISH;
  uint32_t multiplier = 1;
  uint16_t n_length = 0;
  uint8_t digit = 0;
  uint16_t skip = 0;
  uint8_t start = 0;

  do {
    digit = f_readByte();
    s_buffer[len++] = digit;
    n_length += (digit & 127) * multiplier;
    multiplier *= 128;
  }
  while ((digit & 128) != 0);
  *lengthLength = len - 1;

  if (isPublish) {
    // Read in topic length to calculate bytes to skip over for Stream writing
    s_buffer[len++] = f_readByte();
    s_buffer[len++] = f_readByte();
    skip = (s_buffer[*lengthLength + 1] << 8) + s_buffer[*lengthLength + 2];
    start = 2;
    if (s_buffer[0] & MQTTQOS1_HEADER_MASK)
      // skip message id
      skip += 2;
  }

  for (uint16_t i = start; i < n_length; i++) {
    digit = f_readByte();
    if (len < this->n_maxPacketSize)
        s_buffer[len] = digit;
    len++;
  }

  if (len > this->n_maxPacketSize)
    len = 0; // This will cause the packet to be ignored.

  return len;
}

bool MQTT::f_process() {
  switch (n_state) {
    case STATE_CONNECTED:
      f_onConnected();
      f_receive();
      break;

    case STATE_CONNECTING:
      f_onConnecting();
      break;

    case STATE_PINGING:
      f_receive();
      f_onPinging();
      break;

    case STATE_REJECTED:
    case STATE_TIMEOUT:
    case STATE_RETRY:

      f_clear();
      break;

    case STATE_IDLE:
      break;
  }
  return true;
}

bool MQTT::f_publish(const char* s_topic, const char* s_payload) {
  return f_publish(s_topic, (uint8_t*)s_payload, strlen(s_payload), false, QOS0, NULL);
}

bool MQTT::f_publish(const char * s_topic, const char* s_payload, EMQTT_QOS qos, bool dup, uint16_t *messageid) {
  return f_publish(s_topic, (uint8_t*)s_payload, strlen(s_payload), false, qos, dup, messageid);
}

bool MQTT::f_publish(const char * s_topic, const char* s_payload, EMQTT_QOS qos, uint16_t *messageid) {
  return f_publish(s_topic, (uint8_t*)s_payload, strlen(s_payload), false, qos, messageid);
}

bool MQTT::f_publish(const char* s_topic, const uint8_t* a_payload, unsigned int plength) {
  return f_publish(s_topic, a_payload, plength, false, QOS0, NULL);
}

bool MQTT::f_publish(const char* s_topic, const uint8_t* a_payload, unsigned int plength, EMQTT_QOS qos, bool dup, uint16_t *messageid) {
  return f_publish(s_topic, a_payload, plength, false, qos, dup, messageid);
}

bool MQTT::f_publish(const char* s_topic, const uint8_t* a_payload, unsigned int plength, EMQTT_QOS qos, uint16_t *messageid) {
  return f_publish(s_topic, a_payload, plength, false, qos, messageid);
}

bool MQTT::f_publish(const char* s_topic, const uint8_t* a_payload, unsigned int plength, bool retain) {
  return f_publish(s_topic, a_payload, plength, retain, QOS0, NULL);
}

bool MQTT::f_publish(const char* s_topic, const uint8_t* a_payload, unsigned int plength, bool retain, EMQTT_QOS qos, uint16_t *messageid) {
  return f_publish(s_topic, a_payload, plength, retain, qos, false, messageid);
}

bool MQTT::f_publish(const char* s_topic, const uint8_t* a_payload, unsigned int plength, bool retain, EMQTT_QOS qos, bool dup, uint16_t *messageid) {
    if (f_isConnected()) {
        // Leave room in the s_buffer for header and variable length field
        uint16_t n_length = 5;
        memset(s_buffer, 0, this->n_maxPacketSize);

        n_length = f_writeString(s_topic, s_buffer, n_length);

        if (qos == QOS2 || qos == QOS1) {
            n_nextMsgId += 1;
            s_buffer[n_length++] = (n_nextMsgId >> 8);
            s_buffer[n_length++] = (n_nextMsgId & 0xFF);
            if (messageid != NULL)
                *messageid = n_nextMsgId++;
        }

        for (uint16_t i = 0; i < plength && n_length < this->n_maxPacketSize; i++) {
            s_buffer[n_length++] = a_payload[i];
        }

        uint8_t header = MQTTPUBLISH;
        if (retain) {
            header |= 1;
        }

        if (dup) {
            header |= DUP_FLAG_ON_MASK;
        }

        if (qos == QOS2)
            header |= MQTTQOS2_HEADER_MASK;
        else if (qos == QOS1)
            header |= MQTTQOS1_HEADER_MASK;
        else
            header |= MQTTQOS0_HEADER_MASK;

        return f_write(header, s_buffer, n_length - 5);
    }
    return false;
}

bool MQTT::f_publishRelease(uint16_t n_messageId) {
  if (!f_isConnected())
    return false;
  uint16_t n_length = 0;
  s_buffer[n_length++] = MQTTPUBREL | MQTTQOS1_HEADER_MASK;
  s_buffer[n_length++] = 2;
  s_buffer[n_length++] = (n_messageId >> 8);
  s_buffer[n_length++] = (n_messageId & 0xFF);
  return o_client->write(s_buffer, n_length);
}

bool MQTT::f_write(uint8_t header, uint8_t* buf, uint16_t n_length) {
  uint8_t lenBuf[4];
  uint8_t llen = 0;
  uint8_t digit;
  uint8_t pos = 0;
  uint16_t rc;
  uint16_t len = n_length;
  do {
    digit = len % 128;
    len = len / 128;
    if (len > 0) {
        digit |= 0x80;
    }
    lenBuf[pos++] = digit;
    llen++;
  }
  while (len > 0);

  buf[4-llen] = header;
  for (int i = 0; i < llen; i++)
    buf[5 - llen + i] = lenBuf[i];

  rc = o_client->write(buf + (4 - llen), n_length + 1 + llen);
  return (rc == 1 + llen + n_length);
}

bool MQTT::f_subscribe(const char* s_topic) {
  return f_subscribe(s_topic, QOS0);
}

bool MQTT::f_subscribe(const char* s_topic, EMQTT_QOS n_qos) {
  if (!f_isConnected() || n_qos < 0 || n_qos > 1)
    return false;

  // Leave room in the buffer for header and variable length field
  uint16_t n_length = 5;
  n_nextMsgId++;
  if (n_nextMsgId == 0)
    n_nextMsgId = 1;
  s_buffer[n_length++] = (n_nextMsgId >> 8);
  s_buffer[n_length++] = (n_nextMsgId & 0xFF);

  n_length = f_writeString(s_topic, s_buffer, n_length);
  s_buffer[n_length++] = n_qos;
  return f_write(MQTTSUBSCRIBE | MQTTQOS1_HEADER_MASK, s_buffer, n_length - 5);
}

bool MQTT::f_unsubscribe(const char* s_topic) {
  if (f_isConnected()) {
    uint16_t n_length = 5;
    n_nextMsgId++;
    if (n_nextMsgId == 0)
        n_nextMsgId = 1;
    s_buffer[n_length++] = (n_nextMsgId >> 8);
    s_buffer[n_length++] = (n_nextMsgId & 0xFF);
    n_length = f_writeString(s_topic, s_buffer, n_length);
    return f_write(MQTTUNSUBSCRIBE | MQTTQOS1_HEADER_MASK, s_buffer, n_length - 5);
  }
  return false;
}

void MQTT::f_disconnect() {
  if (!f_isConnected())
    return;
  s_buffer[0] = MQTTDISCONNECT;
  s_buffer[1] = 0;
  o_client->write(s_buffer, 2);
  f_clear();
}

void MQTT::f_clear() {
  o_client->stop();
  f_timerReset(MQTT_RECONNECT_TIMEOUT);
  n_state = STATE_IDLE;
}

uint16_t MQTT::f_writeString(const char* string, uint8_t* buf, uint16_t pos) {
  const char* idp = string;
  uint16_t i = 0;
  pos += 2;
  while (*idp && pos < this->n_maxPacketSize) {
    buf[pos++] = *idp++;
    i++;
  }
  buf[pos - i - 2] = (i >> 8);
  buf[pos - i - 1] = (i & 0xFF);
  return pos;
}

bool MQTT::f_isConnected() {
  return o_client && o_client->connected();
}
