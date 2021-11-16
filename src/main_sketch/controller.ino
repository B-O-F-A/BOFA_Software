void controller(void *pvParameters)
{
  (void) pvParameters;

  int coutner = 0;
  int stepper_msg_counter = 0;
  int motor_speed = 80;
  msg_union ultrasonic_msg;
  msg_union msg;

  for (;;)
  {

    if (xQueueReceive(ultrasonic_Mailbox, &ultrasonic_msg, 0) == pdPASS ) {
      switch (ultrasonic_msg.generic_message.type) {
        case MSG_ULTRASONIC:
          ultrasonic_message_t* ultrasonic_message;
          ultrasonic_message = &ultrasonic_msg.ultrasonic_message;

          if (debugEnabled){
            Serial.print("Controller: Received Dist: ");
            Serial.println(ultrasonic_message->dist);  
          }
          
          break;

        default:
          Serial.print("Error: controller task recieved unknown message type with type: ");
          Serial.println(msg.generic_message.type);
      }

    }

    msg.motor_message.type = MSG_MOTOR;
    msg.motor_message.timestamp = millis();
    msg.motor_message.step_dir = STEPPER_MOVE_NONE;

    if (coutner == 10) {
      motor_speed = 65;
    }

    if (coutner == 20) {
      msg.motor_message.step_dir = CLOSE_GATE;
    }

    if (coutner == 30) {
      motor_speed = 80;
    }

    if (coutner == 40) {
      motor_speed = 0;
    }

    msg.motor_message.spd = motor_speed;
    msg.motor_message.dir = FORWARD;

    coutner ++;
    //Serial.print("Controller: Motor spd is ");
    //Serial.println(msg.motor_message.spd);
    xQueueSend(actuators_Mailbox, &msg, 0);
    vTaskDelay(pdMS_TO_TICKS(200));
  }
}
