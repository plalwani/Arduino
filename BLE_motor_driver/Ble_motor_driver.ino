/* This code sets up the Arduino as BLE peripheral and waits for connection from the App
  Once the connection is established, connect LED turns on
  Left, Right and Accelerate button control three different LEDs respectively
*/
/*
  Grove- i2C motor driver demo v1.0
  by: http://www.seeedstudio.com
//  Author:LG
//  
//  
//  This demo code is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//
*/
// UUID generated from: https://www.uuidgenerator.net

// Author: Prashant Lalwani and Purval Sule

// Curie Bluetooth Library
#include <CurieBLE.h>
#include <Wire.h>

#define MotorSpeedSet             0x82
#define PWMFrequenceSet           0x84
#define DirectionSet              0xaa
#define MotorSetA                 0xa1
#define MotorSetB                 0xa5
#define Nothing                   0x01
//#define EnableStepper             0x1a
//#define UnenableStepper           0x1b
//#define Stepernu                  0x1c
#define I2CMotorDriverAdd_f         0x0f   // Set the address of the I2CMotorDriver - front
#define I2CMotorDriverAdd_b         0x0a   // Set the address of the I2CMotorDriver - back

//////////////////////////////////////////////////////////////////////
//Function to set the 2 DC motor speed
//motorSpeedA : the DC motor A speed; should be 0~100;
//motorSpeedB: the DC motor B speed; should be 0~100;

// Procedures for the motors in the front
void MotorSpeedSetAB_f(unsigned char MotorSpeedA , unsigned char MotorSpeedB)  {
  MotorSpeedA=map(MotorSpeedA,0,100,0,255);
  MotorSpeedB=map(MotorSpeedB,0,100,0,255);
  Wire.beginTransmission(I2CMotorDriverAdd_f); // transmit to device I2CMotorDriverAdd
  Wire.write(MotorSpeedSet);        // set pwm header 
  Wire.write(MotorSpeedA);              // send pwma 
  Wire.write(MotorSpeedB);              // send pwmb    
  Wire.endTransmission();    // stop transmitting
}

//set the direction of DC motor. 
void MotorDirectionSet_f(unsigned char Direction)  {     //  Adjust the direction of the motors 0b0000 I4 I3 I2 I1
  Wire.beginTransmission(I2CMotorDriverAdd_f); // transmit to device I2CMotorDriverAdd
  Wire.write(DirectionSet);        // Direction control header
  Wire.write(Direction);              // send direction control information
  Wire.write(Nothing);              // need to send this byte as the third byte(no meaning)  
  Wire.endTransmission();    // stop transmitting 
}

void MotorDriectionAndSpeedSet_f(unsigned char Direction,unsigned char MotorSpeedA,unsigned char MotorSpeedB)  {  //you can adjust the driection and speed together
  MotorDirectionSet_f(Direction);
  MotorSpeedSetAB_f(MotorSpeedA,MotorSpeedB);  
}

// Procedures for motors at the back

void MotorSpeedSetAB_b(unsigned char MotorSpeedA , unsigned char MotorSpeedB)  {
  MotorSpeedA=map(MotorSpeedA,0,100,0,255);
  MotorSpeedB=map(MotorSpeedB,0,100,0,255);
  Wire.beginTransmission(I2CMotorDriverAdd_b); // transmit to device I2CMotorDriverAdd
  Wire.write(MotorSpeedSet);        // set pwm header 
  Wire.write(MotorSpeedA);              // send pwma 
  Wire.write(MotorSpeedB);              // send pwmb    
  Wire.endTransmission();    // stop transmitting
}

//set the direction of DC motor. 
void MotorDirectionSet_b(unsigned char Direction)  {     //  Adjust the direction of the motors 0b0000 I4 I3 I2 I1
  Wire.beginTransmission(I2CMotorDriverAdd_b); // transmit to device I2CMotorDriverAdd
  Wire.write(DirectionSet);        // Direction control header
  Wire.write(Direction);              // send direction control information
  Wire.write(Nothing);              // need to send this byte as the third byte(no meaning)  
  Wire.endTransmission();    // stop transmitting 
}

void MotorDriectionAndSpeedSet_b(unsigned char Direction,unsigned char MotorSpeedA,unsigned char MotorSpeedB)  {  //you can adjust the driection and speed together
  MotorDirectionSet_b(Direction);
  MotorSpeedSetAB_b(MotorSpeedA,MotorSpeedB);  
}


// Pin list
const int connect_led_pin = 13; // pin used for connect status LED
const int left_led_pin = 12; // pin used for left button press LED
const int right_led_pin = 11; // pin used for right button press LED
const int accelerate_led_pin = 10; // pin used for Accelerate button press LED

int servo_count;

BLEPeripheral BLE_Peripheral; // BLE peripheral instance
BLEService Intel_Kart_Service("da699607-dbc2-4776-82f6-80011575daa0"); // Create Intel Kart Service with some uuid
BLEIntCharacteristic Left_Button_Characteristic("2895b648-99c4-46c5-911e-5adfcd8d821e", BLERead | BLEWrite); //Characterisitic (type Int) for Left button within Intel_Kart_Service
BLEIntCharacteristic Right_Button_Characteristic("557645c2-a52c-42dc-b164-6c9e7b65f114", BLERead | BLEWrite); //Characterisitic (type Int) for Right button within Intel_Kart_Service
BLEIntCharacteristic Accelerate_Button_Characteristic("aca57d8a-051a-4298-ae92-010e4079f026", BLERead | BLEWrite); //Characterisitic (type Int) for Accelerate button within Intel_Kart_Service



// Setup Loop
void setup()
{

  // Setting up serial connection
  Serial.begin(9600);

  Wire.begin(); // join i2c bus (address optional for master)
  delayMicroseconds(10000);
  Serial.println("setup begin");
  //stepperrun();

  // specifying all LED pins as output
  pinMode(connect_led_pin, OUTPUT);
  pinMode(left_led_pin, OUTPUT);
  pinMode(right_led_pin, OUTPUT);
  pinMode(accelerate_led_pin, OUTPUT);

  // Set Local name for BLE Peripheral
  BLE_Peripheral.setLocalName("Intel_Kart_Player_1");

  // add service and characterisitics
  BLE_Peripheral.addAttribute(Intel_Kart_Service);
  BLE_Peripheral.addAttribute(Left_Button_Characteristic);
  BLE_Peripheral.addAttribute(Right_Button_Characteristic);
  BLE_Peripheral.addAttribute(Accelerate_Button_Characteristic);

  // Start advertising the service
  BLE_Peripheral.begin();
}


// Main loop
void loop() {
  // Keep polling over the Peripheral
  BLE_Peripheral.poll();

  // Check BLE connection and turn on LED when connected else OFF
  if (BLE_Peripheral.connected())
  {
    digitalWrite(connect_led_pin, HIGH);
      //Check if Left button on App is pressed
    if (Left_Button_Characteristic.written())
    { 
      if (Left_Button_Characteristic.value() == 1)
      {
        digitalWrite(left_led_pin, HIGH);
        MotorDriectionAndSpeedSet_f(0b1010,100,100);//defines the direction and speed of the forward left and forward right motors."0b1010" defines the output polarity, "10" means the M+ is "positive" while the M- is "negtive" 
        MotorDriectionAndSpeedSet_b(0b1010,100,100);//defines the direction and speed of the forward left and forward right motors."0b1010" defines the output polarity, "10" means the M+ is "positive" while the M- is "negtive" 
      }
       else
      {
        digitalWrite(left_led_pin, LOW);
        MotorDriectionAndSpeedSet_f(0b1001,0,0);//defines the direction and speed of the forward left and forward right motors."0b1010" defines the output polarity, "10" means the M+ is "positive" while the M- is "negtive" 
        MotorDriectionAndSpeedSet_b(0b1001,0,0);//defines the direction and speed of the forward left and forward right motors."0b1010" defines the output polarity, "10" means the M+ is "positive" while the M- is "negtive" 
      }
     // Serial.print("left led written\n");  

    //}else{
    //    digitalWrite(left_led_pin, LOW);
    }

    //Check if Right button on App is pressed
    if (Right_Button_Characteristic.written())
    {
     if (Right_Button_Characteristic.value() == 1)
     {
       digitalWrite(right_led_pin, HIGH);
        MotorDriectionAndSpeedSet_f(0b0101,100,100);//defines the direction and speed of the forward left and forward right motors."0b1010" defines the output polarity, "10" means the M+ is "positive" while the M- is "negtive" 
        MotorDriectionAndSpeedSet_b(0b0101,100,100);//defines the direction and speed of the forward left and forward right motors."0b1010" defines the output polarity, "10" means the M+ is "positive" while the M- is "negtive"  
     }
     else
     {
        digitalWrite(right_led_pin, LOW);
        MotorDriectionAndSpeedSet_f(0b1001,0,0);//defines the direction and speed of the forward left and forward right motors."0b1010" defines the output polarity, "10" means the M+ is "positive" while the M- is "negtive" 
        MotorDriectionAndSpeedSet_b(0b1001,0,0);//defines the direction and speed of the forward left and forward right motors."0b1010" defines the output polarity, "10" means the M+ is "positive" while the M- is "negtive" 
     }
     // Serial.print("right led written\n");
    }


   //Check if Accelerate button on App is pressed
    if (Accelerate_Button_Characteristic.written())
    {
      if (Accelerate_Button_Characteristic.value() == 1)
      {
          digitalWrite(accelerate_led_pin, HIGH);
           MotorDriectionAndSpeedSet_b(0b1001,100,100);//defines the direction and speed of the forward left and forward right motors."0b1010" defines the output polarity, "10" means the M+ is "positive" while the M- is "negtive" 
          MotorDriectionAndSpeedSet_f(0b1001,100,100);//defines the direction and speed of the forward left and forward right motors."0b1010" defines the output polarity, "10" means the M+ is "positive" while the M- is "negtive" 
       }
      else
      {
          digitalWrite(accelerate_led_pin, LOW);
          MotorDriectionAndSpeedSet_f(0b1001,0,0);//defines the direction and speed of the forward left and forward right motors."0b1010" defines the output polarity, "10" means the M+ is "positive" while the M- is "negtive" 
          MotorDriectionAndSpeedSet_b(0b1001,0,0);//defines the direction and speed of the forward left and forward right motors."0b1010" defines the output polarity, "10" means the M+ is "positive" while the M- is "negtive" 
      }
     // Serial.print("accelerate led written\n");  
   }
  }
  else
  {
    digitalWrite(connect_led_pin, LOW);
    digitalWrite(accelerate_led_pin, LOW);
    digitalWrite(right_led_pin, LOW);
    digitalWrite(left_led_pin, LOW);
    MotorDriectionAndSpeedSet_f(0b1001,0,0);//defines the direction and speed of the forward left and forward right motors."0b1010" defines the output polarity, "10" means the M+ is "positive" while the M- is "negtive" 
    MotorDriectionAndSpeedSet_b(0b1001,0,0);//defines the direction and speed of the forward left and forward right motors."0b1010" defines the output polarity, "10" means the M+ is "positive" while the M- is "negtive" 
  }


}
