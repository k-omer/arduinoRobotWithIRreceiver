#include "Servo.h"
#include <IRremote.h>
#include <IRremoteInt.h>
Servo myServo;

/*
	The car has 4 wheels, each wheels has his own pin to interact with
*/
const char RIGHT_PLUS = 9;//The first wheel is positioned at the front right side of the car
const char RIGHT_MINUS = 10;//The second wheel is positioned at the back right side of the car
const char LEFT_PLUS = 11;//The third wheel is positioned at the front left side of the car
const char LEFT_MINUS = 12;//The fourth wheel is positioned at the back left side of the car

/*
	To manipulate the ultrasonic module, we set three pins
*/
const char PWM_SERVO = 2; //This is needed to manipulate the position of the servomotor
int servoPosition = 90; //To set the servomotor at 90°

const char DOUT_TRIGGER = 3;
const char DIN_ECHO = 4;

/*
	The car has 4 IR sensors
*/
const char IR_SENSOR_TOP_RIGHT = 6;
const char IR_SENSOR_TOP_LEFT = 5;
const char IR_SENSOR_BOTTOM_RIGHT = 7;
const char IR_SENSOR_BOTTOM_LEFT = 8;
/*
	The car has one IR receiver to interact with a remote controller
*/
const char IR_RECEIVER_PIN = 13;
IRrecv irrecv(IR_RECEIVER_PIN);
decode_results results;


void setup() {
	pinMode(RIGHT_PLUS, OUTPUT);
	pinMode(RIGHT_MINUS, OUTPUT);
	pinMode(LEFT_PLUS, OUTPUT);
	pinMode(LEFT_MINUS, OUTPUT);

	pinMode(DOUT_TRIGGER, OUTPUT);
	pinMode(DIN_ECHO, INPUT);

	pinMode(IR_SENSOR_TOP_RIGHT, INPUT);
	pinMode(IR_SENSOR_TOP_LEFT, INPUT);

	//Initializing the servomotor and setting his initial position at servoPosition (90°)
	myServo.attach(PWM_SERVO);
	myServo.write(servoPosition);

	irrecv.enableIRIn();

	Serial.begin(9600);
}

void loop() {
	//Getting the result of a remote controller signal
	if (irrecv.decode(&results)) {
		Serial.print("irCode: ");
		Serial.print(results.value, HEX);
		Serial.print(", bits: ");
		Serial.println(results.bits);
		irrecv.resume();
	}
    
	if (results.value == 0xFF10EF)
	{
		look(135);
		look(180);
		turnLeft();
	}
	else if (results.value == 0xFF18E7)
	{
		look(90);
		moveForward();
	}
	else if (results.value == 0xFF5AA5)
	{
		look(45);
		look(0);
		turnRight();
	}
	else if (results.value == 0xFF4AB5)
	{
		moveBack();
	}
	else if (results.value == 0xFF38C7)
	{
		stopMoving();
	}
}

void checkCorners() {
	if (digitalRead(IR_SENSOR_TOP_RIGHT) == 0)
	{
		turnLeft();
		delay(500);
	}
	else if (digitalRead(IR_SENSOR_TOP_LEFT) == 0)
	{
		turnRight();
		delay(500);
	}
}

void look(int a)
{
	myServo.write(a);
	checkObstacles();
}

void stopMoving()
{
	digitalWrite(RIGHT_PLUS, LOW);
	digitalWrite(RIGHT_MINUS, LOW);
	digitalWrite(LEFT_PLUS, LOW);
	digitalWrite(LEFT_MINUS, LOW);
}

void moveForward()
{
	digitalWrite(RIGHT_PLUS, HIGH);
	digitalWrite(RIGHT_MINUS, LOW);
	digitalWrite(LEFT_PLUS, HIGH);
	digitalWrite(LEFT_MINUS, LOW);
}

void moveBack()
{
	digitalWrite(RIGHT_PLUS, LOW);
	digitalWrite(RIGHT_MINUS, HIGH);
	digitalWrite(LEFT_PLUS, LOW);
	digitalWrite(LEFT_MINUS, HIGH);
}

void turnRight() {
	digitalWrite(RIGHT_PLUS, LOW);
	digitalWrite(RIGHT_MINUS, HIGH);
	digitalWrite(LEFT_PLUS, HIGH);
	digitalWrite(LEFT_MINUS, LOW);
}

void turnLeft() {
	digitalWrite(RIGHT_PLUS, HIGH);
	digitalWrite(RIGHT_MINUS, LOW);
	digitalWrite(LEFT_PLUS, LOW);
	digitalWrite(LEFT_MINUS, HIGH);
}

int checkObstacles()
{
	digitalWrite(DOUT_TRIGGER, LOW);
	delayMicroseconds(2);
	digitalWrite(DOUT_TRIGGER, HIGH);
	delayMicroseconds(10);
	digitalWrite(DOUT_TRIGGER, LOW);
	delayMicroseconds(10);
	long distance = pulseIn(DIN_ECHO, HIGH, 2500) / 58.0;

	delay(200);

	//Checks if there is no obstacles in 18 cm
	if (distance >= 2 && distance <= 18)
	{
		moveBack();
		delay(1000);
		turnRight();
		delay(500);
	}
	//Checks if there is no obstacles right and left sides of the car thanks to the IR sensors
	checkCorners();
}