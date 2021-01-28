#include "mbed.h"

#include "fsl_port.h"

#include "fsl_gpio.h"

#include <string>

#define UINT14_MAX        16383

// FXOS8700CQ I2C address

#define FXOS8700CQ_SLAVE_ADDR0 (0x1E<<1) // with pins SA0=0, SA1=0

#define FXOS8700CQ_SLAVE_ADDR1 (0x1D<<1) // with pins SA0=1, SA1=0

#define FXOS8700CQ_SLAVE_ADDR2 (0x1C<<1) // with pins SA0=0, SA1=1

#define FXOS8700CQ_SLAVE_ADDR3 (0x1F<<1) // with pins SA0=1, SA1=1

// FXOS8700CQ internal register addresses

#define FXOS8700Q_STATUS 0x00

#define FXOS8700Q_OUT_X_MSB 0x01

#define FXOS8700Q_OUT_Y_MSB 0x03

#define FXOS8700Q_OUT_Z_MSB 0x05

#define FXOS8700Q_M_OUT_X_MSB 0x33

#define FXOS8700Q_M_OUT_Y_MSB 0x35

#define FXOS8700Q_M_OUT_Z_MSB 0x37

#define FXOS8700Q_WHOAMI 0x0D

#define FXOS8700Q_XYZ_DATA_CFG 0x0E

#define FXOS8700Q_CTRL_REG1 0x2A

#define FXOS8700Q_M_CTRL_REG1 0x5B

#define FXOS8700Q_M_CTRL_REG2 0x5C

#define FXOS8700Q_WHOAMI_VAL 0xC7


I2C i2c( PTD9,PTD8);

Serial pc(USBTX, USBRX);

DigitalOut led2(LED2);

DigitalIn sw2(SW2);

Timeout tout;

int m_addr = FXOS8700CQ_SLAVE_ADDR1;

float t[3];

//float H[3];

std::string logger[105];//105 for save

char deplaceflag='0';

float Xdeplace=0.0;

float Ydeplace=0.0;

int i=0;

uint8_t who_am_i, data[2], res[6];

int16_t acc16;

void FXOS8700CQ_readRegs(int addr, uint8_t * data, int len);

void FXOS8700CQ_writeRegs(uint8_t * data, int len);

//void SetHori()

void PrintLogger();

void StartLogger();

void Trig_led2();

void Close_led2();

int main() {

   pc.baud(115200);

   // Enable the FXOS8700Q

   FXOS8700CQ_readRegs( FXOS8700Q_CTRL_REG1, &data[1], 1);

   data[1] |= 0x01;

   data[0] = FXOS8700Q_CTRL_REG1;

   FXOS8700CQ_writeRegs(data, 2);


   // Get the slave address

   FXOS8700CQ_readRegs(FXOS8700Q_WHOAMI, &who_am_i, 1);


   pc.printf("Here is %x\r\n", who_am_i);

   EventQueue queue;  
   led2=1; 

   //SetHori();

   while (1)
   {
      if(sw2==0){
      i=0;
      //queue.call(printf, "called immediately\r\n");
      queue.call_every(100, &StartLogger);
      queue.call_every(500, &Trig_led2);
      //queue.call_in(9999, &Close_led2);
      queue.call_in(10200, &PrintLogger);
      tout.attach(callback(&queue, &EventQueue::break_dispatch ), 11);//1s for print 100 logger
      queue.dispatch();
      }
      
   }
   
}
/*
void SetHori(){

   FXOS8700CQ_readRegs(FXOS8700Q_OUT_X_MSB, res, 6);


      acc16 = (res[0] << 6) | (res[1] >> 2);

      if (acc16 > UINT14_MAX/2)

         acc16 -= UINT14_MAX;

      H[0] = ((float)acc16) / 4096.0f;


      acc16 = (res[2] << 6) | (res[3] >> 2);

      if (acc16 > UINT14_MAX/2)

         acc16 -= UINT14_MAX;

      H[1] = ((float)acc16) / 4096.0f;


      acc16 = (res[4] << 6) | (res[5] >> 2);

      if (acc16 > UINT14_MAX/2)

         acc16 -= UINT14_MAX;

      H[2] = ((float)acc16) / 4096.0f;
}
*/
void PrintLogger(){
   for(int i=0;i<100;i++){pc.printf("%s",logger[i].c_str());
   pc.printf("%d",i);}
}

void StartLogger(){

   FXOS8700CQ_readRegs(FXOS8700Q_OUT_X_MSB, res, 6);


      acc16 = (res[0] << 6) | (res[1] >> 2);

      if (acc16 > UINT14_MAX/2)

         acc16 -= UINT14_MAX;

      t[0] = ((float)acc16) / 4096.0f;


      acc16 = (res[2] << 6) | (res[3] >> 2);

      if (acc16 > UINT14_MAX/2)

         acc16 -= UINT14_MAX;

      t[1] = ((float)acc16) / 4096.0f;


      acc16 = (res[4] << 6) | (res[5] >> 2);

      if (acc16 > UINT14_MAX/2)

         acc16 -= UINT14_MAX;

      t[2] = ((float)acc16) / 4096.0f;


      Xdeplace = Xdeplace + 9.8*t[0]/2; //*0.1*0.1*100=1

      Ydeplace = Ydeplace + 9.8*t[1]/2;

      if( (Xdeplace*Xdeplace) + (Ydeplace*Ydeplace) > 25){ //0.7071 = g / sqrt(2)
         deplaceflag='1';
      }else{
         deplaceflag='0';
      }
      /*printf("FXOS8700Q ACC: X=%1.4f(%x%x) Y=%1.4f(%x%x) Z=%1.4f(%x%x)\r\n",\

            t[0], res[0], res[1],\

            t[1], res[2], res[3],\

            t[2], res[4], res[5]\

      );*/

      logger[i]=std::to_string(t[0]) + " " + std::to_string(t[1]) + " " + std::to_string(t[2]) + " " + deplaceflag + "\r\n"; //std::to_string() convert to string
      //check t[0] convert successfully pc.printf("%s %f \r\n",(std::to_string(t[0])).c_str(),t[0]); // string.c_str() to be printfable

      i++; 
}

void Trig_led2()  {

    led2 = !led2;

}

void Close_led2() {
    led2 = 1;
}

void FXOS8700CQ_readRegs(int addr, uint8_t * data, int len) {

   char t = addr;

   i2c.write(m_addr, &t, 1, true);

   i2c.read(m_addr, (char *)data, len);

}


void FXOS8700CQ_writeRegs(uint8_t * data, int len) {

   i2c.write(m_addr, (char *)data, len);

}