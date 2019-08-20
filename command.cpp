#include "command.h"

// int Command::execute(Reader& reader, Writer& writer) = 0;

jsmntok_t* JSONCommand::json_tokenise(char *s_json) {
  jsmn_parser parser;
  parser.size = sizeof(parser);
  jsmn_init(&parser, NULL);

  unsigned int n = 64;
  jsmntok_t* a_tokens = (jsmntok_t*)malloc(sizeof(jsmntok_t) * n);
  if (!a_tokens) return nullptr;
  int ret = jsmn_parse(&parser, s_json, strlen(s_json), a_tokens, n, NULL);
  while (ret == JSMN_ERROR_NOMEM) {
    n = n * 2 + 1;
    jsmntok_t* prev = a_tokens;
    a_tokens = (jsmntok_t*)realloc(a_tokens, sizeof(jsmntok_t) * n);
    if (!a_tokens) {
      free(prev);
      return nullptr;
    }
    ret = jsmn_parse(&parser, s_json, strlen(s_json), a_tokens, n, NULL);
  }
  return a_tokens;
}

bool JSONCommand::json_token_streq(const char *s_json, jsmntok_t *a_token, const char *s_string) {
  return (strncmp(s_json + a_token->start, s_string, a_token->end - a_token->start) == 0
    && strlen(s_string) == (size_t) (a_token->end - a_token->start));
}

char* JSONCommand::json_token_tostr(char *s_json, jsmntok_t *a_token) {
  s_json[a_token->end] = '\0';
  return s_json + a_token->start;
}

void JSONCommand::write_result_code(Writer& writer, int n_result) {
  write_char(writer, '{');
  write_json_int(writer, "r", n_result);
  write_char(writer, '}');
}

int JSONCommand::parse_request(Reader& reader) {
  return 0;
}

void JSONCommand::produce_response(Writer& writer, int n_result) {
  write_result_code(writer, n_result);
}

int JSONCommand::process() {
  return 0;
}

void JSONCommand::write_char(Writer& w, char c) {
  w.write((uint8_t*) & c, 1);
}

void JSONCommand::write_quoted_string(Writer& out, const char* s_string) {
  write_char(out, '"');
  out.write(s_string);
  write_char(out, '"');
}

void JSONCommand::write_json_string(Writer& out, const char* s_name, const char* s_value) {
  write_quoted_string(out, s_name);
  write_char(out, ':');
  write_quoted_string(out, s_value);
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

void JSONCommand::write_json_int(Writer& out, const char* s_name, int n_value) {
  char buf[20];
  write_quoted_string(out, s_name);
  write_char(out, ':');
  out.write(int_to_ascii(n_value, buf, 20));
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
int JSONRequestCommand::parse_json_string(char* s_json, const char* const a_keys[], const jsmntype_t a_types[], unsigned n_types) {
  if (!s_json)
    return -1;

  int n_result = -1;
  jsmntok_t *a_tokens = json_tokenise(s_json);

  if (a_tokens) {
    enum c_parseState { START, KEY, VALUE, SKIP, STOP };

    c_parseState n_state = START;
    jsmntype_t expected_type = JSMN_OBJECT;

  	n_result = 0;
  	int n_key = -1;

    for (size_t n_token = 0, j = 1; j > 0; n_token++, j--) {
      jsmntok_t *a_token = &a_tokens[n_token];

      if (a_token->type == JSMN_ARRAY || a_token->type == JSMN_OBJECT)
        j += a_token->size * 2;

      switch (n_state) {
        case START:
          n_state = KEY;
          break;

        case KEY:
          n_state = VALUE;
          n_key = -1;
          for (size_t n_type = 0; n_type < n_types; n_type++) {
            if (json_token_streq(s_json, a_token, a_keys[n_type])) {
              expected_type = a_types[n_type];
              n_key = n_type;
//              Log.info("key: %s %d %d", a_keys[n_type], n_type, (int)expected_type);
            }
          }
          if (n_key == -1) {
            Log.warn("JSON Unknown Key: %s", json_token_tostr(s_json, a_token));
            n_result = -1;
          }
          break;

        case VALUE:
          if (n_key != -1) {
            if (a_token->type != expected_type) {
              n_result = -1;
              Log.error("JSON Type Mismatch");
            }
            else {
              char *s_string = json_token_tostr(s_json, a_token);
              if (!parsed_value(n_key, a_token, s_string))
                n_result = -1;
            }
          }
          n_state = KEY;
          break;

        case STOP: // Just consume the tokens
          break;

        default:
          n_result = -1;
      }
    }
    free(a_tokens);
  }
  return n_result;
}

int JSONRequestCommand::parse_json_request(Reader& reader, const char* const a_keys[], const jsmntype_t a_types[], unsigned n_types) {
  char* s_json = reader.fetch_as_string();
  int n_result = parse_json_string(s_json, a_keys, a_types, n_types);
  free(s_json);
  return n_result;
}

int SetConfigCommand::execute(Reader& o_reader, Writer& o_writer) {
  int n_result = parse_json_request(o_reader, KEY, TYPE, arraySize(KEY));
  o_config.f_save();
  produce_response(o_writer, n_result);
  return n_result;
}

int SetConfigCommand::execute(char* s_config) {
  int n_result = parse_json_string(s_config, KEY, TYPE, arraySize(KEY));
  o_config.f_save();
  return n_result;
}

bool SetConfigCommand::parsed_value(unsigned n_key, jsmntok_t* a_type, char* s_value) {
  Log.info("JSON Key: %s, Value: %s", KEY[n_key], (char*)s_value);
  int8_t n_result = o_config.f_setValue(String(KEY[n_key]), String(s_value));
  return (n_result != -1);
}

c_config& SetConfigCommand::o_config = c_config::f_getInstance();
const char* SetConfigCommand::KEY[13] = {"nme","rdt","mtt","rlt","rlp","srr","srt","mqtt","mqip","mqpt","mqus","mqpw","mqto"};
const jsmntype_t SetConfigCommand::TYPE[] = {
  JSMN_STRING,
  JSMN_PRIMITIVE,
  JSMN_PRIMITIVE,
  JSMN_PRIMITIVE,
  JSMN_PRIMITIVE,
  JSMN_PRIMITIVE,
  JSMN_PRIMITIVE,
  JSMN_PRIMITIVE,
  JSMN_STRING,
  JSMN_PRIMITIVE,
  JSMN_STRING,
  JSMN_STRING,
  JSMN_PRIMITIVE
};
