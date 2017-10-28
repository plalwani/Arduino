//Test code with dual L298N motor drivers (4 motors)that utilizes serial input from keyboard
// to turn on all four motors in specific direction to move a rover in different directions
// Author: Prashant Lalwani
// 10/27/2017


// defines for ASCII input
#define LEFT 52
#define RIGHT 54
#define UP 56
#define DOWN 50

//defines to send to motor driver function indicating direction of motors
#define FORWARD 8
#define BACKWARD 2

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
  {8,7,6}, //IN1,IN2,ENA
  {11,10,9}, //IN3,IN4,ENB
};

void setup() 
{
  Serial.begin(9600);
}

void loop() 
{
  if(Serial.available() > 0){    
    direction = Serial.read();
  }
  switch (direction){
    
    case UP:
    Serial.println("received UP\nDriving all motors forward");
    break;

    case DOWN:
    //down
    Serial.println("received DOWN\nDriving all motors backwards");
    break;

    case LEFT:
    // left
    Serial.println("received UP\nDriving left motors forward");
    break;

    case RIGHT:
    //right
    Serial.println("received UP\nDriving right motors forward");
    break;
  }
}

void drive_left_motors(int motor_direction)
{
  
}

void drive_right_motors(int motor_direction)
{
  
}

