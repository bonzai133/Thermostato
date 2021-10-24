#ifndef _web_h_
#define _web_h_

#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

void init_server(void);
void server_cleanup(void);

#endif