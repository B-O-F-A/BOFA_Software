//  COLOUR SENSORS USED:
//  LEFT_COLOUR
//  MID_COLOUR
//  RIGHT_COLOUR
//  LEFT_AUX_COLOUR
//  RIGHT_AUX_COLOUR

state_e state_slow(msg_union &msg) {
  static colour_type_e tcs_sen[5] = {COLOUR_NONE, COLOUR_NONE, COLOUR_NONE, COLOUR_NONE, COLOUR_NONE};
  static motor_message_t motor_msg;

  const int AVG_SPEED = 30;

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
        
        //QUESTIONABLE CHECK FOR SURE
        else if((tcs_sen[MID_COL] == RED) && (tcs_sen[LEFT_COL] == RED) && (tcs_sen[RIGHT_COL] == RED)){
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
        
        xQueueSend(actuators_Mailbox, &motor_msg, 0);
        break;
        
      case MSG_ULTRASONIC_ACK:
        gate_servo.write(CLOSE_SERVO_GATE);
        delay(200);
        return STATE_RETURN;
        
        break;

      default:
        Serial.print("Error: controller task recieved unknown message type with type: ");
        Serial.println(msg.generic_message.type);
    }
  }
 
  return STATE_SLOW;
}