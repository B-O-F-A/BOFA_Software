int motor1pin1 = 9;
int motor1pin2 = 10;

int motor2pin1 = 11;
int motor2pin2 = 12;
int enA = 8;
int enB = 13;

int spdL = 128;
int spdR = 128;

void setup() {
  // put your setup code here, to run once:
  pinMode(motor1pin1, OUTPUT);
  pinMode(motor1pin2, OUTPUT);
  pinMode(motor2pin1, OUTPUT);
  pinMode(motor2pin2, OUTPUT);

  pinMode(enA, OUTPUT); 
  pinMode(enB, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:   

  //Controlling speed (0 = off and 255 = max speed):
  analogWrite(enA, spdR); //ENA pin
  analogWrite(enB, spdL); //ENB pin

  //Controlling spin direction of motors:
  digitalWrite(motor1pin1, LOW);
  digitalWrite(motor1pin2, HIGH);

  digitalWrite(motor2pin1, HIGH);
  digitalWrite(motor2pin2, LOW);
  delay(1000);
}
