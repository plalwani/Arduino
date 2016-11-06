//**********************
// LED control via BLE *
//**********************

// This code lets you turn an LED on/off via BLE
// UUID generated from: https://www.uuidgenerator.net
//Author: Prashant Lalwani


////////////////
// Libraries //
//////////////

#include <CurieBLE.h> // Curie Bluetooth Library


///////////////
// Pin list //
/////////////

const int led_pin = 13; // pin being used to control LED


/////////////////////////////////
// BLE handle and definitions //
///////////////////////////////

// BLE peripheral instance
BLEPeripheral BLE_Peripheral;  
// Create LED Service with some uuid
BLEService BLE_LED_Service("c0ff20c3-4861-441c-9bec-5fee6f5b4f28");  
// Characterisitic (type Int) for LED control
BLEIntCharacteristic BLE_LED_Characteristic("f1de4f2e-8e6d-42bd-b14e-b0309f7f57b1", BLERead | BLEWrite);  


/////////////////
// setup code // 
///////////////

void setup() {
// Setting up serial connection
Serial.begin(9600);
  
// specifying connection LED pin as output
pinMode(led_pin, OUTPUT);

// Set Local name for BLE Peripheral
BLE_Peripheral.setLocalName("Prashant_BLE_LED");  

// add service and characterisitics
BLE_Peripheral.addAttribute(BLE_LED_Service);
BLE_Peripheral.addAttribute(BLE_LED_Characteristic);

// Initialize all characteristics to zero
BLE_LED_Characteristic.setValue(0);

// Start advertising the service
BLE_Peripheral.begin();
}


////////////////
// Main Loop //
//////////////

void loop() {
// Keep polling over the Peripheral
  BLE_Peripheral.poll();

// Check BLE connection before executing any code
if (BLE_Peripheral.connected())
{
  if(BLE_LED_Characteristic.written())
  {
    if(BLE_LED_Characteristic.value() == 1)
    {
      Serial.println("LED On");
      digitalWrite(led_pin, HIGH);  
    }
    else
    {
      Serial.println("LED Off");
      digitalWrite(led_pin, LOW);
    } 
  }
}
}
