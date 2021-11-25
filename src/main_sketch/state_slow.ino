state_e state_slow(msg_union &msg, colour_type_e (&tcs_sen)[5]) {
  static motor_message_t motor_msg;
  static bool TURNING_180 = false;
  motor_msg.type = MSG_MOTOR;

  const float SLOW_SPEED = 25;
  const float AVG_SPEED_ROT = 60;


  if (xQueueReceive(controller_Mailbox, &msg, 0) == pdPASS ) {
    switch (msg.generic_message.type) {
      case MSG_COLOUR:
        colour_message_t* colour_message;
        colour_message = &msg.colour_message;

        for (int i = 0; i < (TOT_NUM_I2C - 1); i++) {
          tcs_sen[i] = colour_message->colour[i];
        }

        if (DEBUG_ENABLED) {
          xSemaphoreTake(mutexPrint, portMAX_DELAY);
          Serial.print("Controller SLOW: Received colour_msg: ");
          for (int i = 0; i < (TOT_NUM_I2C - 1); i++) {

            Serial.print(colour_message->colour[i]); Serial.print(" ");
          }
          Serial.println(" --- ");

          Serial.print("Controller SLOW: turning_180: ");
          Serial.print(TURNING_180);
          Serial.println(" ");
          xSemaphoreGive(mutexPrint);
        }

        if (TURNING_180) {
          //          if (tcs_sen[MID_COL] == RED) {
          //            motor_msg.dir = STOP;
          //            motor_msg.spd = 0;
          //            motor_msg.error = 0;
          //            xQueueSend(actuators_Mailbox, &motor_msg, 0);
          //            TURNING_180 = false;
          //            return STATE_DROPOFF;
          //          }
          if (tcs_sen[LEFT_AUX_COL] == RED) {
            motor_msg.dir = LEFT;
            motor_msg.spd = 0;
            motor_msg.error = -1;
            xQueueSend(actuators_Mailbox, &motor_msg, 0);
            TURNING_180 = false;
            return STATE_RETURN;
          }
        }
        else if (tcs_sen[MID_COL] != RED && (tcs_sen[LEFT_COL] == RED || tcs_sen[RIGHT_COL] == RED)) {
          motor_msg.dir = FORWARD;
          motor_msg.spd = 0;
          motor_msg.error = 0;
          xQueueSend(actuators_Mailbox, &motor_msg, 0);
        }
        else {
          motor_msg.dir = FORWARD;
          motor_msg.spd = SLOW_SPEED;
          motor_msg.error = 0;
          xQueueSend(actuators_Mailbox, &motor_msg, 0);
        }

        break;
      case MSG_ULTRASONIC_ACK:

        motor_msg.dir = STOP;
        motor_msg.spd = 0;
        motor_msg.error = 0;
        xQueueSend(actuators_Mailbox, &motor_msg, 0);
        gate_servo.write(CLOSE_SERVO_GATE);
        delay(200);
        backup_robot(1000, SLOW_SPEED);

        //center robot

        motor_msg.dir = FORWARD;
        motor_msg.spd = 0;
        motor_msg.error = 0;
        xQueueSend(actuators_Mailbox, &motor_msg, 0);

        delay(1000);

        turn_left_robot_slow(1500);

        TURNING_180 = true;

        xQueueReset(actuators_Mailbox);
        //wait for red line to reapear in mid col state

        break;
    }

  }
  return STATE_SLOW;
}
