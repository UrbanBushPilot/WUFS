/*
Skip to content

    All gists
    GitHub

Instantly share code, notes, and snippets.

0

    0

@marc21121980 marc21121980/Radio Code
Last active 16 hours ago
Code
Revisions 8
Radio Code
Radio Code
*/
#include "LedControl.h"  
#include "math.h"
#include "Quadrature.h"  

Quadrature quad1(22, 23);
Quadrature quad2(24, 25);        
                               // ******   LedControl Library is required to drive the MAX7219 7 segment LED displays.  *******
                                                    // led_Display_1 is the variable name of my set of displays chained
LedControl led_Display_1 = LedControl(2,3,4,8);  // together running off of pins 12,11,10.  
                                                    // Pin 12 is the DataOut 
                                                    // Pin 11 is Clock
                                                    // Pin 10 CS
                                                    // The 8 is for how many displays you have to chain.
                                                    // I have 4 but put 8 incase of expansion. It cost no memory.
                                                    // You can run any pins you want in the instruction, 
                                                    // just plug the module in correctly.
                                                    // LedControl(Data,Clock,Load,Displays)
                                                    // I used 12,10,11 so I could have a straight ribbon connection
                                                    // from the module without crossing leads.
                                                    
                                                  
int CodeIn;      // used on all serial reads from Jim's code
String Digit;    // Variable as a string to take from getChar()
int Count;       // This variable used in loops in the EQUALS() function
int Com2Active[5] = {1, 2, 1, 9, 0};   // Com2Active[5] is an array of the 5 digits that are the Com 2 Radio digits
int Com2Stby[5]   = {1, 1, 9, 1, 0};
int Nav2Active[5] = {1, 0, 9, 5, 0};
int Nav2Stby[5]   = {1, 1, 7, 6, 0};
int X1;
int X2;
int Xold1;// the first encoder "old" reading
int Xold2;// the second encoder "old" reading
int Xdif;
String avm, avmOld;
bool avmi;
int  KpinNo; 
int  Koutpin;
bool sw_avionics;
bool sw_battery;

String KoldpinStateSTR, KpinStateSTR, Kstringnewstate,Kstringoldstate;
 
 
void setup() 
{
 Kstringoldstate = "111111111111111111111111111111111111111111111111111111111111111111111";
  for (int KoutPin = 22; KoutPin <=70; KoutPin++)// Get all the pins ready for simconnect codes and "Keys"(all inputs)  
  {
    pinMode(KoutPin, INPUT_PULLUP);
    digitalWrite(KoutPin, HIGH); 
  }
    
     //The MAX72XX is in power-saving mode on startup, we have to do a wakeup call
    delay (500);
    led_Display_1.shutdown(0,false); delay (500); // ** I have 4 displays, these start them up -- I put the delay in
    led_Display_1.shutdown(1,false); delay (500); //  * so they all dont start drawing current at the same time.
    led_Display_1.shutdown(2,false); delay (500); //  * I had an issue with running on only USB power and getting a 
    led_Display_1.shutdown(3,false); delay (500); // ** display glitch. The delay seems to have fixed this issue.

    // Set the brightness to a medium value -- and clear the display
    led_Display_1.setIntensity(0,8); led_Display_1.clearDisplay(0);
    led_Display_1.setIntensity(1,8); led_Display_1.clearDisplay(1);
    led_Display_1.setIntensity(2,8); led_Display_1.clearDisplay(2);
    led_Display_1.setIntensity(3,8); led_Display_1.clearDisplay(3);

    Serial.begin(115200);
}// end setup()
 

 
void loop() {

   {INPUTS();} //Check the Simconnect and "keys" section for any input pins
   {ROTARYS();} // Check Rotary Encoder
   if (Serial.available()) {
     CodeIn = getChar();
     if (CodeIn == '=') {EQUALS();}    // The first identifier is "="
     if (CodeIn == '?') {QUESTION();}  // The first identifier is "?"
     if (CodeIn == '<') {LESS_THAN();} // The first identifier is "<"
   }
   //delay(4);
}// end loop()                            

 
char getChar()// Get a character from the serial buffer
{
  while(Serial.available() == 0);   // wait for data
  return((char)Serial.read());      // Thanks Doug
}// end getchar()                                  
 
void EQUALS() {            // The first identifier was "="
  
  CodeIn = getChar();     // Get another character
  switch(CodeIn) {         // Now lets find what to do with it
    
  //Comm2 Active -- the download is fixed 6 character format eg. 123.45
  case 'C': {
    Com2Active[0] = (getChar() - '0'); led_Display_1.setDigit(0,7, Com2Active[0], false); // 1st digit of Com2 Active is displayed 
    Com2Active[1] = (getChar() - '0'); led_Display_1.setDigit(0,6, Com2Active[1], false); // 2nd digit ditto
    Com2Active[2] = (getChar() - '0'); led_Display_1.setDigit(0,5, Com2Active[2], true);  // 3rd digit ditto -- true sets decimal led
    getChar();    // removes decimal point character from the serial bus without processing.
    Com2Active[3] = (getChar() - '0'); led_Display_1.setDigit(0,4, Com2Active[3], false); // 4th digit ditto
    Com2Active[4] = (getChar() - '0'); led_Display_1.setDigit(0,3, Com2Active[4], false); // 5th digit ditto
  }// close case C
  break;
  
        case 'D':           //  Comm2 Stby
            Count = 0;               // clear the Count
            while (Count < 5)        // we will count to 5 from 0 to 4
            {
                Digit = "";          // clears the string variable Digit
                Digit += getChar();  // Makes the string Digit what ever getChar() is holding
                  if (Digit ==".")          // ******   This looks for the "." *************
            {
        Digit = "";           // If we find the "." we clear it and get the next digit
        Digit += getChar();  //  Because we know where the "." always goes. 
            } 
                Com2Stby[Count] = Digit.toInt(); // Turns the string Digit into an Integer and
                                                 // then stores it in the Array AP_alt_set[] 0,1,2,3,4
                Count++;
            }
            
            led_Display_1.setDigit(0,1,Com2Stby[0],false);   // First digit of Comm2 Stby is displayed  
            led_Display_1.setDigit(0,0,Com2Stby[1],false);   // Second digit of Comm2 Stby is displayed 
            led_Display_1.setDigit(1,7,Com2Stby[2],true);   // Third digit of Comm2 Stby is displayed 
            led_Display_1.setDigit(1,6,Com2Stby[3],false);   // Fourth digit of Comm2 Stby is displayed 
            led_Display_1.setDigit(1,5,Com2Stby[4],false);   // Fifth digit of Comm2 Stby is displayed 
                                                               // false denotes NO decimal point
 break;
    
      case 'G':           //  Nav2 Active
            Count = 0;               // clear the Count
            while (Count < 5)        // we will count to 5 from 0 to 4
            {
                Digit = "";          // clears the string variable Digit
                Digit += getChar();  // Makes the string Digit what ever getChar() is holding
                  if (Digit ==".")          // ******   This looks for the "." *************
            {
        Digit = "";           // If we find the "." we clear it and get the next digit
        Digit += getChar();  //  Because we know where the "." always goes. 
            } 
                Nav2Active[Count] = Digit.toInt(); // Turns the string Digit into an Integer and
                                                   // then stores it in the Array AP_alt_set[] 0,1,2,3,4
                Count++;
            }
            led_Display_1.setDigit(1,2,Nav2Active[0],false);   // First digit of Nav2 Active is displayed  
            led_Display_1.setDigit(1,1,Nav2Active[1],false);   // Second digit of Nav2 Active is displayed 
            led_Display_1.setDigit(1,0,Nav2Active[2],true);   // Third digit of Nav2 Active is displayed 
            led_Display_1.setDigit(2,7,Nav2Active[3],false);   // Fourth digit of Nav2 Active is displayed 
            led_Display_1.setDigit(2,6,Nav2Active[4],false);   // Fifth digit of Nav2 Active is displayed 
                                                               // false denotes NO decimal point

         break;
         

      case 'H':           //  Nav2 Stby
            Count = 0;               // clear the Count
            while (Count < 5)        // we will count to 5 from 0 to 4
            {
                Digit = "";          // clears the string variable Digit
                Digit += getChar();  // Makes the string Digit what ever getChar() is holding
                  if (Digit ==".")          // ******   This looks for the "." *************
            {
        Digit = "";           // If we find the "." we clear it and get the next digit
        Digit += getChar();  //  Because we know where the "." always goes. 
            } 
                Nav2Stby[Count] = Digit.toInt(); // Turns the string Digit into an Integer and
                                                   // then stores it in the Array AP_alt_set[] 0,1,2,3,4
                Count++;
            }
            
            led_Display_1.setDigit(2,4,Nav2Stby[0],false);   // First digit of Nav2 Stby is displayed  
            led_Display_1.setDigit(2,3,Nav2Stby[1],false);   // Second digit of Nav2 Stby is displayed 
            led_Display_1.setDigit(2,2,Nav2Stby[2],true);   // Third digit of Nav2 Stby is displayed 
            led_Display_1.setDigit(2,1,Nav2Stby[3],false);   // Fourth digit of Nav2 Stby is displayed 
            led_Display_1.setDigit(2,0,Nav2Stby[4],false);   // Fifth digit of Nav2 Stby is displayed 
                                                               // false denotes NO decimal point
    } // ***  End of Switch Case ***
}
//***************( End of EQUALS() Function )**************
//*********************************************************

void QUESTION() {             // The first identifier was "?"

  CodeIn = getChar();     // Get another character
  switch(CodeIn) {         // Now lets find what to do with it
    

//Avionics Bus Voltage -- download format XX.X volts
  case 'U': {
    avmi = (getChar() != '0');
    for (int i=0; i<3; i++) getChar(); // strip remaining characters
    DISPLAY_OR_BLANK(avmi); 
  }// close U
  break;
  }// close switch
}// end question()




void LESS_THAN() {             // The first identifier was "<"

  CodeIn = getChar();     // Get another character
  switch(CodeIn) {         // Now lets find what to do with it
    
//battery master switch -- download format '0' or '1'
  case 'a': {
   sw_battery = (getChar() != '0');
  }
  break;
  
//avionics switch -- download format '0' or '1'
  case 'g': {
   sw_avionics = (getChar() != '0');
  }
  break;
  }// close switch
}// end less_than()




 void ROTARYS(){

    // ******Comm 2 Encoder******

  X1 =(quad1.position()/2);  // for full cycle encoders
  if (X1 != Xold1) { // checks to see if it is different
    (Xdif = (X1-Xold1)); // finds out the difference

    if (Xdif == 1) { 
    if (digitalRead(26) == 0){Serial.println("A07");} else Serial.println("A09");}
    
     if (Xdif == -1){
     if (digitalRead(26) == 0) {Serial.println("A08");} else Serial.println("A10");}
     // end of "check for HOLD-DOWN"
    Xold1 = X1; // overwrites the old reading with the new one. 
    }//end of quad1 read

    // ******Nav 2 Encoder******

   X2 =(quad2.position()/2);  // for full cycle encoders
  if (X2 != Xold2) { // checks to see if it is different
    (Xdif = (X2-Xold2)); // finds out the difference

    if (Xdif == 1) {
    if (digitalRead(27) == 0){Serial.println("A19");} else Serial.println("A21");}
    
     if (Xdif == -1){
     if (digitalRead(27) == 0) {Serial.println("A20");} else Serial.println("A22");}
     // end of "check for HOLD-DOWN"
    Xold2 = X2; // overwrites the old reading with the new one. 
    }//end of quad2 read
   }



void INPUTS(){ // Simconnect codes and "Keys" section
  Kstringnewstate = "";
  for (int KpinNo = 26; KpinNo <=70; KpinNo++) { //set this to the input pins. (pins 10 to 19)
    KpinStateSTR = String(digitalRead(KpinNo)); //read the state of the pin
    KoldpinStateSTR = String(Kstringoldstate.charAt(KpinNo - 26));// gets the 'old' state of the pin from string
    if (KpinStateSTR != KoldpinStateSTR) {// checks if it's different to the last reading of that pinNo
    // pin 13 is not within set 26 to 69      if (KpinNo != 13) { // avoid using pin 13 as an input
  //Swap Comm 2 with Standby
    if (KpinNo == 28 && KpinStateSTR == "0"){Serial.println ("A12");} 
  //Swap Nav 2 with Standby
    if (KpinNo == 29 && KpinStateSTR == "0"){Serial.println ("A24");} 
  //Avionics Master Switch
    if (KpinNo == 30) {
      if ((KpinStateSTR == "0") && (!sw_avionics)) Serial.println("A431"); // close toggle sw
      if ((KpinStateSTR == "1") && ( sw_avionics)) Serial.println("A430"); // open  toggle sw
    }
  //Battery Master Swtich
    if (KpinNo == 31) {
      if ((KpinStateSTR == "0") && (!sw_battery)) Serial.println("E18"); // close toggle sw
      if ((KpinStateSTR == "1") && ( sw_battery)) Serial.println("E17"); // open  toggle sw
    }
  }//end of 'its different'       
  Kstringnewstate += KpinStateSTR;
}//end of 'for' loop (read the pins)
  Kstringoldstate = Kstringnewstate;

}//end of INPUTS void



void DISPLAY_OR_BLANK(bool which) {
  
// which = true display-- download
  if (which) {
  //Com2 Active
    led_Display_1.setDigit(0,7,Com2Active[0],false); //**
    led_Display_1.setDigit(0,6,Com2Active[1],false); // *
    led_Display_1.setDigit(0,5,Com2Active[2],true);  // *
    led_Display_1.setDigit(0,4,Com2Active[3],false); // *
    led_Display_1.setDigit(0,3,Com2Active[4],false); // *
  //Com2 Standby
    led_Display_1.setDigit(0,1,Com2Stby[0],false);   // *
    led_Display_1.setDigit(0,0,Com2Stby[1],false);   // *
    led_Display_1.setDigit(1,7,Com2Stby[2],true);    // *
    led_Display_1.setDigit(1,6,Com2Stby[3],false);   // *
    led_Display_1.setDigit(1,5,Com2Stby[4],false);   // *
  //Nav2 Active
    led_Display_1.setDigit(1,2,Nav2Active[0],false); //  * > display 1st through 5th digits
    led_Display_1.setDigit(1,1,Nav2Active[1],false); // *
    led_Display_1.setDigit(1,0,Nav2Active[2],true);  // *
    led_Display_1.setDigit(2,7,Nav2Active[3],false); // *
    led_Display_1.setDigit(2,6,Nav2Active[4],false); // *
  //Nav2 Standby
    led_Display_1.setDigit(2,4,Nav2Stby[0],false);   // *
    led_Display_1.setDigit(2,3,Nav2Stby[1],false);   // *
    led_Display_1.setDigit(2,2,Nav2Stby[2],true);    // * 
    led_Display_1.setDigit(2,1,Nav2Stby[3],false);   // *
    led_Display_1.setDigit(2,0,Nav2Stby[4],false);   //**
  }// close true
  else {
  //Com2 Active
    led_Display_1.setChar(0,7, ' ',false); //**
    led_Display_1.setChar(0,6, ' ',false); // *
    led_Display_1.setChar(0,5, ' ',false); // *
    led_Display_1.setChar(0,4, ' ',false); // * 
    led_Display_1.setChar(0,3, ' ',false); // * 
  //Com2 Standby  
    led_Display_1.setChar(0,1, ' ',false); // *
    led_Display_1.setChar(0,0, ' ',false); // *
    led_Display_1.setChar(1,7, ' ',false); // *
    led_Display_1.setChar(1,6, ' ',false); // *
    led_Display_1.setChar(1,5, ' ',false); //  * > All displays blank when avionis master is selected off
  //Nav2 Active
    led_Display_1.setChar(1,2, ' ',false); // *
    led_Display_1.setChar(1,1, ' ',false); // *
    led_Display_1.setChar(1,0, ' ',false); // *
    led_Display_1.setChar(2,7, ' ',false); // *
    led_Display_1.setChar(2,6, ' ',false); // *
  //Nav2 Standby
    led_Display_1.setChar(2,4, ' ',false); // *
    led_Display_1.setChar(2,3, ' ',false); // *
    led_Display_1.setChar(2,2, ' ',false); // *
    led_Display_1.setChar(2,1, ' ',false); // *
    led_Display_1.setChar(2,0, ' ',false); //**
  }// close else
}// end display_or_blank()



/*
         
         
to join this conversation on GitHub. Already have an account? Sign in to comment

    © 2018 GitHub, Inc.
    Terms
    Privacy
    Security
    Status
    Help

    Contact GitHub
    Pricing
    API
    Training
    Blog
    About

Press h to open a hovercard with more details.
*/


