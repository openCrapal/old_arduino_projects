/*
  Arduino.cpp - Dummy
  Created by Ulysse R. Delplanque, Mars 26, 2017.
  Released into the public domain.
*/
#ifndef ARDUINO_H_INCLUDED
#define ARDUINO_H_INCLUDED

#define OUTPUT 0
#define INPUT 1
#define HIGH 0
#define LOW 0

#include <Arduino.h>

int setup();
int loop();
long millis();
void pinMode(int a, int b);
void digitalWrite(int a, int b);
void analogWrite(int a, int b);
int analogRead(int a);
int map(int a, int b, int c, int d, int e);
unsigned int abs(int a);
double constrain(double a, double const& b, double const& c);
#endif // ARDUINO_H_INCLUDED
