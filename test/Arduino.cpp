/*
  Arduino.cpp - Dummy
  Created by Ulysse R. Delplanque, Mars 26, 2017.
  Released into the public domain.
*/

#include <Arduino.h>

int setup();
int loop();

long dummy_time(0);
long millis()
{
    return dummy_time ++;
}

int analogRead(int a)
{
    return 0;
}
int digitalRead(int a)
{
    return 0;
}
void digitalWrite(int a, int b)
{
    return;
}
void analogWrite(int a, int b)
{
    return;
}
void pinMode(int a, int b)
{
    return;
}
unsigned int abs(int a)
{
    return 0;
}
double constrain(double a, double const& b, double const& c)
{
    return 0;
}
int map(int a, int b, int c, int d, int e)
{
    return 0;
}

class serial
{
public:
    void print(char _str[])
    {
        return;
    }
    void print(double _str)
    {
        return;
    }
    void print(long _str)
    {
        return;
    }
    void println(char _str[])
    {
        return;
    }
    void println(double _str)
    {
        return;
    }
    void println(long _str)
    {
        return;
    }
};

serial Serial = serial();

int main()
{
    setup();
    for(long i(0); i<100; i++)
    {
        loop();
    }
    return 0;
}
