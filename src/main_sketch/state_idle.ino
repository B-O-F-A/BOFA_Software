
state_e state_idle(msg_union &msg) {
  //May need to add timer if we want to switch to search state
  //after some time instead of instantly.
  vTaskDelay(pdMS_TO_TICKS(500));
  if (DEBUG_ENABLED) {
    Serial.println("Controller: CURR_STATE = IDLE");
  }
  return STATE_SEARCH;
}
