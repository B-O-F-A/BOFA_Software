void controller(void *pvParameters)
{
  (void) pvParameters;

  int coutner = 0;
  for (;;)
  {
    msg_union msg;
    msg.motor_message.type = MSG_MOTOR;
    msg.motor_message.timestamp = millis();
    msg.motor_message.spd = coutner;
    msg.motor_message.dir = FORWARD;
    
    coutner++;
    coutner%=100;
    Serial.println(msg.motor_message.spd);
    xQueueSend(actuators_Mailbox,&msg,portMAX_DELAY);
    vTaskDelay(pdMS_TO_TICKS(200));
  }
}
