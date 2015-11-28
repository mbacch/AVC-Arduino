#include <Odometry.h>

//Global Functions
void rightEncoderAChange();
void rightEncoderBChange();
void leftEncoderAChange();
void setupPinChangeInterrupt(byte pin);

//Global Variables
int rightEncoderCounter;
int leftEncoderCounter;
byte _rightEncoderAPin;
byte _rightEncoderBPin;
byte _leftEncoderAPin;
byte _leftEncoderBPin;

/**
 *	Constructor arg are pins for channels A and B on right and left encoders
 **/
Odometry::Odometry(byte rightEncoderAPin, byte rightEncoderBPin, byte leftEncoderAPin, byte leftEncoderBPin, float wheelBase, float wheelDiameter, int cpr) {
    pinMode(rightEncoderAPin, INPUT);
    pinMode(rightEncoderBPin, INPUT);
    pinMode(leftEncoderAPin, INPUT);
    pinMode(leftEncoderBPin, INPUT);
    digitalWrite(leftEncoderBPin, HIGH);
    rightEncoderCounter = 0.;
    leftEncoderCounter = 0.;
    attachInterrupt(digitalPinToInterrupt(rightEncoderAPin), rightEncoderAChange, CHANGE);
    attachInterrupt(digitalPinToInterrupt(rightEncoderBPin), rightEncoderBChange, CHANGE);
    attachInterrupt(digitalPinToInterrupt(leftEncoderAPin), leftEncoderAChange, CHANGE);
    setupPinChangeInterrupt(leftEncoderBPin);
    _rightEncoderAPin = rightEncoderAPin;
    _rightEncoderBPin = rightEncoderBPin;
    _leftEncoderAPin = leftEncoderAPin;
    _leftEncoderBPin = leftEncoderBPin;
    _wheelBase = wheelBase;
    _wheelDiameter = wheelDiameter;
    _cpr = cpr;
}

void Odometry::update() {
    //Calculate linear distance that each wheel has traveled
    float rightWheelDistance = rightEncoderCounter / (_cpr * _wheelDiameter * PI);
    float leftWheelDistance = leftEncoderCounter / (_cpr * _wheelDiameter * PI);
    
    //Calculate angle that robot has turned
    float theta = (leftWheelDistance - rightWheelDistance) / _wheelBase;
    
    //Decompose linear distance into its component values
    float meanWheelDistance = (rightWheelDistance + leftWheelDistance) / 2;
    x = meanWheelDistance * sin(theta);
    y = meanWheelDistance * cos(theta);
    
    //Reset counters
    rightEncoderCounter = 0;
    leftEncoderCounter = 0;
}

void rightEncoderAChange() {
    bool rightEncoderAStatus = digitalRead(_rightEncoderAPin);
    bool rightEncoderBStatus = digitalRead(_rightEncoderBPin);
    if (((rightEncoderAStatus == HIGH) && (rightEncoderBStatus == LOW)) || ((rightEncoderAStatus == LOW) && (rightEncoderBStatus == HIGH))) {
        rightEncoderCounter++;
    }
    else {
        rightEncoderCounter--;
    }
}

void rightEncoderBChange() {
    bool rightEncoderAStatus = digitalRead(_rightEncoderAPin);
    bool rightEncoderBStatus = digitalRead(_rightEncoderBPin);
    if (((rightEncoderAStatus == HIGH) && (rightEncoderBStatus == HIGH)) || ((rightEncoderAStatus == LOW) && (rightEncoderBStatus == LOW))) {
        rightEncoderCounter++;
    }
    else {
        rightEncoderCounter--;
    }
}

void leftEncoderAChange() {
    bool leftEncoderAStatus = digitalRead(_leftEncoderAPin);
    bool leftEncoderBStatus = digitalRead(_leftEncoderBPin);
    if (((leftEncoderAStatus == HIGH) && (leftEncoderBStatus == HIGH)) || ((leftEncoderAStatus == LOW) && (leftEncoderBStatus == LOW))) {
        leftEncoderCounter++;
    }
    else {
        leftEncoderCounter--;
    }
}

ISR (PCINT0_vect) {
    bool leftEncoderAStatus = digitalRead(_leftEncoderAPin);
    bool leftEncoderBStatus = digitalRead(_leftEncoderBPin);
    if (((leftEncoderAStatus == HIGH) && (leftEncoderBStatus == LOW)) || ((leftEncoderAStatus == LOW) && (leftEncoderBStatus == HIGH))) {
        leftEncoderCounter++;
    }
    else {
        leftEncoderCounter--;
    }
}

void setupPinChangeInterrupt(byte pin) {
    *digitalPinToPCMSK(pin) |= bit (digitalPinToPCMSKbit(pin));  // enable pin
    PCIFR  |= bit (digitalPinToPCICRbit(pin)); // clear any outstanding interrupt
    PCICR  |= bit (digitalPinToPCICRbit(pin)); // enable interrupt for the group
}