void controller(void *pvParameters)
{
  (void) pvParameters;

  CURR_STATE = STATE_IDLE;

  int HIGH_SPEED = 60;
  int MED_SPEED = 30;
  int HIGH_SPEED_ROT = 90;
  int MED_SPEED_ROT = 90;

  int counter = 0;
  int stepper_msg_counter = 0;
  int motor_speed = HIGH_SPEED_ROT;

  msg_union ultrasonic_msg;

  msg_union imu_msg;

  msg_union msg;
  msg.motor_message.dir = RIGHT;
  for (;;)
  {
    
    if (counter == secs_to_count(3)) {
      send_to_imu(-90);
    }

    //IMU_MESSAGE_ACK to be used in SATE_DROPOFF
    if (xQueueReceive(imu_ack_Mailbox, &imu_msg, 0) == pdPASS ) {
      switch (imu_msg.generic_message.type) {
        case MSG_IMU_ACK:
          imu_ack_message_t* imu_ack_message;
          imu_ack_message = &imu_msg.imu_ack_message;

          if (DEBUG_ENABLED) {
            Serial.print("Controller: Received IMU ACK: ");
            Serial.println(imu_ack_message->angle_reached);
          }
          //Do stuf when Angle reached
          break;

        default:
          Serial.print("Error: controller task recieved unknown message type with type: ");
          Serial.println(msg.generic_message.type);
      }
    }

    //ULTRASONIC_MESSAGE_ACK to be used in SATE_SLOW
    if (xQueueReceive(ultrasonic_Mailbox, &ultrasonic_msg, 0) == pdPASS ) {
      switch (ultrasonic_msg.generic_message.type) {
        case MSG_ULTRASONIC_ACK:
          ultrasonic_ack_message_t* ultrasonic_ack_message;
          ultrasonic_ack_message = &ultrasonic_msg.ultrasonic_ack_message;

          if (DEBUG_ENABLED) {
            Serial.print("Controller: Received Dist: ");
            Serial.println(ultrasonic_ack_message->john_in_range);
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

    //    //FORWARD SPIKE
    //    if (counter == secs_to_count(1)) {
    //      msg.motor_message.dir = FORWARD;
    //      motor_speed = MED_SPEED;
    //    }
    //
    //    //STOP
    //    if (counter == secs_to_count(3)) {
    //      msg.motor_message.dir = STOP;
    //    }
    //
    //    //BACKWARD SPIKE
    //    if (counter == secs_to_count(4)) {
    //      msg.motor_message.dir = BACKWARD;
    //      motor_speed = HIGH_SPEED;
    //    }
    //
    //    if (counter == secs_to_count(5)) {
    //      msg.motor_message.dir = BACKWARD;
    //      motor_speed = MED_SPEED;
    //    }
    //
    //    //STOP
    //    if (counter == secs_to_count(7)) {
    //      msg.motor_message.dir = STOP;
    //    }


    //RIGHT SPIKE
    if (counter == secs_to_count(8 - 7)) {
      msg.motor_message.dir = RIGHT;
      motor_speed = HIGH_SPEED_ROT;
    }

    if (counter == secs_to_count(9 - 7)) {
      msg.motor_message.dir = RIGHT;
      motor_speed = MED_SPEED_ROT;
    }


    //STOP
    if (counter == secs_to_count(11 - 7)) {
      msg.motor_message.dir = STOP;
    }


    //LEFT SPIKE
    if (counter == secs_to_count(12 - 7)) {
      msg.motor_message.dir = LEFT;
      motor_speed = HIGH_SPEED_ROT;
    }

    if (counter == secs_to_count(13 - 7)) {
      msg.motor_message.dir = LEFT;
      motor_speed = MED_SPEED_ROT;
    }


    //STOP
    if (counter == secs_to_count(15 - 7)) {
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
void send_to_imu(float des_angle) {

  Serial.print("Controller: Msg Sent to IMU: ");
  Serial.println(des_angle);

  msg_union msg;
  msg.imu_command_message.type = MSG_IMU_COMMAND;
  msg.imu_command_message.yaw = des_angle;

  xQueueSend(imu_command_Mailbox, &msg, 0); //maybe need portMAX_DELAY ??
}

int secs_to_count(float secs) {
  return secs / 0.2;
}
