
#define PWM_PERIOD_MS 100



static void motor_sun_timer_callback(TimerHandle_t pxTimer);
static void motor_moon_timer_callback(TimerHandle_t pxTimer); //sun and moon timer implements pwm

static void motor_watchdog_timer_callback(TimerHandle_t pxTimer);




void actuators(void *pvParameters)
{
  (void) pvParameters;
  
  //create timers here
  TimerHandle_t sun_timer = xTimerCreate( "motor_sun_timer", 
                                            pdMS_TO_TICKS(100),               // initialize to 100, will reset with motor message
                                            pdFALSE,
                                            (void*)0,
                                            motor_sun_timer_callback); //unimplemented

  TimerHandle_t moon_timer = xTimerCreate( "motor_moon_timer", 
                                            pdMS_TO_TICKS(100),               // initialize to 100, will reset with motor message
                                            pdFALSE,
                                            (void*)0,
                                            motor_moon_timer_callback); //unimplemented

  TimerHandle_t moon_timer = xTimerCreate( "motor_watchdog_timer", 
                                            pdMS_TO_TICKS(PWM_PERIOD_MS*2),               // initialize to 100, will reset with motor message
                                            pdFALSE,
                                            (void*)0,
                                            motor_watchdog_timer_callback); //unimplemented
  bool watchdog_timer_started_flag = false;
  msg_union msg;
  for (;;)
  {
    if(xQueueReceive(actuators_Mailbox, &msg, portMAX_DELAY) == pdPASS ){
      switch (msg.generic_message.type){
        case MSG_MOTOR:   //motor control port
          motor_message_t* motor_message; 
          motor_message = &msg.motor_message;
          Serial.print(motor_message->type);//there is currently an error interperating this
          Serial.print("  ");
          Serial.println(motor_message->spd);
          
        break;

        case MSG_TIMER: //watchdog timer port
          
        break;

        
        default:
          Serial.print("Error: actuator task recieved unknown message type with type: ");
          Serial.println(msg.generic_message.type);
      }
      
    }
  }
}

static void motor_sun_timer_callback(TimerHandle_t pxTimer){
  
}
static void motor_moon_timer_callback(TimerHandle_t pxTimer){
  
}
static void motor_watchdog_timer_callback(TimerHandle_t pxTimer){
  
}
