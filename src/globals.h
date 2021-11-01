#ifndef GLOBALS_H
#define GLOBALS_H

#include <Arduino_FreeRTOS.h>
#include <Arduino.h>
#include <queue.h>

// define two tasks for Blink & AnalogRead
void controller( void *pvParameters );
void TaskAnalogRead( void *pvParameters );

static QueueHandle_t controller_Mailbox;
static QueueHandle_t AnalogRead_Mailbox;

#endif
