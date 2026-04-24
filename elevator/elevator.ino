// An elevator that understands queued calls.
// It's an index-one based system because 0 is needed to reflect an empty space on the queue. Ground floor is 1, first floor is 2, and so on

// Buttons - for calling the elevator from each floor.
const int buttonPin_1 = 2;
const int buttonPin_2 = 3;
const int buttonPin_3 = 4;
const int buttonPinArray[] = {buttonPin_1, buttonPin_2, buttonPin_3};

// Button Lights - LEDs to indicate each floor the elevator has been called to.
const int buttonLEDPin_1 = 7;
const int buttonLEDPin_2 = 8;
const int buttonLEDPin_3 = 12;
const int buttonLEDPinArray[] = {buttonLEDPin_1, buttonLEDPin_2, buttonLEDPin_3};

// Elevator Lights - LEDs to indicate in which floor the elevator is. Need to be connected to PWM~ pins for fade effect
const int floorLEDPin_1 = 9;
const int floorLEDPin_2 = 10;
const int floorLEDPin_3 = 11;
const int floorLEDPinArray[] = {floorLEDPin_1, floorLEDPin_2, floorLEDPin_3};
int floorLEDValueArray[] = {0,0,0};
const int fadeStep = 5, fadePeriod = 30;
unsigned long fadePreviousMillis = 0;

// Floor switches - Indicates when the elevator arrived to each floor
const int floorSwitchPin_1 = A1;
const int floorSwitchPin_2 = A2;
const int floorSwitchPin_3 = A3;
const int floorSwitchPinArray[] = {floorSwitchPin_1, floorSwitchPin_2, floorSwitchPin_3};
int floorSwitchValue_1 = LOW;

// Motor direction
const int motorUpPin = 5; // Clockwise means up
const int motorDownPin = 6; // Counter clockwise means down

// Potentiometers - Allows to live-tweak motor speeds
const int potentiometerUpPin = A4;
const int potentiometerDownPin = A5;
int motorUpSpeed = 0;
int motorDownSpeed = 0;

// Logic elements
const int queueSize = 3; // Equal to the number of floors
int elevatorQueue[queueSize]; // Initializes an array representing the queue
int elevatorPosition = 0; // Current floor the elevator is in
int destinationFloor = 0; // Next floor the elevator has to go to
bool direction = 0;

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
      digitalWrite(buttonLEDPinArray[floorNumber - 1], HIGH); // Turn on LED indicating button call
      return;
    }
  }
}

void moveElevator() {
  /*
  Receives the first item in elevatorQueue and tells the motor which direction to go until it reaches the right floor
  Then, it removes the first item in queue and moves every other item one step forward
  */

  // If the queue is empty, wait
  if (destinationFloor == 0) {
    Serial.println("Waiting for call...");
    return;
  }

  // Otherwise, start moving
  Serial.print("elevator's position: ");
  Serial.println(elevatorPosition);

  int targetFloorSwitchValue = digitalRead(floorSwitchPinArray[destinationFloor - 1]);

  if (targetFloorSwitchValue == 0) { // if the corresponding floorSwitch isnt' triggered
    if (direction) {
      Serial.println("Going up");
      analogWrite(motorDownPin,0);
      analogWrite(motorUpPin,motorUpSpeed);
    } else {
      Serial.println("Going down");
      analogWrite(motorUpPin,0);
      analogWrite(motorDownPin,motorDownSpeed);
    }
  } else if (targetFloorSwitchValue == 1) {
    Serial.println("Turning off motor...");
    analogWrite(motorUpPin,0);
    analogWrite(motorDownPin,0);
    Serial.println("...and opening doors");

    digitalWrite(buttonLEDPinArray[destinationFloor - 1],LOW); // Turn off floor's buttonLED
    
    elevatorPosition = destinationFloor;

    // Remove first item from the queue and move the rest one position forward
    for (int i = 0; i < queueSize - 1; i++) {
      elevatorQueue[i] = elevatorQueue[i + 1];
    }
    elevatorQueue[queueSize - 1] = 0; // Last position is always empty

    // delay(2000); // TODO: This interferes with floor LED lights, so I need to adapt this function to millis() too
  }
}

void setup() {
  Serial.begin(9600);

  pinMode(buttonPin_1,INPUT);
  pinMode(buttonPin_2,INPUT);
  pinMode(buttonPin_3,INPUT);
  
  pinMode(buttonLEDPin_1,OUTPUT);
  pinMode(buttonLEDPin_2,OUTPUT);
  pinMode(buttonLEDPin_3,OUTPUT);
  digitalWrite(buttonLEDPin_1,LOW);
  digitalWrite(buttonLEDPin_2,LOW);
  digitalWrite(buttonLEDPin_3,LOW);

  pinMode(floorLEDPin_1,OUTPUT);
  pinMode(floorLEDPin_2,OUTPUT);
  pinMode(floorLEDPin_3,OUTPUT);
  digitalWrite(floorLEDPin_1,LOW);
  digitalWrite(floorLEDPin_2,LOW);
  digitalWrite(floorLEDPin_3,LOW);
  
  pinMode(floorSwitchPin_1, INPUT);
  pinMode(floorSwitchPin_2, INPUT);
  pinMode(floorSwitchPin_3, INPUT);
  
  pinMode(motorUpPin,OUTPUT);
  pinMode(motorDownPin,OUTPUT);
  analogWrite(motorUpPin,0);
  analogWrite(motorDownPin,0);
  
  pinMode(potentiometerUpPin,INPUT);
  pinMode(potentiometerDownPin,INPUT);

  delay(2000); // Give the board time to execute the following code, otherwise it just skips it.
  
  // Move the elevator to ground floor. This ensures the elevator's position wíll be known before starting the loop.
  digitalWrite(buttonLEDPin_1,HIGH); // Turn ground floor light as if the elevator was called
  while (floorSwitchValue_1 == LOW) { // As long as the ground floor switch isn't triggered
    floorSwitchValue_1 = digitalRead(floorSwitchPin_1); // Read switch value
    motorDownSpeed = analogRead(potentiometerDownPin); // Read potentiometer to control speed
    Serial.print("Going to ground floor at speed: ");
    Serial.println(motorDownSpeed);
    analogWrite(motorDownPin,motorDownSpeed); // Turn on motor to go down
  };
  analogWrite(motorDownPin,0); // Stop the motor
  digitalWrite(buttonLEDPin_1,LOW); // Turn ground floor light off
  elevatorPosition = 1; // Sets value to first floor
}

void loop() {
  motorUpSpeed = analogRead(potentiometerUpPin);
  motorDownSpeed = analogRead(potentiometerDownPin);
  destinationFloor = elevatorQueue[0];
  direction = destinationFloor > elevatorPosition ? true : false; // Define motor's direction: true is up, false is down
  
  // Read buttons
  for (int i = 0; i < queueSize; i++) { // For each floor
    if (digitalRead(buttonPinArray[i]) == HIGH) { // If the button is pressed
      addToQueue(i + 1); // Add that floor to the queue
    }
  }

  // Move the elevator to the next floor in queue
  moveElevator();

  // Elevator position lights fade
  unsigned long fadeCurrentMillis = millis();

  if (fadeCurrentMillis - fadePreviousMillis >= fadePeriod) {
    fadePreviousMillis = fadeCurrentMillis;

    for (int i = 0; i < queueSize; i++) { // For each floor
      // Fade in
      if (digitalRead(floorSwitchPinArray[i]) == HIGH && floorLEDValueArray[i] < 255) { // If the switch is active and the light isn't already on
        floorLEDValueArray[i] += fadeStep;
        analogWrite(floorLEDPinArray[i], floorLEDValueArray[i]);
      }
      // Fade out
      if (digitalRead(floorSwitchPinArray[i]) == LOW && floorLEDValueArray[i] > 0) { // If the switch is NOT active and the light is still on
        floorLEDValueArray[i] -= fadeStep;
        analogWrite(floorLEDPinArray[i], floorLEDValueArray[i]);
      }
    }
  }

  // Print queue status
  Serial.print("Queue status: ");
  for (int i = 0; i < queueSize; i++) {
    Serial.print(elevatorQueue[i]);
  }
  Serial.println();
}