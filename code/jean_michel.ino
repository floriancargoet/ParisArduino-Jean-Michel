#include "pitches.h"

const boolean ON  = LOW;  //Define on as LOW (this is because we use a common Anode RGB LED (common pin is connected to +5 volts)
const boolean OFF = HIGH;

//Predefined Colors
const boolean RED[]     = {ON,  OFF, OFF};
const boolean GREEN[]   = {OFF, ON,  OFF};
const boolean BLUE[]    = {OFF, OFF, ON};
const boolean YELLOW[]  = {ON,  ON,  OFF};
const boolean CYAN[]    = {OFF, ON,  ON};
const boolean MAGENTA[] = {ON,  OFF, ON};
const boolean WHITE[]   = {ON,  ON,  ON};
const boolean BLACK[]   = {OFF, OFF, OFF};

const boolean* COLORS[] = {RED, GREEN, BLUE, YELLOW, CYAN, MAGENTA, WHITE, BLACK};


//predefined notes
/*
const int notes[] = {
    0, //0, no balloon pressed
    NOTE_A1, //1,  pressed: 1
    NOTE_A2, //2,  pressed: 2
    NOTE_A3, //3,  pressed: 1,2
    NOTE_A4, //4,  pressed: 3
    NOTE_A5, //5,  pressed: 1,3
    NOTE_A7, //6,  pressed: 2,3
    NOTE_B1, //8,  pressed: 4
    NOTE_B2, //9,  pressed: 1,4
    NOTE_B3, //10, pressed: 2,4
    NOTE_B4, //11, pressed: 1,2,4
    NOTE_B5, //12, pressed: 3,4
    NOTE_B6, //13, pressed: 1,3,4
    NOTE_B7, //14, pressed: 2,3,4
    NOTE_B0  //15, pressed: 1,2,3,4
};
*/
const int notes[] = {
    0, //0, no balloon pressed
    NOTE_C4, //1,  DO  pressed: 1
    NOTE_D4, //2,  RE  pressed: 2
    NOTE_E4, //3,  MI  pressed: 1,2
    NOTE_F4, //4,  FA  pressed: 3
    NOTE_G4, //5,  SOL pressed: 1,3
    NOTE_A4, //6,  LA  pressed: 2,3
    NOTE_B4, //8,  SI  pressed: 4
    NOTE_B2, //9,  pressed: 1,4
    NOTE_B3, //10, pressed: 2,4
    NOTE_B4, //11, pressed: 1,2,4
    NOTE_B5, //12, pressed: 3,4
    NOTE_B6, //13, pressed: 1,3,4
    NOTE_B7, //14, pressed: 2,3,4
    NOTE_B0  //15, pressed: 1,2,3,4
};

//Pin Definitions
//The 74HC595 uses a serial communication
//link which has three pins
int data1  = 0;
int clock1 = 1;
int latch1 = 2;

int data2  = 4; // pin 3 is used internally by the tone() function, skip it
int clock2 = 5;
int latch2 = 6;

int input1 = 7; 
int input2 = 8;
int input3 = 10; //pin 9 is sound, skip it
int input4 = 12; // pin 11 is used internally by the tone() function, skip it

int sound = 9; //pwm, generating square sound signals

int lastSum = 0;

void setup()
{
    //Serial.begin(9600); //can't use it (uses digital0 & digital1)
    pinMode(data1,  OUTPUT);
    pinMode(clock1, OUTPUT);
    pinMode(latch1, OUTPUT);
    
    pinMode(data2,  OUTPUT);
    pinMode(clock2, OUTPUT);
    pinMode(latch2, OUTPUT);

    pinMode(input1, INPUT);
    pinMode(input2, INPUT);
    pinMode(input3, INPUT);
    pinMode(input4, INPUT);

    pinMode(sound, OUTPUT);
}

void loop(){
    int delayTime = 100;
    
    int in1 = digitalRead(input1);
    int in2 = digitalRead(input2);
    int in3 = digitalRead(input3);
    int in4 = digitalRead(input4);
    
    int inputSum = in1 + 2*in2 + 4*in3 + 8*in4;
    
    //change lights
    updateLights(in1, in2, in3, in4);
    if(lastSum != inputSum){
        updateTone(inputSum);
        lastSum = inputSum;
    }
    
    delay(delayTime);
}

void updateLights(int in1, int in2, int in3, int in4){

    //pick a color, depending on the triggers' states
    //TODO: something more creative?
    const boolean* color1 = in1 ? CYAN   : MAGENTA;
    const boolean* color2 = in2 ? YELLOW : RED;
    const boolean* color3 = in3 ? GREEN  : MAGENTA;
    const boolean* color4 = in4 ? BLUE   : YELLOW;
    
    //compute serial data
    int colorSum1 = toColorCode(1, color1) + toColorCode(2, color2);
    int colorSum2 = toColorCode(1, color3) + toColorCode(2, color4);
    
    //shift serial data
    reg1ShiftValue(colorSum1); //led 1 and 2
    reg2ShiftValue(colorSum2); //led 3 and 4
    
}

void updateTone(int inputSum){
    int note = notes[inputSum];
    noTone(sound);
    if(inputSum != 0){ //0 = no balloon pressed = no sound
        tone(sound, note);
    }
}

int toColorCode(int led, const boolean* color){
    switch(led){
        case 1:
            return 1*color[0] + 2*color[1] + 4*color[2];
        case 2:
            return 8*color[0] + 16*color[1] + 32*color[2];
        default:
            return 0;
    }
}

void reg1ShiftValue(int value){
    digitalWrite(latch1, LOW);     //Pulls the chips latch low
    shiftOut(data1, clock1, MSBFIRST, value); //Shifts out the 8 bits to the shift register
    digitalWrite(latch1, HIGH);
}
void reg2ShiftValue(int value){
    digitalWrite(latch2, LOW);     //Pulls the chips latch low
    shiftOut(data2, clock2, MSBFIRST, value); //Shifts out the 8 bits to the shift register
    digitalWrite(latch2, HIGH);
}
