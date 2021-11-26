
state_e state_idle(msg_union &msg) {
  //May need to add timer if we want to switch to search state
  //after some time instead of instantly.

    vTaskDelay(pdMS_TO_TICKS(1000));

    xSemaphoreTake(mutexColoursInit, portMAX_DELAY);
    xSemaphoreGive(mutexColoursInit);
    
    motor_message_t motor_msg;
    motor_msg.type = MSG_MOTOR;
    motor_msg.spd = 80;
    motor_msg.error = 0;
    motor_msg.dir = FORWARD;
    xQueueSend(actuators_Mailbox, &motor_msg, 0);

  
  static bool ultrasound_switch = false;
//
//  if (xQueueReceive(controller_Mailbox, &msg, 0) == pdPASS ) {
//    switch (msg.generic_message.type) {
//      case MSG_COLOUR:
//
//        break;
//
//      case MSG_ULTRASONIC_ACK:
//        ultrasonic_ack_message_t* ultrasonic_ack_message;
//        ultrasonic_ack_message = &msg.ultrasonic_ack_message;
//
//        Serial.print("CONTROLLER: BeamBreak found value of ");
//        Serial.println(ultrasonic_ack_message->john_in_range);
//        ultrasound_switch = !ultrasound_switch;
//        Serial.println("CONTROLLER: Servo CLOSE ");
//        gate_servo.write(CLOSE_SERVO_GATE);
//
//        break;
//
//      default:
//        Serial.print("Error: controller task recieved unknown message type with type: ");
//        Serial.println(msg.generic_message.type);
//    }
//  }
//  return STATE_IDLE;
  return STATE_SEARCH;
//  return STATE_RETURN;
//     return STATE_SLOW;
}
