#ifndef WIFI_H
#define WIFI_H

#include <stdbool.h>

bool wifi_init();
bool wifi_connect(const char* ssid, const char* password);

#endif
