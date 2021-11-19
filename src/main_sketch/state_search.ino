//  COLOUR SENSORS USED:
//  LEFT_COLOUR
//  MID_COLOUR
//  RIGHT_COLOUR
//  LEFT_AUX_COLOUR
//  RIGHT_AUX_COLOUR

state_e state_search(msg_union &msg) {
  static colour_type_e tcs_sen[5] = {0, 0, 0, 0, 0};
  static msg_union motor_msg;

  const int AVG_SPEED = 90;

  const int SPEED_DIFF = 10;

  const int AVG_SPEED_ROT = 60;
  
//  const int HIGH_SPEED = 60;
//  const int MED_SPEED = 30;
//  const int HIGH_SPEED_ROT = 90;
//  const int MED_SPEED_ROT = 90;

  
  //msg.motor_message.dir = FORWARD;


  if (xQueueReceive(controller_Mailbox, &msg, portMAX_DELAY) == pdPASS ) {
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

        }


        break;
      case MSG_ULTRASONIC_ACK:

        break;

      default:
        Serial.print("Error: controller task recieved unknown message type with type: ");
        Serial.println(msg.generic_message.type);
    }
  }

  if (DEBUG_ENABLED) {
    Serial.println("Controller: CURR_STATE = SEARCH");
  }

  return STATE_SEARCH;
}
