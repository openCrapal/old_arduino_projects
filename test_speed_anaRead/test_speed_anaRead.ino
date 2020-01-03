// ==================================== set analogRead speed ===============================
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

// set prescale to 16
sbi(ADCSRA,ADPS2) ;
cbi(ADCSRA,ADPS1) ;
cbi(ADCSRA,ADPS0) ;
// ===================================== end set analogRead speed =============================

void setup() {
 int start ;
 int i ;
 long sum=0;
 

 Serial.begin(9600) ;
 Serial.print("ADCTEST: ") ;
 start = millis() ;
 for (i = 0 ; i < 1000 ; i++)
   sum += analogRead(0) ;
   analogRead(1) ;
   analogRead(2) ;
   analogRead(3) ;
   analogRead(4) ;
 Serial.print(millis() - start) ;
 Serial.println(" msec (4000 calls)") ;
 Serial.println(sum/1000) ;
}

void loop() {
}
