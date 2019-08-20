// $Id$
/**
 * @file command.h
 * @brief Implements SoftAP command with JSON request/response
 */
// $Log$

#ifndef COMMAND_H
#define COMMAND_H

#include "application.h"
#include "softap_http.h"
#include "config.h"
#include <jsmn.h>

class Command {
  public:
    virtual int execute(Reader& reader, Writer& writer);
};

class JSONCommand : public Command {
  protected:
    jsmntok_t* json_tokenise(char *s_json);
    bool json_token_streq(const char *s_json, jsmntok_t *t, const char *s_string);
    char* json_token_tostr(char *s_json, jsmntok_t *a_token);
    void write_result_code(Writer& writer, int result);
    virtual int parse_request(Reader& reader);
    virtual void produce_response(Writer& writer, int result);
    virtual int process();
    void write_char(Writer& w, char c);
    void write_quoted_string(Writer& out, const char* s_string);
    void write_json_string(Writer& out, const char* s_name, const char* s_value);
    char* int_to_ascii(int val, char* buf, int i);
    void write_json_int(Writer& out, const char* s_name, int n_value);
  public:
    virtual int execute(Reader& reader, Writer& writer);
};

class JSONRequestCommand : public JSONCommand {
  protected:
    virtual bool parsed_value(unsigned index, jsmntok_t* a_token, char* s_value);
    int parse_json_string(char* s_json, const char* const keys[], const jsmntype_t a_types[], unsigned n_types);
    int parse_json_request(Reader& reader, const char* const keys[], const jsmntype_t a_types[], unsigned n_types);
};

class SetConfigCommand : public JSONRequestCommand {
    static const char* KEY[13];
    static const jsmntype_t TYPE[];
    static c_config& o_config;
    bool parsed_value(unsigned n_key, jsmntok_t* a_token, char* s_string);

  public:
    int execute(Reader& o_reader, Writer& o_writer);
    int execute(char* s_config);
};

#endif
