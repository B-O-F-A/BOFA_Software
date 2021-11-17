void ultrasonic(void *pvParameters)
{
  (void) pvParameters;

  const float MAX_DIST = 10.0;

  float duration;
  float distance = 1000.0;


  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  RunningMedian samples = RunningMedian(5);
  for (int i = 0; i < 5; i++) {
    samples.add(distance);
  }

  float med_dist = samples.getMedian();
  
  int counter = 0;

  for (;;)
  {

    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    duration = pulseIn(echoPin, HIGH);
    distance = (duration * .0343) / 2;

    samples.add(distance);
    
    med_dist = samples.getMedian();

    //Testing without sensor
    //    distance = 20 - (counter * 0.5);

    if (DEBUG_ENABLED) {
      Serial.print("Ultrasonic: ");
      Serial.println(med_dist);
    }

    if (med_dist < MAX_DIST) {
      send_ultrasonic_to_controller(med_dist);
    }

    counter ++;
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

void send_ultrasonic_to_controller(float distance) {
  //  if (DEBUG_ENABLED) {
  //    Serial.print("Ultrasonic: Msg Sent to Controller is ");
  //    Serial.println(distance);
  //  }
  msg_union msg;
  msg.ultrasonic_message.type = MSG_ULTRASONIC;
  msg.ultrasonic_message.timestamp = millis();
  msg.ultrasonic_message.dist = distance;

  xQueueSend(ultrasonic_Mailbox, &msg, 0); //maybe need portMAX_DELAY ??
}
