#include <SpeedyStepper.h>
#include <Bounce2.h>

int endstop_CP_min = 2;
int endstop_CY_min = 3;

int endstop_X_min = 14;
int endstop_Y_min = 15;
int endstop_Z_min = 19;

int fan = 10;
String command_Y;
Bounce debouncer = Bounce(); // prevents detecting held buttons

SpeedyStepper stepper_X;
SpeedyStepper stepper_Y;
SpeedyStepper stepper_Z;
SpeedyStepper stepper_CP;
SpeedyStepper stepper_CY;

// let's define our stepper pins
const int X_STEP_PIN = 54;
const int X_DIR_PIN = 55;
const int X_ENABLE_PIN = 38;

const int Y_STEP_PIN = 60;
const int Y_DIR_PIN = 61;
const int Y_ENABLE_PIN = 56;

const int Z_STEP_PIN = 46;
const int Z_DIR_PIN = 48;
const int Z_ENABLE_PIN = 62;

const int CY_STEP_PIN = 26;
const int CY_DIR_PIN = 28;
const int CY_ENABLE_PIN = 24;

const int CP_STEP_PIN = 36;
const int CP_DIR_PIN = 34;
const int CP_ENABLE_PIN = 30;

int X_revolution = 0;

const unsigned int MAX_COMMAND_LENGTH = 100;
#define SPTR_SIZE   20
char   *sPtr [SPTR_SIZE];
signed long number;

// define global rotations for XY1 and XY2 motors
signed long XY_0 = 0;
signed long XY_1 = 0;

// define global positions for all axes
signed long glob_x  = 0;
signed long glob_y  = 0;
signed long glob_x_prev  = 0;
signed long glob_y_prev  = 0;
signed long glob_z  = 0;
signed long glob_cp = 0;
signed long glob_cy = 0;

// home offset steps
signed long offset_z = 0;
signed long offset_cp = 0;
signed long offset_cy = 0;

signed long home_z = 1000000;

int separate (String str, char **p, int size)
{
    int  n;
    char s [100];

    strcpy (s, str.c_str ());

    *p++ = strtok (s, " ");
    for (n = 1; NULL != (*p++ = strtok (NULL, " ")); n++)
        if (size == n)
            break;

    return n;
}

void setup() {
  // start communication
  Serial.begin(9600);
  
  // configure entstop as input
  pinMode(endstop_X_min, INPUT_PULLUP);
  debouncer.attach(endstop_X_min);
  debouncer.interval(0.2);

  pinMode(endstop_Y_min, INPUT_PULLUP);
  debouncer.attach(endstop_Y_min);
  debouncer.interval(0.2);

  pinMode(endstop_Z_min, INPUT_PULLUP);
  debouncer.attach(endstop_Z_min);
  debouncer.interval(0.2);

  pinMode(endstop_CP_min, INPUT_PULLUP);
  debouncer.attach(endstop_CP_min);
  debouncer.interval(0.2);

  pinMode(endstop_CY_min, INPUT_PULLUP);
  debouncer.attach(endstop_CY_min);
  debouncer.interval(0.2);

  // configure stepper motors
  stepper_X.connectToPins(X_STEP_PIN, X_DIR_PIN);
  pinMode(X_ENABLE_PIN, OUTPUT);
  digitalWrite(X_ENABLE_PIN, LOW);
  stepper_X.setSpeedInStepsPerSecond(100*16);
  stepper_X.setAccelerationInStepsPerSecondPerSecond(100*16);

  stepper_Y.connectToPins(Y_STEP_PIN, Y_DIR_PIN);
  pinMode(Y_ENABLE_PIN, OUTPUT);
  digitalWrite(Y_ENABLE_PIN, LOW);
  stepper_Y.setSpeedInStepsPerSecond(100*16);
  stepper_Y.setAccelerationInStepsPerSecondPerSecond(100*16);

  stepper_Z.connectToPins(Z_STEP_PIN, Z_DIR_PIN);
  pinMode(Z_ENABLE_PIN, OUTPUT);
  digitalWrite(Z_ENABLE_PIN, LOW);
  stepper_Z.setSpeedInStepsPerSecond(75*16);
  stepper_Z.setAccelerationInStepsPerSecondPerSecond(50*16);

  stepper_CP.connectToPins(CP_STEP_PIN, CP_DIR_PIN);
  pinMode(CP_ENABLE_PIN, OUTPUT);
  digitalWrite(CP_ENABLE_PIN, LOW);
  stepper_CP.setSpeedInStepsPerSecond(100*16);
  stepper_CP.setAccelerationInStepsPerSecondPerSecond(50*16);

  stepper_CY.connectToPins(CY_STEP_PIN, CY_DIR_PIN);
  pinMode(CY_ENABLE_PIN, OUTPUT);
  digitalWrite(CY_ENABLE_PIN, LOW);
  stepper_CY.setSpeedInStepsPerSecond(50*16);
  stepper_CY.setAccelerationInStepsPerSecondPerSecond(50*16);

  delay(500);
}


void loop() {

  //check for new command in serial buffer
  while (Serial.available() > 0)
  {

    //hold new command from Blender Python
    static char command[MAX_COMMAND_LENGTH];
    static unsigned int command_pos = 0;

    //read next available bytes from Serial
    char inByte = Serial.read();

    // avoids reading negative values 
    if (inByte > 0)
      {
        // check if byte is NOT a terminating character
      if (inByte != '\n' && (command_pos < MAX_COMMAND_LENGTH - 1))
      {
        //add bytes to string
        command[command_pos] = inByte;
        command_pos++;
      }
      else
      {
        //add null character to string, print the command, and reset command_pos
        //only print non-empty lines
        if (command_pos > 0)
        {
          Serial.print("Received new command: ");
          Serial.println(command);

                  
          //using atoi() we can convert null-terminated strings into integers
          //output integer
          int N = separate (command, sPtr, SPTR_SIZE);

          if (strcmp(sPtr [0], "test") ==0)
          {
            Serial.println("So you have chosen death...");

            // move forward and back
            stepper_X.setupMoveInSteps(300*16);
            stepper_Y.setupMoveInSteps(300*16);
            while(!(stepper_X.motionComplete() && stepper_Y.motionComplete()))
            {
              stepper_X.processMovement(); // this call moves the motor
              stepper_Y.processMovement(); // this call moves the motor
            }
        
            delay(250);
        
            stepper_X.setupMoveInSteps(-300*16);
            stepper_Y.setupMoveInSteps(-300*16);
            while(!(stepper_X.motionComplete() && stepper_Y.motionComplete()))
            {
              stepper_X.processMovement(); // this call moves the motor
              stepper_Y.processMovement(); // this call moves the motor
            }
        
            delay(250);
        
            stepper_X.setupMoveInSteps(0);
            stepper_Y.setupMoveInSteps(0);
            while(!(stepper_X.motionComplete() && stepper_Y.motionComplete()))
            {
              stepper_X.processMovement(); // this call moves the motor
              stepper_Y.processMovement(); // this call moves the motor
            }
        
            delay(500);
        
            // move right left
        
            stepper_X.setupMoveInSteps(300*16);
            stepper_Y.setupMoveInSteps(-300*16);
            while(!(stepper_X.motionComplete() && stepper_Y.motionComplete()))
            {
              stepper_X.processMovement(); // this call moves the motor
              stepper_Y.processMovement(); // this call moves the motor
            }
        
            delay(250);
        
            stepper_X.setupMoveInSteps(-300*16);
            stepper_Y.setupMoveInSteps(300*16);
            while(!(stepper_X.motionComplete() && stepper_Y.motionComplete()))
            {
              stepper_X.processMovement(); // this call moves the motor
              stepper_Y.processMovement(); // this call moves the motor
            }
        
            delay(250);
        
            stepper_X.setupMoveInSteps(0);
            stepper_Y.setupMoveInSteps(0);
            while(!(stepper_X.motionComplete() && stepper_Y.motionComplete()))
            {
              stepper_X.processMovement(); // this call moves the motor
              stepper_Y.processMovement(); // this call moves the motor
            }
        
            delay(500);
        
            // move camera pitch
        
            stepper_CY.setupMoveInSteps(150*16);
            while(!(stepper_CY.motionComplete()))
            {
              stepper_CY.processMovement(); // this call moves the motor
            }
        
            stepper_CY.setupMoveInSteps(-150*16);
            while(!(stepper_CY.motionComplete()))
            {
              stepper_CY.processMovement(); // this call moves the motor
            }
            
            stepper_CY.setupMoveInSteps(0);
            while(!(stepper_CY.motionComplete()))
            {
              stepper_CY.processMovement(); // this call moves the motor
            }
        
            delay(250);
        
                // move camera Pitch
         
            stepper_CP.setupMoveInSteps(500*16);
            while(!(stepper_CP.motionComplete()))
            {
              stepper_CP.processMovement(); // this call moves the motor
            }
            
            stepper_CP.setupMoveInSteps(-500*16);
            while(!(stepper_CP.motionComplete()))
            {
              stepper_CP.processMovement(); // this call moves the motor
            }
        
            stepper_CP.setupMoveInSteps(0);
            while(!(stepper_CP.motionComplete()))
            {
              stepper_CP.processMovement(); // this call moves the motor
            }
        
            delay(250);
          
          }
          else if (strcmp(sPtr [0], "X") ==0)
          {
            Serial.println("Sounds like you wanna move to some set position.");
            
            //set global X position
            glob_x_prev = glob_x;
            glob_x  = strtoul(sPtr [1], NULL, 0) *16;
            
            //set global Y position
            glob_y_prev = glob_y;
            glob_y  = strtoul(sPtr [3], NULL, 0) *16;
            
            //set global Z position (axis is inverted)
            glob_z  = strtoul(sPtr [5], NULL, 0) *16;

            //set global CP angle
            glob_cp = strtoul(sPtr [7], NULL, 0) *16;

            //set global CY angle
            glob_cy = strtoul(sPtr [9], NULL, 0) *16;
            
            // difference of both results in X movement, sum in Y movement
            XY_0 += (-glob_x + glob_x_prev) + (glob_y - glob_y_prev);
            XY_1 += ( glob_x - glob_x_prev) + (glob_y - glob_y_prev);

            stepper_X.setupMoveInSteps(XY_0);
            stepper_Y.setupMoveInSteps(XY_1);
            stepper_Z.setupMoveInSteps(glob_z + offset_z);
            stepper_CP.setupMoveInSteps(glob_cp + offset_cp);
            stepper_CY.setupMoveInSteps(glob_cy + offset_cy);
            
            while(!(stepper_X.motionComplete() && 
                    stepper_Y.motionComplete() && 
                    stepper_Z.motionComplete() &&
                    stepper_CP.motionComplete()&&
                    stepper_CY.motionComplete()))
              {
                stepper_X.processMovement(); // this call moves the motor
                stepper_Y.processMovement(); 
                stepper_Z.processMovement(); 
                stepper_CP.processMovement();
                stepper_CY.processMovement();
              }
            
            

            Serial.print("Global X: "); 
            Serial.print(glob_x); 
            Serial.print(" | Global Y: "); 
            Serial.print(glob_y); 
            Serial.print(" | Global Z: "); 
            Serial.print(glob_z); 
            Serial.print(" | Global CP: "); 
            Serial.print(glob_cp); 
            Serial.print(" | Global CY: "); 
            Serial.println(glob_cy); 
                
            
          }
          else if ((strcmp(sPtr [0], "HOME") ==0) && (strcmp(sPtr [1], "X") ==0))
          {
            Serial.println("Sounds like you wanna go home.");

            stepper_X.setSpeedInStepsPerSecond(50*16);
            stepper_Y.setSpeedInStepsPerSecond(50*16);
            
            stepper_X.setupMoveInSteps(XY_0 + 1500*16);
            stepper_Y.setupMoveInSteps(XY_1 - 1500*16);
            
            while(!(digitalRead(endstop_X_min) == LOW))
            {
              stepper_X.processMovement(); // this call moves the motor
              stepper_Y.processMovement(); // this call moves the motor
            }

            stepper_X.setupStop();
            stepper_Y.setupStop();

            XY_0 = stepper_X.getCurrentPositionInSteps();
            XY_1 = stepper_Y.getCurrentPositionInSteps();

            glob_x = 0;

            //update target position to avoid continue running 
            stepper_X.setupMoveInSteps(XY_0);
            stepper_Y.setupMoveInSteps(XY_1);
            while(!(stepper_X.motionComplete() && stepper_Y.motionComplete()))
            {
              stepper_X.processMovement(); // this call moves the motor
              stepper_Y.processMovement(); // this call moves the motor
            }

            delay(200);    
            Serial.println("Welcome home, X");

            stepper_X.setSpeedInStepsPerSecond(100*16);
            stepper_Y.setSpeedInStepsPerSecond(100*16);
            
          }
          else if ((strcmp(sPtr [0], "HOME") ==0) && (strcmp(sPtr [1], "Y") ==0))
          {
            Serial.println("Sounds like you wanna go home.");

            stepper_X.setSpeedInStepsPerSecond(50*16);
            stepper_Y.setSpeedInStepsPerSecond(50*16);
            
            stepper_X.setupMoveInSteps(XY_0 - 1500*16);
            stepper_Y.setupMoveInSteps(XY_1 - 1500*16);
            
            while(!(digitalRead(endstop_Y_min) == LOW))
            {
              stepper_X.processMovement(); // this call moves the motor
              stepper_Y.processMovement(); // this call moves the motor
            }

            stepper_X.setupStop();
            stepper_Y.setupStop();

            XY_0 = stepper_X.getCurrentPositionInSteps();
            XY_1 = stepper_Y.getCurrentPositionInSteps();

            glob_y = 0;

            //update target position to avoid continue running 
            stepper_X.setupMoveInSteps(XY_0);
            stepper_Y.setupMoveInSteps(XY_1);
            while(!(stepper_X.motionComplete() && stepper_Y.motionComplete()))
            {
              stepper_X.processMovement(); // this call moves the motor
              stepper_Y.processMovement(); // this call moves the motor
            }

            delay(200);    
            Serial.println("Welcome home, Y");

            stepper_X.setSpeedInStepsPerSecond(100*16);
            stepper_Y.setSpeedInStepsPerSecond(100*16);
            
          }
          else if ((strcmp(sPtr [0], "HOME") ==0) && (strcmp(sPtr [1], "Z") ==0))
          {
            Serial.println("Sounds like you wanna go home.");

            stepper_Z.setSpeedInStepsPerSecond(75*16);
            stepper_Z.setupMoveInSteps(-home_z);
            
            while(!(digitalRead(endstop_Z_min) == LOW))
            {
              stepper_Z.processMovement(); // this call moves the motor
            }

            stepper_Z.setupStop();
            
            offset_z = stepper_Z.getCurrentPositionInSteps();

            glob_z = 0;

            //update target position to avoid continue running 
            stepper_Z.setupMoveInSteps(offset_z);
            while(!(stepper_X.motionComplete()))
            {
              stepper_Z.processMovement(); // this call moves the motor
            }

            delay(200);    
            Serial.println("Welcome home, Z");

            stepper_Z.setSpeedInStepsPerSecond(75*16);
            
          }
          else if ((strcmp(sPtr [0], "HOME") ==0) && (strcmp(sPtr [1], "CP") ==0))
          {
            Serial.println("Sounds like you wanna go home.");

            stepper_CP.setSpeedInStepsPerSecond(50*16);
            stepper_CP.setupMoveInSteps(-2000*16);
            
            while(!(digitalRead(endstop_CP_min) == LOW))
            {
              stepper_CP.processMovement(); // this call moves the motor
            }

            stepper_CP.setupStop();
            
            offset_cp = stepper_CP.getCurrentPositionInSteps();

            glob_cp = 0;

            //update target position to avoid continue running 
            stepper_CP.setupMoveInSteps(offset_cp);
            while(!(stepper_CP.motionComplete()))
            {
              stepper_CP.processMovement(); // this call moves the motor
            }

            delay(200);    
            Serial.println("Welcome home, CP");

            stepper_CP.setSpeedInStepsPerSecond(100*16);
            
          }
          else if ((strcmp(sPtr [0], "HOME") ==0) && (strcmp(sPtr [1], "CY") ==0))
          {
            Serial.println("Sounds like you wanna go home.");

            stepper_CY.setSpeedInStepsPerSecond(25*16);
            stepper_CY.setupMoveInSteps(-2000*16);
            
            while(!(digitalRead(endstop_CY_min) == LOW))
            {
              stepper_CY.processMovement(); // this call moves the motor
            }

            stepper_CY.setupStop();
            
            offset_cy = stepper_CY.getCurrentPositionInSteps();

            glob_cy = 0;

            //update target position to avoid continue running 
            stepper_CY.setupMoveInSteps(offset_cy);
            while(!(stepper_CY.motionComplete()))
            {
              stepper_CY.processMovement(); // this call moves the motor
            }

            delay(200);    
            Serial.println("Welcome home, CY");

            stepper_CY.setSpeedInStepsPerSecond(50*16);
            
          }
          else
          {
            Serial.println("Yeah, that does not sound like a valid command, bro.");
          }
          
        }
        command_pos = 0;
      }
    }
  }

}
