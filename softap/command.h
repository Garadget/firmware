// $Id$
/**
 * @file command.h
 * @brief Implements SoftAP command with JSON request/response
 */
// $Log$

#ifndef COMMMAND_H
#define COMMAND_H

#include "Particle.h"
#include "softap_http.h"
#include "../config.h"
#include <jsmn.h>
#define JSON_DEBUG(x) Serial.printlnf x

class Command {
  public:
    virtual int execute(Reader& reader, Writer& writer);
};

class JSONCommand : public Command {
  protected:
    jsmntok_t* json_tokenise(char *js);
    bool json_token_streq(const char *js, jsmntok_t *t, const char *s);
    char* json_token_tostr(char *js, jsmntok_t *t);
    void write_result_code(Writer& writer, int result);
    virtual int parse_request(Reader& reader);
    virtual void produce_response(Writer& writer, int result);
    virtual int process();
    void write_char(Writer& w, char c);
    void write_quoted_string(Writer& out, const char* s);
    void write_json_string(Writer& out, const char* name, const char* value);
    char* int_to_ascii(int val, char* buf, int i);
    void write_json_int(Writer& out, const char* name, int value);
  public:
    virtual int execute(Reader& reader, Writer& writer);
};

class JSONRequestCommand : public JSONCommand {
  protected:
    virtual bool parsed_value(unsigned index, jsmntok_t* t, char* value);
    int parse_json_request(Reader& reader, const char* const keys[], const jsmntype_t types[], unsigned count);
};

class SetConfigCommand : public JSONRequestCommand {
    static const char* KEY[11];
    static const jsmntype_t TYPE[];
    static c_config& o_config;
    bool parsed_value(unsigned key, jsmntok_t* t, char* str);
  public:
    int execute(Reader& o_reader, Writer& o_writer);
};

#endif
