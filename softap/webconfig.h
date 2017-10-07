// $Id$
/**
 * @file door.h
 * @brief Soft AP configuration for LAN mode
 * @author Denis Grisak
 * @version 1.9
 */
// $Log$

#ifndef WEBCONFIG_H
#define WEBCONFIG_H

#include "Particle.h"
#include "softap_http.h"
#include "command.h"
#include "config.h"

struct c_page {
    const char* url;
    const char* mime_type;
    const char* data;
    const bool force_type;
};

void f_pageHandler(const char* s_url, ResponseCallback* f_callback, void* cbArg, Reader* o_request, Writer* o_response, void* p_reserved);

#endif
