// Author: Prashant Lalwani and Purval Sule

// This code drives 4 sets of motor on a 4WD rover using a grove I2C motor driver shield
// and an app built using mit app inventor available here:
// TBD

// Note: motors were wired such that all of them rotate in the same direction when moving forward
// other users may have to either change wiring or directions in code accordingly

// UUID generated from: https://www.uuidgenerator.net

/* Motor driver example code copied from:
  https://github.com/Seeed-Studio/Grove_I2C_Motor_Driver
*/


////////////////
// Libraries //
//////////////

// Curie Bluetooth Library
#include <CurieBLE.h>
// I2C Library
#include <Wire.h>


//////////////
// Defines //
////////////

// defines required by motor driver functions
#define MotorSpeedSet             0x82
#define PWMFrequenceSet           0x84
#define DirectionSet              0xaa
#define MotorSetA                 0xa1
#define MotorSetB                 0xa5
#define Nothing                   0x01
// Note: each I2C motor driver has a unique address (selectable on board)
#define I2CMotorDriver_right_Addr 0x0f   // Set the address of the I2CMotorDriver - right
#define I2CMotorDriver_left_Addr  0x01   // Set the address of the I2CMotorDriver - left


//////////////////////////////////////
// Motor Speed  and direction task //
////////////////////////////////////

// Function to set Motor A and B speed as well as direction for a specific motor driver (address)
// Note: 1001 indicates M1 in forward and M2 in reverse direction
// Note: Bit 0 and 1 should be a complement and Bit 1 and 2 should be complement
void Set_MotorSpeed_and_direction(unsigned char MotorSpeedA, unsigned char MotorSpeedB, unsigned char Direction, unsigned char I2C_MotorDriver_Addr)
{

  //Convert 0-100% to analog values 0-255
  MotorSpeedA = map(MotorSpeedA, 0, 100, 0, 255);
  MotorSpeedB = map(MotorSpeedB, 0, 100, 0, 255);

  // Speed
  Wire.beginTransmission(I2C_MotorDriver_Addr); // transmit to specified address
  Wire.write(MotorSpeedSet);                    // set pwm header
  Wire.write(MotorSpeedA);                      // send pwma
  Wire.write(MotorSpeedB);                      // send pwmb
  Wire.endTransmission();               	// stop transmitting

  // Direction
  Wire.beginTransmission(I2C_MotorDriver_Addr); // transmit to device I2CMotorDriverAdd
  Wire.write(DirectionSet);                     // Direction control header
  Wire.write(Direction);                        // send direction control information
  Wire.write(Nothing);                          // need to send this byte as the third byte(no meaning)
  Wire.endTransmission();                       // stop transmitting

}


///////////////
// Pin list //
/////////////

const int connect_led_pin = 13; // pin used for connect status LED


/////////////////////////////////
// BLE handle and definitions //
///////////////////////////////

BLEPeripheral BLE_Peripheral; // BLE peripheral instance
BLEService Intel_4wd_Rover_Service("da699607-dbc2-4776-82f6-80011575daa0"); // Create Intel  4wd Rover Service with some uuid
BLEIntCharacteristic Direction_Characteristic("2895b648-99c4-46c5-911e-5adfcd8d821e", BLERead | BLEWrite); //Characterisitic (type Int) for directions. 1-up,2-right,3-down,4-left


/////////////////
// Setup Loop //
///////////////

void setup()
{

  // Setting up serial connection
  Serial.begin(9600);

  Wire.begin(); // join i2c bus (address optional for master)
  delayMicroseconds(10000); // wait to make sure I2C is initialized

  // specifying LED pin as output
  pinMode(connect_led_pin, OUTPUT);

  // Set Local name for BLE Peripheral
  BLE_Peripheral.setLocalName("Intel_4WD_Rover_1");

  // add service and characterisitics
  BLE_Peripheral.addAttribute(Intel_4wd_Rover_Service);
  BLE_Peripheral.addAttribute(Direction_Characteristic);

  // Start advertising the service
  BLE_Peripheral.begin();

}


////////////////
// Main loop //
//////////////

void loop() 
{
  
  // Keep polling over the Peripheral
  BLE_Peripheral.poll();

  // Check BLE connection and turn on LED when connected else OFF
  if (BLE_Peripheral.connected())
  {
    digitalWrite(connect_led_pin, HIGH);
    //Check if Left button on App is pressed
    if (Direction_Characteristic.written())
    {
      switch (Direction_Characteristic.value())
      {

      case 1:
      {
        // UP
        Set_MotorSpeed_and_direction(60, 60, 1010, I2CMotorDriver_right_Addr);
        Set_MotorSpeed_and_direction(60, 60, 1010, I2CMotorDriver_left_Addr);
        break;
      }
      case 2:
      {
        // RIGHT
        Set_MotorSpeed_and_direction(100, 100, 1010, I2CMotorDriver_right_Addr);
        Set_MotorSpeed_and_direction(1, 1, 0101, I2CMotorDriver_left_Addr);
        break;
      }
      case 3:
      {
        // DOWN
        Set_MotorSpeed_and_direction(60, 60, 0101, I2CMotorDriver_right_Addr);
        Set_MotorSpeed_and_direction(60, 60, 0101, I2CMotorDriver_left_Addr);
        break;
      }
      case 4:
      {
        // LEFT
        Set_MotorSpeed_and_direction(1, 1, 0101, I2CMotorDriver_right_Addr);
        Set_MotorSpeed_and_direction(100, 100, 1010, I2CMotorDriver_left_Addr);
        break;
      }
      default:
      {
        // OFF
        Set_MotorSpeed_and_direction(0, 0, 1010, I2CMotorDriver_right_Addr);
        Set_MotorSpeed_and_direction(0, 0, 1010, I2CMotorDriver_left_Addr);
        break;
      }

      } // switch  
    } // if(Direction_Characteristic.written())
  } // if (BLE_Peripheral.connected())
  else
  {
    // Turn off connection LED
    digitalWrite(connect_led_pin, LOW);
    // Turn off all Motors (to be safe)
    Set_MotorSpeed_and_direction(0, 0, 1010, I2CMotorDriver_right_Addr);
    Set_MotorSpeed_and_direction(0, 0, 1010, I2CMotorDriver_left_Addr);
  } 
} // void loop()

