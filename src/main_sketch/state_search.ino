//  COLOUR SENSORS USED:
//  LEFT_COLOUR
//  MID_COLOUR
//  RIGHT_COLOUR
//  LEFT_AUX_COLOUR
//  RIGHT_AUX_COLOUR
bool first_time = true;

state_e state_search(msg_union &msg) {
  static colour_type_e tcs_sen[5] = {0, 0, 0, 0, 0};
  static motor_message_t motor_msg;
  static uint32_t prev_time = millis();

  uint32_t curr_time = millis();

  const int AVG_SPEED = 40;

  const int AVG_SPEED_ROT = 60;
  motor_msg.type = MSG_MOTOR;
  motor_msg.spd = AVG_SPEED;
  motor_msg.dir = FORWARD;
  motor_msg.error = 0;

  if (xQueueReceive(controller_Mailbox, &msg, 0) == pdPASS ) {
    switch (msg.generic_message.type) {
      case MSG_COLOUR:
        colour_message_t* colour_message;
        colour_message = &msg.colour_message;

        if (DEBUG_ENABLED) {
          Serial.print("Controller: Received colour_msg: ");
          for (int i = 0; i < (TOT_NUM_I2C - 1); i++) {

            Serial.print(colour_message->colour[i]); Serial.print(" ");
          }
          Serial.println(" ");
        }

        for (int i = 0; i < (TOT_NUM_I2C - 1); i++) {
          tcs_sen[i] = colour_message->colour[i];
        }

        //STRAIGHT LINE
        if ((tcs_sen[MID_COL] == RED) && (tcs_sen[LEFT_COL] != RED) && (tcs_sen[RIGHT_COL] != RED)) {
          motor_msg.error = 0;
        }
        //TURN LEFT
        else if ((tcs_sen[LEFT_COL] == RED) && (tcs_sen[RIGHT_COL] != RED)) {
          motor_msg.error = -1;
        }
        //TURN RIGHT
        else if ((tcs_sen[LEFT_COL] != RED) && (tcs_sen[RIGHT_COL] == RED)) {
          motor_msg.error = 1;
        }
/*
        Serial.print("Controller: motor_msg{type: ");
        Serial.print(motor_msg.type);
        Serial.print(" spd: ");
        Serial.print(motor_msg.spd);
        Serial.print(" dir: ");
        Serial.print(motor_msg.dir);
        Serial.print(" error: ");
        Serial.print(motor_msg.error);
        Serial.println("------------------");
*/
        xQueueSend(actuators_Mailbox, &motor_msg, 0);
        break;
      case MSG_ULTRASONIC_ACK:

        break;

      default:
        Serial.print("Error: controller task recieved unknown message type with type: ");
        Serial.println(msg.generic_message.type);
    }
  }
  if (first_time){
    first_time = false;
    send_to_imu();
    
  }
  
  return STATE_SEARCH;
}

void send_to_imu() {
  //  if (DEBUG_ENABLED) {
  //    Serial.print("Ultrasonic: Msg Sent to Controller is ");
  //    Serial.println(true);
  //  }
  msg_union msg;
  msg.imu_command_message.type = MSG_IMU_COMMAND;
  msg.imu_command_message.yaw = 90;

  xQueueSend(imu_command_Mailbox, &msg, portMAX_DELAY);
}
