//******************************
// Plant Moisture Sensing code *
//******************************

// 1. This code lets the user sense moisture in soil and
// display the value on serial monitor
// 2. Based on a threshold, led turns on indicating
// need for watering the plants

// Author: Prashant Lalwani

////////////////////////////
// variable declarations //
//////////////////////////

const int moisture_sensor_pin = A0; // pin A0 connected to sensor
int sensor_value; // variable to store sensor data
int moisture_threshold = 800; // threshold to control when to water the plants
const int led_pin = 13; // indication led for low moisture


/////////////////
// setup code //
///////////////

void setup() {
pinMode(led_pin, OUTPUT); // specifying led pin as output
Serial.begin(9600); // initialize serial monitor
}


////////////////
// Main loop //
//////////////

void loop() {
// Read sensor data  
sensor_value = analogRead(moisture_sensor_pin);

// Print data on serial monitor
Serial.print("Sensor value: ");
Serial.println(sensor_value);

// check if sensor value > threshold to water plants
if(sensor_value > moisture_threshold)
{
  // Print message and turn on LED
  Serial.println("Plants need Water");
  digitalWrite(led_pin, HIGH);
}
else
{
  // turn LED off
  digitalWrite(led_pin, LOW);
}

// delay controls how often to test for moisture
delay(2000);

}
