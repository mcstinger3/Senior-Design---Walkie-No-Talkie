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

//Definition of variables for shifted keypad
char hexaShiftedKeys[ROWS][COLS] = {
  {'A', 'B', 'C', 'D', 'E'},
  {'F', 'G', 'H', 'I', 'J'},
  {'K', 'L', 'M', 'N', 'O'},
  {'P', 'Q', 'R', 'S', 'T'},
  {'U', 'V', '1', '2', '3'},
  {'W', 'X', '4', '5', '6'},
  {'Y', 'Z', '7', '8', '9'},
  {'+', 'b', '*', '0', '/'} //Shift,backspace,space,0,enter
};

//Definition of variables for non-shifted keypad
char hexaKeys[ROWS][COLS] = {
  {'a', 'b', 'c', 'd', 'e'},
  {'f', 'g', 'h', 'i', 'j'},
  {'k', 'l', 'm', 'n', 'o'},
  {'p', 'q', 'r', 's', 't'},
  {'u', 'v', '1', '2', '3'},
  {'w', 'x', '4', '5', '6'},
  {'y', 'z', '7', '8', '9'},
  {'^', '@', ' ', '0', '$'} //Shift,backspace,space,0,enter
};

//Arrays for the row pins and column pins
byte rowPins[ROWS] = {22, 21, 20, 19, 18, 23, 14, 2}; //Pin 14=D17
byte colPins[COLS] = {11, 10, 6, 5, 3};

//Initialization of an instance of the class Keypad
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

//Initialization of an instace of the class Keypad for the Shifted Keys
Keypad shiftedKeypad = Keypad( makeKeymap(hexaShiftedKeys), rowPins, colPins, ROWS, COLS);

//Initial setup for LCD and Xbee
void setup()
{
  Serial.begin(9600); //Communication with usb (Serial Monitor)
  Serial1.begin(9600); //Communication with xbee

  lcdBegin(); //Initialize LCD and setup pins
  updateDisplay(); //Update display
  setContrast(50); //Sets contrast for the LCD
  delay(2000);

  clearDisplay(WHITE); //Clears display
  updateDisplay(); //Update Display
}

//Variables for Texting
char customKey;
char customShiftedKey;
static int cursorX = 0;
static int cursorY = 0;
String receivedMessage = ">";
String sentMessage = "";

//Boolean Variables
boolean shifted = false;
boolean window = true;
boolean finishedFirstText = false;
boolean firstText = true;
boolean initialShift = true;
boolean printText = false;


void loop()
{
  
  //Checks which window the user wants to see
  //Starts out in the sending window
  if (window)
  {

    //Shift not pressed
    if (!shifted)
    {
      customKey = customKeypad.getKey();


      if (firstText && customKey != NULL)
      {
        firstText = false;
        char theKey = '>';
        setChar(theKey, cursorX, cursorY, BLACK);
        updateDisplay();
        sentMessage.concat('>');
        cursorX += 6;
      }//End inner if

      switch (customKey)
      {
        case NULL:
          //Do Nothing
        break;
        case '^': //Shift key was pressed
          shifted = true;
        break;
        case '@': //Backspace key
          //Case if user has backspaced too much
          if(cursorX == 0 && cursorY == 0)
          {
             //Don't backspace (off of screen)
          } 
          else
          {
            cursorX -= 6;
            setChar(' ', cursorX, cursorY, BLACK);
            updateDisplay();
            //Find last spot in string and take it away
            int positionToSubtract = sentMessage.length() - 1;
            sentMessage.remove(positionToSubtract);
          } 
        break;
        case '$': //Send key
          Serial1.write('>');
          cursorY += 8;
          cursorX = 0;
          sentMessage.concat('>'); //added this but haven't tested iajofjiasfojojdfoajdfijadf
          setChar('>', cursorX, cursorY, BLACK); //added this but haven't tested iajofjiasfojojdfoajdfijadf
          updateDisplay();
          printText = true;
          Serial1.write(customKey);     
        break;
        default:
          sentMessage.concat(customKey);
          Serial1.write(customKey);
          setChar(customKey, cursorX, cursorY, BLACK);
          updateDisplay();
          cursorX += 6; // Increment cursor
        break;
      }// End Switch
    }//End not shifted if

    //Shift pressed
    if (shifted)
    {
      customShiftedKey = shiftedKeypad.getKey();

      switch (customShiftedKey)
      {
        case NULL:
          //Do Nothing
        break;
        case '*':  //Case when switching between screens
          window = false;
          printText = true;
        break;
        case '+':  //Shift key was pressed
          if (initialShift)
          {
            initialShift = false;
            shifted = true;
          }
          else
          {
            shifted = false;
          }
        break;
        case '/':
          //do nothing - cannot Send in Received Messages window
        break;
        default:
          sentMessage.concat(customShiftedKey);
          Serial1.write(customShiftedKey);
          setChar(customShiftedKey, cursorX, cursorY, BLACK);
          updateDisplay();
          cursorX += 6; // Increment cursor
          shifted = false;
        break;
      }//End switch

    }//End shifted if
    
    textOverlap();
  }//End window if

  else //Switch window -- To Received Messages Window
  {
    //Case to printText once for user to see received message
    if(printText)
    {
      printText = false;
      clearDisplay(WHITE);
      updateDisplay();
      cursorX = 0; // reset the cursor
      cursorY = 0;
      printMessage(receivedMessage);
    }

    char customShiftedKey = shiftedKeypad.getKey();

    switch (customShiftedKey)
    {
      case NULL:
        //Do nothing
      break;
      case '*':  //Press Space to return to "Sent" Screen
        clearDisplay(WHITE); //Clear display for writing Sent message
        updateDisplay();
        cursorX = 0;
        cursorY = 0;
        
        printMessage(sentMessage);
        
        window = true; //Go back to texting window
        shifted = false;
      break;
    }
  } //End window else




  //Received information
  if (Serial1.available())
  {
    char c = Serial1.read();


    if (c != '$')
    {
      receivedMessage.concat(c);
    }
    else
    {
      //User has sent the information and new message should be printed
      printText = 1;
    }//end else

  }//End if

} //End of LOOP



////////////////Additional Methods//////////////////////

//Overlap for LCD
void textOverlap ()
{
  if (cursorX >= (LCD_WIDTH - 4))
    { // If the next char will be off screen...
      cursorX = 0; // ... reset x to 0...
      cursorY += 8; // ...and increment to next line.
      if (cursorY >= (LCD_HEIGHT - 7))
      { // If the next line takes us off screen...
        cursorY = 0; // ...go back to the top.
      }
    }
}//End textOverlap()

//Prints the String parameter
void printMessage(String messageToPrint) {
  for (int i = 0; i < messageToPrint.length(); i++)
  {
    if (messageToPrint.charAt(i) == '>' && finishedFirstText)//could replace with i != 0 (probably better)
    {
      cursorY += 8;
      cursorX = 0;
      setChar(messageToPrint.charAt(i), cursorX, cursorY, BLACK);
      updateDisplay();
      cursorX += 6;
    }
    else
    {
      finishedFirstText = true;
      setChar(messageToPrint.charAt(i), cursorX, cursorY, BLACK);
      updateDisplay();
      cursorX += 6;
    }
    
    textOverlap();
  }

}//End printMessage()
