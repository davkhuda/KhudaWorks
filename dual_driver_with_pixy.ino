#include <SPI.h>  
#include <Pixy.h>
#include <SoftwareSerial.h>
#include <SabertoothSimplified.h>

SoftwareSerial leftSerial(NOT_A_PIN, 9); // RX on no pin (unused), TX on pin 11 (to S1).
SabertoothSimplified leftDriver(SWSerial1); // 
SoftwareSerial rightSerial(NOT_A_PIN, 10); // RX on no pin (unused), TX on pin 11 (to S1).
SabertoothSimplified rightDriver(SWSerial2); 
unsigned int counter=0;


int powerL = 0;
int powerR = 0;
boolean turning = false;
int turningVal = 0;


Pixy pixy;
uint16_t blocks;
void setup()
{

  Serial.begin(9600);
  Serial.print("Starting...\n");

  leftSerial.begin(9600);
  rightSerial.begin(9600);

  pixy.init();
}

void loop()
{ 
  static int i = 0;
  int j;
  char buf[32]; 
  
  blocks = pixy.getBlocks();
  if (blocks)
  {
    i++;
    counter=0;
    if (i%50==0)
    {
      sprintf(buf, "Detected %d:\n", blocks);
      Serial.print(buf);
      for (j=0; j<blocks; j++)
      {
        sprintf(buf, "  block %d: ", j);
        Serial.print(buf); 
        pixy.blocks[j].print();
        //pixy.blocks[j].x,y,width,height,signature
        turningVal = canTurn(pixy.blocks[j].x);
        if(turningVal != 0)
          turning = true;

        turnCar(turningVal);
      }
    }
    if(powerL <=120 || powerR <= 120 && !turning) //if turning, there will be a power difference, need the !turning to stop from random ramping
      rampUp();
  }else if (!blocks && (powerL != 0 || powerR != 0))
  {
    slowToZero();
  }

}

void rampUp()
{
  if(powerL != powerR)
  {
    if(powerL > powerR) //equating power before both motors ramp up
    {
      rightDriver.motor(1,++powerR);
      rightDriver.motor(2,powerR);
    }
    else if(powerR > powerL)
    {
      leftDriver.motor(1,++powerL);
      leftDriver.motor(2,powerL);      
    }
  }
  else
  {
    leftDriver.motor(1, ++powerL);
    leftDriver.motor(2, powerL);
    rightDriver.motor(1, ++powerR);
    rightDriver.motor(2, powerR);
  }
  delay(10);
  Serial.print("Ramping Up");
 }

void slowToZero()
{
  if(powerL < 0 && powerR < 0)
  {
    if(powerL != powerR)
    {
      if(powerL < powerR)
      {
        leftDriver.motor(1,++powerL);
        leftDriver.motor(2,powerL);
      }
      else if(powerR < powerL)
      {
        rightDriver.motor(1,++powerR);
        rightDriver.motor(2,powerR);      
      }
    }
    else
    {
      leftDriver.motor(1, ++powerL);
      leftDriver.motor(2, powerL);
      rightDriver.motor(1, ++powerR);
      rightDriver.motor(2, powerR);      
    }

    delay(10);
  }
  else if(powerL > 0 && powerR > 0)
  {
    if(powerL != powerR)
    {
      if(powerL > powerR)
      {
        leftDriver.motor(1,--powerL);
        leftDriver.motor(2,powerL);
      }
      else if(powerR > powerL)
      {
        rightDriver.motor(1,--powerR);
        rightDriver.motor(2,powerR);      
      }
    }
    else
    {
      leftDriver.motor(1, --powerL);
      leftDriver.motor(2, powerL);
      rightDriver.motor(1, --powerR);
      rightDriver.motor(2, powerR);    
    }

    delay(10);
    Serial.print("Slowing Down");
  }
}

int canTurn(int pixy_x)
{
  if(pixy_x >= 0 && pixy_x <= 106) // LEFT RANGE
    return -1;
  if(pixy_x >= 107 && pixy_x <= 213) //MIDDLE/STRAIGHT RANGE
    return 0;
  if(pixy_x >= 214 && pixy_x <= 319) //RIGHT RANGE
    return 1;
}

void turnCar(int side)
{
  if(side == -1)
  {
    leftDriver.motor(1, --powerL);
    leftDriver.motor(2, powerL);
    delay(10);
  }
  if(side == 1)
  {
    rightDriver.motor(1, --powerR);
    rightDriver.motor(2, powerR);
    delay(10);  
  }
}


