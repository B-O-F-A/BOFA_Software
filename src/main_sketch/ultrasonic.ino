void ultrasonic(void *pvParameters)
{
  (void) pvParameters;

  const int trigPin = 12;
  const int echoPin = 13;

  const float MAX_DIST = 10.0;

  float duration;
  float distance = 1000.0;

  
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  
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

    //Testing without sensor
    distance = 20 - (counter * 0.5);
    
    if (debugEnabled) {
      Serial.print("Ultrasonic: ");
      Serial.println(distance);
    }

    if (distance < MAX_DIST) {
      send_to_controller(distance);
    }

    counter ++;
    vTaskDelay(pdMS_TO_TICKS(200));
  }
}

void send_to_controller(float distance) {
  if (debugEnabled) {
    Serial.print("Ultrasonic: Msg Sent to Controller is ");
    Serial.println(distance);
  }
  msg_union msg;
  msg.ultrasonic_message.type = MSG_ULTRASONIC;
  msg.ultrasonic_message.timestamp = millis();
  msg.ultrasonic_message.dist = distance;

  xQueueSend(ultrasonic_Mailbox, &msg, 0); //maybe need portMAX_DELAY ??
}
