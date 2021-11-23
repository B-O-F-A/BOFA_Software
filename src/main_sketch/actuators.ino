
float pid_value(direction_type_e &dir, int8_t &error);
void run_motors(uint8_t spd, direction_type_e dir, float motor_diff);
void pxMotorTimerCallback (TimerHandle_t xTimer);
void actuators(void *pvParameters)
{
  (void) pvParameters;


  msg_union msg;
  motor_message_t motor_msg;
  const int CLOSE_GATE_STEPS = (int)(-1 * 90 * (2052 / 360.0)); //Rotating -90 degrees
  const int OPEN_GATE_STEPS = (int)(1 *  90 * (2052 / 360.0)); //Rotating 90 degrees

  // Motor Speed Values - Start at zero
  int MotorSpeedL = 0;
  int MotorSpeedR = 0;

  // Set all the motor control pins to outputs
  pinMode(pwmA, OUTPUT);
  pinMode(pwmB, OUTPUT);
  pinMode(in1A, OUTPUT);
  pinMode(in2A, OUTPUT);
  pinMode(in1B, OUTPUT);
  pinMode(in2B, OUTPUT);

  float motor_diff;
  int timer_delay = 5;
  if (DEBUG_ENABLED && DEBUG_ACTUATORS_ENABLED) {
    timer_delay = 500;
  }
  //  TimerHandle_t write_motor_timer = xTimerCreate
  //                                    ( "motor_timer",
  //                                      timer_delay,
  //                                      pdTRUE,
  //                                      ( void * ) 0,
  //                                      pxMotorTimerCallback);
  //
  //  xTimerStart(write_motor_timer, 0);

  for (;;)
  {
    if (xQueueReceive(actuators_Mailbox, &msg, 0) == pdPASS ) {
      switch (msg.generic_message.type) {
        case MSG_MOTOR:   //motor control port
          motor_msg = msg.motor_message;
   
          break;

        //        case MSG_TIMER:
        //          timer_message_t* timer_message;
        //          timer_message = &msg.timer_message;
        //          motor_diff = pid_value(motor_msg.dir, motor_msg.error);
        //          if (DEBUG_ENABLED && DEBUG_ACTUATORS_ENABLED) {
        //            Serial.print("ACTUATORS: motor_message{error: ");
        //            Serial.print(motor_msg.error);Serial.println("-----------------");
        //            Serial.println("ACTUATORS: motor_message dir: "); Serial.print(motor_msg.dir); Serial.println("}");
        //            Serial.print("motor_diff: "); Serial.println(motor_diff);
        //          }


        //          break;

        default:
          Serial.print("Error: actuator task recieved unknown message type with type: ");
          Serial.println(msg.generic_message.type);
      }

    }
    motor_diff = pid_value(motor_msg.dir, motor_msg.error);
    run_motors(motor_msg.spd, motor_msg.dir, motor_diff, motor_msg.error);
    delay(timer_delay);


  }
}

float pid_value(direction_type_e &dir, int8_t &error) {
  const float Ki = 0;
  const float Kp = 10;
  static uint32_t prev_time = millis();
  uint32_t curr_time = millis();
  if (error == 0) {
    prev_time = curr_time;
    return 0;
  }

  uint32_t time_diff = curr_time - prev_time;

  if (dir == FORWARD) {
    return Ki * error * time_diff + Kp * error;
  }
  else if (dir == BACKWARD) {
    return Ki * error * time_diff * (-1) + Kp * error;
  }else{
    return 0;
  }

}
void run_motors(uint8_t spd, direction_type_e dir, float motor_diff, int8_t &error) {

  float left_speed = (float)spd - motor_diff - abs(error)*10;
  float right_speed = (float)spd + motor_diff - abs(error)*10;

  left_speed = (left_speed <= 100) ? left_speed : 100;
  right_speed = (right_speed <= 100) ? right_speed : 100;

  left_speed = (left_speed >= -100) ? left_speed : -100;
  right_speed = (right_speed >= -100) ? right_speed : -100;


  int PWM_L = (int) (255 * left_speed / 100);
  int PWM_R = (int) (255 * right_speed / 100);
  
  switch (dir) {
    case STOP:
      PWM_L = 0;
      PWM_R = 0;
      break;

    case FORWARD:

      set_motor_left(PWM_L);
      set_motor_right(PWM_R);

      break;

    case BACKWARD:
      set_motor_left(PWM_L);
      set_motor_right(PWM_R);
      break;

    case LEFT:
    
      set_motor_left_backward();
      set_motor_right_forward();
      break;

    case RIGHT:
      set_motor_left_forward();
      set_motor_right_backward();

    default:
      Serial.println("Actuators: Unknown direction");
  }
  
  bofa_analogWrite(pwmA, PWM_L);
  bofa_analogWrite(pwmB, PWM_R);

}
void set_motor_left(int &PWM){
  if (PWM >= 0){
    set_motor_left_forward();
  }else{
    set_motor_left_backward();
    PWM = -1*PWM;
  }
}
void set_motor_right(int &PWM){
  if (PWM >= 0){
    set_motor_right_forward();
  }else{
    set_motor_right_backward();
    PWM = -1*PWM;
  }
}

void send_to_stepper(int desSteps) {
  //Serial.print("Msg Sent to Stepper: ");
  //Serial.println(desSteps);

  msg_union msg;
  msg.stepper_message.type = MSG_STEPPER;
  msg.stepper_message.steps = desSteps;

  xQueueSend(stepper_Mailbox, &msg, portMAX_DELAY);
}

void bofa_analogWrite(int pin, int PWM_Signal) {
  if (DEBUG_ENABLED) {
    if (DEBUG_ACTUATORS_ENABLED) {
      Serial.print("Actuators: ");
      Serial.print("PWM_Pin: ");
      Serial.print(pin);
      Serial.print(" PWM_Signal: ");
      Serial.print(PWM_Signal);

      int motor_speed = PWM_Signal * 100 / 255;

      Serial.print(" Speed: ");
      Serial.println(motor_speed);
    }
  }
  else {
    analogWrite(pin, PWM_Signal);
  }
}

void set_motor_left_forward() {
  if (DEBUG_ENABLED && DEBUG_ACTUATORS_ENABLED) {
    Serial.println("Actuators: Move LEFT Forward ------");
  }
  digitalWrite(in1A, LOW);
  digitalWrite(in2A, HIGH);
}
void set_motor_left_backward() {
  if (DEBUG_ENABLED && DEBUG_ACTUATORS_ENABLED) {
    Serial.println("Actuators: Move LEFT Backward ------");
  }
  digitalWrite(in1A, HIGH);
  digitalWrite(in2A, LOW);
}
void set_motor_right_forward() {
  if (DEBUG_ENABLED && DEBUG_ACTUATORS_ENABLED) {
    Serial.println("Actuators: Move RIGHT Forward ------");
  }
  digitalWrite(in1B, LOW);
  digitalWrite(in2B, HIGH);
}
void set_motor_right_backward() {
  if (DEBUG_ENABLED && DEBUG_ACTUATORS_ENABLED) {
    Serial.println("Actuators: Move RIGHT Backward ------");
  }
  digitalWrite(in1B, HIGH);
  digitalWrite(in2B, LOW);
}

void pxMotorTimerCallback (TimerHandle_t xTimer) {
  timer_message_t timer_msg;
  timer_msg.type = MSG_TIMER;
  xQueueSend(actuators_Mailbox, &timer_msg, portMAX_DELAY);
}
