/* This code sets up the Arduino as BLE peripheral and waits for connection from the App
  Once the connection is established, connect LED turns on
  Left, Right and Accelerate button control three different LEDs respectively
*/

// UUID generated from: https://www.uuidgenerator.net

// Author: Prashant Lalwani

// Curie Bluetooth Library
#include <CurieBLE.h>

// Pin list
const int connect_led_pin = 13; // pin used for connect status LED
const int left_led_pin = 12; // pin used for left button press LED
const int right_led_pin = 11; // pin used for right button press LED
const int accelerate_led_pin = 10; // pin used for Accelerate button press LED

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
      }
       else
      {
        digitalWrite(left_led_pin, LOW);
      }
      Serial.print("left led written\n");  

   }

    //Check if Right button on App is pressed
    if (Right_Button_Characteristic.written())
    {
     if (Right_Button_Characteristic.value() == 1)
     {
       digitalWrite(right_led_pin, HIGH);
     }
     else
     {
        digitalWrite(right_led_pin, LOW);
     }
      Serial.print("right led written\n");
    }


   //Check if Accelerate button on App is pressed
    if (Accelerate_Button_Characteristic.written())
    {
      if (Accelerate_Button_Characteristic.value() == 1)
      {
          digitalWrite(accelerate_led_pin, HIGH);
      }
      else
      {
          digitalWrite(accelerate_led_pin, LOW);
      }
      Serial.print("accelerate led written\n");  
   }
  }
  else
  {
    digitalWrite(connect_led_pin, LOW);
    digitalWrite(accelerate_led_pin, LOW);
    digitalWrite(right_led_pin, LOW);
    digitalWrite(left_led_pin, LOW);
  }


}
