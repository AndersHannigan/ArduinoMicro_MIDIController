
#include <MIDIUSB.h>
void noteOn(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
}
void noteOff(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
}
void controlChange(byte channel, byte control, byte value) {
  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
  MidiUSB.sendMIDI(event);
}

//Buttons------------------------------------------
  //Define NPins and what pins 
const int NButtons = 9;                         
const int buttonPin[NButtons] = {2, 3, 4, 5, 6, 7, 15 , 14, 16};         
const int cMajor[9] = {60, 62, 64, 65, 67, 69, 71, 72, 74};
  //Changing variables---------------------     
int buttonCState[NButtons] = {0};               
int buttonPState[NButtons] = {0};               
  //Debounce-------------------------------     
unsigned long lastDebounceTime[NButtons] = {0}; 
unsigned long debounceDelay = 30;               


//POTENTIOMETERS-----------------------------------
  //Define NPins and what pins-------------     
const int NPots = 5;                            
const int potPin[NPots] = {A0, A1, A2, A3, A10};                 
  //Changing variables---------------------     
int potCState[NPots] = {0};
int potPState[NPots] = {0};
int potVar = 0;
  //Smoothing -----------------------  
int midiCState [NPots] = {0};
int midiPState [NPots] = {0};
int TIMEOUT = 300; //
int varThreshold = 8; //
boolean potMoving = true;
unsigned long PTime[NPots] = {0};
unsigned long timer[NPots] = {0};
//-------------------------------------------------

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  //Serial.begin(9600);

  for (int i = 0; i < NButtons; i++) {
    pinMode(buttonPin[i], INPUT_PULLUP);
  }

  for (int i = 0; i < NPots; i++) {
    pinMode(potPin[i], INPUT);
  }
}

void loop() {
//BUTTONS--------------------------------------------

  for (int i = 0; i < NButtons; i++) {
    buttonCState[i] = digitalRead(buttonPin[i]);

    if ((millis() - lastDebounceTime[i]) > debounceDelay) {
      if (buttonPState[i] != buttonCState[i]) {
        lastDebounceTime[i] = millis();

        if (buttonCState[i] == LOW) {
          //Serial.print("Button on >>");
          //Serial.println(i);
          noteOn(0, cMajor[i], 64);
          MidiUSB.flush();
          
        }
        else {
          //Serial.print("Button off >>");
          //Serial.println(i);
          noteOff(0, cMajor[i], 64);
          MidiUSB.flush();
        }
        buttonPState[i] = buttonCState[i];
      }
    }
  }
//---------------------------------------------------

//POTS-----------------------------------------------

for (int i = 0; i<NPots; i++){
  potCState[i] = analogRead(potPin[i]);
  delay(2);
  if (i < 2){
      midiCState[i] = map(potCState[i], 0, 1025, 128, 0); //Had to flip the variables as i mounted the potentiometers in the wrong way
    } else {
           midiCState[i] = map(potCState[i], 0, 1024, 0, 128); 
      }

  potVar = abs(potCState[i] - potPState[i]);


  if (potVar > varThreshold) {
    PTime[i] = millis();
  }

  timer[i] = millis() - PTime[i];

  if (timer[i] < TIMEOUT) {
    potMoving = true;
  }
  else {
    potMoving = false;
  }

  if (potMoving == true) {
    if (midiPState[i] != midiCState[i]){
      //Serial.println(midiCState);
      //Serial.println(midiCState[i]);
      controlChange(0, 20+i, midiCState[i]);
      MidiUSB.flush();
      potPState[i] = potCState[i];
      midiPState[i] = midiCState[i];
    }
  }
    potVar = abs(potCState[i] - potPState[i]);


    

  if (potVar > varThreshold) {
    PTime[i] = millis();
  }

  timer[i] = millis() - PTime[i];

  if (timer[i] < TIMEOUT) {
    potMoving = true;
  }
  else {
    potMoving = false;
  }

  if (potMoving == true) {
    if (midiPState[i] != midiCState[i]){
      //Serial.println(midiCState);
      //Serial.println(midiCState[i]);
      controlChange(0, 20+i, midiCState[i]);
      MidiUSB.flush();
      potPState[i] = potCState[i];
      midiPState[i] = midiCState[i];
    }
  }
}
}
