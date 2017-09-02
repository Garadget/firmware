#include "command.h"

// int Command::execute(Reader& reader, Writer& writer) = 0;

jsmntok_t* JSONCommand::json_tokenise(char *js) {
  jsmn_parser parser;
  parser.size = sizeof(parser);
  jsmn_init(&parser, NULL);

  unsigned int n = 64;
  jsmntok_t* tokens = (jsmntok_t*)malloc(sizeof(jsmntok_t) * n);
  if (!tokens) return nullptr;
  int ret = jsmn_parse(&parser, js, strlen(js), tokens, n, NULL);
  while (ret == JSMN_ERROR_NOMEM) {
    n = n * 2 + 1;
    jsmntok_t* prev = tokens;
    tokens = (jsmntok_t*)realloc(tokens, sizeof(jsmntok_t) * n);
    if (!tokens) {
      free(prev);
      return nullptr;
    }
    ret = jsmn_parse(&parser, js, strlen(js), tokens, n, NULL);
  }
  return tokens;
}

bool JSONCommand::json_token_streq(const char *js, jsmntok_t *t, const char *s) {
  return (strncmp(js + t->start, s, t->end - t->start) == 0
    && strlen(s) == (size_t) (t->end - t->start));
}

char* JSONCommand::json_token_tostr(char *js, jsmntok_t *t) {
  js[t->end] = '\0';
  return js + t->start;
}

void JSONCommand::write_result_code(Writer& writer, int result) {
  write_char(writer, '{');
  write_json_int(writer, "r", result);
  write_char(writer, '}');
}

int JSONCommand::parse_request(Reader& reader) {
  return 0;
}

void JSONCommand::produce_response(Writer& writer, int result) {
  write_result_code(writer, result);
}

int JSONCommand::process() {
  return 0;
}

void JSONCommand::write_char(Writer& w, char c) {
  w.write((uint8_t*) & c, 1);
}

void JSONCommand::write_quoted_string(Writer& out, const char* s) {
  write_char(out, '"');
  out.write(s);
  write_char(out, '"');
}

void JSONCommand::write_json_string(Writer& out, const char* name, const char* value) {
  write_quoted_string(out, name);
  write_char(out, ':');
  write_quoted_string(out, value);
}

char* JSONCommand::int_to_ascii(int val, char* buf, int i) {
  buf[--i] = 0;
  bool negative = val < 0;
  if (negative) {
    val = -val;
  }
  buf[--i] = (val % 10) + '0';
  val /= 10;
  for(; val && i ; val /= 10) {
    buf[--i] = (val % 10) + '0';
  }
  if (negative)
    buf[--i] = '-';
  return &buf[i];
}

void JSONCommand::write_json_int(Writer& out, const char* name, int value) {
  char buf[20];
  write_quoted_string(out, name);
  write_char(out, ':');
  out.write(int_to_ascii(value, buf, 20));
}

int JSONCommand::execute(Reader& reader, Writer& writer) {
  int result = parse_request(reader);
  if (!result) {
    result = process();
    produce_response(writer, result);
  }
  return result;
}


/**
 * A command that parses a JSON request.
 */

int JSONRequestCommand::parse_json_request(Reader& reader, const char* const keys[], const jsmntype_t types[], unsigned count) {
  int result = -1;
  char* js = reader.fetch_as_string();
  if (js) {
    jsmntok_t *tokens = json_tokenise(js);
    if (tokens) {
      enum parse_state { START, KEY, VALUE, SKIP, STOP };

      parse_state state = START;
      jsmntype_t expected_type = JSMN_OBJECT;

    	result = 0;
    	int key = -1;

      for (size_t i = 0, j = 1; j > 0; i++, j--) {
        jsmntok_t *t = &tokens[i];
        if (t->type == JSMN_ARRAY || t->type == JSMN_OBJECT)
          j += t->size * 2;

        switch (state) {
          case START:
            state = KEY;
            break;

          case KEY:
            state = VALUE;
            key = -1;
            for (size_t i = 0; i < count; i++) {
              if (json_token_streq(js, t, keys[i])) {
                expected_type = types[i];
                key = i;
                JSON_DEBUG( ( "key: %s %d %d\n", keys[i], i, (int)expected_type ) );
              }
            }
            if (key == -1) {
              JSON_DEBUG( ( "unknown key: %s\n", json_token_tostr(js, t) ) );
              result = -1;
            }
            break;

          case VALUE:
            if (key != -1) {
              if (t->type != expected_type) {
                result = -1;
                JSON_DEBUG( ( "type mismatch\n" ) );
              }
              else {
                char *str = json_token_tostr(js, t);
                if (!parsed_value(key, t, str))
                  result = -1;
              }
            }
            state = KEY;
            break;

          case STOP: // Just consume the tokens
            break;

          default:
            result = -1;
        }
      }
      free(tokens);
    }
    free(js);
  }
  return result;
}

bool SetConfigCommand::parsed_value(unsigned key, jsmntok_t* t, char* str) {
  void* data = ((uint8_t*)&a_config) + OFFSET[key];
  JSON_DEBUG(("Key: %u, Offset: %u", key, OFFSET[key]));

  if (!data) {
    JSON_DEBUG( ( "no data\n" ) );
    return false;
  }
  if (t->type == JSMN_STRING) {
    strncpy((char*)data, str, MAXNAMESIZE - 1);
    JSON_DEBUG( ( "copied value %s\n", (char*)str ) );
  }
  else {
    int32_t value = atoi(str);
    *((int32_t*)data) = value;
    JSON_DEBUG( ( "copied number %s (%d)\n", (char*)str, (int)value ) );
  }
  return true;
}

int SetConfigCommand::parse_request(Reader& reader) {
  memset(&a_config, 0, sizeof(doorConfig));
  return parse_json_request(reader, KEY, TYPE, arraySize(KEY));
}

int SetConfigCommand::process() {
  // @todo: continue from here
  JSON_DEBUG(("all parsed, name: %s", (char*)a_config.s_deviceName));
  JSON_DEBUG(("timeout: %u", (unsigned)a_config.n_mqttTimeout));
  return 0;
  //return save_credentials();
}

const char* SetConfigCommand::KEY[11] = {"nme","rdt","mtt","rlt","rlp","srr","srt","mqtt","mqip","mqpt","mqto"};
const int SetConfigCommand::OFFSET[] = {
  offsetof(doorConfig, s_deviceName),
  offsetof(doorConfig, n_readTime),
  offsetof(doorConfig, n_motionTime),
  offsetof(doorConfig, n_relayTime),
  offsetof(doorConfig, n_relayPause),
  offsetof(doorConfig, n_sensorReads),
  offsetof(doorConfig, n_sensorThreshold),
  offsetof(doorConfig, n_protocols),
  offsetof(doorConfig, n_mqttBrokerIp),
  offsetof(doorConfig, n_mqttBrokerPort),
  offsetof(doorConfig, n_mqttTimeout)
};

const jsmntype_t SetConfigCommand::TYPE[] = {
  JSMN_STRING,
  JSMN_PRIMITIVE,
  JSMN_PRIMITIVE,
  JSMN_PRIMITIVE,
  JSMN_PRIMITIVE,
  JSMN_PRIMITIVE,
  JSMN_PRIMITIVE,
  JSMN_PRIMITIVE,
  JSMN_PRIMITIVE,
  JSMN_PRIMITIVE,
  JSMN_PRIMITIVE
};
