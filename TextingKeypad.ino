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
byte rowPins[ROWS] = {13, 14, 15, 16, 17, 18, 19, 10}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {2, 4, 7, 8, 12}; //connect to the column pinouts of the keypad

//initialize an instance of class NewKeypad
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

void setup(){
  Serial.begin(9600);
}
  
void loop(){
  char customKey = customKeypad.getKey();
  
  if (customKey){
    Serial.println(customKey);
  }
}
