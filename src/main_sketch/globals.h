#ifndef GLOBALS_H
#define GLOBALS_H

#define on true
#define off false

#define DEBUG_ENABLED on
#define DEBUG_ACTUATORS_ENABLED off
#define DEBUG_IMU_ENABLED off
#define DEBUG_TCS_ENABLED off

//includes
#include <Arduino_FreeRTOS.h>
#include <queue.h>
#include <timers.h>
#include <semphr.h>
#include <Stepper.h>
#include "MPU9250.h"
#include "Adafruit_TCS34725.h"
#include <RunningMedian.h>

MPU9250 mpu;
//colour sensor stuff
Adafruit_TCS34725 tcs[] = {Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_2_4MS, TCS34725_GAIN_16X),
                           Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_2_4MS, TCS34725_GAIN_16X),
                           Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_2_4MS, TCS34725_GAIN_16X),
                           Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_2_4MS, TCS34725_GAIN_16X),
                           Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_2_4MS, TCS34725_GAIN_16X)
                          };
const float COLOUR_THRESH = 25;

// Left Motor Pins
const int pwmA = 8;
const int in1A = 9;
const int in2A = 10;

// Right Motor Pins
const int in1B = 11;
const int in2B = 12;
const int pwmB = 13;

//Stepper Motor Pins
const int StepperPins[4] = {5, 4, 3, 2}; //STEPPED PINS NEEDS CHANGES

//Ultrasonic Sensor Pins
const int trigPin = 6; //ULTRASONIC PIN NEEDS CHAGE
const int echoPin = 7; //ULTRASONIC PIN NEEDS CHAGE

static SemaphoreHandle_t mutex;

typedef enum
{
  STATE_IDLE,
  STATE_SEARCH,
  STATE_SLOW,
  STATE_COLLECT,
  STATE_RETURN,
  STATE_DROPOFF,
  STATE_STOP,

  TOT_NUM_STATES
} state_e;

//STATE DECLARATION/////////////
state_e CURR_STATE;
////////////////////////////////

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
  MSG_ULTRASONIC_ACK,
  MSG_COLOUR,
  MSG_IMU_COMMAND,
  MSG_IMU_ACK,
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
  int steps; //num Steps moving, +ve Steps(counter_clockwise gate), -ve Steps(clockwise gate)
} stepper_message_t;

typedef struct {
  message_type_e type;
  bool john_in_range; //Acknowledgement
} ultrasonic_ack_message_t;

typedef struct {
  message_type_e type;
  colour_type_e colour[TOT_NUM_I2C - 1];
} colour_message_t;

typedef struct {
  message_type_e type;
  float yaw; //+ve CounterClockwise, -ve Clockwise
} imu_command_message_t;

typedef struct {
  message_type_e type;
  bool angle_reached; //Confirmation of reaching angle.
} imu_ack_message_t;

typedef struct {
  message_type_e type;
  uint32_t timestamp; // time this message was generated
  const char* timer_name;
} timer_message_t;

typedef union {
  generic_message_t generic_message;

  motor_message_t motor_message;
  stepper_message_t stepper_message;

  ultrasonic_ack_message_t ultrasonic_ack_message;

  colour_message_t colour_message;
  imu_command_message_t imu_command_message;
  imu_ack_message_t imu_ack_message;

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
//static QueueHandle_t ultrasonic_Mailbox;
//static QueueHandle_t colour_Mailbox;
static QueueHandle_t imu_command_Mailbox;
//static QueueHandle_t imu_ack_Mailbox;

#endif
