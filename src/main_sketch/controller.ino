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
  state_e prev_state = TOT_NUM_STATES;
  for (;;)
  {
    if (prev_state != CURR_STATE) {
      prev_state = CURR_STATE;
      if (DEBUG_ENABLED) {
        Serial.print("Controller: CURR_STATE = "); Serial.println(CURR_STATE);
      }
    }

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



  }
}
