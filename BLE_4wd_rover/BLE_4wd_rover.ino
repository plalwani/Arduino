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
// Servo Library
#include <Servo.h>
Servo myservo;  // create servo object to control a servo


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


///////////////
// Pin list //
/////////////

const int connect_led_pin = 2;  // pin used for connect status LED. Doubles up as Calibration failed
const int laser_diode_pin = 9;   // pin used for connecting laser powerup
const int servo_pin = 14;// pin for controlling servo (Note: servo lib considers pin 14 as A0, servo actually connected to A0)

///////////////////////
// Global Variables //
/////////////////////

const int laser_interval =10000; // laser turn on interval
long int current_time = 0;
long int previous_time = 0;
int laser_on = 0;              // laser powerup used flag

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
  Wire.endTransmission();               	      // stop transmitting

  // Direction
  Wire.beginTransmission(I2C_MotorDriver_Addr); // transmit to device I2CMotorDriverAdd
  Wire.write(DirectionSet);                     // Direction control header
  Wire.write(Direction);                        // send direction control information
  Wire.write(Nothing);                          // need to send this byte as the third byte(no meaning)
  Wire.endTransmission();                       // stop transmitting
}


/////////////////////////////////
// BLE handle and definitions //
///////////////////////////////

BLEPeripheral BLE_Peripheral;                                                                                        // BLE peripheral instance
BLEService Intel_4wd_Rover_Service("da699607-dbc2-4776-82f6-80011575daa0");                                          // Create Intel  4wd Rover Service with some uuid
BLEIntCharacteristic Direction_Characteristic("2895b648-99c4-46c5-911e-5adfcd8d821e", BLERead | BLEWrite);           // Characterisitic (type Int) for directions. 1-up,2-right,3-down,4-left
BLEIntCharacteristic Laser_Characteristic("6fe8cac0-3d98-4a4d-bca4-71a85e11c2fd", BLERead | BLEWrite | BLENotify);   // Characterisitic (type Int) for laser
BLEIntCharacteristic Servo_Characteristic("ef4b54ab-ea85-44cc-be7a-4ee2871b4f42", BLERead | BLEWrite | BLENotify);   // Characterisitic (type Int) for laser


/////////////////
// Setup Loop //
///////////////

void setup()
{
  // Setting up serial connection
  Serial.begin(9600);

  // join i2c bus (address optional for master)
  Wire.begin();

  // wait to make sure I2C is initialized
  delayMicroseconds(10000);

  // specifying connection LED pin as output
  pinMode(connect_led_pin, OUTPUT);

  // Setting laser to be off by default
  analogWrite(laser_diode_pin, 0);
  
  // attach the servo on pin A0 (A0 is 14 in servo library) to the servo object
  myservo.attach(servo_pin);  

  // Setup the servo to be center aligned
  myservo.write(90);
  delay(15);

  // Set Local name for BLE Peripheral
  BLE_Peripheral.setLocalName("Intel_4WD_Rover");
  
  // add service and characterisitics
  BLE_Peripheral.addAttribute(Intel_4wd_Rover_Service);
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


////////////////
// Main loop //
//////////////

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

    // If App writes to powerup characteristic
    Laser_Control();

    // Check to see if App updated Laser direction
    Laser_Direction_Control();
    
  } // if (BLE_Peripheral.connected())
  else
  {
    // Turn off connection LED
    digitalWrite(connect_led_pin, LOW);
    
    // Turn off all Motors (to be safe)
    Set_MotorSpeed_and_direction(0, 0, 0b1010, I2CMotorDriver_right_Addr);
    Set_MotorSpeed_and_direction(0, 0, 0b1010, I2CMotorDriver_left_Addr);

    // Turn off Laser
    laser_on = 0;
    analogWrite(laser_diode_pin, 0);
    
    // Move servo to 90 degrees
    myservo.write(90);
    delay(15);
  }
} // void loop()


//////////////////////////////
// Rover Direction Control //
////////////////////////////

void Rover_Direction_Control()
{
  if (Direction_Characteristic.written())
  {
    switch (Direction_Characteristic.value())
    {
      case 1:
        {
          // UP
          Serial.println("Moving Forward");
          Set_MotorSpeed_and_direction(40, 40, 0b1010, I2CMotorDriver_right_Addr);
          Set_MotorSpeed_and_direction(40, 40, 0b1010, I2CMotorDriver_left_Addr);
          break;
        }
      case 2:
        {
          // RIGHT
          Serial.println("Moving Right");
          Set_MotorSpeed_and_direction(100, 100, 0b1010, I2CMotorDriver_right_Addr);
          Set_MotorSpeed_and_direction(1, 1, 0b0101, I2CMotorDriver_left_Addr);
          break;
        }
      case 3:
        {
          // DOWN
          Serial.println("Moving Backwards");
          Set_MotorSpeed_and_direction(40, 40, 0b0101, I2CMotorDriver_right_Addr);
          Set_MotorSpeed_and_direction(40, 40, 0b0101, I2CMotorDriver_left_Addr);
          break;
        }
      case 4:
        {
          // LEFT
          Serial.println("Moving Left");
          Set_MotorSpeed_and_direction(1, 1, 0b0101, I2CMotorDriver_right_Addr);
          Set_MotorSpeed_and_direction(100, 100, 0b1010, I2CMotorDriver_left_Addr);
          break;
        }
      default:
        {
          // OFF
          Serial.println("Turning Off all motors");
          Set_MotorSpeed_and_direction(0, 0, 0b1010, I2CMotorDriver_right_Addr);
          Set_MotorSpeed_and_direction(0, 0, 0b1010, I2CMotorDriver_left_Addr);
          break;
        }
    } // switch
  } // if(Direction_Characteristic.written())
}


////////////////////
// Laser Control //
//////////////////

void Laser_Control()
{
  if(Laser_Characteristic.written() && (Laser_Characteristic.value() == 1))
    {
      Serial.println("Laser Fired");
      analogWrite(laser_diode_pin, 128);
      laser_on = 1;
      // non blocking delay to keep laser on for a set duration
      previous_time = millis();
    }
    
    current_time = millis();
    // Turn off laser after set amount of time
    if ((current_time >= previous_time + laser_interval) && laser_on)
    {
      Serial.println("Laser turned off");
      //digitalWrite(laser_diode_pin, LOW);
      analogWrite(laser_diode_pin, 0);
      laser_on = 0;
    }
}

//////////////////////////////
// Servo Direction Control //
////////////////////////////

void Laser_Direction_Control()
{
  // update angle with new value
  if(Servo_Characteristic.written())
  {
    Serial.print("updating servo position with value ");
    Serial.println(Servo_Characteristic.value());
    myservo.write((180-Servo_Characteristic.value())); // Subtracting from 180 to align the directions between App's slider and rotation of servo.
    delay(15);
  }
}

