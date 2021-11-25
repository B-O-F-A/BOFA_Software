void controller(void *pvParameters)
{
  (void) pvParameters;

  CURR_STATE = STATE_IDLE;

  RETURN_MODE = INITIAL;

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
  colour_type_e tcs_sen[5] = {COLOUR_NONE, COLOUR_NONE, COLOUR_NONE, COLOUR_NONE, COLOUR_NONE};

  for (;;)
  {
    if (prev_state != CURR_STATE) {
      prev_state = CURR_STATE;
      if (DEBUG_ENABLED) {
        Serial.print("Controller: CURR_STATE = "); Serial.println(CURR_STATE);
      }
      if(prev_state == STATE_SLOW)
      {
        attachInterrupt(digitalPinToInterrupt(beamPin), xBeamInterruptHandler, RISING);
      }
    }

    switch (CURR_STATE)
    {
      case STATE_IDLE:
        CURR_STATE = state_idle(msg);
        break;

      case STATE_SEARCH:
        CURR_STATE = state_search(msg, tcs_sen);
        break;

      case STATE_SLOW:
      
        CURR_STATE = state_slow(msg, tcs_sen);
        break;

      case STATE_COLLECT:
        //CURR_STATE = state_collect(msg);
        break;

      case STATE_RETURN:
        CURR_STATE = state_return(msg, tcs_sen);
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
