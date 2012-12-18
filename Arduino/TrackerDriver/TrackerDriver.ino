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
#define STEP_PIN 2
#define DIR_PIN  3

// ---- Stepper hardcoded values ----
#define STEPS_PER_REV   3200   // 3200 Steps to a revolution (16x Step Mode)
#define REVS_PER_INCH   20    // 1/4-20 rod
#define TRAVEL_LENGTH   6.0   // 6 inches of rod
#define TRAVERSAL_TIME  60.0  // 60 seconds to traverse entire rod on fast travel.
#define STEPPER_ACCEL   200.0 // 100 steps/s^2

// ---- Stepper calculated values ----
// Tracking speed is 1 rev per minute
float tracking_speed    = STEPS_PER_REV/60.0;

// Fast travel speed is (ideally) TRAVEL_LENGTH in TRAVERSAL_TIME
// So we find the total # of steps in the TRAVEL_LENGTH and divide it by TRAVERSAL_TIME
// At 6 inches in 60 seconds we get 400 steps / second
float fast_travel_speed = 4000; //(STEPS_PER_REV * REVS_PER_INCH * TRAVEL_LENGTH) / TRAVERSAL_TIME;

// ---- Program Variables ----
// Define a stepper and the pins it will use
AccelStepper stepper(AccelStepper::DRIVER, STEP_PIN, DIR_PIN);

// ---- Program Functions ----
void setup() {
  stepper.setMaxSpeed(fast_travel_speed);
  stepper.setAcceleration(STEPPER_ACCEL);
  // Perform 1 revolutions going 'forward'
  stepper.moveTo(STEPS_PER_REV);
}

void loop() {
  // if we have reached our destination, go two revolutions the other direction.
  if(stepper.distanceToGo() == 0)
    stepper.moveTo(-stepper.currentPosition());
  
  stepper.run();
  
  // Check limit switches
  // Check button states (Stop, Fast up, fast down, start tracking)
  // run steppers
}


