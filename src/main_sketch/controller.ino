void controller(void *pvParameters)
{
  (void) pvParameters;

  int counter = 0;
  int stepper_msg_counter = 0;
  int motor_speed = 80;
  msg_union ultrasonic_msg;
  msg_union msg;
  msg.motor_message.dir = FORWARD;

int HIGH_SPEED = 80;
int MED_SPEED = 65;

int HIGH_SPEED_ROT = 70;
int MED_SPEED_ROT = 55;

  for (;;)
  {

    if (xQueueReceive(ultrasonic_Mailbox, &ultrasonic_msg, 0) == pdPASS ) {
      switch (ultrasonic_msg.generic_message.type) {
        case MSG_ULTRASONIC:
          ultrasonic_message_t* ultrasonic_message;
          ultrasonic_message = &ultrasonic_msg.ultrasonic_message;

          if (DEBUG_ENABLED) {
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

    //FORWARD SPIKE
    if (counter == secs_to_count(1)) {
      msg.motor_message.dir = FORWARD;
      motor_speed = MED_SPEED;
    }

    //STOP
    if (counter == secs_to_count(3)) {
      msg.motor_message.dir = STOP;
    }

    //BACKWARD SPIKE
    if (counter == secs_to_count(4)) {
      msg.motor_message.dir = BACKWARD;
      motor_speed = HIGH_SPEED;
    }

    if (counter == secs_to_count(5)) {
      msg.motor_message.dir = BACKWARD;
      motor_speed = MED_SPEED;
    }

    //STOP
    if (counter == secs_to_count(7)) {
      msg.motor_message.dir = STOP;
    }


    //RIGHT SPIKE
    if (counter == secs_to_count(8)) {
      msg.motor_message.dir = RIGHT;
      motor_speed = HIGH_SPEED_ROT;
    }

    if (counter == secs_to_count(9)) {
      msg.motor_message.dir = RIGHT;
      motor_speed = MED_SPEED_ROT;
    }


    //STOP
    if (counter == secs_to_count(11)) {
      msg.motor_message.dir = STOP;
    }


    //LEFT SPIKE
    if (counter == secs_to_count(12)) {
      msg.motor_message.dir = LEFT;
      motor_speed = HIGH_SPEED_ROT;
    }

    if (counter == secs_to_count(13)) {
      msg.motor_message.dir = LEFT;
      motor_speed = MED_SPEED_ROT;
    }


    //STOP
    if (counter == secs_to_count(15)) {
      msg.motor_message.dir = STOP;
    }

    msg.motor_message.spd = motor_speed;


    counter ++;
    
    //Serial.print("Controller: Motor spd is ");
    //Serial.println(msg.motor_message.spd);
    xQueueSend(actuators_Mailbox, &msg, 0);
    vTaskDelay(pdMS_TO_TICKS(200));
  }
}
int secs_to_count(float secs) {
  return secs / 0.2;
}
