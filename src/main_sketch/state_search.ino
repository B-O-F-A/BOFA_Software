//  COLOUR SENSORS USED:
//  LEFT_COLOUR
//  MID_COLOUR
//  RIGHT_COLOUR
//  LEFT_AUX_COLOUR
//  RIGHT_AUX_COLOUR

state_e state_search(msg_union &msg) {
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

        break;
      case MSG_ULTRASONIC_ACK:
        if (DEBUG_ENABLED) {
          Serial.println("Controller: Received ultrasonic_msg but ignoring now"); 
        }
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
