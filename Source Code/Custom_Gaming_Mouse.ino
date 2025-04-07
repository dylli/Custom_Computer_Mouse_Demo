// Name: Custom_Gaming_Mouse
// Arduino Nano RP2040 Connect with PMW3360 sensor for a wired and wireless gaming mouse application + gyromouse
// Version: v0.02
// Developer: Dylan Li

//#include "PMW3360.h"      // also contains SPI.h
#include "USBMouseKeyboard.h"

// Chip reset(active low) pin {Symbol: NRESET}
#define NRES 8

// Motion detect pin {Symbol: MOTION}
#define MOT 9

// SPI pins (Master = RP2040; Slave = PMW3360)
#define SCLK 13
#define MOSI 11
#define MISO 12
#define NCS 10

// External Push Button and Switch pins
#define LEFT 6       // left mouse button
#define RIGHT 5      // right mouse button
#define PREV 3       // previous mouse button (ALT+Left on keyboard)
#define NEXT 2       // next mouse button (ALT+Right on keyboard)        
#define SPEED 4      // changes cursor speed

// Rotary Encoder pins
#define MIDDLE 14    // middle mouse button
#define ROT_CLK 16
#define ROT_DT 15

// RGB pins (on-board RGB led)
#define RGB_TOGGLE 17

// Master On/Off Switch pin
#define POW 7
#define ON HIGH
#define OFF LOW

// Wired/Wireless/Gyro Selection pin
#define MODE_CHANGE 20      //************************************** will have to change this because A6 is not an interrupt pin (also fuck the RGB)
#define WIRED 0
#define WIRELESS 1
#define GYRO 2


/*****************************************************************************************************************************************/
/****************** KY-040 globals ****************/
volatile int scroll_v = 0;

/***************** PMW3360 globals ****************/
//PMW3360 sensor;
uint8_t currentMode = 0;        // start in wired mode

/***************** other globals ******************/
USBMouseKeyboard key_mouse;     // relative mouse
PinStatus statePOW;
PinStatus stateRGB;

/***************** Function declarations ******************/
void setup();


/*****************************************************************************************************************************************/
int main(void) 
{
  setup();

  while (1) {
    /// Poll for button presses
    if (digitalRead(LEFT) == LOW) {
      delay(20);
      if (digitalRead(LEFT) == LOW) {
        key_mouse.click(MOUSE_LEFT);
        while (digitalRead(LEFT) == LOW);
      }
    }
    if (digitalRead(RIGHT) == LOW) {
      delay(20);
      if (digitalRead(RIGHT) == LOW) {
        key_mouse.click(MOUSE_RIGHT);
        while (digitalRead(RIGHT) == LOW);
      }
    }
    else if (digitalRead(MIDDLE) == LOW) {
      delay(20);
      if (digitalRead(MIDDLE) == LOW) {
        key_mouse.click(MOUSE_MIDDLE);
        while (digitalRead(MIDDLE) == LOW);
      }
    }
    if (digitalRead(PREV) == LOW) {
      delay(20);
      if (digitalRead(PREV) == LOW) {
        key_mouse.key_code(LEFT_ARROW, KEY_ALT);
        while (digitalRead(PREV) == LOW);
      }
    }
    if (digitalRead(NEXT) == LOW) {
      delay(20);
      if (digitalRead(NEXT) == LOW) {
        key_mouse.key_code(RIGHT_ARROW, KEY_ALT);
        while (digitalRead(NEXT) == LOW);
      }
    }

    if (scroll_v > 0) {
      key_mouse.scroll(1);
      scroll_v = 0;
    }
    else if (scroll_v < -0) {
      key_mouse.scroll(-1);
      scroll_v = 0;
    }
  }
  // end of loop
}


/****************************************************************************************************************************
Name: setup.
Description: Start of program. Runs once.
Parameters:
  none.
Return: void.
*****************************************************************************************************************************/
void setup() {
  // Enable the pull-ups for all pushbuttons and switches
  pinMode(LEFT, INPUT_PULLUP);
  pinMode(RIGHT, INPUT_PULLUP);
  pinMode(PREV, INPUT_PULLUP);
  pinMode(NEXT, INPUT_PULLUP);
  pinMode(SPEED, INPUT_PULLUP);
  pinMode(POW, INPUT_PULLUP);
  pinMode(RGB_TOGGLE, INPUT_PULLUP);
  pinMode(MODE_CHANGE, INPUT_PULLUP);

  // Enable pull-ups for rotary encoder
  pinMode(MIDDLE, INPUT_PULLUP);
  pinMode(ROT_CLK, INPUT_PULLUP);
  pinMode(ROT_DT, INPUT_PULLUP);

  // Set interrupts for rotary encoder
  attachInterrupt(digitalPinToInterrupt(ROT_DT), ISR_rotaryEncoder, FALLING);

  // Read initial states
  statePOW = digitalRead(POW);
  stateRGB = digitalRead(RGB_TOGGLE);
}


// ISR to handle the interrupts for CLK and DT
void ISR_rotaryEncoder() {
  // Process pin states for CLK and DT
  if (digitalRead(ROT_CLK) == LOW)    // clockwise = scroll down
    scroll_v = 1;
  else                                // counterclockwise = scroll up
    scroll_v = -1;

}
