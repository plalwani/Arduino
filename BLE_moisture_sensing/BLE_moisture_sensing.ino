//***********************************
//  BLE Plant Moisture Sensing code *
//***********************************

// 1. This code lets the user sense moisture in soil and
// display the value on serial monitor
// 2. Based on a threshold, led turns on indicating
// need for watering the plants
// 3. Sensor value can be read over BLE using 
// nRF master BLE app

// Author: Prashant Lalwani


////////////////
// Libraries //
//////////////

#include <CurieBLE.h> // Curie Bluetooth Library


////////////////////////////
// variable declarations //
//////////////////////////

const int moisture_sensor_pin = A0; // pin A0 should be connected to sensor
int sensor_value; // variable to store sensor data
int moisture_threshold = 800; // threshold to control when to water the plants
const int led_pin = 13; // indication led for low moisture


/////////////////////////////////
// BLE handle and definitions //
///////////////////////////////

// BLE peripheral instance
BLEPeripheral BLE_Peripheral;                                                                                     
// Create moisture sensing Service with some uuid   
BLEService BLE_moisture_Service("c0ff20c3-4861-441c-9bec-5fee6f5b4f28");                                          
// Characterisitic (type Int) for moisture sensing
BLEIntCharacteristic BLE_moisture_Characteristic("f1de4f2e-8e6d-42bd-b14e-b0309f7f57b1", BLERead | BLEWrite);           


/////////////////
// setup code //
///////////////

void setup() {
pinMode(led_pin, OUTPUT); // specifying led pin as output
Serial.begin(9600); // initialize serial monitor

// Set Local name for BLE Peripheral
BLE_Peripheral.setLocalName("Prashant_BLE_moisture");  

// add service and characterisitics
BLE_Peripheral.addAttribute(BLE_moisture_Service);
BLE_Peripheral.addAttribute(BLE_moisture_Characteristic);

// Initialize all characteristics to zero
BLE_moisture_Characteristic.setValue(0);

// Start advertising the service
BLE_Peripheral.begin();
}


////////////////
// Main loop //
//////////////

void loop() {
// Keep polling over the Peripheral
BLE_Peripheral.poll();

// Read sensor data  
sensor_value = analogRead(moisture_sensor_pin);

// Print data on serial monitor
Serial.print("Sensor value: ");
Serial.println(sensor_value);

// check if sensor value > threshold to water plants
if(sensor_value > moisture_threshold)
{
  // Print message, write 1 to BLE and turn on LED
  Serial.println("Plants need Water");
  digitalWrite(led_pin, HIGH);
  BLE_moisture_Characteristic.setValue(1);
}
else
{
  // turn LED off and write 0 to BLE
  digitalWrite(led_pin, LOW);
  BLE_moisture_Characteristic.setValue(0);
}

// delay controls how often to test for moisture
delay(2000);

}
