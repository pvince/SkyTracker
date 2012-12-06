#include <AccelStepper.h>

// ---- Hardware Pin Setup ----
#define STEP_PIN 1
#define DIR_PIN  2

// ---- Stepper hardcoded values ----
#define STEPS_PER_REV   200   // 200 Steps to a revolution (Full Step Mode)
#define REVS_PER_INCH   20    // 1/4-20 rod
#define TRAVEL_LENGTH   6.0   // 6 inches of rod
#define TRAVERSAL_TIME  60.0  // 60 seconds to traverse entire rod on fast travel.
#define STEPPER_ACCEL   100.0 // 100 steps/s^2

// ---- Stepper calculated values ----
// Tracking speed is 1 rev per minute
float tracking_speed    = STEPS_PER_REV/60.0;

// Fast travel speed is (ideally) TRAVEL_LENGTH in TRAVERSAL_TIME
// So we find the total # of steps in the TRAVEL_LENGTH and divide it by TRAVERSAL_TIME
// At 6 inches in 60 seconds we get 400 steps / second
float fast_travel_speed = (STEPS_PER_REV * REVS_PER_INCH * TRAVEL_LENGTH) / TRAVERSAL_TIME;

// ---- Program Variables ----
// Define a stepper and the pins it will use
AccelStepper stepper(AccelStepper::DRIVER, STEP_PIN, DIR_PIN);

// ---- Program Functions ----
void setup() {
  stepper.setMaxSpeed(fast_travel_speed);
  stepper.setAcceleration(STEPPER_ACCEL);
  // Perform 1 revolutions going 'forward'
  stepper.moveTo(200);
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


