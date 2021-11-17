#include "globals.h"

// the setup function runs once when you press reset or power the board
void setup() {

  controller_Mailbox = xQueueCreate(1, sizeof(msg_union));
  actuators_Mailbox = xQueueCreate(1, sizeof(msg_union));
  stepper_Mailbox = xQueueCreate(1, sizeof(msg_union));
  ultrasonic_Mailbox = xQueueCreate(1, sizeof(msg_union));
  colour_Mailbox = xQueueCreate(1, sizeof(msg_union));
  imu_Mailbox = xQueueCreate(1, sizeof(msg_union));

  Serial.begin(9600);
  Serial.println("Initiallizing Setup........");

  xTaskCreate(
    controller
    , "controller" // A name just for humans
    , 128 // This stack size can be checked & adjusted by reading the Stack Highwater
    , NULL
    , 1 // Priority, with 1 being the highest, and 4 being the lowest.
    , NULL );

  xTaskCreate(
    actuators
    , "actuators"
    , 128 // Stack size
    , NULL
    , 1 // Priority
    , NULL );

//  xTaskCreate(
//    stepper
//    , "stepper"
//    , 128 // Stack size
//    , NULL
//    , 1 // Priority
//    , NULL );
//
//  xTaskCreate(
//    ultrasonic
//    , "ultrasonic"
//    , 128 // Stack size
//    , NULL
//    , 1 // Priority
//    , NULL );
//
//  xTaskCreate(
//    colour_imu
//    , "colour_imu"
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
