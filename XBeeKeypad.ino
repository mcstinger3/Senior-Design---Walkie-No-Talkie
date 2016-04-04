#include <SoftwareSerial.h>

#include <Keypad.h>

/* @file CustomKeypad.pde
|| @version 1.0
|| @author Alexander Brevig
|| @contact alexanderbrevig@gmail.com
||
|| @description
|| | Demonstrates changing the keypad size and key values.
|| #
*/

SoftwareSerial XBee(0, 1); // RX, TX

const byte ROWS = 8; //eight rows
const byte COLS = 5; //five columns
//define the symbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = {
    {'A','B','C','D','E'},
    {'F','G','H','I','J'},
    {'K','L','M','N','O'},
    {'P','Q','R','S', 'T'},
    {'U','V','1','2','3'},
    {'W','X','4','5','6'},
    {'Y','Z','7','8','9'},
    {'s','b','p','0','e'} //shift,backspace,space,0,enter
  };
byte rowPins[ROWS] = {22, 21, 20, 19, 18, 23, 16, 15}; //connect to the row pinouts of the keypad;-------->13,14,15,16,17,18,19,10
byte colPins[COLS] = {11, 8, 7, 5, 4}; //connect to the column pinouts of the keypad;-------- 12-->14

//initialize an instance of class NewKeypad
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

void setup(){
  XBee.begin(9600);
 // Serial.begin(9600);
}
  
void loop(){
  char customKey = customKeypad.getKey();
  
  if (customKey){
   // Serial.println(customKey);
    XBee.write(customKey);
  }

  /*if (Serial.available())
  { // If data comes in from serial monitor, send it out to XBee
    XBee.write(customKey);
  }
  if (XBee.available())
  { // If data comes in from XBee, send it out to serial monitor
    Serial.write(XBee.read());
  }*/
}
