#include "mbed.h"
#include <string>
#include "math.h"
#include "uLCD_4DGL.h"


uLCD_4DGL uLCD(D1, D0, D2); // serial tx, serial rx, reset pin;
Serial pc(USBTX, USBRX);
InterruptIn button(SW2);
InterruptIn keyboard0(SW3);
DigitalOut green_led(LED2);

std::string song_name;
char buff;
int frequency[42];
int len[42];
int j;

int main(void){
    green_led = 1;
    while(1){
        
        if(pc.readable()){
            buff = pc.getc();
            if(buff == '\n'){
                break;
            }else{
                song_name = song_name + buff;
            }
        }
    }
    
    for(int i=0;i<42;i++){
        j=2;
        while(1){
            if(pc.readable()){
                buff = pc.getc();
                if(buff == '\n'){
                    break;
                }else{
                    frequency[i] = frequency[i] + pow(10,j)*(buff-48);
                    j--;
                }
            }  
        }
    }
    
    for(int i=0;i<42;i++){
        while(1){
            if(pc.readable()){
                buff = pc.getc();
                if(buff == '\n'){
                    break;
                }else{
                    len[i] = buff-48;
                }
            }  
        }
    }

    green_led = 1;
    uLCD.printf("%s\n",song_name.c_str());
    for(int i=0;i<42;i++){
        uLCD.printf("%d %d\n",frequency[i],len[i]);
    }
}