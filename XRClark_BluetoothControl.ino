int8_t x;
int8_t y;
int8_t z;
int InitX;
int xAfter;
bool flagInit = false;

unsigned long currentTime = 0;

int buttonState_0 = 0;
int buttonState_1 = 0;

#include "MMA7660.h"
MMA7660 accelemeter;
#include<Wire.h>
#include <math.h>
const int MPU_0 = 0x68;
const int MPU_1 = 0x69;
int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;
double pitch, roll;
int AcXoff, AcYoff, AcZoff, GyXoff, GyYoff, GyZoff;
String message;

void setup()
{
  pinMode(8, INPUT_PULLUP);
  pinMode(9, INPUT_PULLUP);

  accelemeter.init();

  Wire.begin();
  Wire.beginTransmission(MPU_0);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);
  Wire.begin();
  Wire.beginTransmission(MPU_1);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);
  Serial.begin(9600);
}


void loop()
{
  currentTime = millis();

  accelemeter.getXYZ(&x, &y, &z);

  if (currentTime > 200 && flagInit == false ) {
    Serial.println("Début Init");
    BeginInitX();
  }





  buttonState_0 = digitalRead(8);
  buttonState_1 = digitalRead(9);

  Wire.beginTransmission(MPU_0);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_0, 14, true);

  AcXoff = -250;
  AcYoff = 36;
  AcZoff = 1200;

  AcX = (Wire.read() << 8 | Wire.read()) + AcXoff;
  AcY = (Wire.read() << 8 | Wire.read()) + AcYoff;
  AcZ = (Wire.read() << 8 | Wire.read()) + AcZoff;

  getAngle(AcX, AcY, AcZ);

  message = "";

  if (pitch > -10.0 && pitch < 10.0)
    message += "0";
  else if (pitch > -25.0 && pitch < -10.0)
    message += "1";
  else if (pitch < -25.0)
    message += "2";
  else if (pitch < 22.0 && pitch > 10.0)
    message += "3";
  else if (pitch > 22.0)
    message += "4";

  message += "/";

  Wire.beginTransmission(MPU_1);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_1, 14, true);

  AcXoff = -250;
  AcYoff = 36;
  AcZoff = 1200;

  AcX = (Wire.read() << 8 | Wire.read()) + AcXoff;
  AcY = (Wire.read() << 8 | Wire.read()) + AcYoff;
  AcZ = (Wire.read() << 8 | Wire.read()) + AcZoff;

  getAngle(AcX, AcY, AcZ);

  if (pitch < -15.0)
    message += "0";
  else if (pitch > -15.0 && pitch < 1.0)
    message += "1";

  message += "/";
  
  if (flagInit == true) {
    xAfter = (x + InitX);

    if (xAfter > 6) {
      message += "0";
    }
    else if (xAfter < -5) {
      message += "1";
    }
    else
    {
      message += "9";
    }
  }

  message += "/";

  if (buttonState_0 != HIGH)
  {
    message += "0";
    delay(35);
  }
  else if (buttonState_1 != HIGH)
  {
    message += "1";
    delay(35);
  }
  else
  {
    message += "9";
  }



  Serial.println(message);
}


void getAngle(int Vx, int Vy, int Vz) {
  double x = Vx;
  double y = Vy;
  double z = Vz;
  pitch = atan(x / sqrt((y * y) + (z * z)));
  roll = atan(y / sqrt((x * x) + (z * z)));
  //convert radians into degrees
  pitch = pitch * (180.0 / 3.14);
  roll = roll * (180.0 / 3.14) ;
}

void BeginInitX() { //Insert la valeur inverse de X dans InitX afin de faire l'addition des deux et ainsi être égal à 0

  Serial.println(x);

  InitX = x * (-1); // Inverse la valeur de X
  Serial.println(InitX);
  flagInit = true;
}
