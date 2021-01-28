#include "mbed.h"

#include "mbed_rpc.h"

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

int m_addr = FXOS8700CQ_SLAVE_ADDR1;


void FXOS8700CQ_readRegs(int addr, uint8_t * data, int len);

void FXOS8700CQ_writeRegs(uint8_t * data, int len);


void getAcc();

void getAcc_logger(Arguments *in, Reply *out);

RPCFunction rpcAddr(&getAcc_logger, "getAcc_logger");

RawSerial pc(USBTX, USBRX);

RawSerial xbee(D12, D11);

void xbee_rx_interrupt(void);

void xbee_rx(void);

EventQueue queue(32 * EVENTS_EVENT_SIZE);

Thread t;

Timer runtime;

std::string acc_logger[1000];

int acc_counter = 0;

int main(){

   uint8_t data[2] ;

   // Enable the FXOS8700Q

   FXOS8700CQ_readRegs( FXOS8700Q_CTRL_REG1, &data[1], 1);

   data[1] |= 0x01;

   data[0] = FXOS8700Q_CTRL_REG1;

   FXOS8700CQ_writeRegs(data, 2);

  pc.baud(9600);

  pc.printf("hello");

  char xbee_reply[4];


  // XBee setting

  xbee.baud(9600);

  // start

  pc.printf("start\r\n");

  runtime.start();

  t.start(callback(&queue, &EventQueue::dispatch_forever));

  queue.call_every(100,&getAcc);

  // Setup a serial interrupt function of receiving data from xbee
  xbee.attach(xbee_rx_interrupt, Serial::RxIrq);
  

}

void getAcc() {

   int16_t acc16;

   float t[3];

   uint8_t res[6];

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


   acc_logger[acc_counter]= std::to_string(t[0]) + " " + std::to_string(t[1]) + " " + std::to_string(t[2]) + " " + std::to_string(runtime.read());// 0~n-1
   
  pc.printf("%s \r\n",acc_logger[acc_counter].c_str());

   acc_counter++;

   if(acc_counter>=1000){
     acc_counter=0;
   }

}


void getAcc_logger(Arguments *in, Reply *out) {
    int i = acc_counter;
    xbee.printf("%d\r\n",i);
    pc.printf("in getAcc_logger");
    for(int j=0;j<i;j++){
      xbee.printf("%s \r\n",acc_logger[j].c_str());
      acc_logger[j]="";
    }
    acc_counter=0;
}


void FXOS8700CQ_readRegs(int addr, uint8_t * data, int len) {

   char t = addr;

   i2c.write(m_addr, &t, 1, true);

   i2c.read(m_addr, (char *)data, len);

}


void FXOS8700CQ_writeRegs(uint8_t * data, int len) {

   i2c.write(m_addr, (char *)data, len);

}

void xbee_rx_interrupt(void)

{

  xbee.attach(NULL, Serial::RxIrq); // detach interrupt

  queue.call(&xbee_rx);

}


void xbee_rx(void)

{

  char buf[100] = {0};

  char outbuf[100] = {0};
  
  while(xbee.readable()){

    for (int i=0; ; i++) {

      char recv = xbee.getc();

      if (recv == '\r') {

        pc.printf("\r\n");
        
        break;

      }

      buf[i] = pc.putc(recv);

    }

    RPC::call(buf, outbuf);

    pc.printf("%s\r\n", outbuf);

  }

  xbee.attach(xbee_rx_interrupt, Serial::RxIrq); // reattach interrupt

}

/*
void reply_messange(char *xbee_reply, char *messange){

  xbee_reply[0] = xbee.getc();

  xbee_reply[1] = xbee.getc();

  xbee_reply[2] = xbee.getc();

  if(xbee_reply[1] == 'O' && xbee_reply[2] == 'K'){

    pc.printf("%s\r\n", messange);

    xbee_reply[0] = '\0';

    xbee_reply[1] = '\0';

    xbee_reply[2] = '\0';

  }

}*/

/*
void check_addr(char *xbee_reply, char *messenger){

  xbee_reply[0] = xbee.getc();

  xbee_reply[1] = xbee.getc();

  xbee_reply[2] = xbee.getc();

  xbee_reply[3] = xbee.getc();

  pc.printf("%s = %c%c%c\r\n", messenger, xbee_reply[1], xbee_reply[2], xbee_reply[3]);

  xbee_reply[0] = '\0';

  xbee_reply[1] = '\0';

  xbee_reply[2] = '\0';

  xbee_reply[3] = '\0';

}*/