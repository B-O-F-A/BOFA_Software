#include "globals.h"

// the setup function runs once when you press reset or power the board
static void xBeamInterruptHandler(){
  detachInterrupt(digitalPinToInterrupt(beamPin));
  msg_union msg;
  msg.ultrasonic_ack_message.type = MSG_ULTRASONIC_ACK;
  msg.ultrasonic_ack_message.john_in_range = true;
  xQueueSend(controller_Mailbox, &msg, portMAX_DELAY);        
}

void setup() {

  controller_Mailbox = xQueueCreate(1, sizeof(msg_union));
  actuators_Mailbox = xQueueCreate(1, sizeof(msg_union));
  stepper_Mailbox = xQueueCreate(1, sizeof(msg_union));
  //ultrasonic_Mailbox = xQueueCreate(1, sizeof(msg_union));
  //colour_Mailbox = xQueueCreate(1, sizeof(msg_union));
  imu_command_Mailbox = xQueueCreate(1, sizeof(msg_union));
  //imu_ack_Mailbox = xQueueCreate(1, sizeof(msg_union));
  
  mutex = xSemaphoreCreateMutex();
  mutexPrint = xSemaphoreCreateMutex();
  
  
  
  gate_servo.attach(servoPin);
  gate_servo.write(OPEN_SERVO_GATE);
  

  Wire.begin();
  delay(500);
  Serial.begin(9600);
  Serial.println("Initiallizing Setup........");

  xTaskCreate(
    colour_imu
    , "colour_imu"
    , 1024 // Stack size
    , NULL
    , 1 // Priority
    , NULL );
    
  xTaskCreate(
    controller
    , "controller" // A name just for humans
    , 1024 // This stack size can be checked & adjusted by reading the Stack Highwater
    , NULL
    , 1 // Priority, with 1 being the highest, and 4 being the lowest.
    , NULL );

  xTaskCreate(
    actuators
    , "actuators"
    , 512 // Stack size
    , NULL
    , 1 // Priority
    , NULL );

//    xTaskCreate(
//      stepper
//      , "stepper"
//      , 128 // Stack size
//      , NULL
//      , 1 // Priority
//      , NULL );

//  xTaskCreate(
//    ultrasonic
//    , "ultrasonic"
//    , 128 // Stack size
//    , NULL
//    , 1 // Priority
//    , NULL );

  Serial.println("setup complete: starting.....");
  // Now the task scheduler, which takes over control of scheduling individual tasks, is automatically started.
}



void loop()
{
  //Serial.println("Initiallizing Setup........");
  //taskYIELD();
}

//ULTILITY FUNCTIONS
void backup_robot(int delay_time, float speed_val) {
  motor_message_t motor_msg;
  motor_msg.type = MSG_MOTOR;
  motor_msg.spd = speed_val;
  motor_msg.error = 0;
  motor_msg.dir = BACKWARD;
  xQueueSend(actuators_Mailbox, &motor_msg, 0);
  delay(delay_time);
}
void turn_left_robot_slow(int delay_time) {
  motor_message_t motor_msg;
  motor_msg.type = MSG_MOTOR;
  motor_msg.spd = 0;
  motor_msg.error = -1;
  motor_msg.dir = LEFT_SLOW;
  xQueueSend(actuators_Mailbox, &motor_msg, 0);
  delay(delay_time);
}
void turn_left_robot_slow_slow(int delay_time) {
  motor_message_t motor_msg;
  motor_msg.type = MSG_MOTOR;
  motor_msg.spd = 0;
  motor_msg.error = -1;
  motor_msg.dir = LEFT_SLOW_SLOW;
  xQueueSend(actuators_Mailbox, &motor_msg, 0);
  delay(delay_time);
}
void turn_right_robot_slow(int delay_time) {
  motor_message_t motor_msg;
  motor_msg.type = MSG_MOTOR;
  motor_msg.spd = 0;
  motor_msg.error = 1;
  motor_msg.dir = RIGHT_SLOW;
  xQueueSend(actuators_Mailbox, &motor_msg, 0);
  delay(delay_time);
}
