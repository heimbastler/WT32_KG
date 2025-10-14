
//Date: 02.Juni 2017

//###################################################
//#    defines
//###################################################

//Toutorial MPR121 Chipset
//https://learn.sparkfun.com/tutorials/mpr121-hookup-guide/communicating-with-the-breakout-board
//datasheet MPR121:
//http://www.freescale.com/files/sensors/doc/data_sheet/MPR121.pdf
#include <mpr121.h>
//custom defined Constants
//value from 0 to 255 - no reset after editing required
//TOU value < REL value
#define TOUCH1     6
#define RELEASE1   4
//  defines I2C Adress for each Board/MPR121
 char SensorAdresse1 = 0x5A;  //Kabel T13 - Tastenfeld Tür Garten EG
 char SensorAdresse2 = 0x5C;  //Kabel T13 - Tastenfeld Säule Garten EG
 char SensorAdresse3 = 0x5D;  //Kabel T13 - Tastenfeld Säule Strasse EG
//  defines IRQ for each Board/MPR121
 const int irqpin1 = 4;      // irqpin1 is connected to Arduino Pin 4
 const int irqpin2 = 5;      // irqpin2 is connected to Arduino Pin 5
 const int irqpin3 = 6;      // irqpin3 is connected to Arduino Pin 6
//  Variables to remeber each TouchState of each Board 
 boolean touchStates1[12];   //  to keep track of the previous touch states
 boolean touchStates2[12];   //  to keep track of the previous touch states
 boolean touchStates3[12];   //  to keep track of the previous touch states


//  include of standard I2C/Wire library
#include <Wire.h>
//SDL an PIN 16?
//SCL ab PIN 17?

//  include of Serial library to comunicate with ECMD
#include <SerialCommand.h>
SerialCommand sCmd;     // The demo SerialCommand object


//  defines for IR-switch in Kitchen                      // --> KABEL EG11
                                                          // --> br/ws = +5V
                                                          // --> br = GND
const byte irswitchkitchenleft = 18;                      // --> grn
const byte irswitchkitchenright = 19;                     // --> grn/ws
volatile byte stateR08 = LOW;                             // Variable für aktuellen Wert des Taster


//  defines for Kreuzschalter 
const int kreuz_EG1 = 51;          // Digital IN 51 EG10   - unterer Schalter Treppe OG + GND
const int kreuz_EG2 = 52;          // Digital IN 52 EG1 - oberer Schalter Eingang EG + GND
const int kreuz_EG3 = 53;          // Digital IN 53 - GND - nicht im Einsatz
int kreuzstate_EG = 1;             // set OFF after boot
 
const int kreuz_KG1 = 8;           // Digital IN 47 KG1 - Schalter Tür zum Schlazimmer/gelb + GND/blau?
const int kreuz_KG2 = 9;           // Digital IN 48 KG1 - oberer Schalter Bad KG/weiss + GND/schwarz?
const int kreuz_KG3 = 10;          // Digital IN 49 EG1 - Schalter an Treppe zum KG im EG/braun + GND/braunweiss
int kreuzstate_KG = 1;             // set OFF after boot
//boolean kreuzstate_KG = HIGH;    // GEHT NICHT

 
//  defines for Relais
const int R00 = 30;       // the number of Arduino Output Pin for marked Relais -> EG Fensterrollo hoch
const int R01 = 31;       // the number of Arduino Output Pin for marked Relais -> EG Fensterrollo runter
const int R02 = 32;       // the number of Arduino Output Pin for marked Relais -> EG Türrollo hoch
const int R03 = 33;       // the number of Arduino Output Pin for marked Relais -> EG Türrollo runter
const int R04 = 34;       // the number of Arduino Output Pin for marked Relais -> EG AussenlampeGarten
const int R05 = 35;       // the number of Arduino Output Pin for marked Relais -> EG Steinlampe
const int R06 = 36;       // the number of Arduino Output Pin for marked Relais -> Dummy
const int R07 = 37;       // the number of Arduino Output Pin for marked Relais -> KG Flurlampe

const int R08 = 22;       // the number of Arduino Output Pin for marked Relais -> Küchenarbeitslampe
const int R09 = 23;       // the number of Arduino Output Pin for marked Relais -> Küchenlampe
const int R10 = 24;       // the number of Arduino Output Pin for marked Relais -> EG Flurlampe
const int R11 = 25;       // the number of Arduino Output Pin for marked Relais -> Traegerlampen
const int R12 = 26;       // the number of Arduino Output Pin for marked Relais -> Wohnzimmer-Lampe-br-034
const int R13 = 27;       // the number of Arduino Output Pin for marked Relais -> Wohnzimmer-Lampe-schw-033
const int R14 = 28;       // the number of Arduino Output Pin for marked Relais -> EG AussenlampeStasse
const int R15 = 29;       // the number of Arduino Output Pin for marked Relais -> EG Klingel-Trafo

//  Variables to remember State of each Relay / Output-Pin
boolean stateR00 = LOW;  // Varibale Name is same as Relay-Name / set LOW = FALSE = OFF
boolean stateR01 = LOW;  // Varibale Name is same as Relay-Name / set LOW = FALSE = OFF
boolean stateR02 = LOW;  // Varibale Name is same as Relay-Name / set LOW = FALSE = OFF
boolean stateR03 = LOW;  // Varibale Name is same as Relay-Name / set LOW = FALSE = OFF
boolean stateR04 = LOW;  // Varibale Name is same as Relay-Name / set LOW = FALSE = OFF
boolean stateR05 = LOW;  // Varibale Name is same as Relay-Name / set LOW = FALSE = OFF
boolean stateR06 = LOW;  // Varibale Name is same as Relay-Name / set LOW = FALSE = OFF
boolean stateR07 = LOW;  // Varibale Name is same as Relay-Name / set LOW = FALSE = OFF
 
//boolean stateR08 = LOW;  // Varibale Name is same as Relay-Name / set LOW = FALSE = OFF    look to interrupt defines
boolean stateR09 = LOW;  // Varibale Name is same as Relay-Name / set LOW = FALSE = OFF
boolean stateR10 = LOW;  // Varibale Name is same as Relay-Name / set LOW = FALSE = OFF
boolean stateR11 = LOW;  // Varibale Name is same as Relay-Name / set LOW = FALSE = OFF
boolean stateR12 = LOW;  // Varibale Name is same as Relay-Name / set LOW = FALSE = OFF
boolean stateR13 = LOW;  // Varibale Name is same as Relay-Name / set LOW = FALSE = OFF
boolean stateR14 = LOW;  // Varibale Name is same as Relay-Name / set LOW = FALSE = OFF
boolean stateR15 = LOW;  // Varibale Name is same as Relay-Name / set LOW = FALSE = OFF
 
 
//In this first section of the code, the MPR121 library and the Wire library are initialized. The Wire library makes I2C communication easy to use on the Arduino.
//The sketch also defines digital pin 2 as the IRQ pin connection, and creates 12 instances of the boolean variable touchStates.
//For the second section of the code, we define the irqpin as an INPUT, telling the Arduino to monitor the digital signal coming in over that pin
//Serial communication is also started at 9600 bps, s well as the Wire and mpr121 libraries.

//#################################################################################
//       setup 
//#################################################################################

    void setup(){
        //set all state variables to false/0
        for (int i=0; i<12; i++)
          touchStates1[i] = touchStates2[i] = touchStates3[i] = false;
        Serial.begin(9600);         // needed if the Touch State will be displayed via USB/Serial Interface
        Wire.begin();               // start I2C-Sketch
        pinMode(R00, OUTPUT);       // sets the digital pin as output
        digitalWrite(R00, HIGH);    // Prevents relays from starting up engaged
        pinMode(R01, OUTPUT);       // sets the digital pin as output
        digitalWrite(R01, HIGH);    // Prevents relays from starting up engaged
        pinMode(R02, OUTPUT);       // sets the digital pin as output
        digitalWrite(R02, HIGH);    // Prevents relays from starting up engaged
        pinMode(R03, OUTPUT);       // sets the digital pin as output
        digitalWrite(R03, HIGH);    // Prevents relays from starting up engaged
        pinMode(R04, OUTPUT);       // sets the digital pin as output
        digitalWrite(R04, HIGH);    // Prevents relays from starting up engaged
        pinMode(R05, OUTPUT);       // sets the digital pin as output
        digitalWrite(R05, HIGH);    // Prevents relays from starting up engaged
        pinMode(R06, OUTPUT);       // sets the digital pin as output
        digitalWrite(R06, HIGH);    // Prevents relays from starting up engaged
        pinMode(R07, OUTPUT);       // sets the digital pin as output
        digitalWrite(R07, HIGH);    // Prevents relays from starting up engaged
        pinMode(R08, OUTPUT);       // sets the digital pin as output
        digitalWrite(R08, HIGH);    // Prevents relays from starting up engaged
        pinMode(R09, OUTPUT);       // sets the digital pin as output
        digitalWrite(R09, HIGH);    // Prevents relays from starting up engaged
        pinMode(R10, OUTPUT);       // sets the digital pin as output
        digitalWrite(R10, HIGH);    // Prevents relays from starting up engaged
        pinMode(R11, OUTPUT);       // sets the digital pin as output
        digitalWrite(R11, HIGH);    // Prevents relays from starting up engaged
        pinMode(R12, OUTPUT);       // sets the digital pin as output
        digitalWrite(R12, HIGH);    // Prevents relays from starting up engaged
        pinMode(R13, OUTPUT);       // sets the digital pin as output
        digitalWrite(R13, HIGH);    // Prevents relays from starting up engaged
        pinMode(R14, OUTPUT);       // sets the digital pin as output
        digitalWrite(R14, HIGH);    // Prevents relays from starting up engaged
        pinMode(R15, OUTPUT);       // sets the digital pin as output
        digitalWrite(R15, HIGH);    // Prevents relays from starting up engaged
        
        //defines inputs for Kreuzschaltung for EG floor-lamp
        pinMode(kreuz_EG1, INPUT_PULLUP);
        digitalWrite(kreuz_EG1, HIGH); //enable pullup resistor
        pinMode(kreuz_EG2, INPUT_PULLUP);
        digitalWrite(kreuz_EG2, HIGH); //enable pullup resistor
        pinMode(kreuz_EG3, INPUT_PULLUP);
        digitalWrite(kreuz_EG3, HIGH); //enable pullup resistor
        
        //defines inputs for Kreuzschaltung for KG floor-lamp
        pinMode(kreuz_KG1, INPUT_PULLUP);
        digitalWrite(kreuz_KG1, HIGH); //enable pullup resistor
        pinMode(kreuz_KG2, INPUT_PULLUP);
        digitalWrite(kreuz_KG2, HIGH); //enable pullup resistor
        pinMode(kreuz_KG3, INPUT_PULLUP);
        digitalWrite(kreuz_KG3, HIGH); //enable pullup resistor
        
        //defines for IR Switch Kitchen
        pinMode(irswitchkitchenleft, INPUT);    //INPUT for Pin 18
        pinMode(irswitchkitchenright, INPUT);    //INPUT for Pin 19
        attachInterrupt(digitalPinToInterrupt(irswitchkitchenleft), Kuechenarbeitslampen_swleft_ISR, RISING); //Pin 19 is interruptpin 4
        attachInterrupt(digitalPinToInterrupt(irswitchkitchenright), Kuechenarbeitslampen_swright_ISR, RISING); //Pin 19 is interruptpin 4
        
        //setup for MPR121
        pinMode(irqpin1, INPUT);
        pinMode(irqpin2, INPUT);
        pinMode(irqpin3, INPUT);
        digitalWrite(irqpin1, HIGH); //enable pullup resistor
        digitalWrite(irqpin2, HIGH); //enable pullup resistor
        digitalWrite(irqpin3, HIGH); //enable pullup resistor
        // start setup for MPR121 Chipset
        mpr121_setup();
        
        // Setup callbacks for SerialCommand commands
        sCmd.addCommand("1R00",Fensterrollo_up_push);
        sCmd.addCommand("1R01",Fensterrollo_down_push);
        sCmd.addCommand("1R02",Tuerrollo_up_push);
        sCmd.addCommand("1R03",Tuerrollo_down_push);
        sCmd.addCommand("1R04",AussenlampeGarten_on);
        sCmd.addCommand("0R04",AussenlampeGarten_off);
        sCmd.addCommand("1R05",Steinlampen_on);
        sCmd.addCommand("0R05",Steinlampen_off);
        sCmd.addCommand("1R06",R06_on);
        sCmd.addCommand("0R06",R06_off);
        sCmd.addCommand("1R07",KGFlurlampe_on);
        sCmd.addCommand("0R07",KGFlurlampe_off);
        sCmd.addCommand("1R08",Kuechenarbeitslampen_on);
        sCmd.addCommand("0R08",Kuechenarbeitslampen_off);
        sCmd.addCommand("1R09",R09_on);
        sCmd.addCommand("0R09",R09_off);
        sCmd.addCommand("1R10",EGFlurlampe_on);
        sCmd.addCommand("0R10",EGFlurlampe_off);
        sCmd.addCommand("1R11",Traegerlampen_on);
        sCmd.addCommand("0R11",Traegerlampen_off);
        sCmd.addCommand("0R12",Wohnzimmerlampe1_off);
        sCmd.addCommand("1R12",Wohnzimmerlampe1_on);
        sCmd.addCommand("0R13",Wohnzimmerlampe2_off);
        sCmd.addCommand("1R13",Wohnzimmerlampe2_on);
        sCmd.addCommand("1R14",AussenlampeStrasse_on);
        sCmd.addCommand("0R14",AussenlampeStrasse_off);
        sCmd.addCommand("1R15",Klingeltrafo_on);
        sCmd.addCommand("0R15",Klingeltrafo_off);
        sCmd.addCommand("VERSION", Send_Version); // Returns current version to FHEM
        sCmd.setDefaultHandler(unrecognized);      // Handler for command that isn't matched  (says "What?")        
        
        //Serial.println("ready");  // needed if the Touch State will be displayed via USB/Serial Interface
    }

//#################################################################################
//       loop 
//#################################################################################
    
    void loop(){

        readTouchInputs1();  //go into function to check Board1
        readTouchInputs2();  //go into function to check Board2
        readTouchInputs3();  //go into function to check Board3
        kreuzSchalter_EG();  //go into function to check Kreuschaltung-EG
        kreuzSchalter_KG();  //go into function to check Kreuschaltung-KG
        sCmd.readSerial();   // We don't do much, just process and listen to serial commands
        delay (100);         //deay to prevent trouble with incorrect switch data when Door-Ring is active
    }

//#################################################################################
//Function for Send_Version of Arduino
//#################################################################################
void Send_Version() {
  Serial.print("VER 28.08.63\n");
}

void unrecognized(const char *command) {
  Serial.println("What?");
}

//#################################################################################
//Function for Fensterrollo
//Kabel ??? auf Relais R00 + R01
//#################################################################################
void Fensterrollo_up_push(){
  if (digitalRead(R00)){
    digitalWrite(R01, HIGH);  //to be save that down is deaktivated
    digitalWrite(R00, LOW);
    Serial.println("1R00");     //send "1R00" to fhem
  }
  else{
    digitalWrite(R00, HIGH);
    Serial.println("0R00");     //send "0R00" to fhem
}
//void Fensterrollo_up_release(){  //this is only used if button should work only if it is pressed
//  digitalWrite(R00, HIGH);
//Serial.println("0R00");
}
void Fensterrollo_down_push(){
  if (digitalRead(R01)){
    digitalWrite(R00, HIGH);  //to be save that up is deaktivated
    digitalWrite(R01, LOW);
    Serial.println("1R01");     //send "1R01" to fhem
  }
  else{
    digitalWrite(R01, HIGH);
    Serial.println("0R01");     //send "0R01" to fhem
}
//void Fensterrollo_down_release(){  //this is only used if button should work only if it is pressed
//  digitalWrite(R01, HIGH);
//Serial.println("0R01");
}

//#################################################################################
//Function for Türrollo
//Kabel ??? auf Relai R02 + R03
//#################################################################################
void Tuerrollo_up_push(){
  if (digitalRead(R02)){
    digitalWrite(R03, HIGH);  //to be save that down is deaktivated
    digitalWrite(R02, LOW);
    Serial.println("1R02");     //send "1R02" to fhem
  }
  else{
    digitalWrite(R02, HIGH);
    Serial.println("0R02");     //send "0R02" to fhem
}
//void Tuerrollo_up_release(){  //this is only used if button should work only if it is pressed
//  digitalWrite(R02, HIGH);
//Serial.println("0R02");
}
void Tuerrollo_down_push(){
  if (digitalRead(R03)){
    digitalWrite(R02, HIGH);  //to be save that down is deaktivated
    digitalWrite(R03, LOW);
    Serial.println("1R03");     //send "1R03" to fhem
  }
  else{
    digitalWrite(R03, HIGH);
    Serial.println("0R03");     //send "0R03" to fhem
}
//void Tuerrollo_down_release(){  //this is only used if button should work only if it is pressed
//  digitalWrite(R03, HIGH);
//Serial.println("0R03");
}

//#################################################################################
//Function for AussenlampeGarten
//auf Relais R04
//#################################################################################
void AussenlampeGarten(){
  if (digitalRead(R04)){
    digitalWrite(R04, LOW);
    Serial.println("1R04");   //send "1R04" to fhem
    }
  else{
    digitalWrite(R04, HIGH);
    Serial.println("0R04");   //send "0R04" to fhem
    }
}

//Function for AussenlampeGarten via ECMD
void AussenlampeGarten_on(){
    digitalWrite(R04, LOW);
    Serial.println("1R04");
    }
void AussenlampeGarten_off(){
    digitalWrite(R04, HIGH);
    Serial.println("0R04");
    }

/*
//#################################################################################
//Function for Steinlampen
//Kabel ??? auf Relais R05
//#################################################################################
void Steinlampen(){
  if (digitalRead(R05)){
    digitalWrite(R05, LOW);
    Serial.println("1R05");
    {
  else{
    digitalWrite(R05, HIGH);
    Serial.println("0R05");
    {
}
*/
//Function for Steinlampen via ECMD
void Steinlampen_on(){
    digitalWrite(R05, LOW);
    Serial.println("1R05");
    }
void Steinlampen_off(){
    digitalWrite(R05, HIGH);
    Serial.println("0R05");
    }

/*
//#################################################################################
//Function for Dummy
//Kabel ??? auf Relais R06
//#################################################################################
void Dummy(){
  if (digitalRead(R06)){
    digitalWrite(R06, LOW);
    Serial.println("1R06");
    {
  else{
    digitalWrite(R06, HIGH);
    Serial.println("0R06");
    {
}
*/
//Function for Dummy via ECMD
void R06_on(){
    digitalWrite(R06, LOW);
    Serial.println("1R06");
    }
void R06_off(){
    digitalWrite(R06, HIGH);
    Serial.println("0R06");
    }

//#################################################################################
//Function for KGFlurlampe via ECMD
//auf Relais R07
//#################################################################################
void KGFlurlampe_on(){
    digitalWrite(R07, LOW);
    Serial.println("1R07");
    }
void KGFlurlampe_off(){
    digitalWrite(R07, HIGH);
    Serial.println("0R07");
    }

//#################################################################################
//Function for Kuechenarbeitslampen
//Kabel 024 auf R8
//#################################################################################
/*void Kuechenarbeitslampen(){
  stateR08 = digitalRead(R08);
  //delay (100);
  // toggle variable
  stateR08 = !stateR08;
  // do switch
  digitalWrite(R08, stateR08);
  //delay (500);
}
*/
void Kuechenarbeitslampen(){
  if (digitalRead(R08)){
    digitalWrite(R08, LOW);
    Serial.println("1R08");
    }
  else{
    digitalWrite(R08, HIGH);
    Serial.println("0R08");
    }
}

//Function for Kuechenarbeitslampen via ECMD
void Kuechenarbeitslampen_on(){
  digitalWrite(R08, LOW);
  Serial.println("1R08");
}

void Kuechenarbeitslampen_off(){
  digitalWrite(R08, HIGH);
  Serial.println("0R08");
}

//#################################################################################
//Function for Dummy
//Kabel ??? auf Relais R09
//#################################################################################
void Dummy(){
  if (digitalRead(R09)){
    digitalWrite(R09, LOW);
    Serial.println("1R09");
    {
  else{
    digitalWrite(R09, HIGH);
    Serial.println("0R09");
    {
}
*/
//Function for Dummy via ECMD
void R09_on(){
    digitalWrite(R09, LOW);
    Serial.println("1R09");
    }
void R09_off(){
    digitalWrite(R09, HIGH);
    Serial.println("0R09");
    }

//Function for Kuechenarbeitslampen via IR Sensor
void Kuechenarbeitslampen_swleft_ISR(){
  byte state_tiefpass = 0;
  byte i = 0;
  for (i = 0; i < 20; i++) {
    state_tiefpass  += digitalRead(irswitchkitchenleft);
    delay(5);
  }
  if (state_tiefpass > 10) {
    if (stateR08) {
        stateR08 = 0;
        digitalWrite(R08, LOW);
        Serial.println("1R08");
    }
    else
    {
        stateR08 = 1;
        digitalWrite(R08, HIGH);
        Serial.println("0R08");
    }
  }
}

void Kuechenarbeitslampen_swright_ISR(){
  byte state_tiefpass = 0;
  byte i = 0;
  for (i = 0; i < 20; i++) {
    state_tiefpass  += digitalRead(irswitchkitchenright);
    delay(5);
  }
  if (state_tiefpass > 10) {
    if (stateR08) {
        stateR08 = 0;
        digitalWrite(R08, LOW);
        Serial.println("1R08");
    }
    else
    {
        stateR08 = 1;
        digitalWrite(R08, HIGH);
        Serial.println("0R08");
    }
  }
}

//#################################################################################
//Function for Kuechenlampe
//Kabel 011 auf R09
//#################################################################################
void Kuechenlampe(){
  if (digitalRead(R09)){
    digitalWrite(R09, LOW);
    Serial.println("1R09");
    }
  else{
    digitalWrite(R09, HIGH);
    Serial.println("0R09");
    }
}

//Function for Kuechenlampe via ECMD
void Kuechenlampe_on(){
    digitalWrite(R09, LOW);
    Serial.println("1R09");
    }
void Kuechenlampe_off(){
    digitalWrite(R09, HIGH);
    Serial.println("0R09");
    }

//#################################################################################
//Function for Flurlampe
//Kabel 010 auf R10
//#################################################################################
void Flurlampe(){
  if (digitalRead(R10)){
    digitalWrite(R10, LOW);
    Serial.println("1R10");
    }
  else{
    digitalWrite(R10, HIGH);
    Serial.println("0R10");
    }
}

//Function for EGFlurlampe via ECMD
void EGFlurlampe_on(){
    digitalWrite(R10, LOW);
    Serial.println("1R10");
    }
void EGFlurlampe_off(){
    digitalWrite(R10, HIGH);
    Serial.println("0R10");
    }

//#################################################################################
//Function for Traegerlampen
//Kabel 009 auf R11
//#################################################################################
void Traegerlampen(){
  if (digitalRead(R11)){
    digitalWrite(R11, LOW);
    Serial.println("1R11");
    }
  else{
    digitalWrite(R11, HIGH);
    Serial.println("0R11");
    }
}

//Function for Traegerlampen via ECMD
void Traegerlampen_on(){
  digitalWrite(R11, LOW);
  Serial.println("1R11");
  }
void Traegerlampen_off(){
  digitalWrite(R11, HIGH);
  Serial.println("0R11");
  }

//#################################################################################
//Function for Wohnzimmerlampe
//Kabel 034 auf R12
//Kabel 033 auf R13
//#################################################################################
void Wohnzimmerlampe(){
  if (digitalRead(R12)){
    digitalWrite(R12, LOW);
    Serial.println("1R12");
    }
  else{
    digitalWrite(R12, HIGH);
    Serial.println("0R12");
    }
  if (digitalRead(R13)){
    digitalWrite(R13, LOW);
    Serial.println("1R13");
    }
  else{
    digitalWrite(R13, HIGH);
    Serial.println("0R13");
    }
  }

/*  ALTE ANSTEUERUNG UM 2 PHASEN GETRENNT NACHEINANDER ZU SCHALTEN
  void Wohnzimmerlampe(){
wztouchCounter++;  // if the state has changed, increment the counter
// turns on the 2of5 Lamps every 1st touch pushes by checking the modulo of the touch push counter
if (wztouchCounter % 1 == 0){
  digitalWrite(R12, HIGH);  //switch Relay12 on
  digitalWrite(R13, LOW);  //switch Relay12 off / too much action here?
  }
// turns on the 3of5 Lamps every 2nd touch pushes 
if (wztouchCounter % 2 == 0){
    digitalWrite(R12, LOW);
    digitalWrite(R13, HIGH);
    }
// turns on the 2+3of5 Lamps every 3rd touch pushes 
if (wztouchCounter % 3 == 0){
    digitalWrite(R12, LOW);
    digitalWrite(R13, LOW);
    }
// turns off the 2+3of5 Lamps every 4th touch pushes 
if (wztouchCounter % 4 == 0){
    digitalWrite(R12, HIGH);
    digitalWrite(R13, HIGH);
    wztouchCounter = 0;  //reset the counter back to <0>
    }
}
*/

//Function for Wohnzimmerlampe via ECMD
void Wohnzimmerlampe1_on(){
    digitalWrite(R12, LOW);
    Serial.println("1R12");
    }
void Wohnzimmerlampe1_off(){
    digitalWrite(R12, HIGH);
    Serial.println("0R12");
    }
void Wohnzimmerlampe2_on(){
    digitalWrite(R13, LOW);
    Serial.println("1R13");
    }
void Wohnzimmerlampe2_off(){
    digitalWrite(R13, HIGH);
    Serial.println("0R13");
    }

//#################################################################################
//Function for AussenlampeStrasse via ECMD
//Kabel ??? auf Relais R14
//#################################################################################
void AussenlampeStrasse_on(){
    digitalWrite(R14, LOW);
    Serial.println("1R14");
    }
void AussenlampeStrasse_off(){
    digitalWrite(R14, HIGH);
    Serial.println("0R14");
    }

//#################################################################################
//Function for Klingeltrafo via ECMD
//Kabel ??? auf Relais R15
//#################################################################################
void Klingeltrafo_on(){
    digitalWrite(R15, LOW);
    Serial.println("1R15");
    }
void Klingeltrafo_off(){
    digitalWrite(R15, HIGH);
    Serial.println("0R15");
    }



//#################################################################################
//Function for ToggleLamps
//#################################################################################
void ToggleLamps(){
  //when one or some of outputs / relais R12 or R13 or R11 or R09 or R08 or R10 is LOW/relais active, went into if loop...
  if((digitalRead(R12) == LOW) || (digitalRead(R13) == LOW) || (digitalRead(R11) == LOW) || (digitalRead(R09) == LOW) || (digitalRead(R08) == LOW) || (digitalRead(R10) == LOW))
      {   
      stateR12 = digitalRead(R12);  //write state of R12 into stateR-Variable
      stateR13 = digitalRead(R13);
      stateR11 = digitalRead(R11);
      stateR10 = digitalRead(R10);
      stateR09 = digitalRead(R09);
      stateR08 = digitalRead(R08);
      stateR05 = digitalRead(R05);
      
       digitalWrite(R05, HIGH);     //switch relais off
       Serial.println("0R05");
       digitalWrite(R08, HIGH);
       Serial.println("0R08");
       digitalWrite(R09, HIGH);
       Serial.println("0R09");
       digitalWrite(R10, HIGH);
       Serial.println("0R10");
       digitalWrite(R11, HIGH);
       Serial.println("0R11");
       digitalWrite(R12, HIGH);
       Serial.println("0R12");
       digitalWrite(R13, HIGH);
       Serial.println("0R13");
       Serial.println("0S");        //send "0S" to fhem to switch off Schranklicht
   }
   else
   {
      digitalWrite(R12, stateR12);  //set relais to previous state of R12 via stateMCOP-Variable
      digitalWrite(R13, stateR13);
      digitalWrite(R11, stateR11);
      digitalWrite(R09, stateR09);
      digitalWrite(R08, stateR08);
      digitalWrite(R10, stateR10);
      digitalWrite(R05, stateR05);
    }
  }


//#################################################################################
//Function for readTouchInputs1
//Tastenfeld Tür Garten EG
//#################################################################################
void readTouchInputs1(){
  if(!digitalRead(irqpin1)){
    //read the touch state from the MPR121
    Wire.requestFrom(SensorAdresse1,2); 
    byte LSB = Wire.read();
    byte MSB = Wire.read();
    uint16_t touched = ((MSB << 8) | LSB); //16bits that make up the touch states
    for (int i=0; i < 12; i++){  // Check what electrodes were pressed
      if(touched & (1<<i)){
        if(touchStates1[i] == 0){
          //pin i was just touched
          //Serial.print("1:pin ");
          //Serial.print(i);
          //Serial.println(" was just touched");
          switch (i){
            case 2:  //oben 1te von links
              Wohnzimmerlampe();
              break;
            case 5:  //oben 2te von links
              Traegerlampen();
              break;
            case 1:  //unten 1te von links
              Flurlampe();
             break; 
            case 0:  //unten 2te von links
              AussenlampeGarten();
              break;
            //Taster für Fensterrollo up
            //Relais R00 schaltet
            case 9:  //Taster für Fensterrollo up
              Fensterrollo_up_push();
              break;
            case 10:  //Taster für Fensnterrollo down
              Fensterrollo_down_push();
              break;
            case 6:  //push botton for Türrollo up
              Tuerrollo_up_push();
              break;
            case 11:  //push botton for Türrollo down
              Tuerrollo_down_push();
              break;
          }
        }else if(touchStates1[i] == 1){
          //pin i is still being touched
        }  
        touchStates1[i] = 1;      
      }else{
        if(touchStates1[i] == 1){
          //Serial.print("1:pin ");
          //Serial.print(i);
          //Serial.println(" is no longer being touched");
          switch (i){
            
            //release relais Fensterrollo to simulate a push botton
            case 9:           
            case 10:
            //digitalWrite(R00, HIGH);
            //digitalWrite(R01, HIGH);
            break;
            //release relais Türrollo to simulate a push botton
            case 6:           
            case 11:
            //digitalWrite(R02, HIGH);
            //digitalWrite(R03, HIGH);
            break;
          }
       }
        touchStates1[i] = 0;
      }
    }
  }
}

//#################################################################################
//Function for readTouchInputs2
//Tastenfeld Säule Garten EG
//#################################################################################
void readTouchInputs2(){
  if(!digitalRead(irqpin2)){
    //read the touch state from the MPR121
    Wire.requestFrom(SensorAdresse2,2); 
    byte LSB = Wire.read();
    byte MSB = Wire.read();
    uint16_t touched = ((MSB << 8) | LSB); //16bits that make up the touch states
    for (int i=0; i < 12; i++){  // Check what electrodes were pressed
      if(touched & (1<<i)){
        if(touchStates2[i] == 0){
          //pin i was just touched
          Serial.print("2:pin ");
          Serial.print(i);
          Serial.println(" was just touched");
          switch (i){
            case 9:  //oben links 
            ToggleLamps();
            break;  
            case 11:  //oben rechts
            ToggleLamps();
            break;
            case 10:  //2te rechts
            Wohnzimmerlampe();
            break;
            case 1:  //unten rechts
            Traegerlampen();
            break;
            case 2:  //3te links
            Kuechenlampe();
            break;
            case 3:  //unten links
            Kuechenarbeitslampen();
            break;
            case 0:  //3te rechts
            Flurlampe();
            break;
            case 6:  //2te links
            //Steinlampen();
            break;
          }  
        }else if(touchStates2[i] == 1){
          //pin i is still being touched
          }  
        touchStates2[i] = 1;      
      }else{
        if(touchStates2[i] == 1){
          //Serial.print("2:pin ");
          //Serial.print(i);
          //Serial.println(" is no longer being touched");
         }
        touchStates2[i] = 0;
      }
    }
  }
}

//#################################################################################
//Function for readTouchInputs3
//Tastenfeld Säule Strasse EG
//#################################################################################
void readTouchInputs3(){
  if(!digitalRead(irqpin3)){
    //read the touch state from the MPR121
    Wire.requestFrom(SensorAdresse3,2); 
    byte LSB = Wire.read();
    byte MSB = Wire.read();
    uint16_t touched = ((MSB << 8) | LSB); //16bits that make up the touch states
    for (int i=0; i < 12; i++){  // Check what electrodes were pressed
      if(touched & (1<<i)){
        if(touchStates3[i] == 0){
          //pin i was just touched
          Serial.print("3:pin ");
          Serial.print(i);
          Serial.println(" was just touched");
          switch (i){
            case 9:
            ToggleLamps();
            break;
            case 10:
            ToggleLamps();
            break;
            case 11:  //2te rechts
            Wohnzimmerlampe();
            break;
            case 1:  //unten rechts
            Traegerlampen();
            break;
            case 3:  //3te links
            Kuechenlampe();
            break;
            case 2:  //unten links
            Kuechenarbeitslampen();
            break;
            case 0:  //3te rechts
            Flurlampe();
            break;
            case 6:  //2te links
            //Steinlampen();
            break;
           }
        }else if(touchStates3[i] == 1){
          //pin i is still being touched
        }  
        touchStates3[i] = 1;      
      }else{
        if(touchStates3[i] == 1){
          //Serial.print("3:pin ");
          //Serial.print(i);
          //Serial.println(" is no longer being touched");
         }
        touchStates3[i] = 0;
      }
    }
  }
}

//#################################################################################
//Function for "KreuzSchaltung EG" 
//#################################################################################
void kreuzSchalter_EG(){
  // write state of Kreuzschalter into Variable. State is AND-combined of each Input
  int stateEG = digitalRead (kreuz_EG1) | (digitalRead (kreuz_EG2)<<1) | (digitalRead (kreuz_EG3)<<2);
  // if stateEG is different than Kreuzstate went into loop...
  if (kreuzstate_EG != stateEG){
    // toggle Relais on and off
    if (digitalRead(R10))
      digitalWrite(R10, LOW);
    else
      digitalWrite(R10, HIGH);
  }
  //save actual stateEG into Kreuzstate
  kreuzstate_EG = stateEG;
}

//#################################################################################
//Function for "KreuzSchaltung KG" 
//#################################################################################
void kreuzSchalter_KG(){
  // write state of Kreuzschalter into Variable. State is AND-combined of each Input
  int stateKG = digitalRead (kreuz_KG1) | (digitalRead (kreuz_KG2)<<1) | (digitalRead (kreuz_KG3)<<2);
  // if stateKG is different than Kreuzstate went into loop...
  if (kreuzstate_KG != stateKG){
    // toggle Relais on and off
    if (digitalRead(R07))
      digitalWrite(R07, LOW);
    else
      digitalWrite(R07, HIGH);
  }
  //save actual stateKG into Kreuzstate
  kreuzstate_KG = stateKG;
}

//The Arduino scans each electrode and prints out a message over serial if an electrode is triggered as being touched. The Arduino will then print out a message as soon as the electrode is no longer being touched.
//The last major section of the code defines the threshold values for each electrode. Each electrode must have a touch threshold and a release threshold for the Arduino to compare the current state of the electrode.   
   
void mpr121_setup(void){
//Board1
  
  set_register(SensorAdresse1, ELE_CFG, 0x00); 

  // Section A - Controls filtering when data is > baseline.
  set_register(SensorAdresse1, MHD_R, 0x01);
  set_register(SensorAdresse1, NHD_R, 0x01);
  set_register(SensorAdresse1, NCL_R, 0x00);
  set_register(SensorAdresse1, FDL_R, 0x00);

  // Section B - Controls filtering when data is < baseline.
  set_register(SensorAdresse1, MHD_F, 0x01);
  set_register(SensorAdresse1, NHD_F, 0x01);
  set_register(SensorAdresse1, NCL_F, 0xFF);
  set_register(SensorAdresse1, FDL_F, 0x02);

  // Section C - Sets touch and release thresholds for each electrode
  set_register(SensorAdresse1, ELE0_T, TOUCH1);
  set_register(SensorAdresse1, ELE0_R, RELEASE1);

  set_register(SensorAdresse1, ELE1_T, TOUCH1);
  set_register(SensorAdresse1, ELE1_R, RELEASE1);

  set_register(SensorAdresse1, ELE2_T, TOUCH1);
  set_register(SensorAdresse1, ELE2_R, RELEASE1);

  set_register(SensorAdresse1, ELE3_T, TOUCH1);
  set_register(SensorAdresse1, ELE3_R, RELEASE1);

  set_register(SensorAdresse1, ELE4_T, TOUCH1);
  set_register(SensorAdresse1, ELE4_R, RELEASE1);

  set_register(SensorAdresse1, ELE5_T, TOUCH1);
  set_register(SensorAdresse1, ELE5_R, RELEASE1);

  set_register(SensorAdresse1, ELE6_T, TOUCH1);
  set_register(SensorAdresse1, ELE6_R, RELEASE1);

  set_register(SensorAdresse1, ELE7_T, TOUCH1);
  set_register(SensorAdresse1, ELE7_R, RELEASE1);

  set_register(SensorAdresse1, ELE8_T, TOUCH1);
  set_register(SensorAdresse1, ELE8_R, RELEASE1);

  set_register(SensorAdresse1, ELE9_T, TOUCH1);
  set_register(SensorAdresse1, ELE9_R, RELEASE1);

  set_register(SensorAdresse1, ELE10_T, TOUCH1);
  set_register(SensorAdresse1, ELE10_R, RELEASE1);

  set_register(SensorAdresse1, ELE11_T, TOUCH1);
  set_register(SensorAdresse1, ELE11_R, RELEASE1);

  // Section D
  // Set the Filter Configuration
  // Set ESI2
  set_register(SensorAdresse1, FIL_CFG, 0x04);

  // Section E
  // Electrode Configuration
  // Set ELE_CFG to 0x00 to return to standby mode
  set_register(SensorAdresse1, ELE_CFG, 0x0C);  // Enables all 12 Electrodes

  // Section F
  // Enable Auto Config and auto Reconfig
  set_register(SensorAdresse1, ATO_CFG0, 0x0B);
  set_register(SensorAdresse1, ATO_CFGU, 0xC9);  // USL = (Vdd-0.7)/vdd*256 = 0xC9 @3.3V
  set_register(SensorAdresse1, ATO_CFGL, 0x82);  // LSL = 0.65*USL = 0x82 @3.3V
  set_register(SensorAdresse1, ATO_CFGT, 0xB5);  // Target = 0.9*USL = 0xB5 @3.3V

  set_register(SensorAdresse1, ELE_CFG, 0x0C);



//Board2

  set_register(SensorAdresse2, ELE_CFG, 0x00); 

  // Section A - Controls filtering when data is > baseline.
  set_register(SensorAdresse2, MHD_R, 0x01);
  set_register(SensorAdresse2, NHD_R, 0x01);
  set_register(SensorAdresse2, NCL_R, 0x00);
  set_register(SensorAdresse2, FDL_R, 0x00);

  // Section B - Controls filtering when data is < baseline.
  set_register(SensorAdresse2, MHD_F, 0x01);
  set_register(SensorAdresse2, NHD_F, 0x01);
  set_register(SensorAdresse2, NCL_F, 0xFF);
  set_register(SensorAdresse2, FDL_F, 0x02);

  // Section C - Sets touch and release thresholds for each electrode
  set_register(SensorAdresse2, ELE0_T, TOUCH1);
  set_register(SensorAdresse2, ELE0_R, RELEASE1);

  set_register(SensorAdresse2, ELE1_T, TOUCH1);
  set_register(SensorAdresse2, ELE1_R, RELEASE1);

  set_register(SensorAdresse2, ELE2_T, TOUCH1);
  set_register(SensorAdresse2, ELE2_R, RELEASE1);

  set_register(SensorAdresse2, ELE3_T, TOUCH1);
  set_register(SensorAdresse2, ELE3_R, RELEASE1);

  set_register(SensorAdresse2, ELE4_T, TOUCH1);
  set_register(SensorAdresse2, ELE4_R, RELEASE1);

  set_register(SensorAdresse2, ELE5_T, TOUCH1);
  set_register(SensorAdresse2, ELE5_R, RELEASE1);

  set_register(SensorAdresse2, ELE6_T, TOUCH1);
  set_register(SensorAdresse2, ELE6_R, RELEASE1);

  set_register(SensorAdresse2, ELE7_T, TOUCH1);
  set_register(SensorAdresse2, ELE7_R, RELEASE1);

  set_register(SensorAdresse2, ELE8_T, TOUCH1);
  set_register(SensorAdresse2, ELE8_R, RELEASE1);

  set_register(SensorAdresse2, ELE9_T, TOUCH1);
  set_register(SensorAdresse2, ELE9_R, RELEASE1);

  set_register(SensorAdresse2, ELE10_T, TOUCH1);
  set_register(SensorAdresse2, ELE10_R, RELEASE1);

  set_register(SensorAdresse2, ELE11_T, TOUCH1);
  set_register(SensorAdresse2, ELE11_R, RELEASE1);

  // Section D
  // Set the Filter Configuration
  // Set ESI2
  set_register(SensorAdresse2, FIL_CFG, 0x04);

  // Section E
  // Electrode Configuration
  // Set ELE_CFG to 0x00 to return to standby mode
  set_register(SensorAdresse2, ELE_CFG, 0x0C);  // Enables all 12 Electrodes

  // Section F
  // Enable Auto Config and auto Reconfig
  set_register(SensorAdresse2, ATO_CFG0, 0x0B);
  set_register(SensorAdresse2, ATO_CFGU, 0xC9);  // USL = (Vdd-0.7)/vdd*256 = 0xC9 @3.3V
  set_register(SensorAdresse2, ATO_CFGL, 0x82);  // LSL = 0.65*USL = 0x82 @3.3V
  set_register(SensorAdresse2, ATO_CFGT, 0xB5);  // Target = 0.9*USL = 0xB5 @3.3V

  set_register(SensorAdresse2, ELE_CFG, 0x0C);
  
  //Board3

  set_register(SensorAdresse3, ELE_CFG, 0x00); 

  // Section A - Controls filtering when data is > baseline.
  set_register(SensorAdresse3, MHD_R, 0x01);
  set_register(SensorAdresse3, NHD_R, 0x01);
  set_register(SensorAdresse3, NCL_R, 0x00);
  set_register(SensorAdresse3, FDL_R, 0x00);

  // Section B - Controls filtering when data is < baseline.
  set_register(SensorAdresse3, MHD_F, 0x01);
  set_register(SensorAdresse3, NHD_F, 0x01);
  set_register(SensorAdresse3, NCL_F, 0xFF);
  set_register(SensorAdresse3, FDL_F, 0x02);

  // Section C - Sets touch and release thresholds for each electrode
  set_register(SensorAdresse3, ELE0_T, TOUCH1);
  set_register(SensorAdresse3, ELE0_R, RELEASE1);

  set_register(SensorAdresse3, ELE1_T, TOUCH1);
  set_register(SensorAdresse3, ELE1_R, RELEASE1);

  set_register(SensorAdresse3, ELE2_T, TOUCH1);
  set_register(SensorAdresse3, ELE2_R, RELEASE1);

  set_register(SensorAdresse3, ELE3_T, TOUCH1);
  set_register(SensorAdresse3, ELE3_R, RELEASE1);

  set_register(SensorAdresse3, ELE4_T, TOUCH1);
  set_register(SensorAdresse3, ELE4_R, RELEASE1);

  set_register(SensorAdresse3, ELE5_T, TOUCH1);
  set_register(SensorAdresse3, ELE5_R, RELEASE1);

  set_register(SensorAdresse3, ELE6_T, TOUCH1);
  set_register(SensorAdresse3, ELE6_R, RELEASE1);

  set_register(SensorAdresse3, ELE7_T, TOUCH1);
  set_register(SensorAdresse3, ELE7_R, RELEASE1);

  set_register(SensorAdresse3, ELE8_T, TOUCH1);
  set_register(SensorAdresse3, ELE8_R, RELEASE1);

  set_register(SensorAdresse3, ELE9_T, TOUCH1);
  set_register(SensorAdresse3, ELE9_R, RELEASE1);

  set_register(SensorAdresse3, ELE10_T, TOUCH1);
  set_register(SensorAdresse3, ELE10_R, RELEASE1);

  set_register(SensorAdresse3, ELE11_T, TOUCH1);
  set_register(SensorAdresse3, ELE11_R, RELEASE1);

  // Section D
  // Set the Filter Configuration
  // Set ESI2
  set_register(SensorAdresse3, FIL_CFG, 0x04);

  // Section E
  // Electrode Configuration
  // Set ELE_CFG to 0x00 to return to standby mode
  set_register(SensorAdresse3, ELE_CFG, 0x0C);  // Enables all 12 Electrodes

  // Section F
  // Enable Auto Config and auto Reconfig
  set_register(SensorAdresse3, ATO_CFG0, 0x0B);
  set_register(SensorAdresse3, ATO_CFGU, 0xC9);  // USL = (Vdd-0.7)/vdd*256 = 0xC9 @3.3V
  set_register(SensorAdresse3, ATO_CFGL, 0x82);  // LSL = 0.65*USL = 0x82 @3.3V
  set_register(SensorAdresse3, ATO_CFGT, 0xB5);  // Target = 0.9*USL = 0xB5 @3.3V

  set_register(SensorAdresse3, ELE_CFG, 0x0C);
}


void set_register(int address, unsigned char r, unsigned char v){
    Wire.beginTransmission(address);
    Wire.write(r);
    Wire.write(v);
    Wire.endTransmission();
}

