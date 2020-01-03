// ================================= Pins ====================================
#define PIN_DEAD 10 // oups! this one got burned
#define PIN_BAT 9
#define PIN_POW 11
#define PIN_RED 6
#define PIN_ORANGE 7
#define PIN_GREEN 8
#define PIN_BUTTON 2
#define ANA_BAT 2
#define ANA_POW 3
// ================ Index com user : int read_state_button() ===================
#define IDX_AKNO  1
#define IDX_ERROR -1
#define IDX_DONE  0
#define IDX_POW   2
#define IDX_BATT  3
#define TIME_LONG 3000 // 3sec press : security for main power switch
#define TIME_DEBOUNCE 50 // 50 ms mini
long t_begin_button = 0;
bool state_button_old = false;
bool user_pow = false;
bool user_batt= true;
bool user_akno = false;
long t0_akno = 0;
long t0_blink = 0;
#define AKNO_PERIODE 100
#define BLINK_PERIODE 200

int read_state_button()
{
  long t = millis();
  int status_index= -1;
  
  bool state_button = not digitalRead(PIN_BUTTON);
  if( state_button )
  { // begin timer or continue timer
    if ( not state_button_old )
    {
      state_button_old = state_button;
      t_begin_button = t;
    }
    else if ( t - t_begin_button >= TIME_LONG )
    {
      status_index = IDX_AKNO;
    }
  }
  else 
  {
    if ( state_button_old) // button released
    {
      if (t - t_begin_button <= TIME_DEBOUNCE) // too short, ignore
      {
        status_index= -1;
      }
      else if (t - t_begin_button >= TIME_LONG) // long press
      {
        status_index = IDX_POW;
      }
      else
      {
        status_index = IDX_BATT;
      }
    }
  }
  state_button_old = state_button;
  return status_index;
}

void aknoledge( bool do_akno )
{
  if ( not do_akno )
  {
    digitalWrite(PIN_ORANGE, LOW);
    t0_akno = millis();
  }
  else
  {
    if ( ((millis() - t0_akno) % (AKNO_PERIODE)) < (AKNO_PERIODE / 2)) // first half period
    {
      digitalWrite(PIN_ORANGE, HIGH);
    }
    else {digitalWrite(PIN_ORANGE, LOW);} // second half period
  }
}

void blink_batt( double rate_procentual )
{
  if ( rate_procentual <= 0.0 )
  {
    digitalWrite(PIN_GREEN, LOW);
    t0_blink = millis();
  }
  else if ( rate_procentual >= 100.0 )
  {
    digitalWrite(PIN_GREEN, HIGH);
    t0_blink = millis();
  }
  else
  {
    if ( ((millis() - t0_blink) % (BLINK_PERIODE)) < (BLINK_PERIODE * rate_procentual / 100.0)) // first half period
    {
      digitalWrite(PIN_GREEN, HIGH);
    }
    else {digitalWrite(PIN_GREEN, LOW);} // second half period
  }
}

// ================================= Param ==================================
double gain_mesure_bat = 0.029617;
double gain_mesure_pow = 0.027986;

// 4.2 Volt for each cell. A bit less won't kill anybody
//                         A bit more in the other hand...
double U_Max = 4.10 * 2.0;  // Volts
double U_Min = 7.0;         // Volts, requiered to power the Pi
double I_Max = 3.0;         // Amperes
String title ="t(s)\tPWM\tI(A)\tU_0_bat\tU_alim\tU_bat\tbutt";

// ================================= Vars ===================================
#define T_LOOP_DRAW_TITLE 5000
long t_old_draw_title = 0;
#define T_LOOP_DRAW 500
long t_old_draw = 0;
double U_zero_bat = 0.0;
double U_power = 0.0;
double U_bat = 0.0;
double I = 0.0;

double U_inversion = 1.1; // Voltage lost when batterie used as power source

bool alim_on = false;
bool accu_on = false;
double procent_batt = 0.0;

// ================================= Setup ===================================
void setup()
{
  pinMode(PIN_DEAD, INPUT);
  digitalWrite(PIN_DEAD,HIGH);
  pinMode(PIN_RED, OUTPUT);
  pinMode(PIN_ORANGE, OUTPUT);
  pinMode(PIN_GREEN, OUTPUT);
  pinMode(PIN_POW, OUTPUT);
  pinMode(PIN_BAT, OUTPUT);
  
  pinMode(PIN_BUTTON, INPUT);
  digitalWrite(PIN_BUTTON, HIGH);
  
  Serial.begin(115200);
  Serial.println(title);
}
// ================================= Loop ===================================
void loop()
{
  
  // ======== Com User ============
  int idx_user_state_button = read_state_button();
  if (idx_user_state_button == IDX_POW){user_pow = not user_pow;}
  else if (idx_user_state_button == IDX_BATT){user_batt = not user_batt;}

  // ===== Trivial cases ====
  if ( not user_batt){digitalWrite(PIN_BAT, LOW);}
  if ( not user_pow ){digitalWrite(PIN_POW, LOW);}

  // ==== Get tension values ===
  long t = millis();
  if ( t - t_old_draw >= T_LOOP_DRAW)
  {
    if (user_batt)
    {
      digitalWrite(PIN_BAT, LOW); // pwm off to get batterie's tension
      delay(10);
    }
    if ( t - t_old_draw_title >= T_LOOP_DRAW_TITLE)
    {
      Serial.println(title);
      t_old_draw_title = t;
    }
    // get tension Values
    U_zero_bat = gain_mesure_bat*analogRead(ANA_BAT);
    U_power = gain_mesure_pow*analogRead(ANA_POW);
    U_bat = U_power - U_zero_bat;

    // ===== Detect alim issues =====
    if (U_power - U_bat < 0.1 and U_zero_bat >= 0.1) // no reliable external power source connected
    {
      user_batt = false;
      alim_on = false;
      U_bat += U_inversion;
      U_power= U_bat;
    }
    else {alim_on = true;}

    if (U_power < U_Min) // not enought tension to power the Pi
    {
      user_pow = false;
      alim_on = false;
    }
    else
    {
      alim_on = true;
    }
    
    

    // Calculate charge intensity => PWM value if needed
    I = 0.0;
    int i = 0;
    if (user_batt)
    {
      I = max(0.0, min((U_Max - U_bat) * 1.0, I_Max));
      i =255;// min(max(0, int(I*180.0)), 255); //156

      analogWrite(PIN_BAT, i);
    }

    
    
    
    Serial.print(double(millis()/100)/10.0);
    Serial.print("\t");
    Serial.print(i);
    Serial.print("\t");
    Serial.print(I);
    Serial.print("\t");
    Serial.print(U_zero_bat);
    Serial.print("\t");
    Serial.print(U_power);
    Serial.print("\t");
    Serial.print(U_bat);
    Serial.print("\t");
    Serial.print(user_batt);
    Serial.print("\t");
    Serial.println(procent_batt);
    t_old_draw = t;
  } // End draw


  if (alim_on and user_pow){digitalWrite(PIN_POW, HIGH);}
  
  
  
  
  if (alim_on)
  {
    digitalWrite(PIN_RED, LOW);
  }
  else
  {
    digitalWrite(PIN_RED, HIGH);
  }
  if (not user_batt)
  {
    if (U_bat >= U_Min)
    {
      procent_batt = 100.0;
    }
    else
    {
      procent_batt = 0.0;
    }
  }
  else
  {
    
    procent_batt = map(100.0*U_bat, 100.0*U_Min, 100.0*U_Max, 0.0, 100.0);
    procent_batt = min(max(procent_batt, 0.0), 100.0);
  }
  
  blink_batt(procent_batt);
  //digitalWrite(PIN_ORANGE, user_pow);           //Orange shows power statut or akno
  aknoledge(IDX_AKNO == idx_user_state_button);

  
  
}
