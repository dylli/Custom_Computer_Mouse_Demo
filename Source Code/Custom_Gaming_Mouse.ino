// Name: Custom_Gaming_Mouse
// Arduino Nano RP2040 Connect with PMW3360 sensor for a wired and wireless gaming mouse application + gyromouse
// Version: v0.02
// Developer: Dylan Li

#include "WiFiNINA.h"
#include "PMW3360.h"
#include "USBMouseKeyboard.h"

/// PMW3360 pins
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

// Master On/Off Switch pin
#define POW 7

// Wired/Wireless/Gyro Selection pin
#define MODE_CHANGE 17
#define WIRED 0
#define WIRELESS 1
#define GYRO 2


/*****************************************************************************************************************************************/
/****************** KY-040 globals ****************/
volatile int scroll_v = 0;

/***************** PMW3360 globals ****************/
PMW3360 sensor;
int currentCPI;                     // avoids needing to check sensor register
uint8_t currentMode = 0;            // start in wired mode

/***************** other globals ******************/
USBMouseKeyboard key_mouse;     // relative mouse
PinStatus last_statePOW;

/***************** Function declarations ******************/
void setup();
void ISR_rotaryEncoder();


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
    if (digitalRead(MIDDLE) == LOW) {
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
    if (digitalRead(SPEED) == LOW) {
      delay(20);
      if (digitalRead(SPEED) == LOW) {
        if (currentCPI == 1000) {
          sensor.setCPI(8000);
          currentCPI = 8000;
        } else {
          sensor.setCPI(1000);
          currentCPI = 1000;
        }
        while (digitalRead(NEXT) == LOW);
      }
    }

    // Poll for mouse scrolls
    if (scroll_v > 0) {       // clockwise = scroll down
      key_mouse.scroll(1);
      scroll_v = 0;
    }
    else if (scroll_v < 0) { // counter-clockwise = scroll up
      key_mouse.scroll(-1);
      scroll_v = 0;
    }

    // Poll for motion detected from PMW3360 sensor
    PMW3360_DATA data = sensor.readBurst();
    if(data.isOnSurface && data.isMotion)
    {
      key_mouse.move(data.dx, data.dy);
    }

    // Poll for change in switch state
    if (last_statePOW != digitalRead(POW)) 
    {
      delay(100);     // debounce

      // Check again
      if (last_statePOW != digitalRead(POW)) {
        last_statePOW = digitalRead(POW);

        if (last_statePOW == LOW) {   // turn off sensor
          //*****************************Code to turn off sensor WIP**************************************************//
          digitalWrite(LEDR, LOW);
        } else {                      // turn on sensor
          digitalWrite(LEDR, HIGH);
        }
      }

    }

  } // end of loop
}



/****************************************************************************************************************************
Name: setup.
Interrupt: No.
Description: Start of program. Runs once.
Parameters/Variables:
  none.
Return: void.
Errors: none.
*****************************************************************************************************************************/
void setup() {
  /// Initialize the PMW3360 sensor
  if(!sensor.begin(NCS, 1000))   // Arduino will get stuck in infinte loop in case of initialization failure
    while (1);
  currentCPI = 1000;

  // Enable the pull-ups for all pushbuttons and switches
  pinMode(LEFT, INPUT_PULLUP);
  pinMode(RIGHT, INPUT_PULLUP);
  pinMode(PREV, INPUT_PULLUP);
  pinMode(NEXT, INPUT_PULLUP);
  pinMode(SPEED, INPUT_PULLUP);
  pinMode(POW, INPUT_PULLUP);
  pinMode(MODE_CHANGE, INPUT_PULLUP);

  /// Initialize rotary encoder
  // Enable pull-ups for rotary encoder
  pinMode(MIDDLE, INPUT_PULLUP);
  pinMode(ROT_CLK, INPUT_PULLUP);
  pinMode(ROT_DT, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ROT_DT), ISR_rotaryEncoder, FALLING);

  // Set RGB as output
  pinMode(LEDR, OUTPUT);
  last_statePOW = digitalRead(POW);
  if (last_statePOW == HIGH)
    digitalWrite(LEDR, HIGH);
}


/****************************************************************************************************************************
Name: ISR_rotaryEncoder
Interrupt: Yes.
Description: Determine if the rotary encoder rotated clockwise or counter-clockwise.
Parameters/Variables:
  scroll_v
    * Type: volatile int
    * Desc: Will change to 1 for clockwise rotation, and to -1 for counter-clockwise rotation.
Return: void.
Errors: none.
*****************************************************************************************************************************/
void ISR_rotaryEncoder() {
  // Process pin states for CLK and DT
  if (digitalRead(ROT_CLK) == LOW)
    scroll_v = 1;
  else
    scroll_v = -1;
}
