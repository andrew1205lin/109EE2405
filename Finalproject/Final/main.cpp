 #include "mbed.h"

 #include "bbcar.h"

 Serial pc(USBTX, USBRX);

 DigitalOut led1(LED1);

 PwmOut pin9(D9), pin8(D8);

 DigitalInOut pin10(D10);

 Ticker servo_ticker;

 Serial xbee(D12, D11);

 BBCar car(pin8, pin9, servo_ticker);

 Serial uart(D1,D0); //tx,rx

 float ping1_temp[10];
 int peekcount;
 int fallcount;
 int objecttype=0;
 int main() {

    parallax_ping  ping1(pin10);
    wait(1);
    /*car.goStraight(0,-10);//scan
    for(int i=0;i<10;i++){
        ping1_temp[i] = (float)ping1;
        pc.printf("%f \r\n",ping1_temp[i]);
        wait(.1);
    }
    car.stop();
    car.goStraight(0,5);//recover position
    wait(1);
    car.stop();
    for(int i=2;i<8;i++){
        if(ping1_temp[i]<ping1_temp[i+1] && ping1_temp[i]<ping1_temp[i-1] && ping1_temp[i+1]<ping1_temp[i+2] && ping1_temp[i-1]<ping1_temp[i-2]){
            peekcount++;
        }
    }
    for(int i=1;i<10;i++){
        if((ping1_temp[i]-ping1_temp[i-1])>=2 || (ping1_temp[i]-ping1_temp[i-1])<=-2){
            fallcount++;
        }
    }
    if(peekcount>=2){
        objecttype=1;//two peek
    }else if(fallcount >= 2){
        objecttype=2;//square
    }else if(fallcount == 1){
        objecttype=3;//tilted tri
    }else{
        objecttype=4;//tri
    }
    xbee.printf("objecttype = %d",objecttype);
    pc.printf("objecttype = %d",objecttype);*/

    //getting into first task
    xbee.printf("getting into first task\n\r");
    car.goStraight(150,48); //change to go straight
    while(1){
        if((float)ping1>12) led1 = 1;
        else{
            led1 = 0;
            car.stop();
            break;
        }
        wait(.01);
    }
    car.goStraight(-180,55);//change to turn just 90 degree
    wait(1);
    car.stop();

    //First Task:(1)back in to carrage
    xbee.printf("First Task:(1)back in to carrage\n\r");
    car.goStraight(150,48);
    while(1){
        if((float)ping1>12) led1 = 1;
        else{
            led1 = 0;
            car.stop();
            break;
        }
        wait(.01);
    }
    car.goStraight(180,-45);//change to turn just 90 degree *go too far change to -45
    wait(1);
    car.goStraight(-150,-48);//change to go backward
    while(1){
        if((float)ping1<45) led1 = 1;
        else{
            led1 = 0;
            car.stop();
            break;
        }
        wait(.01);
    }
    wait(1);
    //First Task:(1.5)go to detect
    xbee.printf("First Task:(1.5)go to detect\n\r");
    car.goStraight(150,48);
    while(1){
        if((float)ping1>20) led1 = 1;
        else{
            led1 = 0;
            car.stop();
            break;
        }
        wait(.01);
    }
    car.goStraight(180,-55);//change to turn just -90 degree
    wait(1);
    car.goStraight(150,48);
    while(1){
        if((float)ping1>18) led1 = 1;
        else{
            led1 = 0;
            car.stop();
            break;
        }
        wait(.01);
    }     
    car.goStraight(-180,55);//change to turn just 90 degree
    wait(1);
    car.stop();

    //First Task:(2)detect number
    xbee.printf("First Task:(2)detect number\n\r");
    char s[21];
    sprintf(s,"image_classification");
    uart.puts(s);
    wait(3);
    char c='n';
    while(c=='n'){
        if(uart.readable()) {
            c=uart.getc();
        }
    }
    xbee.printf("image classification: %c\n\r",c);

    //First Task:(3)leaving First task
    xbee.printf("First Task:(3)leaving First task\n\r");
    car.goStraight(150,48); //change to go straight
    while(1){
        if((float)ping1>12) led1 = 1;
        else{
            led1 = 0;
            car.stop();
            break;
        }
        wait(.01);
    }
    car.goStraight(180,-55);//change to turn just -90 degree
    wait(1);
    car.stop();
    wait(1);
    car.goStraight(150,48); //change to go straight
    while(1){
        if((float)ping1>36) led1 = 1;
        else{
            led1 = 0;
            car.stop();
            break;
        }
        wait(.01);
    }
    car.goStraight(180,-55);//change to turn just -90 degree
    wait(1);
    car.stop();

    //getting into second task
    xbee.printf("getting into second task\n\r");
    car.goStraight(150,48); //change to go straight
    while(1){
        if((float)ping1>12) led1 = 1;
        else{
            led1 = 0;
            car.stop();
            break;
        }
        wait(.01);
    }
    car.goStraight(180,-45);//change to turn just -90 degree *go too far change to -45
    wait(1);
    car.stop();   
    wait(3);
    //Second Task:(1)get to the scan position
    xbee.printf("Second Task:(1)get to the scan position\n\r");
    car.goStraight(150,48);
    while(1){
        if((float)ping1>50) led1 = 1;
        else{
            led1 = 0;
            car.stop();
            break;
        }
        wait(.01);
    }
    xbee.printf("--change to turn just -90 degree\n\r");
    car.goStraight(180,-55);//change to turn just -90 degree
    wait(1);
    xbee.printf("--change to go straight\n\r");
    car.goStraight(150,48);//change to go straight
    wait(1.5);
    xbee.printf("--change to turn just 90 degree\n\r");
    car.goStraight(-180,55);//change to turn just 90 degree
    wait(1);
    car.stop();

    //Second Task:(2)scan object
    xbee.printf("Second Task:(2)scan object\n\r");
    car.goStraight(0,-20);//scan
    for(int i=0;i<10;i++){
        ping1_temp[i] = (float)ping1;
        pc.printf("%f \r\n",ping1_temp[i]);
        wait(.1);
    }
    car.stop();
    car.goStraight(0,20);//recover position
    wait(1);
    car.stop();
    for(int i=2;i<8;i++){
        if(ping1_temp[i]<ping1_temp[i+1] && ping1_temp[i]<ping1_temp[i-1] && ping1_temp[i+1]<ping1_temp[i+2] && ping1_temp[i-1]<ping1_temp[i-2]){
            peekcount++;
        }
    }
    for(int i=1;i<10;i++){
        if((ping1_temp[i]-ping1_temp[i-1])>=2 || (ping1_temp[i]-ping1_temp[i-1])<=-2){
            fallcount++;
        }
    }
    if(peekcount>=2){
        objecttype=1;//two peek
    }else if(fallcount >= 2){
        objecttype=2;//square
    }else if(fallcount == 1){
        objecttype=3;//tilted tri
    }else{
        objecttype=4;//tri
    }
    xbee.printf("objecttype = %d\n\r",objecttype);
    pc.printf("objecttype = %d",objecttype);

    //Second Task:(3)get out of the scan position
    xbee.printf("Second Task:(3)get out of the scan position\n\r");
    car.goStraight(-180,55);//change to turn just 90 degree
    wait(1);
    car.goStraight(150,48);
    while(1){
        if((float)ping1>12) led1 = 1;
        else{
            led1 = 0;
            car.stop();
            break;
        }
        wait(.01);
    }
    car.goStraight(180,-55);//change to turn just -90 degree
    wait(1);
    car.goStraight(150,48);
    while(1){
        if((float)ping1>12) led1 = 1;
        else{
            led1 = 0;
            car.stop();
            break;
        }
        wait(.01);
    }


    //get out of the region
    xbee.printf("get out of the region\n\r");
    car.goStraight(180,-55);//change to turn just -90 degree
    wait(1);
    car.goStraight(150,48);//change to go straight
    wait(20);
    car.stop();

 }