/*
  Project: Two Way Texting Device using Radio Waves (XBee)
  Group Members: Michael Lastinger, Chase Tenewitz, Darious Mack, Isaac Kim
*/

//Header files to include
#include <SPI.h>
#include "LCD_Functions.h"
#include <string.h>
#include <Keypad.h>

//Declaration of rows and columns for keypad
const byte ROWS = 8;
const byte COLS = 5; 

//Definition of variables for keypad
char hexaKeys[ROWS][COLS] = {
    {'A','B','C','D','E'},
    {'F','G','H','I','J'},
    {'K','L','M','N','O'},
    {'P','Q','R','S', 'T'},
    {'U','V','1','2','3'},
    {'W','X','4','5','6'},
    {'Y','Z','7','8','9'},
    {'s','b','p','0','e'} //Shift,backspace,space,0,enter
  };
  
//Arrays for the row pins and column pins
byte rowPins[ROWS] = {22, 21, 20, 19, 18, 23, 14, 2}; //Pin 14=D17
byte colPins[COLS] = {11, 10, 6, 5, 3};

//Initialization of an instance of the class Keypad
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

//Initial setup for LCD and Xbee 
void setup()
{
  Serial.begin(9600); //Communication with usb
  Serial1.begin(9600); //Communication with xbee

  lcdBegin(); //Initialize LCD and setup pins
  updateDisplay(); //Update display
  setContrast(50); //Sets contrast for the LCD
  delay(2000); 

  /*
  // Wait for serial to come in, then clear display and go to echo
  while (!Serial1.available()) {
    delay(1000);
  }*/
  
  clearDisplay(WHITE); //Clears display
  updateDisplay(); //Update Display
}

//Variables for Texting
String message = ""; //String variable used for concat characters
int len = 1; //Length 
String receivedMessage = "";
int lengthForReceivedMessage = 1;
char * theText = "";

void loop()
{
  char customKey = customKeypad.getKey();
  
  static int cursorX = 0;
  static int cursorY = 0;
  
  //Putting message together to send
    switch (customKey)
    {
      case '\n': // New line
        cursorY += 8;
        break;
      case '\r': // Return feed
        cursorX = 0;
        break;
      case '~': // Use ~ to clear the screen.
        clearDisplay(WHITE);
        updateDisplay();
        cursorX = 0; // reset the cursor
        cursorY = 0;
        break;
      case NULL:
        //Do Nothing
      break;
      default:
        setChar(customKey, cursorX, cursorY, BLACK);
        updateDisplay();
        cursorX += 6; // Increment cursor
        Serial1.write(customKey);
        break;
    }
    // Manage cursor
    if (cursorX >= (LCD_WIDTH - 4))
    { // If the next char will be off screen...
      cursorX = 0; // ... reset x to 0...
      cursorY += 8; // ...and increment to next line.
      if (cursorY >= (LCD_HEIGHT - 7))
      { // If the next line takes us off screen...
        cursorY = 0; // ...go back to the top.
      }
    }
    

 


  //Received information
  if (Serial1.available())
  {
   
    char c = Serial1.read();
 

    if(c != 'e')
    {
       message.concat(c); 
       len++; // buffer size
    } 
    else
    {
       message.toCharArray(theText, len);
       
       setStr(theText, cursorX, cursorY, BLACK);
       updateDisplay();
       
       message = "";
       theText = "";
       len = 1;
    } 
  }
   
  
}



