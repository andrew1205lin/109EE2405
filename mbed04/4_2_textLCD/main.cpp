#include "mbed.h"

#include "TextLCD.h"


DigitalOut led(LED_RED);

TextLCD lcd(D2, D3, D4, D5, D6, D7);


int main()

{

      int x=31;

      lcd.printf("107061127!\n");

      while(true)

      {
            x--;
            led = !led;             // toggle led

            lcd.locate(5,1);

            lcd.printf("%5i",x);    //conuter display

            wait(1);

            
            if(x == 0) {
                  break;
            }

      }



}


