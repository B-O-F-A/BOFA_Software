//  COLOUR SENSORS USED:
//  LEFT_COLOUR
//  MID_COLOUR
//  RIGHT_COLOUR
//  LEFT_AUX_COLOUR
//  RIGHT_AUX_COLOUR


//bool first_time = true;

state_e state_return(msg_union &msg, colour_type_e (&tcs_sen)[5]) {
  static motor_message_t motor_msg;

  static bool NEED_DROPOFF = true;

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
        
        if (tcs_sen[MID_COL] == RED && tcs_sen[LEFT_COL] == RED && tcs_sen[RIGHT_COL] == RED && tcs_sen[LEFT_AUX_COL] == RED && tcs_sen[RIGHT_AUX_COL] == RED && !NEED_DROPOFF) {
          motor_msg.dir = STOP;
          motor_msg.type = MSG_MOTOR;
          motor_msg.error = 0;
          xQueueSend(actuators_Mailbox, &motor_msg, 0);
          return STATE_STOP;
        }
        else if (tcs_sen[LEFT_AUX_COL] == RED) {
          motor_msg.dir = LEFT;
          motor_msg.type = MSG_MOTOR;
          motor_msg.error = -1;
          xQueueSend(actuators_Mailbox, &motor_msg, 0);
        }

        else if (tcs_sen[RIGHT_AUX_COL] == RED) {
          motor_msg.dir = RIGHT;
          motor_msg.type = MSG_MOTOR;
          motor_msg.error = 1;
          xQueueSend(actuators_Mailbox, &motor_msg, 0);
        }
        else if (tcs_sen[MID_COL] == RED) {
          motor_msg.dir = FORWARD;
          motor_msg.type = MSG_MOTOR;
          motor_msg.error = 0;
          xQueueSend(actuators_Mailbox, &motor_msg, 0);
        }
        else {
          motor_msg.dir = FORWARD;
          motor_msg.type = MSG_MOTOR;
          motor_msg.spd = AVG_SPEED;
          xQueueSend(actuators_Mailbox, &motor_msg, 0);
        }

        if (tcs_sen[LEFT_AUX_COL] == GREEN && NEED_DROPOFF) {
          motor_msg.dir = FORWARD;
          motor_msg.spd = SLOW_SPEED;
          motor_msg.error = 0;
          xQueueSend(actuators_Mailbox, &motor_msg, 0);
          delay(1000);

          turn_left_robot_slow(1500);
 
          motor_msg.dir = FORWARD_SLOW;
          motor_msg.spd = 40;
          motor_msg.error = 0;
          xQueueSend(actuators_Mailbox, &motor_msg, 0);
          delay(500);

          motor_msg.dir = STOP;
          motor_msg.type = MSG_MOTOR;
          motor_msg.spd = 0;
          motor_msg.error = 0;
          xQueueSend(actuators_Mailbox, &motor_msg, 0);
          delay(500);

          gate_servo.write(OPEN_SERVO_GATE);
          
          backup_robot(700, SLOW_SPEED);

          turn_right_robot_slow(1500);
          
//          motor_msg.dir = FORWARD;
//          motor_msg.type = MSG_MOTOR;
//          motor_msg.spd = SLOW_SPEED;
//          motor_msg.error = 0;
//          xQueueSend(actuators_Mailbox, &motor_msg, 0);
          NEED_DROPOFF = false;
          xQueueReset(actuators_Mailbox);
         
        }

        

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


  return STATE_RETURN;
}
