void send_to_stepper(int desSteps) {
  Serial.print("Msg Sent to Stepper: ");
  Serial.println(desSteps);

  msg_union msg;
  msg.stepper_message.type = MSG_STEPPER;
  msg.stepper_message.timestamp = millis();
  msg.stepper_message.steps = desSteps;

  xQueueSend(stepper_Mailbox, &msg, portMAX_DELAY);
}

void actuators(void *pvParameters)
{
  (void) pvParameters;


  msg_union msg;

  const int CLOSE_GATE_STEPS = -1 * (2052 / 360) * 160; //Rotating -160 degrees
  const int OPEN_GATE_STEPS = 1 * (2052 / 360) * 160; //Rotating 160 degrees


  for (;;)
  {
    if (xQueueReceive(actuators_Mailbox, &msg, portMAX_DELAY) == pdPASS ) {
      switch (msg.generic_message.type) {
        case MSG_MOTOR:   //motor control port
          motor_message_t* motor_message;
          motor_message = &msg.motor_message;
          //Serial.print(motor_message->type);//there is currently an error interperating this
          //Serial.print("  ");
          Serial.print(motor_message->spd);
          Serial.print("  motor step direction ");
          Serial.println(motor_message->step_dir);

          switch (motor_message->step_dir) {
            case CLOSE_GATE:
              send_to_stepper(CLOSE_GATE_STEPS);
              break;

            case OPEN_GATE:
              send_to_stepper(OPEN_GATE_STEPS);
          }
          
          break;

        default:
          Serial.print("Error: actuator task recieved unknown message type with type: ");
          Serial.println(msg.generic_message.type);
      }

    }

  }
}
