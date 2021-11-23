// Setting up 6 TCS34725 sensors together and reading their output values
// TCA9548A

#include <Wire.h>
#include "Adafruit_TCS34725.h"
#include "MPU9250.h"

byte multiAddress = 0x70;
byte count = 0;
int currentTime = 0;

MPU9250 mpu;


char data[5][3] = {{'r', 'g', 'b'},
  {'r', 'g', 'b'},
  {'r', 'g', 'b'},
  {'r', 'g', 'b'},
  {'r', 'g', 'b'}
};
//                 {'r', 'g', 'b'}};
Adafruit_TCS34725 tcs[] = {Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_24MS, TCS34725_GAIN_16X),//};
                           Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_24MS, TCS34725_GAIN_16X),
                           Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_24MS, TCS34725_GAIN_16X),
                           Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_24MS, TCS34725_GAIN_16X),
                           Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_24MS, TCS34725_GAIN_16X)
                          };
//                           Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_2_4MS, TCS34725_GAIN_1X)};
byte gammatable[256];

void setup() {
  Serial.begin(9600);
  Wire.begin();

  // gammatable for more color accuracy when outputting on LED
  for (int i = 0; i < 256; i++) {
    float x = i;
    x /= 255;
    x = pow(x, 2.5);
    x *= 255;
    gammatable[i] = x;
  }
  //initIMU();
  initColorSensors();

  //attachInterrupt(digitalPinToInterrupt(inPin), changeLED, CHANGE);
}

void loop() {
  //    for(int i = 0; i < sizeof(data); i++){ // get all colors... not necessary right now
  //        readColors(i);
  //    }
//chooseBus(-1);
 for (int iter = 0; iter < 10; iter++){   
  Serial.println(iter);
  for (int i = 0; i < 5; i++) { // get all colors... not necessary right now
    Serial.print("READING Sensor: ");
    Serial.println(i);
    readColors(i);
    Serial.println("-------------------");

  }
 }
 
for (int j = 0; j < 30; j++){ 
  //readIMU();
}

//counter ++;
//chooseBus(-2);
  //for(int i = 0; i < 1; i++){
  //    readColors(count);
  //}

  

}
void changeLED() {
  if (millis() - currentTime > 200) {
    count++;
    currentTime = millis();
  }
  if (count > 5) {
    count = 0;
  }

}
void initColorSensors() {
  for (int i = 0; i < 5; i++) {
    Serial.println(i);
    chooseBus(i);
    if (tcs[i].begin()) {
      Serial.print("Found sensor "); Serial.println(i);
    } else {
      Serial.println("No Sensor Found");
      
      //while (true);
    }
  }


}

void initIMU() {
  //chooseBus(5);
  if (!mpu.setup(0x68)) {  // change to your own address
    while (1) {
      Serial.println("MPU connection failed. Please check your connection with `connection_check` example.");
      delay(5000);
    }
  }
  //Serial.println("Found MPU");
}

void readColors(int sensorNum) {
  chooseBus(sensorNum);
  uint16_t r, g, b, c;
  tcs[sensorNum].getRawData(&r, &g, &b, &c); // reading the rgb values 16bits at a time from the i2c channel

  int multiplier = 255;
  
  float red;
  float green;
  float blue;
  
  red = ((float)r / (float)c) * multiplier;
  green = ((float)g / (float)c) * multiplier;
  blue = ((float)b / (float)c) * multiplier;

  float final_red = red / (red + green + blue)*255;
  float final_green = green / (red + green + blue)*255;
  float final_blue = blue / (red + green + blue)*255;
  //processColors(r, g, b, c); // processing by dividng by clear value and then multiplying by 256
  //data[sensorNum][0] = r;
  //data[sensorNum][1] = g;
  //data[sensorNum][2] = b;
  Serial.print("R: "); Serial.print(final_red, DEC); Serial.print(" ");
  Serial.print("G: "); Serial.print(final_green, DEC); Serial.print(" ");
  Serial.print("B: "); Serial.print(final_blue, DEC); Serial.print(" "); Serial.print(c); Serial.print(" ");
  Serial.println(sensorNum);
}
void processColors(uint16_t &r, uint16_t &g, uint16_t &b, uint16_t c) {
  // getting rid of IR component of light
  float rfloat = r;
  float gfloat = g;
  float bfloat = b;
  rfloat /= c;
  gfloat /= c;
  bfloat /= c;
  // adding it back in
  r = rfloat * 256;
  g = gfloat * 256;
  b = bfloat * 256;
}

void chooseBus(int8_t bus) {
  Wire.beginTransmission(0x70);
  Wire.write(1 << (bus + 2)); // will be using 2-7 instead of 0-5 because of convience (placed better on the breadboard)
  Wire.endTransmission();
}

//void readIMU() {
//  //chooseBus(5);
//  //initIMU();
//  if (mpu.update()) {
//    static uint32_t prev_ms = millis();
//    if (millis() > prev_ms + 25) {
//      print_roll_pitch_yaw();
//      prev_ms = millis();
//    }
//  }
//}

void readIMU() {
  chooseBus(5);
  
  if (mpu.update()) {
    //static uint32_t prev_ms = millis();
    //if (millis() > prev_ms + 25) {
    delay(25);
    mpu.update();
      print_roll_pitch_yaw();
      //prev_ms = millis();
    //}
  }
}

void print_roll_pitch_yaw() {
  Serial.print("Yaw, Pitch, Roll: ");
  Serial.print(mpu.getYaw(), 2);
  Serial.print(", ");
  Serial.print(mpu.getPitch(), 2);
  Serial.print(", ");
  Serial.println(mpu.getRoll(), 2);
}
