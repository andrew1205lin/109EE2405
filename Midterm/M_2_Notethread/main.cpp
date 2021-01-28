#include "DA7212.h"
#include "accelerometer_handler.h"

#include "config.h"

#include "magic_wand_model_data.h"


#include "tensorflow/lite/c/common.h"

#include "tensorflow/lite/micro/kernels/micro_ops.h"

#include "tensorflow/lite/micro/micro_error_reporter.h"

#include "tensorflow/lite/micro/micro_interpreter.h"

#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"

#include "tensorflow/lite/schema/schema_generated.h"

#include "tensorflow/lite/version.h"
#include "mbed.h"
#include "uLCD_4DGL.h"
#include <string>

DA7212 audio;
InterruptIn button(SW2);
DigitalIn click(SW3);
DigitalOut led(LED1);
uLCD_4DGL uLCD(D1, D0, D2); // serial tx, serial rx, reset pin;
std::string song_name[3]={"LittleStar","FastTrain",""};
//song_name[0]="LittleStar";
//song_name[1]="FastTrain";
EventQueue queue(32 * EVENTS_EVENT_SIZE);
Thread Notethread;


int PredictGesture(float* output) {

  // How many times the most recent gesture has been matched in a row

  static int continuous_count = 0;

  // The result of the last prediction

  static int last_predict = -1;


  // Find whichever output has a probability > 0.8 (they sum to 1)

  int this_predict = -1;

  for (int i = 0; i < label_num; i++) {

    if (output[i] > 0.8) this_predict = i;

  }


  // No gesture was detected above the threshold

  if (this_predict == -1) {

    continuous_count = 0;

    last_predict = label_num;

    return label_num;

  }


  if (last_predict == this_predict) {

    continuous_count += 1;

  } else {

    continuous_count = 0;

  }

  last_predict = this_predict;


  // If we haven't yet had enough consecutive matches for this gesture,

  // report a negative result

  if (continuous_count < config.consecutiveInferenceThresholds[this_predict]) {

    return label_num;

  }

  // Otherwise, we've seen a positive result, so clear all our variables

  // and report it

  continuous_count = 0;

  last_predict = -1;


  return this_predict;

}



int16_t waveform[kAudioTxBufferSize];
int song_1[42] = {

  261, 261, 392, 392, 440, 440, 392,

  349, 349, 330, 330, 294, 294, 261,

  392, 392, 349, 349, 330, 330, 294,

  392, 392, 349, 349, 330, 330, 294,

  261, 261, 392, 392, 440, 440, 392,

  349, 349, 330, 330, 294, 294, 261};


int noteLength_1[42] = {

  1, 1, 1, 1, 1, 1, 2,

  1, 1, 1, 1, 1, 1, 2,

  1, 1, 1, 1, 1, 1, 2,

  1, 1, 1, 1, 1, 1, 2,

  1, 1, 1, 1, 1, 1, 2,

  1, 1, 1, 1, 1, 1, 2};

int song_2[42] = {

  392, 392, 330, 261, 392, 392, 330, 
  
  261, 294, 330, 349, 349, 330, 349,

  392, 392, 392, 330, 392, 330, 294,

  330, 261, 349, 294, 294, 294, 330,

  261, 261, 261, 294, 330, 349, 294,

  261, 247, 261, 330, 392, 330, 261};


int noteLength_2[42] = {

  1, 1, 1, 1, 1, 1, 1,

  1, 1, 1, 1, 1, 1, 1,

  1, 1, 1, 1, 1, 1, 1,

  1, 2, 1, 1, 1, 1, 1,

  1, 1, 1, 1, 1, 1, 1,

  1, 1, 1, 1, 1, 1, 1};

int song_3[42];
int noteLength_3[42];
int length;

volatile int state = 0;
int next_state = 1;
int mode = 0;
int current_song = 0;
int MAX_song = 2;
int position;
int gesture_index;
int cursor_move;
int DNN_count;

void ISR1(){
    state = 0;  //press SW2 to enter selecting mode;
}

void playNote(int freq){

  for(int i = 0; i < kAudioTxBufferSize; i++)

  {

    waveform[i] = (int16_t) (sin((double)i * 2. * M_PI/(double) (kAudioSampleFrequency / freq)) * ((1<<16) - 1));

  }

  audio.spk.play(waveform, kAudioTxBufferSize);

}

int main(){
    
    uLCD.printf("flaaaaag\n");
    
    button.rise(&ISR1);

    Notethread.start(callback(&queue, &EventQueue::dispatch_forever));
        //selecting mode:3 option, 1.2 have the same state;
        // Create an area of memory to use for input, output, and intermediate arrays.
        // The size of this will depend on the model you're using, and may need to be
        // determined by experimentation.
        constexpr int kTensorArenaSize = 60 * 1024;
        uint8_t tensor_arena[kTensorArenaSize];
        // Whether we should clear the buffer next time we fetch data
        bool should_clear_buffer = false;
        bool got_data = false;
        // The gesture index of the prediction


        // Set up logging.
        static tflite::MicroErrorReporter micro_error_reporter;

        tflite::ErrorReporter* error_reporter = &micro_error_reporter;


        // Map the model into a usable data structure. This doesn't involve any

        // copying or parsing, it's a very lightweight operation.

        const tflite::Model* model = tflite::GetModel(g_magic_wand_model_data);
        /*
        if (model->version() != TFLITE_SCHEMA_VERSION) {
        error_reporter->Report(
        "Model provided is schema version %d not equal "
        "to supported version %d.",
        model->version(), TFLITE_SCHEMA_VERSION);
        return -1;
        }*/
        // Pull in only the operation implementations we need.
        // This relies on a complete list of all the ops needed by this graph.
        // An easier approach is to just use the AllOpsResolver, but this will
        // incur some penalty in code space for op implementations that are not
        // needed by this graph.

        static tflite::MicroOpResolver<6> micro_op_resolver;
        micro_op_resolver.AddBuiltin(
        tflite::BuiltinOperator_DEPTHWISE_CONV_2D,
        tflite::ops::micro::Register_DEPTHWISE_CONV_2D());
        micro_op_resolver.AddBuiltin(tflite::BuiltinOperator_MAX_POOL_2D,
                               tflite::ops::micro::Register_MAX_POOL_2D());
        micro_op_resolver.AddBuiltin(tflite::BuiltinOperator_CONV_2D,
                               tflite::ops::micro::Register_CONV_2D());
        micro_op_resolver.AddBuiltin(tflite::BuiltinOperator_FULLY_CONNECTED,
                               tflite::ops::micro::Register_FULLY_CONNECTED());
        micro_op_resolver.AddBuiltin(tflite::BuiltinOperator_SOFTMAX,
                               tflite::ops::micro::Register_SOFTMAX());
        micro_op_resolver.AddBuiltin(tflite::BuiltinOperator_RESHAPE,
                               tflite::ops::micro::Register_RESHAPE(),1); //add missing op

        // Build an interpreter to run the model with

        static tflite::MicroInterpreter static_interpreter(
        model, micro_op_resolver, tensor_arena, kTensorArenaSize, error_reporter);
        tflite::MicroInterpreter* interpreter = &static_interpreter;


        // Allocate memory from the tensor_arena for the model's tensors
        interpreter->AllocateTensors();
        // Obtain pointer to the model's input tensor
        TfLiteTensor* model_input = interpreter->input(0);
        /*
        if ((model_input->dims->size != 4) || (model_input->dims->data[0] != 1) ||
        (model_input->dims->data[1] != config.seq_length) ||
        (model_input->dims->data[2] != kChannelNumber) ||
        (model_input->type != kTfLiteFloat32)) {
        error_reporter->Report("Bad input tensor parameters in model");
        return -1;
        }*/


        int input_length = model_input->bytes / sizeof(float);


        TfLiteStatus setup_status = SetupAccelerometer(error_reporter);
        /*
        if (setup_status != kTfLiteOk) {
            error_reporter->Report("Set up failed\n");
            return -1;
        }
        error_reporter->Report("Set up successful...\n");*/

    while(1){
        while(state == 0){  

            DNN_count=50; //BJ:run loop 50 times to get cursor_move 
            cursor_move=-1;
            while (DNN_count--) {
                // Attempt to read new data from the accelerometer
                got_data = ReadAccelerometer(error_reporter, model_input->data.f,
                                 input_length, should_clear_buffer);
                // If there was no new data,
                // don't try to clear the buffer again and wait until next time
                if (!got_data) {
                should_clear_buffer = false;
                continue;
                }
                // Run inference, and report any error
                TfLiteStatus invoke_status = interpreter->Invoke();
                /*
                if (invoke_status != kTfLiteOk) {
                    error_reporter->Report("Invoke failed on index: %d\n", begin_index);
                    continue;
                }*/

                // Analyze the results to obtain a prediction
                gesture_index = PredictGesture(interpreter->output(0)->data.f);
                // Clear the buffer next time we read data

                should_clear_buffer = gesture_index < label_num;


                // Produce an output
                if (gesture_index < label_num) {
                    cursor_move = gesture_index;
                }
            }

            if(cursor_move == 1){
                if(next_state == 3){
                    next_state = 1;
                }else{
                    next_state = next_state+1;
                }
                
            }else if(cursor_move == 0){
                if(next_state == 1){
                    next_state = 3;
                }else{
                    next_state = next_state-1;
                }
            }
            
            if(next_state == 1){
                uLCD.locate(1,1);
                uLCD.printf("-> nextsong\n   lastsong\n   selectsong\n");
            }else if(next_state == 2){
                uLCD.locate(1,1);
                uLCD.printf("   nextsong\n-> lastsong\n   selectsong\n");
            }else if(next_state == 3){
                uLCD.locate(1,1);
                uLCD.printf("   nextsong\n   lastsong\n-> selectsong\n");
            }
            if(click == 0){
                state = 1;
                mode = next_state;
                uLCD.printf("state = %d\n" , state );
            }
        }

        //selecting song (state=1)
        if(mode == 1){
            if (current_song < MAX_song){
                current_song = current_song + 1;
            }else{
                current_song = 1;
            }
            state = 2;
        }else if(mode == 2){
            if (current_song > 1){
                current_song = current_song - 1;
            }else{
                current_song = MAX_song;
            }
            state = 2;
        }else if(mode == 3){
            position = 1;
            uLCD.cls();
            uLCD.locate(0,1);
            for(int i=0;i<MAX_song;i++){
                uLCD.printf("   %s\n",song_name[i].c_str());
            }
            while(state == 1){

                DNN_count=50; //BJ:run loop 50 times to get cursor_move 
                cursor_move = -1;
                while (DNN_count--) {
                    // Attempt to read new data from the accelerometer
                    got_data = ReadAccelerometer(error_reporter, model_input->data.f,
                                 input_length, should_clear_buffer);
                    // If there was no new data,
                    // don't try to clear the buffer again and wait until next time
                    if (!got_data) {
                    should_clear_buffer = false;
                    continue;
                    }
                    // Run inference, and report any error
                    TfLiteStatus invoke_status = interpreter->Invoke();
                    /*
                    if (invoke_status != kTfLiteOk) {
                        error_reporter->Report("Invoke failed on index: %d\n", begin_index);
                        continue;
                    }*/

                    // Analyze the results to obtain a prediction
                    gesture_index = PredictGesture(interpreter->output(0)->data.f);
                    // Clear the buffer next time we read data

                    should_clear_buffer = gesture_index < label_num;


                    // Produce an output
                    if (gesture_index < label_num) {
                        cursor_move = gesture_index;
                    }
                }

                if(cursor_move == 1){
                    if(position == MAX_song){
                        position = 1;
                    }else{
                        position = position+1;
                    }
                }else if(cursor_move == 0){
                    if(position == 1){
                        position = MAX_song;
                    }else{
                        position = position-1;
                    }
                }
                uLCD.locate(0,0);
                uLCD.printf("   \n   \n   \n");
                uLCD.locate(0,position);
                uLCD.printf("->");

                if(click == 0){
                    current_song = position;
                    state = 2;
                }
            }
        }
        
        while(state == 2){
            if(current_song == 1){
                uLCD.cls();
                uLCD.printf("now playing\n  %s\n",song_name[0].c_str());
                /*for(int i = 0; i < 42; i++){
                    length = noteLength_1[i];
                    while(length--){
                    // the loop below will play the note for the duration of 1s
                        for(int j = 0; j < kAudioSampleFrequency / kAudioTxBufferSize; ++j){
                            queue.call(playNote,song_1[i]);
                        }
                        if(length < 1) wait(0.25);
                    }
                }*/
            }else if (current_song == 2){
                uLCD.cls();
                uLCD.printf("now playing\n  %s\n",song_name[1].c_str());
                /*for(int i = 0; i < 42; i++){
                    length = noteLength_2[i];
                    while(length--){
                    // the loop below will play the note for the duration of 1s
                        for(int j = 0; j < kAudioSampleFrequency / kAudioTxBufferSize; ++j){
                            queue.call(playNote,song_2[i]);
                        }
                        if(length < 1) wait(0.25);
                    }
                }*/
            }else if(current_song == 3){
                uLCD.cls();
                uLCD.printf("now playing\n  %s\n",song_name[2].c_str());
                /*for(int i = 0; i < 42; i++){
                    length = noteLength_3[i];
                    while(length--){
                    // the loop below will play the note for the duration of 1s
                        for(int j = 0; j < kAudioSampleFrequency / kAudioTxBufferSize; ++j){
                            queue.call(playNote,song_3[i]);
                        }
                        if(length < 1) wait(0.25);
                    }
                }*/
            }
        }

    }

}

