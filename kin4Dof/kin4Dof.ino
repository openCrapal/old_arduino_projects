#include <reg.h>

#define DIR1 12
#define POW1 11
#define POS1 A0

#define ANA1 A1


#define FRAMERATE 10


long t0=millis();
long t_frame = t0;
int v_read = 0;
Block* Ref = new Block(0.0);
Block* Capteur = new Block(0.0);
PID_Block* MyPID = new PID_Block(Capteur, Ref, 1., 0., 0., 255.);

void setup()
{
  Serial.begin(115200);
  pinMode(POS1, INPUT);
  pinMode(ANA1, INPUT);
  pinMode(DIR1, OUTPUT);
  pinMode(POW1, OUTPUT);
  digitalWrite(DIR1, LOW);
  digitalWrite(POW1, LOW);
  delay(10);
}

void setSpeed(int _speed)
{
  if (_speed > 0)
  {
    digitalWrite(DIR1, LOW);
  }
  else
  {
    digitalWrite(DIR1, HIGH);
  }
  analogWrite(POW1, constrain(abs(_speed), 0, 255));
}
void loop()
{
  Capteur->newCycle();
  double t = millis()-t0;
  int val = int(80.0 * sin(6.28*1.*t/1000.0));
  //int val = map(analogRead(ANA1), 50, 950, -90, 90);
  
  v_read = analogRead(POS1);
  float theta = 0.001*map(v_read, 50, 950, -90000, 90000);

  Ref->setVal(val);
  Capteur->setVal(theta);
  setSpeed(MyPID->getVal());
  
  if(millis()-t_frame>FRAMERATE)
  {
    t_frame = millis();
    Serial.print(val);
    Serial.print("  :   ");
    Serial.print(Capteur->getVal());
    Serial.print("  :   ");
    Serial.println(MyPID->getVal());
  }
  delay(1);
}
