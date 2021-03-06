#ifndef GLOBALS_H
#define GLOBALS_H

#define on true
#define off false

#define DEBUG_ENABLED off
#define DEBUG_ACTUATORS_ENABLED off
#define DEBUG_MOTOR_DIR off
#define DEBUG_IMU_ENABLED off
#define DEBUG_TCS_ENABLED off

//includes
#include <Arduino_FreeRTOS.h>
#include <queue.h>
#include <timers.h>
#include <semphr.h>
//#include <Stepper.h>
#include <Servo.h>
#include "MPU9250.h"
#include "Adafruit_TCS34725.h"

MPU9250 mpu;
//colour sensor stuff
Adafruit_TCS34725 tcs[] = {Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_2_4MS, TCS34725_GAIN_4X),
                           Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_2_4MS, TCS34725_GAIN_4X),
                           Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_2_4MS, TCS34725_GAIN_4X),
                           Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_2_4MS, TCS34725_GAIN_4X),
                           Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_2_4MS, TCS34725_GAIN_4X)
                          };
const float COLOUR_THRESH = 20;
const float COLOUR_THRESH_GREEN = 15;
float red_values[3]  = {0, 0, 0};

//BreakBeam Pin
const int beamPin = 2;

// Right Motor Pins
const int pwmA = 8;
const int in1A = 9;
const int in2A = 10;

// Left Motor Pins
const int in1B = 11;
const int in2B = 12;
const int pwmB = 13;

//Stepper Motor Pins
//const int StepperPins[4] = {5, 4, 3, 2}; //STEPPED PINS NEEDS CHANGES

Servo gate_servo;
const int servoPin = 3;

//Ultrasonic Sensor Pins
const int trigPin = 6; //ULTRASONIC PIN NEEDS CHAGE
const int echoPin = 7; //ULTRASONIC PIN NEEDS CHAGE

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
  LEFT_COL,
  MID_COL,
  RIGHT_COL,
  LEFT_AUX_COL,
  RIGHT_AUX_COL,
  IMU,

  //add direction types before this enum
  TOT_NUM_I2C
} i2c_sensor_type_e;

typedef enum
{
  INITIAL,
  NOMINAL,
  ROTATE1,
  ROTATE2,
  DROPOFF_COMPLETE,

  TOT_NUM_RETURN_MODES
} return_mode_e;

//RETURN MODE DECLARATION//////////
return_mode_e RETURN_MODE;
///////////////////////////////////

typedef enum
{
  STEPPER_MOVE_NONE,
  CLOSE_GATE,
  OPEN_GATE
} stepper_direction_type_e;


const int CLOSE_SERVO_GATE = 20;
const int OPEN_SERVO_GATE = 180;



//enum definitions
typedef enum {
  DIRECTION_NONE,
  STOP,
  FORWARD,
  BACKWARD,
  LEFT,
  RIGHT,
  LEFT_SLOW,
  LEFT_SLOW_SLOW,
  RIGHT_SLOW,
  FORWARD_SLOW,

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
  uint8_t spd; //percentage of max speed
  int8_t error; //error val
  direction_type_e dir; //direction that the robot should be going
} motor_message_t;

typedef struct {
  message_type_e type;
  stepper_direction_type_e step_dir; //direction of stepper
  int steps; //num Steps moving, +ve Steps(counter_clockwise gate), -ve Steps(clockwise gate)
} stepper_message_t;

//typedef struct {
//  message_type_e type;
//  stepper_direction_type_e step_dir; //direction of servo
//} stepper_message_t;
//
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

void backup_robot(int delay_time, float speed_val);
void turn_left_robot_slow(int delay_time);
void turn_right_robot_slow(int delay_time);

static QueueHandle_t controller_Mailbox;
static QueueHandle_t actuators_Mailbox;
static QueueHandle_t stepper_Mailbox;
//static QueueHandle_t ultrasonic_Mailbox;
//static QueueHandle_t colour_Mailbox;
static QueueHandle_t imu_command_Mailbox;
//static QueueHandle_t imu_ack_Mailbox;

static SemaphoreHandle_t mutex;
static SemaphoreHandle_t mutexPrint;

int initial_return_timer_delay = 1000;

void pxInitialReturnTimerCallback (TimerHandle_t xTimer) {
  RETURN_MODE = NOMINAL;
}

TimerHandle_t initial_return_timer = xTimerCreate
                                     ( "initial_return_timer",
                                       initial_return_timer_delay,
                                       pdTRUE,
                                       ( void * ) 0,
                                       pxInitialReturnTimerCallback);



#endif
