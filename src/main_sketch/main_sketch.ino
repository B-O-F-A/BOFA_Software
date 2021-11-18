#include "globals.h"

// the setup function runs once when you press reset or power the board

void setup() {

  controller_Mailbox = xQueueCreate(1, sizeof(msg_union));
  actuators_Mailbox = xQueueCreate(1, sizeof(msg_union));
  stepper_Mailbox = xQueueCreate(1, sizeof(msg_union));
  //ultrasonic_Mailbox = xQueueCreate(1, sizeof(msg_union));
  //colour_Mailbox = xQueueCreate(1, sizeof(msg_union));
  imu_command_Mailbox = xQueueCreate(1, sizeof(msg_union));
  //imu_ack_Mailbox = xQueueCreate(1, sizeof(msg_union));
  
  mutex = xSemaphoreCreateMutex();
  
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
    , 256 // Stack size
    , NULL
    , 1 // Priority
    , NULL );

    xTaskCreate(
      stepper
      , "stepper"
      , 128 // Stack size
      , NULL
      , 1 // Priority
      , NULL );

  xTaskCreate(
    ultrasonic
    , "ultrasonic"
    , 128 // Stack size
    , NULL
    , 1 // Priority
    , NULL );

  Serial.println("setup complete: starting.....");
  // Now the task scheduler, which takes over control of scheduling individual tasks, is automatically started.
}



void loop()
{
  //Serial.println("Initiallizing Setup........");
  //taskYIELD();
}
