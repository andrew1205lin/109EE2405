// uLCD-144-G2 basic text demo program for uLCD-4GL LCD driver library

//

#include "mbed.h"

#include "uLCD_4DGL.h"


uLCD_4DGL uLCD(D1, D0, D2); // serial tx, serial rx, reset pin;


int main()

{
    

    uLCD.background_color(WHITE);
    uLCD.cls();
    // basic printf demo = 16 by 18 characters on screen
    uLCD.color(BLUE);
    
    uLCD.printf("\n107061127\n"); //Default Green on black text

    uLCD.color(RED);

    for (int i=30; i>=0; --i) {

        uLCD.locate(1,1);

        uLCD.printf("-> 1\n   2\n   3\n");

        wait(0.2);        
        
        uLCD.locate(1,1);

        uLCD.printf("   1\n-> 2\n   3\n");

        wait(0.2);        
        
        uLCD.locate(1,1);

        uLCD.printf("   1\n   2\n-> 3\n");

        wait(0.2);



    }

}