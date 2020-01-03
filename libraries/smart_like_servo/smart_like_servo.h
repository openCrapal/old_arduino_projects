/*
  smart_like_servo.h - Library for hacking up to 4 standard ANALOGIC servos
  with an Arduino board and a motor shield, into serial controled, regulated, smart-like servo
  The motors of the servos have to be directly powered by PWM throught a H-bridge with the Pins pinDir ans pinPwm
  The analog pin analogInput must read a tension  image of the position of the servo, for instance with the potentiometer
  The use of encoder (numerical servo) would requier some adaptatation (not supported). Also not recommended for Arduino.
  Created by Ulysse R. Delplanque, Mars 29, 2017.
  Released into the public domain.
*/

#ifndef smart_like
#define smart_like

//#include <Arduino.h>
#include <reg.h>

const float presets[2][8] = {{20, 1, 0, 127, 50, -90, 950, 90}, {10, 1, 0, 100, 60, 10, 900, 80}};

class S_Servo
{
	public:
		S_Servo(unsigned int pinDir, unsigned int pinPwm, unsigned int analogInput, unsigned int preset);
		S_Servo(unsigned int pinDir, unsigned int pinPwm, unsigned int analogInput);
		~S_Servo();
		void setGoaldegrees(float goal);
		void setPID(float kp, float ki, float kd, float sat);
		void setMap(float vmin, float vmax, float dmin, float dmax);
		float getPosdegree();
		void run();
	protected:
	    void _genRef(); // <------------
		PID_Block *_regulation;
		Block *_capteur;
		Block *_consigne;
		unsigned int _dir;
		unsigned int _pwm;
		unsigned int _analogIn;
		unsigned int _vmin;
		unsigned int _vmax;
		int _dmin;
		int _dmax;
		bool _invertDir;
};

#endif
