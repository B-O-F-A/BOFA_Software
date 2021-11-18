void controller(void *pvParameters)
{
  (void) pvParameters;

  CURR_STATE = STATE_IDLE;

  int HIGH_SPEED = 60;
  int MED_SPEED = 30;
  int HIGH_SPEED_ROT = 90;
  int MED_SPEED_ROT = 90;

  int counter = 0;

  //  STATES USED:
  //  STATE_IDLE
  //  STATE_SEARCH
  //  STATE_SLOW
  //  STATE_COLLECT
  //  STATE_RETURN
  //  STATE_DROPOFF
  //  STATE_STOP

  msg_union msg;
  
  for (;;)
  {
    switch (CURR_STATE)
    {
      case STATE_IDLE:
        CURR_STATE = state_idle(msg);
        break;

      case STATE_SEARCH:
        CURR_STATE = state_search(msg);
        break;

      case STATE_SLOW:
        //CURR_STATE = state_slow(msg);
        break;

      case STATE_COLLECT:
        //CURR_STATE = state_collect(msg);
        break;

      case STATE_RETURN:
        //CURR_STATE = state_return(msg);
        break;

      case STATE_DROPOFF:
        //CURR_STATE = state_dropff(msg);
        break;
        
      case STATE_STOP:
        //state_stop(msg);
        break;
    }

    vTaskDelay(pdMS_TO_TICKS(100));

  }
}
