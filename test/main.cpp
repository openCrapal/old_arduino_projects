#include <iostream>
#include <Arduino.h>
#include <reg.h>
#include <smart_like_servo.h>
#include <math.h>

using namespace std;

S_Servo myServo(1, 2, 3);

Block* In = new Block(1.0);
D_Block* D = new D_Block(In);
I_Block* I = new I_Block(D);
S_Block* S = new S_Block(I, D, 1.0, 1. );
//PID_Block* PID = new PID_Block(S, 1,1,1,1000);
PID_Block* PID = new PID_Block(S, 0.01,5,0,100);


int setup()
{
    return 0;
}


int loop()
{
    //In->setVal(sin(millis()/100.0));
    Block::newCycle();
    Block::newCycle();
    //cout << PID->_index << " "<< PID->_current_index << " "<< PID->getVal() << endl;
    cout << In->_index << " "<< In->_current_index << " "<< In->getVal() << endl;
    In->setVal(17.0);
    return 0;
}
