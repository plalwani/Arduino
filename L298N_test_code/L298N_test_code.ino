// When buying motor drivers in bulk, it is essential to test them out
// This simple code helps test L298N motor driver
// Author: Prashant Lalwani

// motor driver array specifying pinouts 
int motor_driver[2][3] =
{
  {0,1,3}, //IN1,IN2,ENA
  {2,4,5}, //IN3,IN4,ENB
};
void setup() {
  
// setup all pins being used by motor driver as OUTPUT
for(int i=0; i<2; i++) // loop over both motors
{
  for(int j=0; j<3; j++) // loop over all pins of motor
  {
    pinMode(motor_driver[i][j],OUTPUT);  
  }
}

}

void loop() {
  
  // Run motor forward for 5 seconds and then backwards for 5 seconds. Repeat for MotorB
  for(int i=0;i<2;i++)
  {
    digitalWrite(motor_driver[i][0],HIGH);
    digitalWrite(motor_driver[i][1],LOW);
    analogWrite(motor_driver[i][2],128); //50% speed
    delay(5000); //delay 5 second
    // Flip direction
    digitalWrite(motor_driver[i][0],LOW);
    digitalWrite(motor_driver[i][1],HIGH);
    delay(5000); //delay 5 seconds
    // STOP motor
    analogWrite(motor_driver[i][2],0);
    delay(5000);
  }
  
}
