
state_e state_idle(msg_union &msg) {
  //May need to add timer if we want to switch to search state
  //after some time instead of instantly.
  
  motor_message_t motor_msg;
  motor_msg.type = MSG_MOTOR;
  motor_msg.spd = 0;
  motor_msg.dir = STOP;
  xQueueSend(actuators_Mailbox, &motor_msg, 0);

  vTaskDelay(pdMS_TO_TICKS(500));
   motor_msg.type = MSG_MOTOR;
  motor_msg.spd = 40;
  motor_msg.dir = FORWARD;
  motor_msg.error = 0;
  xQueueSend(actuators_Mailbox, &motor_msg, 0);
  return STATE_SEARCH;
}
