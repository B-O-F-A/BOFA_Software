#ifndef GLOBALS_H
#define GLOBALS_H

#define DEBUG_ENABLED false
#define DEBUG_ACTUATORS_ENABLED false

//includes
#include <Arduino_FreeRTOS.h>
#include <queue.h>
#include <timers.h>
#include <semphr.h>
#include <Stepper.h>
#include <MPU9250.h>
#include <RunningMedian.h>

// Left Motor Pins
const int pwmA = 8;
const int in1A = 9;
const int in2A = 10;

// Right Motor Pins
const int in1B = 12;
const int in2B = 11;
const int pwmB = 13;

//Stepper Motor Pins
const int StepperPins[4] = {8, 9, 10, 11}; //STEPPED PINS NEEDS CHANGES

//Ultrasonic Sensor Pins
const int trigPin = 12; //ULTRASONIC PIN NEEDS CHAGE
const int echoPin = 13; //ULTRASONIC PIN NEEDS CHAGE


typedef enum
{
  COLOUR_NONE,
  RED,
  GREEN,
  BLUE,

  TOT_NUM_COLOUR
} colour_type_e;

typedef enum
{
  LEFT_COLOUR,
  MID_COLOUR,
  RIGHT_COLOUR,
  LEFT_AUX_COLOUR,
  RIGHT_AUX_COLOUR,
  IMU,

  //add direction types before this enum
  TOT_NUM_I2C
} i2c_sensor_type_e;

typedef enum
{
  STEPPER_MOVE_NONE,
  CLOSE_GATE,
  OPEN_GATE
} stepper_direction_type_e;

//enum definitions
typedef enum {
  DIRECTION_NONE,
  STOP,
  FORWARD,
  BACKWARD,
  LEFT,
  RIGHT,

  //add direction types before this enum
  TOT_NUM_DIR
} direction_type_e;


//message definitions
typedef enum {
  MSG_GENERIC,
  MSG_MOTOR,
  MSG_STEPPER,
  MSG_ULTRASONIC,
  MSG_COLOUR,
  MSG_IMU,
  MSG_TIMER,

  //add messages types before this message
  TOT_NUM_MSG,
} message_type_e;

typedef struct {
  message_type_e type;
} generic_message_t;

typedef struct {
  message_type_e type;
  uint32_t timestamp; // time this message was generated
  uint8_t spd; //percentage of max speed
  direction_type_e dir; //direction that the robot should be going
  stepper_direction_type_e step_dir; //direction of stepper
} motor_message_t;

typedef struct {
  message_type_e type;
  uint32_t timestamp; // time this message was generated
  int steps; //num Steps moving, +ve Steps(counter_clockwise gate), -ve Steps(clockwise gate)
} stepper_message_t;

typedef struct {
  message_type_e type;
  uint32_t timestamp; // time this message was generated
  float dist; //distance to object in cm
} ultrasonic_message_t;

typedef struct {
  message_type_e type;
  uint32_t timestamp; // time this message was generated
  message_type_e colour[5]; //distance to object in cm
} colour_message_t;

typedef struct {
  message_type_e type;
  uint32_t timestamp; // time this message was generated
  float yaw; //distance to object in cm
} imu_message_t;

typedef struct {
  message_type_e type;
  uint32_t timestamp; // time this message was generated
  const char* timer_name;
} timer_message_t;

typedef union {
  generic_message_t generic_message;
  motor_message_t motor_message;
  stepper_message_t stepper_message;
  ultrasonic_message_t ultrasonic_message;
  colour_message_t colour_message;
  imu_message_t imu_message;
  timer_message_t timer_message;
} msg_union;

void controller( void *pvParameters );
void actuators( void *pvParameters );
void stepper( void *pvParameters );
void ultrasonic( void *pvParameters );
void colour_imu( void *pvParameters );

static QueueHandle_t controller_Mailbox;
static QueueHandle_t actuators_Mailbox;
static QueueHandle_t stepper_Mailbox;
static QueueHandle_t ultrasonic_Mailbox;
static QueueHandle_t colour_Mailbox;
static QueueHandle_t imu_Mailbox;

#endif
