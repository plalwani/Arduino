//Test code with dual L298N motor drivers (4 motors)that utilizes serial input from keyboard
// to turn all four motors in specific direction to move a rover in different directions

//8: All motors forward
//2: All motors backwards
//4: Left motors forward
//6: Right motors forward
//5: STOP

// Author: Prashant Lalwani
// 10/27/2017



//////////////////////////////
// defines for ASCII input //
////////////////////////////

#define LEFT 52
#define RIGHT 54
#define UP 56
#define DOWN 50
#define STOP 53



/////////////////////////////////////////////////////////////////////////////
//defines to send to motor driver function indicating direction of motors //
///////////////////////////////////////////////////////////////////////////

#define FORWARD 8
#define BACKWARD 2



///////////////////////
// global variables //
/////////////////////

int direction;

// left motor driver array specifying pinouts 
int left_motor_driver[2][3] =
{
  {0,1,3}, //IN1,IN2,ENA
  {2,4,5}, //IN3,IN4,ENB
};

// right motor driver array specifying pinouts 
int right_motor_driver[2][3] =
{
  {7,8,6}, //IN1,IN2,ENA
  {10,11,9}, //IN3,IN4,ENB
};



////////////
// Setup //
//////////

void setup() 
{
  Serial.begin(9600); // initialize Serial
  
  // setup all pins being used by both motor driver as OUTPUT
  for(int i=0; i<2; i++) // loop over both motors
  {
    for(int j=0; j<3; j++) // loop over all pins of motor
    {
      pinMode(left_motor_driver[i][j],OUTPUT);
      pinMode(right_motor_driver[i][j],OUTPUT);  
    }
  }
  Serial.println("8: All motors forward\n2: All motors backwards\n4:left motors forward\n6: right motors forward\n5: STOP");
}



///////////
// Main //
/////////

void loop() 
{
  // Assign direction received from Serial port
  if(Serial.available() > 0){    
    direction = Serial.read();
  }

  // drive motors as per Serial input
  switch (direction){
      
    case UP:
    drive_left_motors(FORWARD);
    drive_right_motors(FORWARD);
    break;

    case DOWN:
    //down
    drive_left_motors(BACKWARD);
    drive_right_motors(BACKWARD);
    break;

    case LEFT:
    // left
    drive_left_motors(FORWARD);
    drive_right_motors(STOP);
    break;

    case RIGHT:
    //right
    drive_left_motors(STOP);
    drive_right_motors(FORWARD);
    break;

    case STOP:
    //stop
    drive_left_motors(STOP);
    drive_right_motors(STOP);
  }
}



////////////////////////////////////
// function to drive left motors //
//////////////////////////////////

void drive_left_motors(int motor_direction)
{  
  for(int i = 0; i < 2; i++)
  {
    switch(motor_direction)
    {
      case FORWARD:
      digitalWrite(left_motor_driver[i][0], HIGH); //IN1: high
      digitalWrite(left_motor_driver[i][1], LOW); //IN2: low
      analogWrite(left_motor_driver[i][2],128); //EN: PWM
      break;

      case BACKWARD:
      digitalWrite(left_motor_driver[i][0], LOW); //IN1: low
      digitalWrite(left_motor_driver[i][1], HIGH); //IN2: high
      analogWrite(left_motor_driver[i][2],128); //EN: PWM 
      break;

      case STOP:
      analogWrite(left_motor_driver[i][2],0); //EN: low
      break;
    }
  }
}



/////////////////////////////////////
// function to drive right motors //
///////////////////////////////////

void drive_right_motors(int motor_direction)
{
   for(int i = 0; i < 2; i++)
  {
    switch(motor_direction)
    {
      case FORWARD:
      digitalWrite(right_motor_driver[i][0], HIGH); //IN1: high
      digitalWrite(right_motor_driver[i][1], LOW); //IN2: low
      analogWrite(right_motor_driver[i][2],128); //EN: PWM
      break;

      case BACKWARD:
      digitalWrite(right_motor_driver[i][0], LOW); //IN1: low
      digitalWrite(right_motor_driver[i][1], HIGH); //IN2: high
      analogWrite(right_motor_driver[i][2],128); //EN: PWM
      break;

      case STOP:
      analogWrite(right_motor_driver[i][2],0); //EN: low
      break;
    }
  }
}

