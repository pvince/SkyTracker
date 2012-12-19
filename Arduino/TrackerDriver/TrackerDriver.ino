/*	SkyTracker Stepper Driver
 *
 * 	Hardware Setup:
 *  - Pin 2: Step -> Pololu Stepper Driver STEP
 *  - Pin 3: Dir  -> Pololu Stepper Driver DIR
 *  - Pin VIN: 5v VIN shared w/ Pololu
 *  - Pin GND: Ground shared w/ Pololu
 */
 
#include <AccelStepper.h>

// ---- Hardware Pin Setup ----
// Output Pins
#define ENABLE_PIN  2
#define MS1_PIN     3
#define MS2_PIN     4
#define MS3_PIN     5
#define STEP_PIN    6
#define DIR_PIN     7

// Input Pins
#define SW_DIR_PIN      13
#define SW_DIR_OFF_PIN  12
#define SW_SPD_PIN      11
#define LIM_TOP_PIN     10
#define LIM_BOT_PIN     9

// ---- Stepper hardcoded values ----
#define MICRO_STEPS_REV 3200   // 3200 Steps to a revolution (16x Step Mode)
#define FULL_STEPS_REV  200
#define REVS_PER_INCH   20    // 1/4-20 rod
#define TRAVEL_LENGTH   6.0   // 6 inches of rod
#define TRAVERSAL_TIME  60.0  // 60 seconds to traverse entire rod on fast travel.
#define STEPPER_ACCEL   200.0 // 100 steps/s^2

const int eFullStep   = 0;
const int eMicroStep  = 1;

int current_step_mode = eMicroStep;
int current_steps_rev = MICRO_STEPS_REV;

// ---- Stepper calculated values ----
// Tracking speed is 1 rev per minute
float tracking_speed    = MICRO_STEPS_REV/60.0;

// Fast travel speed is (ideally) TRAVEL_LENGTH in TRAVERSAL_TIME
// So we find the total # of steps in the TRAVEL_LENGTH and divide it by TRAVERSAL_TIME
// At 6 inches in 60 seconds we get 400 steps / second
float fast_travel_speed = 4000; //(MICRO_STEPS_REV * REVS_PER_INCH * TRAVEL_LENGTH) / TRAVERSAL_TIME;

// ---- Program Variables ----
// Define a stepper and the pins it will use
AccelStepper stepper(AccelStepper::DRIVER, STEP_PIN, DIR_PIN);

// ---- Program Functions ----
void setStepMode(int stepMode)
{
  switch(stepMode)
  {
    case eMicroStep:
      digitalWrite(MS1_PIN, HIGH);
      digitalWrite(MS2_PIN, HIGH);
      digitalWrite(MS3_PIN, HIGH);
      current_steps_rev = MICRO_STEPS_REV;
      current_step_mode = eMicroStep;
      break;
    case eFullStep:
    default:
      digitalWrite(MS1_PIN, LOW);
      digitalWrite(MS2_PIN, LOW);
      digitalWrite(MS3_PIN, LOW);
      current_steps_rev = FULL_STEPS_REV;
      current_step_mode = eFullStep;
      break;
  }
  tracking_speed = current_steps_rev / 60;
}

void readInputSwitches()
{
  int iLimTop = digitalRead(LIM_TOP_PIN);
  int iLimBot = digitalRead(LIM_BOT_PIN);
  bool bAtTop = (LOW == iLimTop);
  bool bAtBot = (LOW == iLimBot);
  
  /*  Going down + limBot = stop
   *  Going up + limTop = stop
  */

  int iDirection = digitalRead(SW_DIR_PIN);
  int iOff       = digitalRead(SW_DIR_OFF_PIN);
  
  if(LOW == iOff ||
      (bAtTop && (stepper.distanceToGo() > 0)) || // At top, and going up
      (bAtBot && (stepper.distanceToGo() < 0)))   // At bottom, and going down.
  {
    // Stop the stepper motor
    stepper.stop(); // Fast stop
    stepper.runToPosition();
  }
  else if( LOW == iDirection )
  {
    // Set stepper to go up, trust the limit switches to stop us at the end points
    stepper.moveTo(999999999);
  }
  else
  {
    // Set the stepper to go down, trust the limit switches to stop us at the end points
    stepper.moveTo(-999999999);
  }
  
  int iSpd = digitalRead(SW_SPD_PIN);
  if( LOW == iSpd) {
    setStepMode(eFullStep);
    stepper.setMaxSpeed(fast_travel_speed); // Go max speed
  } else {
    setStepMode(eMicroStep);
    stepper.setMaxSpeed(tracking_speed);    // Go Slow
  }
}

void setup() {
  // -- START Pin Setup --
  pinMode(SW_DIR_PIN, INPUT);
  pinMode(SW_DIR_OFF_PIN, INPUT);
  pinMode(SW_SPD_PIN, INPUT);
  pinMode(LIM_TOP_PIN, INPUT);
  pinMode(LIM_BOT_PIN, INPUT);
  
  pinMode(ENABLE_PIN, OUTPUT);
  pinMode(MS1_PIN, OUTPUT);
  pinMode(MS2_PIN, OUTPUT);
  pinMode(MS3_PIN, OUTPUT);
  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  
  // Enable pullup resistors on the input pins
  digitalWrite(SW_DIR_PIN, HIGH);
  digitalWrite(SW_DIR_OFF_PIN, HIGH);
  digitalWrite(SW_SPD_PIN, HIGH);
  digitalWrite(LIM_TOP_PIN, HIGH);
  digitalWrite(LIM_BOT_PIN, HIGH);
  
  // Enable the stepper motor
  digitalWrite(ENABLE_PIN, HIGH);
  
  // Set the stepper output pins low to start.
  digitalWrite(STEP_PIN, LOW);
  digitalWrite(DIR_PIN, LOW);
  // -- END Pin Setup --
  
  // -- START Stepper Setup --
  stepper.setEnablePin(ENABLE_PIN);
  stepper.setAcceleration(STEPPER_ACCEL);
  // -- END Stepper Setup --
  
  // Setup stepper motor settings based on current input.
  readInputSwitches();
}

void loop() {
  // if we have reached our destination, go two revolutions the other direction.
  //if(stepper.distanceToGo() == 0)
  //  stepper.moveTo(-stepper.currentPosition());
  
  // Check limit switches
  // Check button states (Stop, Fast up, fast down, start tracking)
  readInputSwitches();
  
  // run steppers
  stepper.run();
  
}


