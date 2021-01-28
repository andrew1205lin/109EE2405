// uLCD-144-G2 basic text demo program for uLCD-4GL LCD driver library

//

#include "mbed.h"

#include "uLCD_4DGL.h"


uLCD_4DGL uLCD(D1, D0, D2); // serial tx, serial rx, reset pin;
PwmOut PWM1(D7);
Serial pc( USBTX, USBRX );

int main()

{

    // basic printf demo = 16 by 18 characters on screen

    uLCD.printf("\n107061127\n"); //Default Green on black text
    uLCD.filled_rectangle(50, 50, 100, 100, 0x00FFFF);
    

    PWM1.period(0.001);
    PWM1=0;

    while (1)
    {
        for (int i = 0; i < 10; i++)
        {
            PWM1 = PWM1+0.1;
            pc.printf("%d\r\n", PWM1);
            wait(0.1);
        }
        
        for (int i = 0; i < 10; i++)
        {
            PWM1 = PWM1-0.1;
            pc.printf("%d\r\n", PWM1);
            wait(0.1);
        }
        
    }
    
  

}






 

