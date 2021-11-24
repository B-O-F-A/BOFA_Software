
float pid_value(direction_type_e &dir, int8_t &error);
void run_motors(uint8_t spd, direction_type_e dir, float motor_diff);
void pxMotorTimerCallback (TimerHandle_t xTimer);

void actuators(void *pvParameters)
{
  (void) pvParameters;


  msg_union msg;
  motor_message_t motor_msg;
  motor_msg.dir = FORWARD;
  motor_msg.spd = 0;
  motor_msg.error = 0;
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


  int timer_delay = 2;
  if (DEBUG_ENABLED && DEBUG_ACTUATORS_ENABLED) {
    timer_delay = 10;
  }

  for (;;)
  {
    if (xQueueReceive(actuators_Mailbox, &msg, 0) == pdPASS ) {
      switch (msg.generic_message.type) {
        case MSG_MOTOR:   //motor control port
          motor_msg = msg.motor_message;

          break;
        default:
          Serial.print("Error: actuator task recieved unknown message type with type: ");
          Serial.println(msg.generic_message.type);
      }

    }

    float right_mid = 0;
    float left_mid = 0;
    float colour_error = 0;
    
    xSemaphoreTake(mutex, portMAX_DELAY);
    colour_error = red_values[2] - red_values[0];
    xSemaphoreGive(mutex);


//    if (DEBUG_ENABLED && DEBUG_ACTUATORS_ENABLED) {
//      Serial.print("Left_mid: ");
//      Serial.print(left_mid);
//      Serial.print(" right: ");
//      Serial.println(right_mid);
//
//      Serial.print("Colour_error: ");
//      Serial.println(colour_error);
//
//    }

    //colour_error = sqrt_func(colour_error);

//    Serial.print("Colour_error_filtered: ");
//    Serial.println(colour_error);

    
    float motor_diff;

    

    const float MAX_TURN_SPD = 30;
    
    

    /*
    if (abs(colour_error) > GLOBAL_ERROR_THRESH) {
      motor_msg.spd = 0;
      if (abs(motor_diff) < 20){
        
        if (motor_diff < 0.0){
          motor_diff = 20.0 * -1.0;
        } else{
          motor_diff = 20.0; 
        }
      }
      
    }
    */

    motor_diff = pid_value(motor_msg.dir, colour_error, msg.motor_message.error);
    
    //motor_diff = (motor_diff <= MAX_TURN_SPD) ? motor_diff : MAX_TURN_SPD;
    //motor_diff = (motor_diff >= -MAX_TURN_SPD) ? motor_diff : -MAX_TURN_SPD;

    if(motor_diff+motor_msg.spd > 100){
      motor_msg.spd = 100 - motor_diff;
    }

    
    run_motors(motor_msg.spd, motor_msg.dir, motor_diff, colour_error);
    delay(timer_delay);


  }
}

//static float sqrt_func(float error){
//  const float tolerance = 5;
//  if (abs(error)< tolerance){
//    return 0;
//  }
//
//  if (error < 0){
//    return sqrt(abs(error)-tolerance) * -1.0;
//  }else{
//   return sqrt(abs(error)-tolerance);
//  }
//}
float pid_value(direction_type_e &dir, float &colour_error, int8_t error) {
  const float Ki = 0.0;
  const float Kd = 0.0;
  const float Kp = 1;
  const float Ke = 20;
  static float prev_error = 0;
  static float I_error = 0;
  static uint32_t prev_time = millis() - 5;
  uint32_t curr_time = millis();
  float time_diff = curr_time - prev_time;

  float derivative = (colour_error - prev_error) / time_diff;

  float Kd_term = derivative * Kd * -1.0;
  float Kp_term = colour_error * Kp;
  float Ki_term;

  if (colour_error <= 10){
    I_error = 0;
  }
  else{
    I_error += colour_error * time_diff;
  }

  Ki_term = I_error*Ki;
  
  if (DEBUG_ENABLED && DEBUG_ACTUATORS_ENABLED) {
  Serial.print("Kp_term: "); Serial.print(Kp_term);
  Serial.print(" Kd_term: "); Serial.print(Kd_term);
  Serial.print(" Ki_term: "); Serial.print(Ki_term);
  Serial.println("-------------");
  }
  if (dir == FORWARD) {
    prev_error = colour_error;
    prev_time = curr_time;
    return Kp_term + Kd_term+ Ki_term + error*Ke;
  }
  else if (dir == BACKWARD) {
    prev_error = colour_error;
    prev_time = curr_time;
    return Kp_term + Kd_term + Ki_term + error*Ke;

  } else {
    return 0;
  }

}
float left_motor_offset(float spd) {
  float m = (10 - 5) / (70 - 40);
  return m * spd;

}

static const float MAX_SPEED = 100.0;

void run_motors(uint8_t spd, direction_type_e dir, float motor_diff, float &error) {
  static int motor_counter = 0;
  static bool turn_rec = false;
  
//    if (dir != FORWARD) {
//      motor_counter = 0;
//      turn_rec = true;
//    }
//    else if (dir == FORWARD) {
//      if (motor_counter > 100) {
//        turn_rec = false;
//      } else {
//        motor_counter ++;
//      }
//
//    }
//
//  if (turn_rec){
//    spd = 30;
//  }
  
  const float left_offset = 0;
  const float right_offset = 0;

  float left_speed = (float)spd + motor_diff; //- abs(error) * 0.7;
  float right_speed = (float)spd - motor_diff; //- abs(error) * 0.7;

  /*
    if (turn_rec && error == 0) {
      left_speed -=15;
      right_speed -= 15;
    }
  */
  //left_speed = left_speed + left_motor_offset(left_speed);

  left_speed = (left_speed <= MAX_SPEED) ? left_speed : MAX_SPEED;
  right_speed = (right_speed <= MAX_SPEED) ? right_speed : MAX_SPEED;

  left_speed = (left_speed >= -MAX_SPEED) ? left_speed : -MAX_SPEED;
  right_speed = (right_speed >= -MAX_SPEED) ? right_speed : -MAX_SPEED;

  float percent_L = left_speed / 100.0 * 255.0;
  float percent_R = right_speed / 100.0 * 255.0;

  int PWM_L = (int) (percent_L);
  int PWM_R = (int) (percent_R);

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
      PWM_L = 128;
      PWM_R = 128;
      set_motor_left_backward();
      set_motor_right_forward();
      break;

    case RIGHT:
      PWM_L = 128;
      PWM_R = 128;
      set_motor_left_forward();
      set_motor_right_backward();
      break;
    default:
      Serial.print("Error: Actuator task recieved unknown direction type with direction: ");
      Serial.println(dir);
  }

  bofa_analogWrite(pwmB, PWM_L);
  bofa_analogWrite(pwmA, PWM_R);

}
void set_motor_left(int &PWM) {
  if (PWM >= 0) {
    set_motor_left_forward();
  } else {
    set_motor_left_backward();
    PWM = -1 * PWM;
  }
}
void set_motor_right(int &PWM) {
  if (PWM >= 0) {
    set_motor_right_forward();
  } else {
    set_motor_right_backward();
    PWM = -1 * PWM;
  }
}

void send_to_stepper(int desSteps) {
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
  }else{
    analogWrite(pin, PWM_Signal);
  }
    
}

void set_motor_right_forward() {
  if (DEBUG_ENABLED && DEBUG_ACTUATORS_ENABLED & 0) {
    Serial.println("Actuators: Move RIGHT Forward ------");
  }
  digitalWrite(in1A, HIGH);
  digitalWrite(in2A, LOW);
}
void set_motor_right_backward() {
  if (DEBUG_ENABLED && DEBUG_ACTUATORS_ENABLED &0) {
    Serial.println("Actuators: Move RIGHT Backward ------");
  }
  digitalWrite(in1A, LOW);
  digitalWrite(in2A, HIGH);
}
void set_motor_left_forward() {
  if (DEBUG_ENABLED && DEBUG_ACTUATORS_ENABLED &0) {
    Serial.println("Actuators: Move LEFT Forward ------");
  }
  digitalWrite(in1B, HIGH);
  digitalWrite(in2B, LOW);
}
void set_motor_left_backward() {
  if (DEBUG_ENABLED && DEBUG_ACTUATORS_ENABLED &0) {
    Serial.println("Actuators: Move LEFT Backward ------");
  }
  digitalWrite(in1B, LOW);
  digitalWrite(in2B, HIGH);
}

void pxMotorTimerCallback (TimerHandle_t xTimer) {
  timer_message_t timer_msg;
  timer_msg.type = MSG_TIMER;
  xQueueSend(actuators_Mailbox, &timer_msg, portMAX_DELAY);
}
