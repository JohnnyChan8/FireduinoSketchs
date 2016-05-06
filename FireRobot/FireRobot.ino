#include <PS2X_lib.h>  //for v1.6
#include <Servo.h>

Servo leftServo;  // create servo object to control a servo
Servo rightServo;  // create servo object to control a servo
Servo headServo;  // create servo object to control a servo
const int left_servo_pin = 8;
const int right_servo_pin = 9;
const int head_servo_pin = 3;
int left_servo_pos = 90;
int right_servo_pos = 90;
int head_servo_pos = 90;

/******************************************************************
 * set pins connected to PS2 controller:
 *   - 1e column: original 
 *   - 2e colmun: Stef?
 * replace pin numbers by the ones you use
 ******************************************************************/
#define PS2_DAT        13//25
#define PS2_CMD        11//26
#define PS2_SEL        10
#define PS2_CLK        12

/******************************************************************
 * select modes of PS2 controller:
 *   - pressures = analog reading of push-butttons 
 *   - rumble    = motor rumbling
 * uncomment 1 of the lines for each mode selection
 ******************************************************************/
//#define pressures   true
#define pressures   false
//#define rumble      true
#define rumble      false

PS2X ps2x; // create PS2 Controller Class

int error = 0;
byte type = 0;
byte vibrate = 0;

int initPS2X()
{
  //setup pins and settings: GamePad(clock, command, attention, data, Pressures?, Rumble?) check for error
  error = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, pressures, rumble);
  
  if(error == 0){
    Serial.print("Found Controller, configured successful ");
    Serial.print("pressures = ");
  if (pressures)
    Serial.println("true ");
  else
    Serial.println("false");
  Serial.print("rumble = ");
  if (rumble)
    Serial.println("true)");
  else
    Serial.println("false");
    Serial.println("Try out all the buttons, X will vibrate the controller, faster as you press harder;");
    Serial.println("holding L1 or R1 will print out the analog stick values.");
    Serial.println("Note: Go to www.billporter.info for updates and to report bugs.");
  }  
  else if(error == 1)
    Serial.println("No controller found, check wiring, see readme.txt to enable debug. visit www.billporter.info for troubleshooting tips");
   
  else if(error == 2)
    Serial.println("Controller found but not accepting commands. see readme.txt to enable debug. Visit www.billporter.info for troubleshooting tips");

  else if(error == 3)
    Serial.println("Controller refusing to enter Pressures mode, may not support it. ");
  
  type = ps2x.readType(); 
  switch(type) {
    case 0:
      Serial.print("Unknown Controller type found ");
      break;
    case 1:
      Serial.print("DualShock Controller found ");
      break;
    case 2:
      Serial.print("GuitarHero Controller found ");
      break;
  case 3:
      Serial.print("Wireless Sony DualShock Controller found ");
      break;
   } 
   return type;
}

const int motor1_pwm1_pin = 4;
const int motor1_pwm2_pin = 5;
const int motor2_pwm1_pin = 6;
const int motor2_pwm2_pin = 7;
const int led_pin = 2;

int motor_1_speed = 0;
int motor_2_speed = 0;
int MOTOR_STATUS = 0;
int pos = 90;    // variable to store the servo position

unsigned long lasttime;        // Time in miliseconds of main control loop
unsigned int ledState;

void setup() { 
  Serial.begin(115200);
  
  pinMode(motor1_pwm1_pin ,OUTPUT);
  pinMode(motor1_pwm2_pin ,OUTPUT);
  pinMode(motor2_pwm1_pin ,OUTPUT);
  pinMode(motor2_pwm2_pin ,OUTPUT);

  leftServo.attach(left_servo_pin);
  rightServo.attach(right_servo_pin);
  headServo.attach(head_servo_pin);
 
  leftServo.write(left_servo_pos);  
  rightServo.write(right_servo_pos); 
  headServo.write(head_servo_pos);
   
  pinMode(led_pin ,OUTPUT);
  ledState = 1;
  digitalWrite(led_pin, ledState);
  
  delay(300);  //added delay to give wireless ps2 module some time to startup, before configuring it
  //CHANGES for v1.6 HERE!!! **************PAY ATTENTION*************
  while (type == 0)
    initPS2X();

  motor_1_speed = 255;
}

void UpdatePS2XGamePad()
{
    if(error == 1) //skip loop if no controller found
      return; 
    ps2x.read_gamepad(false, vibrate); //read controller and set large motor to spin at 'vibrate' speed
    //if(ps2x.Button(PSB_L1) || ps2x.Button(PSB_R1)) 
    { //print stick values if either is TRUE
      //Serial.print("Stick Values:");
      //Serial.print(ps2x.Analog(PSS_LY), DEC); //Left stick, Y axis. Other options: LX, RY, RX  
     // Serial.print(",");
     // Serial.print(ps2x.Analog(PSS_LX), DEC); 
     // Serial.print(",");
     // Serial.print(ps2x.Analog(PSS_RY), DEC); 
     // Serial.print(",");
     // Serial.println(ps2x.Analog(PSS_RX), DEC); 
      prcessMotorSpeed1();
    }     

    if(ps2x.Button(PSB_L1) )
    {
      left_servo_pos -= 1;
    }
    if (ps2x.Button(PSB_L2))
    {
      left_servo_pos += 1;
    }

    if(ps2x.Button(PSB_R1) )
    {
      right_servo_pos += 1;
    }
    if (ps2x.Button(PSB_R2))
    {
      right_servo_pos -= 1;
    }
/*
    int head = ps2x.Analog(PSS_RX);
    if ((head == 0) ||(head == 255))
    {
     // return ;
    }
    if(head>240)
    {
      head_servo_pos -= 1;
    }
    if (head<15)
    {
      head_servo_pos += 1;
    }*/
}

void loop() {
  // put your main code here, to run repeatedly:
  
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    switch (inChar)
    {
      case 'c': 
          motor_1_speed++;
      break;
      case 'v': 
          motor_1_speed--;
      break;
    }
    
      Serial.print("pwm: ");
      Serial.println(motor_1_speed);
      //analogWrite(motor1_pwm1_pin, motor_1_speed);
      //analogWrite(motor1_pwm2_pin, motor_1_speed);
      //analogWrite(motor2_pwm1_pin, motor_1_speed);
      //analogWrite(motor2_pwm2_pin, motor_1_speed);
  }
  UpdatePS2XGamePad();
  motorOutputControl();
  servoOutput();
  //ledOutput();
}

#define HIGH_VALUE 240
#define LOW_VALUE 15

void prcessMotorSpeed1(void)
{
    if(ps2x.Button(PSB_PAD_UP)) {      //will be TRUE as long as button is pressed
        MOTOR_STATUS =  1;
    } 
    else if(ps2x.Button(PSB_PAD_RIGHT)){
        MOTOR_STATUS =  5;
    }
    else if(ps2x.Button(PSB_PAD_LEFT)){
        MOTOR_STATUS =  4;
    }
    else if(ps2x.Button(PSB_PAD_DOWN)){
        MOTOR_STATUS =  3;
    }   
    else
        MOTOR_STATUS =  0;
        
    //if(ps2x.Button(PSB_TRIANGLE))
    //    Serial.println("Triangle pressed");        
    if(ps2x.Button(PSB_CIRCLE))               //will be TRUE if button was JUST pressed
      head_servo_pos -= 1;
    //  Serial.println("Circle just pressed");
    //if(ps2x.NewButtonState(PSB_CROSS))               //will be TRUE if button was JUST pressed OR released
    //  Serial.println("X just changed");
    if(ps2x.Button(PSB_SQUARE))              //will be TRUE if button was JUST released
      head_servo_pos += 1;
    //  Serial.println("Square just released");          
}
void prcessMotorSpeed(void)
{
   int speed = 0;
   int delta_speed = 0;
   int dir = 0;

  speed = ps2x.Analog(PSS_LY);
  delta_speed = ps2x.Analog(PSS_LX);

  if ((speed == 0) && (delta_speed==0))
  {
    MOTOR_STATUS =  0;
    return ;
  }

  if ((speed == 255) && (delta_speed==255))
  {
    MOTOR_STATUS =  0;
    return ;
  }
  
  if (speed > HIGH_VALUE)
  {
    MOTOR_STATUS =  3;
  }
  else if (speed < LOW_VALUE)
  {
    MOTOR_STATUS =  1;
  }
  else
  {
    MOTOR_STATUS =  0;
  }

  
  if (delta_speed > HIGH_VALUE)
  {
    MOTOR_STATUS =  4;
  }
  else if (delta_speed < LOW_VALUE)
  {
    MOTOR_STATUS =  5;
  }

  //Serial.print(MOTOR_STATUS, DEC); 
 // Serial.print(" , "); 
 // Serial.print(motor_1_speed, DEC); 
 // Serial.print(" , "); 
 // Serial.println(motor_2_speed, DEC); 
}

void motorOutputControl()
{
  motor_1_speed = motor_2_speed = 255;
  switch(MOTOR_STATUS)
  {
    case 0://停止
    case 2://停止
      analogWrite(motor1_pwm1_pin, 0);
      analogWrite(motor1_pwm2_pin, 0);
      analogWrite(motor2_pwm1_pin, 0);
      analogWrite(motor2_pwm2_pin, 0);
      break;

    case 1://前进
      analogWrite(motor1_pwm1_pin, motor_1_speed);
      analogWrite(motor1_pwm2_pin, 0);
      analogWrite(motor2_pwm1_pin, motor_2_speed);
      analogWrite(motor2_pwm2_pin, 0);
      break;

    case 3://后退
      analogWrite(motor1_pwm1_pin, 0);
      analogWrite(motor1_pwm2_pin, motor_1_speed);
      analogWrite(motor2_pwm1_pin, 0);
      analogWrite(motor2_pwm2_pin, motor_2_speed);
      break;

    case 4:
      motor_1_speed = motor_2_speed = 200;  
      analogWrite(motor1_pwm1_pin, motor_1_speed);
      analogWrite(motor1_pwm2_pin, 0);
      analogWrite(motor2_pwm1_pin, 0);
      analogWrite(motor2_pwm2_pin, motor_2_speed);
      break;

    case 5:
      motor_1_speed = motor_2_speed = 200;  
      analogWrite(motor1_pwm1_pin, 0);
      analogWrite(motor1_pwm2_pin, motor_1_speed);
      analogWrite(motor2_pwm1_pin, motor_2_speed);
      analogWrite(motor2_pwm2_pin, 0);
      break;

    default:
      MOTOR_STATUS = 0;
      analogWrite(motor1_pwm1_pin, 0);
      analogWrite(motor1_pwm2_pin, 0);
      analogWrite(motor2_pwm1_pin, 0);
      analogWrite(motor2_pwm2_pin, 0);
  }
}

void servoOutput(void)
{
  if (left_servo_pos>180)
    left_servo_pos = 180;
  if (left_servo_pos<0)
    left_servo_pos = 0;
    
  if (right_servo_pos>180)
    right_servo_pos = 180;
  if (right_servo_pos<0)
    right_servo_pos = 0;

  if (head_servo_pos>180)
    head_servo_pos = 180;
  if (head_servo_pos<0)
    head_servo_pos = 0;


 // Serial.print("L:");
 // Serial.print(left_servo_pos);
 // Serial.print("--R:");
 // Serial.print(right_servo_pos);
 // Serial.print("--H:");
 // Serial.println(head_servo_pos);
  
  leftServo.write(left_servo_pos);  
  rightServo.write(right_servo_pos); 
  headServo.write(head_servo_pos);   
}

void ledOutput(void)
{
    if (MOTOR_STATUS != 0)
  {
    
    if (millis()-lasttime > 300) {
      lasttime = millis();
      if (ledState == 0)
      {
        ledState = 1;
        digitalWrite(led_pin, ledState);
      }
      else
      {
        ledState = 0;
        digitalWrite(led_pin, ledState);
      }
    }
  }
  else
  {
      ledState = 1;
      digitalWrite(led_pin, ledState);
  }
}

