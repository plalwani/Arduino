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
//CurieT
#include <CurieTime.h>

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


// Parameters for black and white

#define color_black               1
#define color_white               0

///////////////
// Pin list //
/////////////

const int connect_led_pin = 13; // pin used for connect status LED
const int ir_ana_in = 0;  // Analog input from IR receiver 
const int led_thresh = 2; // LED to indicate if the power up is received/sensed from the IR circuit
const int laser_diode = 2;
//////////////////////////////////
// Global Variables Init
/////////////////////////////////


int i;

//const int ambient = 1 ; // does ambient reflect ( 0 == less than) or absorb (1 == greater than)
const int transitions = 4 ; // No. of alternating stripes
const int powerup_detect_interval = 1200; // Amount of time for which Curie must advertise that powerup has been picked for the App to register
const int laser_interval = 5000; // LED on for 5 seconds

int count = 0;
int prev_color = 0, next_color = 0;
int powerup_picked = 0; 
int calibration_done = 0;
long int current_time , previous_time;


int threshold_black; //= 200 ( < ~200 )
int threshold_white;// = 900 ( > ~900 ) 

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
  Wire.endTransmission();                       // stop transmitting

  // Direction
  Wire.beginTransmission(I2C_MotorDriver_Addr); // transmit to device I2CMotorDriverAdd
  Wire.write(DirectionSet);                     // Direction control header
  Wire.write(Direction);                        // send direction control information
  Wire.write(Nothing);                          // need to send this byte as the third byte(no meaning)
  Wire.endTransmission();                       // stop transmitting
}

// Calibrate on big black dot away from people
void calibrate(){
  int avg = 0; int sum = 0;
  for(int a=1;a<1000;a++){
     sum = sum + analogRead(ir_ana_in);
     avg = sum/a;
     //avg = ((avg * (a-1)) + analogRead(ir_ana_in))/a;   
     delay(1);  
  }
    threshold_black = avg + 150;
    threshold_white = avg + 750;
    Serial.print("Threshold black is :");
    Serial.println(threshold_black);
    Serial.print("Threshold white is :");
    Serial.println(threshold_white);
    calibration_done = 1;
}


/////////////////////////////////
// BLE handle and definitions //
///////////////////////////////

BLEPeripheral BLE_Peripheral;                                                                              // BLE peripheral instance
BLEService Intel_4wd_Rover_Service("da699607-dbc2-4776-82f6-80011575daa0");                                // Create Intel  4wd Rover Service with some uuid
BLEIntCharacteristic Direction_Characteristic("2895b648-99c4-46c5-911e-5adfcd8d821e", BLERead | BLEWrite); // Characterisitic (type Int) for directions. 1-up,2-right,3-down,4-left
BLEIntCharacteristic PowerUp_Characteristic("6fe8cac0-3d98-4a4d-bca4-71a85e11c2fd", BLERead | BLEWrite);   // Characterisitic (type Int) for power-ups


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

  // specifying LED pin as output
  pinMode(connect_led_pin, OUTPUT);

  // Set Local name for BLE Peripheral
  BLE_Peripheral.setLocalName("Intel_4WD_Rover_1");

  // add service and characterisitics
  BLE_Peripheral.addAttribute(Intel_4wd_Rover_Service);
  BLE_Peripheral.addAttribute(Direction_Characteristic);
  BLE_Peripheral.addAttribute(PowerUp_Characteristic);

  // Reset calibration 
  calibration_done = 0;
  
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
    
// Calibrate on the big black dot    
    if(calibration_done == 0){
        calibrate();
    }
    digitalWrite(connect_led_pin, HIGH);
    
    //Check if Directional buttons on App are pressed
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
        Set_MotorSpeed_and_direction(0, 0, 0b1010, I2CMotorDriver_right_Addr);
        Set_MotorSpeed_and_direction(0, 0, 0b1010, I2CMotorDriver_left_Addr);
        break;
      }
      } // switch  
    } // if(Direction_Characteristic.written())

// start of code for laser characteristic
/*
 * Reset the powerup_picked variable here  - if characteristic is written 
 * 
 */
    if(PowerUp_Characteristic.written())
    {
        if(PowerUp_Characteristic.value() == 2)
       {
         Serial.println("Pew Pew");
         digitalWrite(laser_diode, HIGH);       
         // Add pew pew code here
         previous_time = millis();
       }
    }


// end of code for laser characteristic
 
/**************** IR code portion ************************/
/********************************************************** 

 Black shows a value of < 100 typically and white shows a value of > 900 with the ambient coming in at ~850+.
 In any case, it is clear that the ambient won't come close to black.
 The code makes ensures the need for black - white transitions to recognise a pattern.
 Also pattern with thin lines does not work.

***********************************************************/
  
  //delay(smpl);// wait for a hundredth of a second and log values
  //delay(1000); // For test
  i = analogRead(ir_ana_in);
  //Serial.print("Analog Value is: "); // For test
  //Serial.println(i); // For test

// We know ambient is reflecting
//  if (ambient == 1){

  if (powerup_picked == 0){ // Comment out this line if getting confused. Code will still work fine 
    if (i < threshold_black){ // This is the exception i.e black
       next_color = 1;
       if (prev_color != next_color){
          count++;
          Serial.print("Count is :");
          Serial.println(count);
        }
        prev_color = 1;
    }// if (i < threshold_black)  

    if (i > threshold_white){ // Get ready for sensing next black only if a white comes in between
        next_color = 0;
        prev_color = 0;
    } // if (i > threshold_white)

    if( count == transitions){
       //digitalWrite(led_thresh, HIGH);
       // delay(400);
       // Here write to the laser characteristic. Enable laser in the app 
       count = 0;
       prev_color  = 0;
       next_color  = 0;
       powerup_picked = 1;
       PowerUp_Characteristic.setValue(1);
       previous_time = millis(); 
    } else {
      digitalWrite(led_thresh, LOW);
    } // if (count == transitions)
  } // If powerup_picked == 0

  if (powerup_picked == 1){
      if (PowerUp_Characteristic.value() == 1){ // Don't want the value of 2 to be overwritten with 0
        current_time = millis();
        if (current_time - previous_time  > powerup_detect_interval){          
          PowerUp_Characteristic.setValue(0);
        }
      } else if (PowerUp_Characteristic.value() == 2){
        current_time = millis();
        if (current_time - previous_time  > laser_interval){          
          PowerUp_Characteristic.setValue(0);
          digitalWrite(laser_diode, LOW);
          powerup_picked = 0;
          }
      }
  }

  
  } // if (BLE_Peripheral.connected())
  else
  {
    // Turn off connection LED
    digitalWrite(connect_led_pin, LOW);
    // Turn off all Motors (to be safe)
    Set_MotorSpeed_and_direction(0, 0, 0b1010, I2CMotorDriver_right_Addr);
    Set_MotorSpeed_and_direction(0, 0, 0b1010, I2CMotorDriver_left_Addr);
    // Reset variables and restart at the black hole :)
    //calibration_done = 0;
    count = 0;
    prev_color  = 0;
    next_color  = 0;
    powerup_picked = 0;
  } 
} // void loop()


/*
millis();

// debug code, to be removed
    if(digitalRead(debug_pin))
    {
      PowerUp_Characteristic.setValue(1);
      Serial.println("Button pressed");
    }
    else
    {
      PowerUp_Characteristic.setValue(0);
    }

    if(PowerUp_Characteristic.written())
    {
      if(PowerUp_Characteristic.value() == 2)
      {
        Serial.println("Pew Pew");
        // Add pew pew code here
      }
    }
*/

 
