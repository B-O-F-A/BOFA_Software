
void read_colour_sensors(colour_type_e (&prev_payload)[TOT_NUM_I2C - 1], float (&data)[TOT_NUM_I2C - 1][3]);

void process_data(colour_type_e (&new_payload)[TOT_NUM_I2C - 1], float (&data)[TOT_NUM_I2C - 1][3]);

void colour_imu(void *pvParameters)
{
  (void) pvParameters;

  int counter = 0;

  //setup_imu();
  setup_colour_sensors();

  bool IMU_enabled = false;
  bool IMU_first = false;
  float desired_angle = 0;
  float start_angle = 0;
  float current_angle = 0;
  msg_union imu_incoming_msg;


  float data[TOT_NUM_I2C - 1][3];

  colour_type_e prev_payload[TOT_NUM_I2C - 1];

  for (int i = 0; i < (TOT_NUM_I2C - 1); i++) {
    for (int j = 0; j < 3; j++) {
      data[i][j] = 0.0;
    }
    prev_payload[i] = COLOUR_NONE;

  }

  for (;;)
  {

    if (xQueueReceive(imu_command_Mailbox, &imu_incoming_msg, 0) == pdPASS ) { //need portMAX_DELAY ??

      switch (imu_incoming_msg.generic_message.type) {
        case MSG_IMU_COMMAND:
          imu_command_message_t* imu_command_message;
          imu_command_message = &imu_incoming_msg.imu_command_message;

          desired_angle = imu_command_message->yaw;

          if (DEBUG_ENABLED && DEBUG_TCS_ENABLED) {
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

    read_colour_sensors(prev_payload, data);
    //delay(5);

  }
}

void read_colour_sensors(colour_type_e (&prev_payload)[TOT_NUM_I2C - 1], float (&data)[TOT_NUM_I2C - 1][3]) {
  /* Added by Frank*/
//  static int counter;
//  int num_color_to_read;
//  counter++;
//  counter%=3;
//  if (counter==0){
//    num_color_to_read = 5;
//  }
//  else{
//    num_color_to_read = 3;
//  }
//  for (int i = 0; i < num_color_to_read; i++) {
//    get_colour(i, data);
//  }
  /* Added by Frank*/
  for (int i = 0; i < (TOT_NUM_I2C - 1); i++) {
    get_colour(i, data);
  }

  colour_type_e new_payload[TOT_NUM_I2C - 1];

  process_data(new_payload, data);

  bool send_payload = false;
    
  for (int i = 0; i < (TOT_NUM_I2C - 1); i++) {
    if (prev_payload[i] != new_payload[i]) {
      send_payload = true;
    }
    prev_payload[i] = new_payload[i];
    
  }
  
  xSemaphoreTake(mutex, portMAX_DELAY);
  for (int i = 0; i < 3; i++) {
    red_values[i] = data[i][0];
  }
  xSemaphoreGive(mutex);

  if (DEBUG_ENABLED && DEBUG_TCS_ENABLED) {
    Serial.print("COLOUR_IMU: Colour Diff (1-0) (2-1): ");
    Serial.print(red_values[1] - red_values[0]);
    Serial.print(" ");
    Serial.println(red_values[2] - red_values[1]);
  }

  if (send_payload) {
    send_colour_to_controller(new_payload);
  }

}

void get_colour(int sensorNum, float (&data)[TOT_NUM_I2C - 1][3]) {
  chooseBus(sensorNum);
  uint16_t r, g, b, c;
  tcs[sensorNum].getRawData(&r, &g, &b, &c); // reading the rgb values 16bits at a time from the i2c channel

  const int multiplier = 255;

  float red;
  float green;
  float blue;

  red = ((float)r / (float)c) * multiplier;
  green = ((float)g / (float)c) * multiplier;
  blue = ((float)b / (float)c) * multiplier;

  float final_red = red / (red + green + blue) * 255;
  float final_green = green / (red + green + blue) * 255;
  float final_blue = blue / (red + green + blue) * 255;
  //processColors(r, g, b, c); // processing by dividng by clear value and then multiplying by 256

  data[sensorNum][0] = final_red;
  data[sensorNum][1] = final_green;
  data[sensorNum][2] = final_blue;


    if (DEBUG_ENABLED && DEBUG_TCS_ENABLED) {
      if (sensorNum == 2 || sensorNum == 0 || sensorNum == 1) {
        Serial.print("COLOUR_IMU: READING Sensor: ");
        Serial.println(sensorNum);
        Serial.print ("COLOUR_IMU: ");
        Serial.print(" R: "); Serial.print(data[sensorNum][0], DEC); Serial.print(" ");
        Serial.print("G: "); Serial.print(data[sensorNum][1], DEC); Serial.print(" ");
        Serial.print("B: "); Serial.print(data[sensorNum][2], DEC); Serial.print(" "); Serial.print(c);
        Serial.println("");
        Serial.println("-------------------");
    }
    }
}

void process_data(colour_type_e (&new_payload)[TOT_NUM_I2C - 1], float (&data)[TOT_NUM_I2C - 1][3]) {
  const int r = 0;
  const int g = 1;
  const int b = 2;

  for (int i = 0; i < (TOT_NUM_I2C - 1); i++) {

    float r_g = data[i][r] - data[i][g];
    float r_b = data[i][r] - data[i][b];

    float g_r = -r_g;
    float g_b = data[i][g] - data[i][b];

    float b_r = -r_b;
    float b_g = -g_b;

    colour_type_e colour_val = COLOUR_NONE;

    if ((r_g > COLOUR_THRESH) && (r_b > COLOUR_THRESH)) {
      colour_val = RED;
    }

    if ((g_r > COLOUR_THRESH) && (g_b > COLOUR_THRESH)) {
      colour_val = GREEN;
    }

    if ((b_g > COLOUR_THRESH) && (b_r > COLOUR_THRESH)) {
      colour_val = BLUE;
    }

    new_payload[i] = colour_val;

  }

}


void send_colour_to_controller(colour_type_e colour_vals[TOT_NUM_I2C - 1]) {

  msg_union msg;
  msg.colour_message.type = MSG_COLOUR;
  for (int i = 0; i < (TOT_NUM_I2C - 1); i++)
    msg.colour_message.colour[i] = colour_vals[i];

  xQueueSend(controller_Mailbox, &msg, portMAX_DELAY);
}

void chooseBus(uint8_t bus) {
  Wire.beginTransmission(0x70);
  Wire.write(1 << (bus + 2)); // will be using 2-7 instead of 0-5 because of convience (placed better on the breadboard)
  Wire.endTransmission();
}

void setup_colour_sensors() {
  for (int i = 0; i < TOT_NUM_I2C - 1; i++) {
    Serial.println(i);
    chooseBus(i);
    if (tcs[i].begin()) {
      Serial.print("Found sensor "); Serial.println(i);
    } else {
      Serial.println("No Sensor Found");
      while (true);
    }
  }
}
