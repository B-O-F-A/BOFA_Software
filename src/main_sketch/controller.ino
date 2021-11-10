void controller(void *pvParameters)
{
  (void) pvParameters;

  int coutner = 0;
  int stepper_msg_counter = 0;
  for (;;)
  {
    msg_union msg;
    msg.motor_message.type = MSG_MOTOR;
    msg.motor_message.timestamp = millis();
    msg.motor_message.spd = coutner;
    msg.motor_message.dir = FORWARD;
    msg.motor_message.step_dir = STEPPER_MOVE_NONE;

    if (stepper_msg_counter == 70){
      msg.motor_message.step_dir = CLOSE_GATE;
    }else if (stepper_msg_counter == 140){
      msg.motor_message.step_dir = OPEN_GATE;
      stepper_msg_counter = -1;
    }
    stepper_msg_counter++;
    
    coutner++;
    coutner%=100;
    Serial.println(msg.motor_message.step_dir);
    xQueueSend(actuators_Mailbox,&msg,portMAX_DELAY);
    vTaskDelay(pdMS_TO_TICKS(200));
  }
}
