/* Nokia 5100 LCD Example Code
   Graphics driver and PCD8544 interface code for SparkFun's
   84x48 Graphic LCD.
   https://www.sparkfun.com/products/10168

  by: Jim Lindblom
    adapted from code by Nathan Seidle and mish-mashed with
    code from the ColorLCDShield.
  date: October 10, 2013
  license: Officially, the MIT License. Review the included License.md file
  Unofficially, Beerware. Feel free to use, reuse, and modify this
  code as you see fit. If you find it useful, and we meet someday,
  you can buy me a beer.

  This all-inclusive sketch will show off a series of graphics
  functions, like drawing lines, circles, squares, and text. Then
  it'll go into serial monitor echo mode, where you can type
  text into the serial monitor, and it'll be displayed on the
  LCD.

  This stuff could all be put into a library, but we wanted to
  leave it all in one sketch to keep it as transparent as possible.

  Hardware: (Note most of these pins can be swapped)
    Graphic LCD Pin ---------- Arduino Pin
       1-VCC       ----------------  5V
       2-GND       ----------------  GND
       3-SCE       ----------------  7
       4-RST       ----------------  6
       5-D/C       ----------------  5
       6-DN(MOSI)  ----------------  11
       7-SCLK      ----------------  13
       8-LED       - 330 Ohm res --  9
   The SCLK, DN(MOSI), must remain where they are, but the other
   pins can be swapped. The LED pin should remain a PWM-capable
   pin. Don't forget to stick a current-limiting resistor in line
   between the LCD's LED pin and Arduino pin 9!
*/
#include <SPI.h>
#include "LCD_Functions.h"
#include <string.h>
#include <Keypad.h>


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
byte rowPins[ROWS] = {22, 21, 20, 19, 18, 23, 14, 2}; //connect to the row pinouts of the keypad;-------->13,14,15,16,17,18,19,10
byte colPins[COLS] = {11, 10, 6, 5, 3}; //connect to the column pinouts of the keypad;-------- 12-->14

//initialize an instance of class NewKeypad
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);


void setup()
{
  Serial.begin(9600); //communication with usb
  Serial1.begin(9600); //communication with xbee

  

  lcdBegin(); // This will setup our pins, and initialize the LCD
  updateDisplay(); // with displayMap untouched, SFE logo
  setContrast(60); // Good values range from 40-60
  delay(2000);


  // Wait for serial to come in, then clear display and go to echo
  //while (!Serial1.available()) {
    //delay(1000);
  //}
  clearDisplay(WHITE);
  updateDisplay();
}

String message = "";
char * theText = "";
int len = 1;

// Loop turns the display into a local serial monitor echo.
// Type to the Arduino from the serial monitor, and it'll echo
// what you type on the display. Type ~ to clear the display.
void loop()
{
  char customKey = customKeypad.getKey();
  
  static int cursorX = 0;
  static int cursorY = 0;

  if (customKey){
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
      default:
        setChar(customKey, cursorX, cursorY, BLACK);
        updateDisplay();
        cursorX += 6; // Increment cursor
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
    
  }



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
       Serial.println(message);
       Serial.println(len);
       message.toCharArray(theText, len);
       
       setStr(theText, cursorX, cursorY, BLACK);
       updateDisplay();
       
       message = "";
       theText = "";
       len = 1;
    } 

    
    /*switch (c)
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
      default:
        setChar(c, cursorX, cursorY, BLACK);
        updateDisplay();
        cursorX += 6; // Increment cursor
        break;
    }*/
    
    
    /*
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
    */
  }
}


