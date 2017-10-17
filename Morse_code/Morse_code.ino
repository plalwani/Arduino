// Morse_code.ino

// This code provides functions for Morse code dots dashes to allow user
// to write morse code words with ease
// Author: Prashant Lalwani

/////////////////////////////
// Pin list and variables //
///////////////////////////

const int led_pin =13;
const int speaker_pin = 7;
const int time_unit = 300;

/////////////////
// Setup Loop //
///////////////

void setup() {
pinMode(led_pin, OUTPUT);
pinMode(speaker_pin, OUTPUT);
}

////////////////
// Main loop //
//////////////

void loop() {
// Add your morse code below this line
// don't forget to add change_character();

end_code(); // do not delete
}



// function to transmit morse code dot on led_pin and speaker_pin
void dot()
{
digitalWrite(led_pin, HIGH);
digitalWrite(speaker_pin, HIGH);
delay(time_unit);
digitalWrite(led_pin, LOW);
digitalWrite(speaker_pin, LOW);
delay(time_unit);
}

// function to transmit morse code dash on led_pin and speaker_pin
void dash()
{
digitalWrite(led_pin, HIGH);
digitalWrite(speaker_pin, HIGH);
delay(3*time_unit);
digitalWrite(led_pin, LOW);
digitalWrite(speaker_pin, LOW);
delay(time_unit);
}

// function to change morse code letter
void change_character()
{
  delay(3*time_unit);
}

// function to add a huge delay in order to prevent morse code to loop over and cause confusion
void end_code()
{
  delay(500*time_unit);
}

