// An elevator that understands queued calls.
// It's an index-one based system because 0 is needed to reflect an empty space on the queue. Ground floor is 1, first floor is 2, and so on

// Buttons - for calling the elevator from each floor.
const int button_1 = 2;
const int button_2 = 3;
const int button_3 = 4;
const int buttonArray[] = {button_1, button_2, button_3};

// Button Lights - LEDs to indicate each floor the elevator has been called to.
const int buttonLED_1 = 5;
const int buttonLED_2 = 6;
const int buttonLED_3 = 7;
const int buttonLEDArray[] = {buttonLED_1, buttonLED_2, buttonLED_3};

// Elevator Lights - LEDs to indicate in which floor the elevator is. TODO: Turn into an array of LEDs for each floor (parallel circuit)
const int floorLED_1 = 8;
const int floorLED_2 = 9;
const int floorLED_3 = 12;
const int floorLEDArray[] = {floorLED_1, floorLED_2, floorLED_3};

// Floor switches - Indicates when the elevator arrived to each floor
const int floorSwitch_1 = A1;
const int floorSwitch_2 = A2;
const int floorSwitch_3 = A3;
const int floorSwitchArray[] = {floorSwitch_1, floorSwitch_2, floorSwitch_3};
int floorSwitchValue_1 = 0;
int floorSwitchValue_2 = 0;
int floorSwitchValue_3 = 0;

// Motor direction - Only one can be >=1 while the other one remains 0
const int motorUp = 10; // Clockwise means up
const int motorDown = 11; // Counter clockwise means down

// Potentiometers - Allows to live-tweak with motor speed // TODO: decide if it's a good idea to replace with fixed values once everything's working
const int potentiometerUp = A4;
const int potentiometerDown = A5;
int motorSpeedUp = 0;
int motorSpeedDown = 0;

// Queue Logic
const int queueSize = 3; // Equal to the number of floors/buttons/lights
int elevatorQueue[queueSize]; // Initializes an array representing the queue
int elevatorPosition = 0; // Current floor the elevator is in

// Functions
void addToQueue(int floorNumber) {
  /*
  Receives a floorNumber and loops through elevatorQueue searching for it.
  If it is not there, floorNumber is added in elevatorQueue's first available position.
  Then, turn on floorNumber's corresponding floor LED
  */

  // Check if floorNumber is already in queue
  for (int i = 0; i < queueSize; i++) {
    if (elevatorQueue[i] == floorNumber) {
      return;
    }
  }

  // Find first empty slot and add floorNumber
  for (int i = 0; i < queueSize; i++) {
    if (elevatorQueue[i] == 0) { // Search for the first 0 in the queue
      elevatorQueue[i] = floorNumber; // Replace it for floorNumber
      digitalWrite(buttonLEDArray[floorNumber - 1], HIGH); // Turn on LED indicating button call
      return;
    }
  }
}

void moveElevator(int destinationFloorNumber) {
  /*
  Receives the first item in elevatorQueue and tells the motor which direction to go until it reaches the right floor
  Then, it removes the first item in queue and moves every other item one step forward
  */

  // If the queue is empty, wait
  if (destinationFloorNumber == 0) {
    Serial.println("Waiting for call...");
    return;
  }

  // Otherwise, start moving

  // Define motor's direction: true is up, false is down
  bool direction = destinationFloorNumber > elevatorPosition ? true : false;
  
  Serial.print("elevator's position: ");
  Serial.println(elevatorPosition);

  int targetFloorSwitchValue = digitalRead(floorSwitchArray[destinationFloorNumber - 1]);

  // Serial.println("Closing doors...");
  // delay(1000);
  // Serial.println(".");
  // delay(1000);
  // Serial.println(".");
  // delay(1000);
  // Serial.println(".");
  // delay(1000);
  // Serial.println(".");

  if (targetFloorSwitchValue == 0) { // if the corresponding floorSwitch isnt' triggered
    if (direction) {
      Serial.println("Going up");
      analogWrite(motorUp,motorSpeedUp);
      analogWrite(motorDown,0);
    } else {
      Serial.println("Going down");
      analogWrite(motorUp,0);
      analogWrite(motorDown,motorSpeedDown);
    }
  } else if (targetFloorSwitchValue == 1) {
    Serial.println("Turning off motor...");
    analogWrite(motorUp,0);
    analogWrite(motorDown,0);
    Serial.println("...and opening doors");

    digitalWrite(buttonLEDArray[destinationFloorNumber - 1],LOW); // Turn off floor's buttonLED
    
    elevatorPosition = destinationFloorNumber;

    // Remove first item from the queue and move the rest one position forward
    for (int i = 0; i < queueSize - 1; i++) {
      elevatorQueue[i] = elevatorQueue[i + 1];
    }
    elevatorQueue[queueSize - 1] = 0; // Last position is always empty

    delay(2000);
  }
}

void setup() {
  Serial.begin(9600);
  // Set pin modes
  pinMode(button_1,INPUT);
  pinMode(button_2,INPUT);
  pinMode(button_3,INPUT);
  
  pinMode(buttonLED_1,OUTPUT);
  pinMode(buttonLED_2,OUTPUT);
  pinMode(buttonLED_3,OUTPUT);

  pinMode(floorLED_1,OUTPUT);
  pinMode(floorLED_2,OUTPUT);
  pinMode(floorLED_3,OUTPUT);
  
  pinMode(floorSwitch_1, INPUT);
  pinMode(floorSwitch_2, INPUT);
  pinMode(floorSwitch_3, INPUT);
  
  pinMode(motorUp,OUTPUT);
  pinMode(motorDown,OUTPUT);
  
  pinMode(potentiometerUp,INPUT);
  pinMode(potentiometerDown,INPUT);

  // Move the elevator to ground floor. This ensures the elevator's position wíll be known before starting the loop.
  delay(2000); // Give the board time to execute the following code, otherwise it just skips it.
  digitalWrite(buttonLED_1,HIGH); // Turn ground floor light as if the elevator was called
  analogWrite(motorUp,0); // Make sure there's no signal for the motor to go up
  while (floorSwitchValue_1 == 0) { // As long as the ground floor switch isn't triggered
    floorSwitchValue_1 = digitalRead(floorSwitch_1); // Read switch value
    motorSpeedDown = analogRead(potentiometerDown); // Read potentiometer to control speed
    Serial.print("Going to ground floor at speed: ");
    Serial.println(motorSpeedDown);
    analogWrite(motorDown,motorSpeedDown); // Turn on motor to go down
  };
  analogWrite(motorDown,0); // Stop the motor
  digitalWrite(buttonLED_1,LOW); // Turn ground floor light off
  elevatorPosition = 1; // Sets value to first floor
}

void loop() {
  Serial.print("\n\nStarting loop\nFloor switches values: ");

  // Read inputs
  floorSwitchValue_1 = digitalRead(floorSwitch_1);
  floorSwitchValue_2 = digitalRead(floorSwitch_2);
  floorSwitchValue_3 = digitalRead(floorSwitch_3);
  motorSpeedUp = analogRead(potentiometerUp);
  motorSpeedDown = analogRead(potentiometerDown);

  // Read buttons
  for (int i = 0; i < queueSize; i++) { // For each floor
    if (digitalRead(buttonArray[i]) == HIGH) { // If the button is pressed
      addToQueue(i + 1); // Add that floor to the queue
    }
  }

  // Elevator position lights
  for (int i = 0; i < queueSize; i++) { // For each floor
    digitalWrite(floorLEDArray[i],LOW); // Turn off all LEDs
    if (digitalRead(floorSwitchArray[i]) == 1) {
      digitalWrite(floorLEDArray[i],HIGH);
    }
  }

  Serial.print(floorSwitchValue_1);
  Serial.print(", ");
  Serial.print(floorSwitchValue_2);
  Serial.print(", ");
  Serial.println(floorSwitchValue_3);
  Serial.print("Motor speed: Up: ");
  Serial.print(motorSpeedUp);
  Serial.print(", Down: ");
  Serial.println(motorSpeedDown);

  // Move the elevator to the next floor in queue
  moveElevator(elevatorQueue[0]);

  // Print queue status
  Serial.print("Queue status: ");
  for (int i = 0; i < queueSize; i++) {
    Serial.print(elevatorQueue[i]);
  }

  delay(500);
}