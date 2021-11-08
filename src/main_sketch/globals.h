#ifndef GLOBALS_H
#define GLOBALS_H
//includes
#include <Arduino_FreeRTOS.h>
#include <queue.h>
#include <timers.h>
#include <semphr.h>

//enum definitions
typedef enum{
  DIRECTION_NONE,
  STOP,
  FORWARD,
  BACKWARD,
  LEFT,
  RIGHT,

  //add direction types before this enum
  TOT_NUM_DIR
}direction_type_e;


//message definitions
typedef enum{
  MSG_GENERIC,
  MSG_MOTOR,
  MSG_TIMER,
  
  //add messages types before this message
  TOT_NUM_MSG,
}message_type_e;

typedef struct{
  message_type_e type;
}generic_message_t;

typedef struct{
  message_type_e type;
  uint32_t timestamp; // time this message was generated
  uint8_t spd; //percentage of max speed
  direction_type_e dir; //direction that the robot should be going
}motor_message_t;

typedef struct{
  message_type_e type;
  uint32_t timestamp; // time this message was generated
  const char* timer_name;
}timer_message_t;

typedef union{
  generic_message_t generic_message;
  motor_message_t motor_message;
  timer_message_t timer_message;
}msg_union;

void controller( void *pvParameters );
void TaskAnalogRead( void *pvParameters );

static QueueHandle_t controller_Mailbox;
static QueueHandle_t actuators_Mailbox;

#endif
