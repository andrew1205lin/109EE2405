#include "mbed.h"

#include <cmath>

#include "DA7212.h"


DA7212 audio;

EventQueue queue(32 * EVENTS_EVENT_SIZE);
InterruptIn button(SW2);
DigitalOut led(LED1);
DigitalOut green(LED2);
Thread t;
int16_t waveform[kAudioTxBufferSize];
int song[42] = {

  261, 330, 392, 522, 440, 522, 392, 
  
  349, 392, 330, 261, 294, 294, 261,

  392, 392, 349, 349, 330, 392, 294,

  392, 392, 349, 349, 330, 392, 294,

  261, 330, 392, 522, 440, 522, 392, 
  
  349, 392, 330, 261, 294, 294, 261,};


int noteLength[42] = {

  1, 1, 1, 1, 1, 1, 2,

  1, 1, 1, 1, 1, 1, 2,

  1, 1, 1, 1, 1, 1, 2,

  1, 1, 1, 1, 1, 1, 2,

  1, 1, 1, 1, 1, 1, 2,

  1, 1, 1, 1, 1, 1, 2};



void playNote(int freq)

{

  for(int i = 0; i < kAudioTxBufferSize; i++)

  {

    waveform[i] = (int16_t) (sin((double)i * 2. * M_PI/(double) (kAudioSampleFrequency / freq)) * ((1<<16) - 1));

  }

  audio.spk.play(waveform, kAudioTxBufferSize);

}


int state;
void ISR1(){
    state = 0;  //press SW2 to enter selecting mode;
    led=!led;
    green=1;
}
int main(void)

{
  t.start(callback(&queue, &EventQueue::dispatch_forever));
  button.rise(&ISR1);

  while (1)
  {
    
    if(state==1){
      green=0;
      for(int i = 0; i < 42; i++){
      
      int length = noteLength[i];

      while(length--)

      {

      // the loop below will play the note for the duration of 1s

      for(int j = 0; j < kAudioSampleFrequency / kAudioTxBufferSize; ++j)

      {

        queue.call(playNote, song[i]);
        

      }

      if(length < 1) wait_ms(250);

      }

      } 
    }
      
    state = 1;
  }
  


}