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


///////////////
// Pin list //
/////////////

const int connect_led_pin = 2;  // pin used for connect status LED. Doubles up as Calibration failed
const int ir_receiver_pin = 0;   // pin being used to read from IR receiver
//const int power_up_led_pin = 12; // pin used for powerup notification (may not get used)
const int laser_diode_pin = 9;   // pin used for connecting laser powerup
const int IR_transmitter_pin = 13;// pin for transmitting IR - ON only when connected

///////////////////////
// Global Variables //
/////////////////////

const int color_transitions = 4; // powerup pattern (black and white stripes)
const int laser_interval = 5000; // laser turn on interval
int color_transition_count = 0;  // counter to count the # of transitions of black/white color pair
int black_color_detected = 0;    // black color detection flag
int white_color_detected = 0;    // white color detection flag
int IR_receiver_reading = 0;     // used for reading IR receiver
int calibration_done = 0;        // calibration completion flag
long int current_time = 0;
long int previous_time = 0;
int black_color_threshold = 0; // < ~200, actual values come from calibration
int white_color_threshold = 0; // > ~900, actual values come from calibration
int laser_on = 0;              // laser powerup used flag
int powerup_received = 0;      // flag to monitor laser powerup
const int min_color_difference = 400; // Min difference between black and white thresholds

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


///////////////////////
// Calibration task //
/////////////////////
BLEIntCharacteristic white_Characteristic("6fe8cac0-3d98-4a4d-bca4-71a85e11beef", BLERead | BLEWrite);   // Characterisitic (type Int) for power-ups
BLEIntCharacteristic black_Characteristic("6fe8cac0-3d98-4a4d-bca4-71a85e11dead", BLERead | BLEWrite);   // Characterisitic (type Int) for power-ups
// Task to Calibrate black threshold using a black sheet of paper before operating
void IR_calibration()
{
  int avg = 0; int sum = 0; int a;

  for (a = 1; a < 1001; a++)
  {
    sum = sum + analogRead(ir_receiver_pin);
    delay(1);
  }

  // white numbers are usually higher. 150 and 750 are arbitrary number
  avg = sum / a;
  white_color_threshold = avg - 40 ;

  Serial.print("White color threshold: ");
  Serial.println(white_color_threshold);

  avg = 0;
  sum = 0;

  Serial.println("Moving Forward");
  Set_MotorSpeed_and_direction(20, 20, 0b1010, I2CMotorDriver_right_Addr);
  Set_MotorSpeed_and_direction(20, 20, 0b1010, I2CMotorDriver_left_Addr);

  // Move forward till you find black
  while (analogRead(ir_receiver_pin) > 200) {
    avg = 0;
  }

  //delay(10);//Get closer to centre

  Serial.println("Stopping");
  Set_MotorSpeed_and_direction(0, 0, 0b1010, I2CMotorDriver_right_Addr);
  Set_MotorSpeed_and_direction(0, 0, 0b1010, I2CMotorDriver_left_Addr);

  delay(500);

  for (a = 1; a < 1001; a++)
  {
    sum = sum + analogRead(ir_receiver_pin);
    delay(1);
  }

  
  avg = sum / a;
  black_color_threshold = avg + 40 ;

  Serial.print("Black color threshold: ");
  Serial.println(black_color_threshold);

    
  if ((white_color_threshold - black_color_threshold) < min_color_difference){
// If calibration failed
    while(1){
      digitalWrite(connect_led_pin, HIGH);
      delay(400);
      digitalWrite(connect_led_pin, LOW);
      delay(400);
    }
    
  }
  
  white_Characteristic.setValue((white_color_threshold/4));
  black_Characteristic.setValue(black_color_threshold); 
  
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
  delayMicroseconds(2000);

  // specifying connection LED pin as output
  pinMode(connect_led_pin, OUTPUT);

  // specifying power up LED pin as output
  //pinMode(power_up_led_pin, OUTPUT);

  // specifying laser diode pin as output - Is now PWM output - not required
  //pinMode(laser_diode_pin, OUTPUT);
  analogWrite(laser_diode_pin, 0);

  // specifying IR transmitter pin as output
  pinMode(IR_transmitter_pin, OUTPUT);  


  // Set Local name for BLE Peripheral
  BLE_Peripheral.setLocalName("Intel_4WD_Rover_1");

  // add service and characterisitics
  BLE_Peripheral.addAttribute(Intel_4wd_Rover_Service);
  BLE_Peripheral.addAttribute(Direction_Characteristic);
  BLE_Peripheral.addAttribute(PowerUp_Characteristic);
  BLE_Peripheral.addAttribute(white_Characteristic);
  BLE_Peripheral.addAttribute(black_Characteristic);
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
    digitalWrite(IR_transmitter_pin, HIGH);

    // do IR calibration to get threshold values
    if (calibration_done == 0)
    {
      IR_calibration();
    }


    //Check if Directional buttons on App are pressed
    Rover_Direction_Control();

    // If App writes to powerup characteristic
    if (PowerUp_Characteristic.written() && (PowerUp_Characteristic.value() == 2))
    {
      Serial.println("Laser Fired");
      //digitalWrite(laser_diode_pin, HIGH);
      analogWrite(laser_diode_pin, 175);
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
      powerup_received = 0;
      color_transition_count = 0;
    }

    // IR receiver logic to detect color transitions
    IR_receiver_reading = analogRead(ir_receiver_pin);
    if ((IR_receiver_reading < black_color_threshold) && calibration_done)
    {
      black_color_detected = 1;
    }
    else if ((IR_receiver_reading > white_color_threshold) && calibration_done)
    {
      white_color_detected = 1;
    }

    if (white_color_detected && black_color_detected)
    {
      color_transition_count++;
      Serial.print("Count: ");
      Serial.println(color_transition_count);

      white_color_detected = 0;
      black_color_detected = 0;
    }

    if ((color_transition_count == color_transitions) && (powerup_received == 0))
    {
      PowerUp_Characteristic.setValue(1);
      powerup_received = 1;
    }

  } // if (BLE_Peripheral.connected())
  else
  {
    // Turn off connection LED
    digitalWrite(connect_led_pin, LOW);

    // Turn off all Motors (to be safe)
    Set_MotorSpeed_and_direction(0, 0, 0b1010, I2CMotorDriver_right_Addr);
    Set_MotorSpeed_and_direction(0, 0, 0b1010, I2CMotorDriver_left_Addr);
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
          Set_MotorSpeed_and_direction(0, 0, 0b1010, I2CMotorDriver_right_Addr);
          Set_MotorSpeed_and_direction(0, 0, 0b1010, I2CMotorDriver_left_Addr);
          break;
        }
    } // switch
  } // if(Direction_Characteristic.written())
}
