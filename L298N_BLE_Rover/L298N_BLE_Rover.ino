// Code for a Bluetooth enabled rover utilizing curieBLE library.

// Author: Prashant Lalwani
// 10/31/2017



//////////////////////////////
// defines for BLE input //
////////////////////////////

#define UP 1
#define RIGHT 2
#define DOWN 3
#define LEFT 4
#define STOP 5



/////////////////////////////////////////////////////////////////////////////
//defines to send to motor driver function indicating direction of motors //
///////////////////////////////////////////////////////////////////////////

#define FORWARD 8
#define BACKWARD 2



////////////////
// Libraries //
//////////////

// Curie Bluetooth Library
#include <CurieBLE.h>



///////////////////////
// global variables //
/////////////////////

int direction;
const int connect_led_pin = 13;

// left motor driver array specifying pinouts 
int left_motor_driver[2][3] =
{
  {0,1,3}, //IN1,IN2,ENA
  {2,4,5}, //IN3,IN4,ENB
};

// right motor driver array specifying pinouts 
int right_motor_driver[2][3] =
{
  {7,8,6}, //IN1,IN2,ENA
  {10,11,9}, //IN3,IN4,ENB
};



/////////////////////////////////
// BLE handle and definitions //
///////////////////////////////

BLEPeripheral BLE_Peripheral;                                                                                        // BLE peripheral instance
BLEService Rover_Service("da699607-dbc2-4776-82f6-80011575daa0");                                          // Create Rover Service with some uuid
BLEIntCharacteristic Direction_Characteristic("2895b648-99c4-46c5-911e-5adfcd8d821e", BLERead | BLEWrite);           // Characterisitic (type Int) for directions. 1-up,2-right,3-down,4-left
BLEIntCharacteristic Laser_Characteristic("6fe8cac0-3d98-4a4d-bca4-71a85e11c2fd", BLERead | BLEWrite | BLENotify);   // Characterisitic (type Int) for laser
BLEIntCharacteristic Servo_Characteristic("ef4b54ab-ea85-44cc-be7a-4ee2871b4f42", BLERead | BLEWrite | BLENotify);   // Characterisitic (type Int) for laser



////////////
// Setup //
//////////

void setup() 
{ 
  // initialize Serial
  Serial.begin(9600); 

  // LED to indicate bluetooth connection status
  pinMode(connect_led_pin, OUTPUT);
  
  // setup all pins being used by both motor driver as OUTPUT
  for(int i=0; i<2; i++) // loop over both motors
  {
    for(int j=0; j<3; j++) // loop over all pins of motor
    {
      pinMode(left_motor_driver[i][j],OUTPUT);
      pinMode(right_motor_driver[i][j],OUTPUT);  
    }
  }

  // Set Local name for BLE Peripheral
  BLE_Peripheral.setLocalName("Rover");
  
  // add service and characterisitics
  BLE_Peripheral.addAttribute(Rover_Service);
  BLE_Peripheral.addAttribute(Laser_Characteristic);
  BLE_Peripheral.addAttribute(Servo_Characteristic);
  BLE_Peripheral.addAttribute(Direction_Characteristic);
  
  // Initialize all characteristics to zero
  Laser_Characteristic.setValue(0);
  Direction_Characteristic.setValue(0);
  Servo_Characteristic.setValue(0);

  // Start advertising the service
  BLE_Peripheral.begin();
}



///////////
// Main //
/////////

void loop() 
{  
  // Keep polling over the Peripheral
  BLE_Peripheral.poll();

  // Check BLE connection before executing any code
  if (BLE_Peripheral.connected())
  {
    // Turn on connection LED
    digitalWrite(connect_led_pin, HIGH);

    //Check if Directional buttons on App are pressed
    Rover_Direction_Control();
  }
}



//////////////////////////////
// Rover Direction Control //
////////////////////////////

void Rover_Direction_Control()
{
  if (Direction_Characteristic.written())
  {
    switch (Direction_Characteristic.value())
    {
       
      case UP:
      drive_left_motors(FORWARD,128);
      drive_right_motors(FORWARD,128);
      break;

      case DOWN:
      drive_left_motors(BACKWARD,128);
      drive_right_motors(BACKWARD,128);
      break;

      case RIGHT:
      drive_left_motors(FORWARD,255);
      drive_right_motors(BACKWARD,10);
      break;

      case LEFT:
      drive_left_motors(BACKWARD,10);
      drive_right_motors(FORWARD,255);
      break;

      default:
      drive_left_motors(STOP,0);
      drive_right_motors(STOP,0);
    }
  }
}



////////////////////////////////////
// function to drive left motors //
//////////////////////////////////

void drive_left_motors(int motor_direction, int speed)
{  
  for(int i = 0; i < 2; i++)
  {
    switch(motor_direction)
    {
      case FORWARD:
      digitalWrite(left_motor_driver[i][0], HIGH); //IN1: high
      digitalWrite(left_motor_driver[i][1], LOW); //IN2: low
      analogWrite(left_motor_driver[i][2],speed); //EN: PWM
      break;

      case BACKWARD:
      digitalWrite(left_motor_driver[i][0], LOW); //IN1: low
      digitalWrite(left_motor_driver[i][1], HIGH); //IN2: high
      analogWrite(left_motor_driver[i][2],speed); //EN: PWM 
      break;

      case STOP:
      analogWrite(left_motor_driver[i][2],0); //EN: low
      break;
    }
  }
}



/////////////////////////////////////
// function to drive right motors //
///////////////////////////////////

void drive_right_motors(int motor_direction, int speed)
{
   for(int i = 0; i < 2; i++)
  {
    switch(motor_direction)
    {
      case FORWARD:
      digitalWrite(right_motor_driver[i][0], HIGH); //IN1: high
      digitalWrite(right_motor_driver[i][1], LOW); //IN2: low
      analogWrite(right_motor_driver[i][2],speed); //EN: PWM
      break;

      case BACKWARD:
      digitalWrite(right_motor_driver[i][0], LOW); //IN1: low
      digitalWrite(right_motor_driver[i][1], HIGH); //IN2: high
      analogWrite(right_motor_driver[i][2],speed); //EN: PWM
      break;

      case STOP:
      analogWrite(right_motor_driver[i][2],0); //EN: low
      break;
    }
  }
}
