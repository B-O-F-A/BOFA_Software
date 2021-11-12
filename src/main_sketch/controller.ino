void controller(void *pvParameters)
{
  (void) pvParameters;

  int coutner = -1;
  int stepper_msg_counter = 0;
  for (;;)
  {
    msg_union msg;
    msg.motor_message.type = MSG_MOTOR;
    msg.motor_message.timestamp = millis();
    if (coutner == -1){
      msg.motor_message.dir = FORWARD;
      coutner++;  
    }
    
    
    msg.motor_message.spd = coutner;
    msg.motor_message.step_dir = STEPPER_MOVE_NONE;

    if (stepper_msg_counter == 70){
      msg.motor_message.step_dir = CLOSE_GATE;
    }else if (stepper_msg_counter == 140){
      msg.motor_message.step_dir = OPEN_GATE;
      stepper_msg_counter = -1;
    }
    stepper_msg_counter++;
    
    coutner++;
    if (coutner > 80){
      msg.motor_message.spd = 160 - coutner;
    }
    coutner%=160;
    
    if (coutner == 0) {
      if (msg.motor_message.dir == FORWARD){
        msg.motor_message.dir = BACKWARD;
        
      }
      else if (msg.motor_message.dir == BACKWARD){
        msg.motor_message.dir = FORWARD;
        Serial.println("Set to FORWARD-------------------------");
      }
    }
    
    Serial.println(msg.motor_message.step_dir);
    xQueueSend(actuators_Mailbox,&msg,portMAX_DELAY);
    vTaskDelay(pdMS_TO_TICKS(200));
  }
}
