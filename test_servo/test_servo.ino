
#define FASTADC 1

// defines for setting and clearing register bits
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

#include <smart_like_servo.h>

#define DIR1 12
#define POW1 11
#define POS1 A0

#define ANA1 A1

#define FRAMERATE 10

//using namespace std;

long t0=millis();
long t_frame = t0;
int v_read = 0;

S_Servo myServo(DIR1, POW1, POS1, 0);

void setup()
{
  #if FASTADC
  // set prescale to 16
  sbi(ADCSRA,ADPS2) ;
  cbi(ADCSRA,ADPS1) ;
  cbi(ADCSRA,ADPS0) ;
  #endif
  
  myServo.setPID(20, 1, 0, 255);
  myServo.setMap(100, 900, 5, 175);
  //Serial.begin(115200);
  pinMode(POS1, INPUT);
  pinMode(ANA1, INPUT);
  pinMode(DIR1, OUTPUT);
  pinMode(POW1, OUTPUT);
  digitalWrite(DIR1, LOW);
  digitalWrite(POW1, LOW);
  delay(10);
}


void loop()
{
  double t = millis()-t0;
  //int val = int(80.0 * sin(6.28*1.*t/1000.0));
  float val = 0.001*map(analogRead(ANA1), 0, 1024, 0, 180000);
  
  v_read = analogRead(POS1);
  float theta = 0.001*map(v_read, 50, 950, 0, 180000);

  myServo.setGoaldegrees(val);
  myServo.run();
  
//  if(millis()-t_frame>FRAMERATE)
//  {
//    t_frame = millis();
//    Serial.print(val);
//    Serial.print("  :   ");
//    Serial.print(theta);
//    Serial.print("  :   ");
//    Serial.println(myServo.getPosdegree());
//  }
  delay(1);
}
