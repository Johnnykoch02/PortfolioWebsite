#ifndef __ROUTING_H__
#define __ROUTING_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../mongoose.h"

/* Libraries*/
/**
 * Serves Routes to the client:
 * @param s_route route html file to send
*/
void serve_route(struct mg_connection *nc, int ev, struct mg_http_message* msg, char* s_route);

/**
 * Serves Page Not Found Error
*/
void serve_pnf(struct mg_connection *nc, int ev, struct mg_http_message* msg);

/**
 * Serves Page Forbidden
*/
void serve_fbd(struct mg_connection *nc, int ev, struct mg_http_message* msg);

#endif