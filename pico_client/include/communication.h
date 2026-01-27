#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <string.h>
#include <time.h>
#include <stdbool.h>

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include "secrets.h"

typedef struct 
{
    struct altcp_pcb *pcb;
    bool complete;
} http_request_t;

bool send_to_thingspeak(int reading);

#endif // COMMUNICATION_H
