#include <i2cmaster.h> //used for Ultrasound Sensor
#include <IRremote.h>  //used for IR Remote
//No speed control on steering motor.  Would need to modify the card for that.
int RECV_PIN = 11;
IRrecv irrecv(RECV_PIN);
decode_results results;
#define POWER 0x10EFD827 
#define A 0x10EFF807 
#define B 0x10EF788                  
#define C 0x10EF58A7
#define UP 0x10EFA05F
#define DOWN 0x10EF00FF
#define LEFT 0x10EF10EF
#define RIGHT 0x10EF807F
#define SELECT 0x10EF20DF

const byte motorEN1 = 5; //pwm
const byte motorIN1 = 4;
const byte motorEN2 = 6;  //pwm
const byte motorIN2 = 7;
const byte motorEN3 = 9;  //pwm
const byte motorIN3 = 8; 

byte clockPin = 13;
byte buf[9];//Buffer to store the received valeus
byte addr = 0x02;//address 0x02 in a 8-bit context - 0x01 in a 7-bit context
byte distance;

int speed1 = 0;
int speed2 = 0;
int speed3 = 0;
boolean motorDir = 0;
boolean strDir = 0;

void setup()
{
  
  pinMode(motorEN1, OUTPUT);
  pinMode(motorIN1, OUTPUT);
  pinMode(motorEN2, OUTPUT);
  pinMode(motorIN2, OUTPUT);
  pinMode(motorEN3, OUTPUT);
  pinMode(motorIN3, OUTPUT);
 
  Serial.begin(9600);
  irrecv.enableIRIn(); // Start the receiver
  i2c_init();//I2C frequency = 11494,253Hz
}

void loop()
{

  
  if (irrecv.decode(&results)) 
  {
    if (results.value == POWER) 
    {
      //Serial.println("POWER");    
    }
     if (results.value == A) 
    {
      //Serial.println("A");    
    }
     if (results.value == B) 
    {
      //Serial.println("B");  
    }
     if (results.value == C) 
    {
      //Serial.println("C");
    }
     if (results.value == UP) 
    {
      Serial.println("UP");
      speed1 = 255;
      speed2 = 255;
      motorDir = 0;
    }
     if (results.value == DOWN) 
    {
      Serial.println("DOWN");
      speed1 = 255;
      speed2 = 255;
      motorDir = 1;
    }
     if (results.value == LEFT) 
    {
      //Serial.println("LEFT");
      
      speed3 = 100;
      strDir = 0;
      motor3(speed3, strDir);
      
      delay(200);
      speed3 = 0;
      motor3(speed3, strDir);
    }
     if (results.value == RIGHT) 
    {
      //Serial.println("RIGHT");
      strDir = 1;
      speed3 = 100;
      motor3(speed3, strDir);
      delay(200);
      speed3 = 0;
      motor3(speed3, strDir);
    }
     if (results.value == SELECT) 
    {
      //Serial.println("SELECT");
      speed1 = 0;
      speed2 = 0;
    }
    irrecv.resume();
  }
  distance = readDistance();
  //Serial.println(distance);
  
  motor1(speed1, motorDir);
  motor2(speed2, motorDir);
}

void motor1(int pwm1, boolean reverse)
{
  analogWrite(motorEN1, pwm1);
  if (reverse) {
    digitalWrite(motorIN1, LOW);
  }
  else if (distance <= 15) {
    analogWrite(motorEN1, 0);
  }
  else
    digitalWrite(motorIN1, HIGH);
}

void motor2(int pwm2, boolean reverse)
{
  analogWrite(motorEN2, pwm2);
  if (reverse) {
    digitalWrite(motorIN2, LOW);
  }
  else if (distance <= 15) {
    analogWrite(motorEN2, 0);
  }
  else {
    digitalWrite(motorIN2, HIGH);
  }

}

void motor3(int pwm3, boolean reverse)
{
  analogWrite(motorEN3, pwm3);
  if (reverse) {
    digitalWrite(motorIN3, LOW);
  }
  else {
    digitalWrite(motorIN3, HIGH);
  }
}

//=============================================  

byte readDistance()
{  
  //Serial.println("In read distance");
  delay(100);//There has to be a delay between commands
  byte cmd = 0x42;//Read Measurement Byte 0
    
  pinMode(clockPin, INPUT);//Needed for writing to work
  digitalWrite(clockPin, HIGH);  
  
  if(i2c_start(addr+I2C_WRITE))//Check if there is an error
  {
    //Serial.println("ERROR i2c_start");
    i2c_stop();
    return 0xFF;
  }    
  if(i2c_write(cmd))//Check if there is an error
  {
    //Serial.println("ERROR i2c_write");
    i2c_stop();
    return 0xFF;
  }  
  
  i2c_stop();
    
  delayMicroseconds(60);//Needed for receiving to work
  pinMode(clockPin, OUTPUT);
  digitalWrite(clockPin, LOW); 
  delayMicroseconds(34);
  pinMode(clockPin, INPUT);
  digitalWrite(clockPin, HIGH); 
  delayMicroseconds(60);  
   
  
  if(i2c_rep_start(addr+I2C_READ))//Check if there is an error
  {
    //Serial.println("ERROR i2c_rep_start");
    i2c_stop();    
    return 0xFF;
  } 
  
 
  for(int i = 0; i < 8; i++)
    buf[i] = i2c_readAck();
  buf[8] = i2c_readNak();  
  i2c_stop(); 
  
  return buf[0];   
}






