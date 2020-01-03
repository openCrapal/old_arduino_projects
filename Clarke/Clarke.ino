#define OUT0 3
#define OUT1 5
#define OUT2 6
#define ANA0 0
#define ANA1 1
#define ANA2 2

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

// Modèle moteur
float L = 0.001;  //inductance cyclique des enroulements statoriques H
float R = 1.;     //r shunt Ohm
int p = 1;        //nbre paires de poles
float J = .1;   //inertie Kgm²
float phiR = 0.05;     //flux mag rotor T

long t_old = 0;     // ms
float dt =.005;       //s
float omega_r= 1.; //rad/s  vitesse rotor
float theta_r = 0.; //rad   position rotor

float theta_c=0.; // rad  comande
float U_c=0.; // V magnitute comande

float CFEM[3] = {0.}; //V Force contre electromotrice bobine
float i_abc[3]= {0.}; //A Intensité dans les branches du moteur
float u_abc[3]= {0.}; //V Tension aux bornes des bobines + resistance

float phases[3] = {1., 0., 0.};
float phasesRead[3] = {0.};
float P_clarke[3] = {0., 0., 0.};
float theta_abs = 0.;
long count = 0;

long compteur=0;

void updateU_c()
{
  for (int i = 0; i<3; i++)
  {
    u_abc[i] = U_c * sin (p * theta_c - 2*i*PI/3);
  }
}

void updateFCEM()
{
  for (int i = 0; i<3; i++)
  {
    CFEM[i] = -omega_r * p * phiR * sin(p * theta_r - 2*i*PI/3);
  }
}

void updateI_acb()
{
   // Methode 1
  float i_old[3] = {i_abc[0], i_abc[1], i_abc[2]};
  float increment = constrain(dt / (L/R), 0., 1.);
  for (int i = 0; i<3; i++)
  {
    i_abc[i] = i_old[i] * (1-increment) + ((u_abc[i] - CFEM[i]) / R) * increment; // dt millisecondes
  }
  /*
  // Methode 2
  analogWrite(OUT0, (127+127*u_abc[0] - CFEM[0]) / R);
  analogWrite(OUT1, (127+127*u_abc[1] - CFEM[1]) / R);
  analogWrite(OUT2, (127+127*u_abc[2] - CFEM[2]) / R);
  delay(8*5);
  i_abc[0] = analogRead(ANA0)/512. - 1.;
  i_abc[1] = analogRead(ANA1)/512. - 1.;
  i_abc[2] = analogRead(ANA2)/512. - 1.;
  */
}


void ClarkeDirect(float *in, float* out)
{
  out[0] = 0.666667 *( in[0] - 0.5        * in[1] - 0.5       * in[2]);
  out[1] = 0.666667 *(         0.8660254  * in[1] - 0.8660254 * in[2]);
  out[2] = 0.666667 *( 0.5 * in[0] + 0.5  * in[1] + 0.5       * in[2]);
}


float ampl =0.;
float val[2] = {0.};
float estimateur_theta(float *in)
{
  ampl = sqrt(sq(in[0]) + sq(in[1])) ;
  if (ampl < 0.0001)
  {
    return theta_r;
  }
  for (int i=0; i<2; i++)
  {
    val[i] = in[i] / ampl;
  }
  float mag=0.;
  if(abs(val[0] >= abs(val[1])))
  {
    mag = acos(constrain(val[0], -1, 1));
    if (val[1]<=0)
    {
      mag = -1.0 * mag; // + 2 * PI
    }
  }
  else
  {
    mag = asin(constrain(val[1], -1, 1));
    if (val[0]<=0)
    {
      if (val[1]<=0)
      {
        mag = PI - mag;
      }
      else
      {
        mag = PI - mag;
      }
    }
  }
  mag += 2 * count * PI;
  if (mag-theta_abs > 4)
  {
    mag -= 2*PI;
    count--;
  }
  else if(mag-theta_abs < -4)
  {
    mag += 2*PI;
    count ++;
  }
  theta_abs = mag;
  return mag;
}


void setup() {
  TCCR2B = TCCR2B & B11111000 | B00000010;    // set timer 2 divisor to     8 for PWM frequency of  3921.16 Hz
  //TCCR1B = TCCR1B & B11111000 | B00000011;    // set timer 1 divisor to    64 for PWM frequency of   490.20 Hz (The DEFAULT)
  TCCR0B = TCCR0B & B11111000 | B00000010;    // set timer 0 divisor to     8 for PWM frequency of  7812.50 Hz
  // put your setup code here, to run once:
  pinMode(ANA0, INPUT);
  pinMode(ANA1, INPUT);
  pinMode(ANA2, INPUT);
  pinMode(OUT0, OUTPUT);
  pinMode(OUT1, OUTPUT);
  pinMode(OUT2, OUTPUT);

  #if FASTADC
  // set prescale to 16
  sbi(ADCSRA,ADPS2) ;
  cbi(ADCSRA,ADPS1) ;
  cbi(ADCSRA,ADPS0) ;
  #endif
  
  Serial.begin(115200);

}

void loop() {
  // time
  dt = (millis() - t_old)/1000.;
  if (dt < 0.001)
  {
    dt = 0.001;
  }
  t_old = millis();

  // modèle méca
  float couple = .0;
  for (int i=0; i<3; i++)
  {
    couple += CFEM[i] * i_abc[i] / omega_r;
  }
  couple -= omega_r * 0.01;     // Frottements
  omega_r += dt * couple / J;   // PFD
  theta_r += omega_r * dt;
  
  //modèle elec
  updateU_c();
  updateFCEM();
  updateI_acb();

  //trafo
  ClarkeDirect(i_abc, P_clarke);
  float theta_ist =  estimateur_theta(P_clarke);
  //regulation

  //trafo inverse
  
  //commande
  
  theta_c-=0 ; // rad  comande
  U_c=0.1; // V magnitute comande

  
  /*
  analogWrite(OUT0, 127+127*phases[0]);
  analogWrite(OUT1, 127+127*phases[1]);
  analogWrite(OUT2, 127+127*phases[2]);
  delay(2);
  phasesRead[0] = analogRead(ANA0)/512. - 1.;
  phasesRead[1] = analogRead(ANA1)/512. - 1.;
  phasesRead[2] = analogRead(ANA2)/512. - 1.;
  */
  
  Serial.print(theta_r, 2);
  Serial.print('\t');
  Serial.print(estimateur_theta(P_clarke), 2);
  Serial.print('\t');
  Serial.print(ampl, 3);
  Serial.print('\t');
  Serial.print(val[0], 3);
  Serial.print('\t');
  Serial.print(val[1], 3);
  Serial.print('\t');
  Serial.print(i_abc[0], 3);
  Serial.print('\t');
  Serial.print(i_abc[1], 3);
  Serial.print('\t');
  Serial.println(i_abc[2], 3);
  delay(50);
  
  /*
  compteur++;
  if (compteur%500 == 0)
  {
    Serial.print(theta_r, 2);
    Serial.print('\t');
    Serial.print(estimateur_theta(P_clarke), 2);
    Serial.print('\t');
    Serial.print("t_500_loop, s");
    Serial.print('\t');
    Serial.println(millis()/8000., 3);
  }*/
}
