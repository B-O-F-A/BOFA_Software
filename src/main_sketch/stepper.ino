//const int stepsPerRevolution = 2052;
const int StepperPins[4] = {8, 9, 10, 11};
int stepperPinCount = 0;
int stepperSignal = 0;

void moveForward(int setSteps) {

  for (int steps = setSteps; steps > 0;  steps--) {
    if ((stepperSignal == 0) || (stepperSignal == 1)) {
      stepperSignal = 16;//0b10000
    }
    stepperSignal >>= 1;
    if(!debugEnabled){
      for (stepperPinCount = 3; stepperPinCount >= 0; stepperPinCount--) {
        digitalWrite(StepperPins[stepperPinCount], stepperSignal >> stepperPinCount & 0x01);
      }
    }
    delay(3);
  }
}

void freeze() {

  for (stepperPinCount = 3; stepperPinCount >= 0; stepperPinCount--) {
    digitalWrite(StepperPins[stepperPinCount], 0);
  }

}
void moveBackward(int setSteps) {

  for (int steps = setSteps; steps > 0;  steps--) {
    if ((stepperSignal == 0) || (stepperSignal == 1)) {
      stepperSignal = 16; //0b10000
    }
    stepperSignal >>= 1;
    if(!debugEnabled){
      for (stepperPinCount = 3; stepperPinCount >= 0; stepperPinCount--) {
        digitalWrite(StepperPins[3 - stepperPinCount], stepperSignal >> stepperPinCount & 0x01);
      }
    }

    delay(3);
  }
}

void stepper_delay(uint32_t set_delay) {
  uint32_t start_time = millis();
  while ((millis() - start_time) < set_delay) {
    taskYIELD();
  }
}

void move_stepper(int received_Steps) {

  if (received_Steps < 0) {
    moveBackward(abs(received_Steps));
    if (debugEnabled) {
      Serial.print("Stepper: ");
      Serial.println("Finished Moving Backward");
    }
    freeze();
  }
  else if (received_Steps > 0) {
    moveForward(received_Steps);
    if (debugEnabled) {
      Serial.print("Stepper: ");
      Serial.println("Finished Moving Forward");
    }
    freeze();
  }

}

void stepper(void *pvParameters)
{
  (void) pvParameters;

  for (stepperPinCount = 0; stepperPinCount < 4; stepperPinCount++) {
    pinMode(StepperPins[stepperPinCount], OUTPUT);
  }

  msg_union msg;

  freeze();

  for (;;)
  {

    if (xQueueReceive(stepper_Mailbox, &msg, portMAX_DELAY) == pdPASS ) {
      switch (msg.generic_message.type) {
        case MSG_STEPPER:
          stepper_message_t* stepper_message;
          stepper_message = &msg.stepper_message;
          //Serial.print(stepper_message->type);//there is currently an error interperating this
          //Serial.print("  ");
          //Serial.println(stepper_message->steps);

          move_stepper(stepper_message->steps);

          break;


        default:
          Serial.print("Error: stepper task recieved unknown message type with type: ");
          Serial.println(msg.generic_message.type);
      }

    }
  }
}
