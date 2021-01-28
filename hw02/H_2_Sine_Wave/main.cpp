#include "mbed.h"
#include "math.h"

//pico
Serial pc( USBTX, USBRX );
AnalogOut Aout(DAC0_OUT);
AnalogIn Ain(A0);

//7segs
BusOut display(D6, D7, D9, D10, D11, D5, D4, D8);

//SW,LED
DigitalIn  Switch(SW3);
DigitalOut redLED(LED1);
DigitalOut greenLED(LED2);

//pico
int sample = 400;
int i;
int j;

int peek[110]={0};//400 samples per second ,100Hz => 100 peeks, choose 110 for acccident
float ADCdata[400];

//7segs
char table[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};



int main(){
    
    //sampling
    for (i = 0; i < sample; i++){
        
        ADCdata[i] = Ain;
        wait(1./sample);//sample 400 times need about 1 s
    }

    for (i = 0; i < sample; i++){
        pc.printf("%1.3f\r\n", ADCdata[i]);
        wait(0.1);
    }
    
    j=0;
    for (i = 0; i < sample; i++){
        if(ADCdata[i]>ADCdata[i+1] && ADCdata[i]>ADCdata[i-1]){
            peek[j]=i;
            j++;
        }
    }

    //pc.printf("The Frequency is %d Hz\r\n", j);// there are j peeks in 1 s => j Hz

    while(1){
        
        if( Switch == 1 ){
            greenLED = 0;
            redLED = 1;
            display = 0;
            for (i = 0; i < sample*4; i++)
            {
                Aout = (sin(2*M_PI*j*i/sample/4))/2+0.5;//peek is 3.3V?
                wait(1./sample/4);
                if(Switch == 0){
                    break;
                }
            }
            
                
        }else{
            redLED = 0;
            greenLED = 1;
            display = table[j/100];
            wait(0.25);
            display = table[(j-(j/100)*100)/10];
            wait(0.25);
            display = table[j%10] | 0b10000000;
            wait(0.25);
        }
    }

}

