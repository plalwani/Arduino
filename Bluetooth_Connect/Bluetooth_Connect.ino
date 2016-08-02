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
BLECharacteristic Left_Button_Characteristic("FF01", BLERead | BLEWrite); //Characterisitic for Left button within Intel_Kart_Service
BLECharacteristic Right_Button_Characteristic("FF02", BLERead | BLEWrite); //Characterisitic for Left button within Intel_Kart_Service
BLECharacteristic Accelerate_Button_Characteristic("FF03", BLERead | BLEWrite); //Characterisitic for Left button within Intel_Kart_Service

// Setup Loop
void setup() 
{
// specifying all LED pins as output  
pinMode(connect_led_pin, OUTPUT);  
pinMode(left_led_pin, OUTPUT);  
pinMode(right_led_pin, OUTPUT);  
pinMode(accelerate_led_pin, OUTPUT);  

// Set Local name for BLE Peripheral
BLE_Peripheral.setLocalName("Intel_Kart_Player_1");
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

}
