/* This code sets up the Arduino as BLE peripheral and waits for connection from the App
Once the connection is established, connect LED turns on
Left, Right and Accelerate button control three different LEDs respectively 
 */

 // Author: Prashant Lalwani

// Curie Bluetooth Library
#include <CurieBLE.h>

const int connect_led_pin = 13; // pin used for connect status LED
const int left_led_pin = 12; // pin used for left button press LED
const int right_led_pin = 11; // pin used for right button press LED
const int accelerate_led_pin = 10; // pin used for Accelerate button press LED

BLEPeripheral BLE_Peripheral; // BLE peripheral instance
BLEService Intel_Kart_Service("FF00"); // Creat Intel Kart Service with some uuid
BLEIntCharacteristic Left_Button_Characteristic("FF01", BLERead | BLEWrite); //Characterisitic for Left button within Intel_Kart_Service
BLECharCharacteristic Right_Button_Characteristic("FF02", BLERead | BLEWrite); //Characterisitic for Left button within Intel_Kart_Service
BLECharCharacteristic Accelerate_Button_Characteristic("FF03", BLERead | BLEWrite); //Characterisitic for Left button within Intel_Kart_Service

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

// If connected then turn LED on  
if(BLE_Peripheral.connected())
{
  digitalWrite(connect_led_pin, HIGH);
}
else
{
  digitalWrite(connect_led_pin, LOW);
}

//Check if Left button on App is pressed
if(Left_Button_Characteristic.written())
{
  //if(Left_Button_Characteristic.value() == 1)
  //{
  //  digitalWrite(left_led_pin, HIGH);
    Serial.println("Left button pressed with value: ");
    Serial.println(Left_Button_Characteristic.value());  
  //}

}

}
