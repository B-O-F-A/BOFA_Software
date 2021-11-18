
void actuators(void *pvParameters)
{
  (void) pvParameters;


  msg_union msg;

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

  for (;;)
  {
    if (xQueueReceive(actuators_Mailbox, &msg, portMAX_DELAY) == pdPASS ) {
      switch (msg.generic_message.type) {
        case MSG_MOTOR:   //motor control port
          motor_message_t* motor_message;
          motor_message = &msg.motor_message;

          //Serial.print(motor_message->type);//there is currently an error interperating this
          //Serial.print("  ");
          //Serial.print(motor_message->spd);
          //Serial.print("  motor step direction ");
          //Serial.println(motor_message->step_dir);

          switch (motor_message->step_dir) {
            case CLOSE_GATE:
              send_to_stepper(CLOSE_GATE_STEPS);
              break;

            case OPEN_GATE:
              send_to_stepper(OPEN_GATE_STEPS);
          }

          run_motors(motor_message->spd, motor_message->dir);

          //run_motors(30, FORWARD);


          break;

        default:
          Serial.print("Error: actuator task recieved unknown message type with type: ");
          Serial.println(msg.generic_message.type);
      }

    }

  }
}
void run_motors(uint8_t spd, direction_type_e dir) {
  int PWM_L = 255 * spd / 100;
  int PWM_R = 255 * spd / 100;

  switch (dir) {
    case STOP:
      PWM_L = 0;
      PWM_R = 0;
      break;

    case FORWARD:
      set_motor_left_forward();
      set_motor_right_forward();
      break;

    case BACKWARD:
      set_motor_left_backward();
      set_motor_right_backward();
      break;

    case LEFT:
      set_motor_left_backward();
      set_motor_right_forward();
      break;

    case RIGHT:
      set_motor_left_forward();
      set_motor_right_backward();

  }

  bofa_analogWrite(pwmA, PWM_L);
  bofa_analogWrite(pwmB, PWM_R);

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
  if (DEBUG_ENABLED) {
    Serial.println("Actuators: Move LEFT Forward ------");
  }
  digitalWrite(in1A, HIGH);
  digitalWrite(in2A, LOW);
}
void set_motor_left_backward() {
  if (DEBUG_ENABLED) {
    Serial.println("Actuators: Move LEFT Backward ------");
  }
  digitalWrite(in1A, LOW);
  digitalWrite(in2A, HIGH);
}
void set_motor_right_forward() {
  if (DEBUG_ENABLED) {
    Serial.println("Actuators: Move RIGHT Forward ------");
  }
  digitalWrite(in1B, HIGH);
  digitalWrite(in2B, LOW);
}
void set_motor_right_backward() {
  if (DEBUG_ENABLED) {
    Serial.println("Actuators: Move RIGHT Backward ------");
  }
  digitalWrite(in1B, LOW);
  digitalWrite(in2B, HIGH);
}
