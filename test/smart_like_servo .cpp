/*
  smart_like_servo.cpp - Library for hacking up to 4 standard ANALOGIC servos
  with an Arduino board and a motor shield, into serial controlled, regulated, smart-like servo
  The motors of the servos have to be directly powered by PWM through a H-bridge with the Pins pinDir and pinPwm
  The analog pin analogInput must read a tension  image of the position of the servo, for instance with the potentiometer
  The use of encoder (numerical servo) would requier some adaptatation (not supported). Also not recommended for Arduino.
  Created by Ulysse R. Delplanque, Mars 29, 2017.
  Released into the public domain.
*/

#include <smart_like_servo.h>
#include <reg.h>
#include <Arduino.h>

S_Servo::S_Servo(unsigned int pinDir, unsigned int pinPwm, unsigned int analogInput, unsigned int preset)
{
	_dir = pinDir;
	_pwm = pinPwm;
	_analogIn = analogInput;
	pinMode(_dir, OUTPUT);
	pinMode(_pwm, OUTPUT);
	pinMode(_analogIn, INPUT);
	_capteur = new Block();
	_consigne= new Block();
	_regulation = new PID_Block(_capteur, _consigne, presets[preset][0], presets[preset][1], presets[preset][2], presets[preset][3]);
	_vmin = presets[preset][4];
	_vmax = presets[preset][6];
	_dmin = presets[preset][5];
	_dmax = presets[preset][7];
	_invertDir = false;
}

S_Servo::S_Servo(unsigned int pinDir, unsigned int pinPwm, unsigned int analogInput) : S_Servo(pinDir, pinPwm, analogInput, 0)
{

}

S_Servo::~S_Servo()
{
}

void S_Servo::setGoaldegrees(float goal)
{
	_consigne->setVal(constrain(goal, _dmin, _dmax));
}

float S_Servo::getPosdegree() // Returns the last read position of the servo
{
    return _capteur->getVal();
}

void S_Servo::setPID(float kp, float ki, float kd, float sat)
{
    delete (_regulation);
    _regulation = new PID_Block(_capteur, _consigne, kp, ki, kd, sat);
}


void S_Servo::setMap(float vmin, float vmax, float dmin, float dmax)
{
    _vmin = vmin;
	_vmax = vmax;
	_dmin = dmin;
	_dmax = dmax;
}

void S_Servo::run()
{
	_regulation->newCycle();
	float valCapteur = 0.001*map(analogRead(_analogIn), _vmin, _vmax, _dmin*1000., _dmax*1000.);
	if (abs(valCapteur-_consigne->getVal()) < 0.1)
    {
        valCapteur = _consigne->getVal();
    }
	_capteur->setVal(valCapteur);

	int rate = _regulation->getVal();
	if (_invertDir)
	{
		rate = -rate;
	}
	if (rate > 0)
	{
		digitalWrite(_dir, LOW);
	}
	else
	{
		digitalWrite(_dir, HIGH);
	}
	analogWrite(_pwm, constrain(abs(rate), 0, 255));

}
