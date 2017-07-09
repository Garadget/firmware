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
#include "door.h"
#include "config.h"

struct c_page {
    const char* url;
    const char* mime_type;
    const char* data;
};

void f_pageHandler(const char* url, ResponseCallback* cb, void* cbArg, Reader* body, Writer* result, void* reserved);

#endif
