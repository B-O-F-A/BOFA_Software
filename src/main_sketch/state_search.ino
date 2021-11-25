//  COLOUR SENSORS USED:
//  LEFT_COLOUR
//  MID_COLOUR
//  RIGHT_COLOUR
//  LEFT_AUX_COLOUR
//  RIGHT_AUX_COLOUR


//bool first_time = true;

state_e state_search(msg_union &msg, colour_type_e (&tcs_sen)[5]) {
  static motor_message_t motor_msg;
  static uint32_t prev_time = millis();
  uint32_t curr_time = millis();

  const float AVG_SPEED = 80;
  const float SLOW_SPEED = 25;
  const float AVG_SPEED_ROT = 60;

  motor_msg.type = MSG_MOTOR;
  motor_msg.spd = AVG_SPEED;
  motor_msg.dir = DIRECTION_NONE;

  //  xSemaphoreTake(mutexPrint, portMAX_DELAY);
  //  Serial.print("Controller: Error_val:");
  //  Serial.println(motor_msg.error);
  //  xSemaphoreGive(mutexPrint);

  if (xQueueReceive(controller_Mailbox, &msg, 0) == pdPASS ) {
    switch (msg.generic_message.type) {
      case MSG_COLOUR:
        colour_message_t* colour_message;
        colour_message = &msg.colour_message;

        if (DEBUG_ENABLED) {
          xSemaphoreTake(mutexPrint, portMAX_DELAY);
          Serial.print("Controller: Received colour_msg: ");
          for (int i = 0; i < (TOT_NUM_I2C - 1); i++) {

            Serial.print(colour_message->colour[i]); Serial.print(" ");
          }
          Serial.println(" --- ");
          xSemaphoreGive(mutexPrint);
        }

        for (int i = 0; i < (TOT_NUM_I2C - 1); i++) {
          tcs_sen[i] = colour_message->colour[i];
        }
        //BACKWARD EMERGENCY STRAIGHT LINE
        if (tcs_sen[LEFT_AUX_COL] == RED) {
          motor_msg.dir = LEFT;
          motor_msg.type = MSG_MOTOR;
          motor_msg.error = -1;
        }

        else if (tcs_sen[RIGHT_AUX_COL] == RED) {
          motor_msg.dir = RIGHT;
          motor_msg.type = MSG_MOTOR;
          motor_msg.error = 1;
        }
        else if (tcs_sen[MID_COL] == RED) {
          motor_msg.dir = FORWARD;
          motor_msg.type = MSG_MOTOR;
          motor_msg.error = 0;
        }
        else {
          motor_msg.dir = FORWARD;
          motor_msg.type = MSG_MOTOR;
          motor_msg.spd = AVG_SPEED;
        }

        if (tcs_sen[LEFT_AUX_COL] == BLUE && tcs_sen[RIGHT_AUX_COL] == BLUE) {
          motor_msg.dir = STOP;
          motor_msg.type = MSG_MOTOR;
          motor_msg.spd = 0;
          motor_msg.error = 0;
          xQueueSend(actuators_Mailbox, &motor_msg, 0);
          delay(200);
          motor_msg.dir = FORWARD;
          motor_msg.type = MSG_MOTOR;
          motor_msg.spd = SLOW_SPEED;
          motor_msg.error = 0;
          xQueueSend(actuators_Mailbox, &motor_msg, 0);
          return STATE_SLOW;

        }

        xQueueSend(actuators_Mailbox, &motor_msg, 0);

        break;

      case MSG_ULTRASONIC_ACK:

        break;

      default:
        xSemaphoreTake(mutexPrint, portMAX_DELAY);
        Serial.print("Error: controller task recieved unknown message type with type: ");
        Serial.println(msg.generic_message.type);
        xSemaphoreGive(mutexPrint);
    }
  }
  //  if (first_time){
  //    first_time = false;
  //    send_to_imu();
  //  }

  return STATE_SEARCH;
}

//void send_to_imu() {
//  //  if (DEBUG_ENABLED) {
//  //    Serial.print("Ultrasonic: Msg Sent to Controller is ");
//  //    Serial.println(true);
//  //  }
//  msg_union msg;
//  msg.imu_command_message.type = MSG_IMU_COMMAND;
//  msg.imu_command_message.yaw = 90;
//
//  xQueueSend(imu_command_Mailbox, &msg, portMAX_DELAY);
//}
