void colour_imu(void *pvParameters)
{
  (void) pvParameters;

  int counter = 0;

  MPU9250 mpu;

  Wire.begin();
  delay(500);

  setup_imu(mpu);

  bool IMU_enabled = false;
  bool IMU_first = false;
  float desired_angle = 0;
  float start_angle = 0;
  float current_angle = 0;

  msg_union imu_incoming_msg;

  for (;;)
  {
    if (xQueueReceive(imu_command_Mailbox, &imu_incoming_msg, 0) == pdPASS ) {
      switch (imu_incoming_msg.generic_message.type) {
        case MSG_IMU_COMMAND:
          imu_command_message_t* imu_command_message;
          imu_command_message = &imu_incoming_msg.imu_command_message;

          desired_angle = imu_command_message->yaw;

          if (DEBUG_ENABLED) {
            Serial.print("COLOUR_IMU: Received Angle: ");
            Serial.println(desired_angle);
          }
          IMU_first = true;
          IMU_enabled = true;

          break;

        default:
          Serial.print("Error: Colour_IMU task recieved unknown message type in imu_command_Mailbox with type: ");
          Serial.println(imu_incoming_msg.generic_message.type);
      }
    }

    if (IMU_enabled) {
      Serial.println("IMU enabled!");
      
      chooseBus(IMU);
      if (mpu.update()) {

        if (IMU_first) {
          start_angle = mpu.getYaw();
          current_angle = start_angle;
          IMU_first = false;
        }
        else {
          current_angle = mpu.getYaw();
          Serial.print("Colour_IMU: ");
          Serial.print("Current angle: "); Serial.print(current_angle);
          Serial.print(" Start angle: "); Serial.print(start_angle);
          Serial.print(" Desired angle: "); Serial.print(desired_angle);
          
          if (goal_reached(current_angle, start_angle, desired_angle, IMU_enabled)) {
            send_imu_ack_to_controller();
          }

        }


      }
    }

//    read_colours();

  }
}

void setup_imu(MPU9250 &mpu) {
  chooseBus(IMU);
  if (!mpu.setup(0x68)) {  // change to your own address
    while (1) {
      Serial.println("MPU connection failed. Please check your connection with `connection_check` example.");
      delay(1000);
    }
  }
}

bool goal_reached(float &current_angle, float &start_angle, float &desired_angle, bool &IMU_enabled) {
  if (desired_angle > 0) {
    if ((current_angle - start_angle) >= desired_angle) {
      IMU_enabled = false;
      return true;
    }
  } else { //des_angle is negative
    if ((current_angle - start_angle) <= desired_angle) {
      IMU_enabled = false;
      return true;
    }
  }
  return false;
}

//void read_IMU(MPU9250 &mpu) {
//  if (mpu.update()) {
//    //static uint32_t prev_ms = millis();
//    //if (millis() > prev_ms + 50) {
//    prev_ms = millis();
//    if (DEBUG_ENABLED) {
//      Serial.print("Colour_IMU: Yaw: ");
//      Serial.println(mpu.getYaw());
//    }
//    send_imu_to_controller();
//    //mpu.getPitch();
//    //mpu.getRoll();
//    //} else {
//    //  taskYIELD();
//    //}
//  }
//}

void send_colour_to_controller(message_type_e colour_vals[5]) {

  msg_union msg;
  msg.colour_message.type = MSG_COLOUR;
  for (int i = 0; i < TOT_NUM_COLOUR; i++)
    msg.colour_message.colour[i] = colour_vals[i];

  xQueueSend(colour_Mailbox, &msg, 0);
}

void send_imu_ack_to_controller() {
  msg_union msg;
  msg.imu_ack_message.type = MSG_IMU_ACK;
  msg.imu_ack_message.angle_reached = true;

  xQueueSend(imu_ack_Mailbox, &msg, 0);
}

void chooseBus(uint8_t bus) {
  Wire.beginTransmission(0x70);
  Wire.write(1 << (bus + 2)); // will be using 2-7 instead of 0-5 because of convience (placed better on the breadboard)
  Wire.endTransmission();
}
