#include <Wire.h>

#define MOTORSHIELDaddr 0x0f
#define SETFREQ 0x84
#define MOTOR_1 0xa1
#define MOTOR_2 0xa5

unsigned char freq_prescaler=8;

void initialiser_moteur()
{
  Wire.begin();
  delayMicroseconds(10);
    
  //init frequence pwm
  Wire.write(SETFREQ);
  Wire.write(freq_prescaler);
  Wire.write(0);
}

void set_pwm_values(char pwm1, char pwm2)
{
  Wire.beginTransmission(MOTORSHIELDaddr); 
  
  if (pwm1<0)
  {
    pwm1 = -1 * pwm1;
    Wire.write(MOTOR_1);
    Wire.write(2);
    Wire.write(pwm1);
  }
  else
  {
    Wire.write(MOTOR_1);
    Wire.write(1);
    Wire.write(pwm1);
  }
  if (pwm2<0)
  {
    pwm2 = -1 * pwm2;
    Wire.write(MOTOR_2);
    Wire.write(2);
    Wire.write(pwm2);
  }
  else
  {
    Wire.write(MOTOR_2);
    Wire.write(1);
    Wire.write(pwm2);
  }
}


void setup() {
  // put your setup code here, to run once:
  initialiser_moteur();
}

long compteur(0);

void loop() {
  // put your main code here, to run repeatedly:
  //set_pwm_values(128*sin(compteur * 6.28 / 1000.), 0);
  set_pwm_values(compteur%255-127, 30);

  compteur++;
}
