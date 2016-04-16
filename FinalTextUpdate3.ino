/*
  Project: Two Way Direct Line of Site Texting Without Satellite or Cell Tower 
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
  {'U', 'V', '.', '?', '!'},
  {'W', 'X', ',', ';', ':'},
  {'Y', 'Z', '"', '(', ')'},
  {'+', '&', '*', '@', '/'} //Shift,backspace,space,0,enter
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
  {'^', '-', ' ', '0', '$'} //Shift,backspace,space,0,enter
};

//Arrays for the row pins and column pins
byte rowPins[ROWS] = {22, 21, 20, 19, 18, 23, 14, 2}; //Pin 14=D17
byte colPins[COLS] = {11, 10, 6, 5, 3};

//Initialization of an instance of the class Keypad
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

//Initialization of an instace of the class Keypad for the Shifted Keys
Keypad shiftedKeypad = Keypad( makeKeymap(hexaShiftedKeys), rowPins, colPins, ROWS, COLS);

//Variables for Texting
char customKey;//Value from the keypad when not shifted
char customShiftedKey;//Value from the keypad when shifted
static int cursorX = 0; //Values that represent pixel distance on the x-axis
static int cursorY = 0;//Values that represent pixel distance on the y-axis
String receivedMessage = "---Received--->";//String that will hold the received message
String sentMessage = "-----Sent-----";//String that will hold the send message

//Boolean Variables
boolean shifted = false;
boolean window = true;
boolean finishedFirstText = false;
boolean firstText = true;
boolean initialShift = true;
boolean printText = false;

//Testing for long texts/////////////////////////////////////////
boolean receivedLongText = false;
boolean sentLongText = false;
////////////////////////////////////////////////////////////////

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
  
  printMessage(sentMessage);//Prints originial send display on LCD

}



void loop()
{
  //Checking if user is typing more than available on LCD
  textOverwrite();
  
  // last spot in the string
  int locationOfCursor = sentMessage.length() - 1; 

  //Checks which window the user wants to see
  //Starts out in the sending window
  if (window)
  { 

    
    //Shift not pressed
    if (!shifted)
    {
      //Obtain custom key from keypad
      customKey = customKeypad.getKey();

      //Puts the first > on string
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
        case '-': //Backspace key
          //Case if user has backspaced too much
          if(sentMessage.charAt(locationOfCursor) == '>')
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
            Serial1.write(customKey);//Write to Xbee
          } 
        break;
        case '$': //Send key was pressed
          Serial1.write('>');//Send a new '>' to display a new text
          cursorY += 8;
          cursorX = 0;
          sentMessage.concat('>'); //Display on LCD
          setChar('>', cursorX, cursorY, BLACK); 
          updateDisplay();
          cursorX += 6;
          printText = true;
          Serial1.write(customKey);//Send Pressed key from keypad to XBee 
        break;
        default:
          sentMessage.concat(customKey);//Puts customKey into sentMessage
          Serial1.write(customKey);//Sends character to other Xbee
          setChar(customKey, cursorX, cursorY, BLACK);//Display on Screen
          updateDisplay();
          cursorX += 6; // Increment cursor
        break;
      }// End Switch
    }//End not shifted if

    //Shift pressed
    if (shifted)
    {
      //Obtain shifted value
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
          if (initialShift) //Accounts for an issue with shifting on first attempt
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
          sentMessage.concat(customShiftedKey);//Put value String SentMessage
          Serial1.write(customShiftedKey); //Send value to other Xbee
          setChar(customShiftedKey, cursorX, cursorY, BLACK); //Update display
          updateDisplay();
          cursorX += 6; 
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
      cursorX = 0;
      cursorY = 0;
      printMessage(receivedMessage);
    }
    
    ///////////////////////////////////////////////////////////////
    if(receivedLongText)
    {
      clearDisplay(WHITE);//Clear display for the new corrected overlapping text
      updateDisplay();
      cursorX = 0;
      cursorY = 0;
      receivedLongText = false;//Print only once until user overlaps again
      printMessage(receivedMessage);
    }
    //////////////////////////////////////////////////////////////
    
    //Obtains key from custom keypad
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
        
        printMessage(sentMessage);//Print the Users Sent interface
        
        window = true; //Go back to texting window
        shifted = false;//Shift button set back to off
      break;
    }
  } //End window else




  //Received information from another Xbee
  if (Serial1.available())
  {
    char c = Serial1.read(); //Reads the value into a char value

    //User has not sent the message; Sending characters
    if (c != '$')
    {
      if(c == '-') //Backspace was pressed
      {
         //Find last spot in string and take it away
         int receivedPositionToSubtract = receivedMessage.length()-1;
         receivedMessage.remove(receivedPositionToSubtract);
      }
      else
      {
         receivedMessage.concat(c);
      }
    
    }
    //User sent the message
    else
    {
      //User has sent the information and new message should be printed
      printText = 1;
    }//End Else
  }//End if
} ///////////////////////////////////////////////////////End LOOP



///////////////////////////Additional Methods/////////////////////////////////

//Determines length of first text
int determineCount()
{
   int count = 1;
   //obtains amount of characters for the first text
   for(int i = 15; sentMessage.charAt(i) != '>'; i++)
   {
      count++;
   }
   return count;
}

//Deletes first message so that user can type more
void deleteFirstMessage()
{
   //The first message starting point
   int startingIndex = 14;
   
   //Obtain value for length of the first text
   int countForDeleting = determineCount();
   
   //Calls the remove method form the string class deleting the characters
   //from the starting location to the length of the first text 
   //in both the sent and received message
   sentMessage.remove(startingIndex, countForDeleting);
   receivedMessage.remove(startingIndex, countForDeleting);
}

//Determines if screen will overwrite
void textOverwrite ()
{
   // If the next line takes the user off the screen.
   if (cursorY >= (LCD_HEIGHT - 7))
   { 
      cursorY = 0; 
      cursorX = 0;
      deleteFirstMessage();//Call method
      clearDisplay(WHITE); //Clears display
      updateDisplay(); //Update Display
      printMessage(sentMessage);//print the new sent message with deletion
      receivedLongText = true;//Notify the other Xbee of a new text to be printed
   }//End if statement
}//End textOverwrite()


//Overlap for LCD
void textOverlap ()
{
  // If the next char will be off screen.
  if (cursorX >= (LCD_WIDTH - 4))
  { 
      cursorX = 0; 
      cursorY += 8; //Increment to next line for Wrapping line
  }//End of if statement
}//End textOverlap()

//Prints the String parameter
void printMessage(String messageToPrint) {
  //For loop used to print each individual character from String
  for (int i = 0; i < messageToPrint.length(); i++)
  {
    //Determines if the character is '>' and not the first '>' at i=14
    if (messageToPrint.charAt(i) == '>' && i != 14)
    {
      //New Line
      cursorY += 8;
      cursorX = 0;
      //Print to screen
      setChar(messageToPrint.charAt(i), cursorX, cursorY, BLACK);
      updateDisplay();
      cursorX += 6;
    }//End of if statement
    else
    {
      //Print to screen
      setChar(messageToPrint.charAt(i), cursorX, cursorY, BLACK);
      updateDisplay();
      cursorX += 6;
    }//End of else statement
    textOverlap();//Method for overlap
  }//End of for loop
}//End printMessage() method
