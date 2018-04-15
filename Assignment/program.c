#include "allcode_api.h"
#include "functions.h"
#include <stdio.h>
#include <stdlib.h>

//if these are defined then debug data is outputted via bluetooth
#define SENSOR__OUTPUTS
#define TILT_OUTPUTS
// #define COMPASS_OUTPUTS
#define MAP_ID_OUTPUTS
#define SQUARE_SEPERATOR

square *activeSquare;
square *startSquare;
square *darkSquare;

//if the IR sensors report numbers greater than these then a wall is detected
static int RIGHT_NUMBER = 30;
static int LEFT_NUMBER = 30;
static int FRONT_NUMBER = 50;
static int BACK_NUMBER = 595;
static int FRONT_LEFT_NUMBER = 70;
static int FRONT_RIGHT_NUMBER = 60;

//if the light level is lower than this the square is marked as a dark square
static int LIGHT_LEVEL = 900;

int compass;
int squareId;


int main() {
	FA_RobotInit();
	FA_LCDBacklight(50);

	square *startSquare = createSquare();

	activeSquare = startSquare;

	while (FA_BTConnected() == 0) {
		FA_LCDPrint("BT Not Connected", 16, 20, 25, FONT_NORMAL, LCD_OPAQUE);
		FA_DelaySecs(2);
		FA_LCDClear();
		if (FA_ReadSwitch(0) == 1) {
			FA_DelaySecs(2);
			break;
		}
		if (FA_ReadSwitch(1) == 1) {
			FA_DelaySecs(2);
			break;
		}
	}
	FA_BTSendString("Connected\n", 20);
	FA_LCDBacklight(0);

	compass = 0;
	squareId = 0;
	startSquare = createSquare();

	discoverMaze();

	return 0;
}

void discoverMaze(void) {

	while (1) {

		#ifdef SQUARE_SEPERATOR
		FA_BTSendString("\n\n\n\n", 20);
		#endif

		unsigned short rightIR = FA_ReadIR (4);
		unsigned short frontIR = FA_ReadIR (2);
		unsigned short rearIR = FA_ReadIR (6);
		unsigned short leftIR = FA_ReadIR (0);

		#ifdef SENSOR__OUTPUTS
		FA_BTSendString("Right: ", 20);
		FA_BTSendNumber(rightIR);
		FA_BTSendString("\n", 10);

		FA_BTSendString("Front: ", 20);
		FA_BTSendNumber(frontIR);
		FA_BTSendString("\n", 10);

		FA_BTSendString("Rear: ", 20);
		FA_BTSendNumber(rearIR);
		FA_BTSendString("\n", 10);


		FA_BTSendString("Left: ", 20);
		FA_BTSendNumber(leftIR);
		FA_BTSendString("\n", 10);
		#endif

		unsigned short lightLevel = FA_ReadLight();

		if (lightLevel < LIGHT_LEVEL) {
			FA_BTSendString("Found dark square\n", 30);

			allLEDOn();
			FA_PlayNote(261, 100);
			FA_LCDBacklight(100);

			darkSquare = activeSquare;

			FA_Right(180);
			compass180();
			moveUntillOverLine();
			continue;
		}

		//if left front and right are all blocked do a 180 and go forwards
		if (leftIR >= LEFT_NUMBER && rightIR >= RIGHT_NUMBER && frontIR >= FRONT_NUMBER) {
			//need to go backwards
			FA_Right(180);
			compass180();

			moveUntillOverLine();
			continue;
		}

		//if left and right is free go right
		if (leftIR < LEFT_NUMBER && rightIR < RIGHT_NUMBER) {
			FA_Right(90);
			compassRight();

			moveUntillOverLine();
			continue;
		}

		//while there is a wall to the right move forwards
		if (rightIR >= RIGHT_NUMBER && frontIR < FRONT_NUMBER) {
			moveUntillOverLine();
		} else if (leftIR <= LEFT_NUMBER) {
			FA_Left(90);
			compassLeft();

			moveUntillOverLine();
		} else {
			FA_Right(90);
			compassRight();

			moveUntillOverLine();
		}
	}
}

void compassLeft(void) {

	#ifdef COMPASS_OUTPUTS
	FA_BTSendString("Compass was ", 20);
	FA_BTSendNumber(compass);
	FA_BTSendString("\n", 5);
	#endif

	if (compass == 0) {
		compass = 270;
	} else {
		compass -= 90;
		compass %= 360;
	}

	#ifdef COMPASS_OUTPUTS
	FA_BTSendString("Compass now ", 20);
	FA_BTSendNumber(compass);
	FA_BTSendString("\n", 5);
	#endif
}

void compassRight(void) {

	#ifdef COMPASS_OUTPUTS
	FA_BTSendString("Compass was ", 20);
	FA_BTSendNumber(compass);
	FA_BTSendString("\n", 5);
	#endif

	compass += 90;
	compass %= 360;

	#ifdef COMPASS_OUTPUTS
	FA_BTSendString("Compass now ", 20);
	FA_BTSendNumber(compass);
	FA_BTSendString("\n", 5);
	#endif
}

void compass180(void) {

	#ifdef COMPASS_OUTPUTS
	FA_BTSendString("Compass was ", 20);
	FA_BTSendNumber(compass);
	FA_BTSendString("\n", 5);
	#endif

	compass += 180;
	compass %= 360;

	#ifdef COMPASS_OUTPUTS
	FA_BTSendString("Compass now ", 20);
	FA_BTSendNumber(compass);
	FA_BTSendString("\n", 5);
	#endif

}

void moveUntillOverLine(void) {
	unsigned short frontLeftIR = FA_ReadIR(1);
	unsigned short frontRightIR = FA_ReadIR(3);

	#ifdef MAP_ID_OUTPUTS
	FA_BTSendString("Was in square ", 20);
	FA_BTSendNumber(activeSquare->id);
	FA_BTSendString("\n", 20);
	#endif


	if (compass == 0) {
		//north

		//first check to see if that square is free if so move activeSquare to that
		//if not create a new square and set the current activeSquare direction pointer
		// to that new square and then set the activeSquare pointer to the new square

		if (activeSquare->north != NULL) activeSquare = activeSquare->north;
		else {
			square *moveSquare = createSquare();
			activeSquare->north = moveSquare;
			activeSquare = moveSquare;
		}

	} else if (compass == 90) {
		//east

		if (activeSquare->east != NULL) activeSquare = activeSquare->east;
		else {
			square *moveSquare = createSquare();
			activeSquare->east = moveSquare;
			activeSquare = moveSquare;
		}

	} else if (compass == 180) {
		//south

		if (activeSquare->south != NULL) activeSquare = activeSquare->south;
		else {
			square *moveSquare = createSquare();
			activeSquare->south = moveSquare;
			activeSquare = moveSquare;
		}

	} else {
		//west

		if (activeSquare->west != NULL) activeSquare = activeSquare->west;
		else {
			square *moveSquare = createSquare();
			activeSquare->west = moveSquare;
			activeSquare = moveSquare;
		}
	}

	#ifdef MAP_ID_OUTPUTS
	FA_BTSendString("now in square ", 20);
	FA_BTSendNumber(activeSquare->id);
	FA_BTSendString("\n", 20);
	#endif


	//these two while loops check to make sure that wheels aren't going to hit the wall
	while (frontLeftIR >= 350) {

		#ifdef TILT_OUTPUTS
		FA_BTSendString("Front left is too close\n", 30);
		#endif

		FA_Backwards(10);
		FA_Right(5);
		FA_Forwards(10);
		frontLeftIR = FA_ReadIR(1);
	}

	while (frontRightIR >= 200) {

		#ifdef TILT_OUTPUTS
		FA_BTSendString("Front right is too close\n", 30);
		#endif

		FA_Backwards(10);
		FA_Left(5);
		FA_Forwards(10);
		frontRightIR = FA_ReadIR(3);
	}

	FA_SetMotors(20, 24);

	while (1) {
		//go forwards untill the line sensor detects the line
		if (FA_ReadLine(0) < 10) {
			allLEDOn();
			break;
			//gone over the line
		}
	}

	FA_LCDBacklight(0);
	allLEDOff();
	//have the light turn off at the right time when exiting the dark square

	FA_SetMotors(0, 0);
	FA_SetDriveSpeed(50);

	frontLeftIR = FA_ReadIR(1);
	frontRightIR = FA_ReadIR(3);

	//the robot is now poitioned on the line and this checks to make sure that the robot won't hit anyhting when it moves forwards
	while (frontLeftIR >= FRONT_LEFT_NUMBER) {

		#ifdef TILT_OUTPUTS
		FA_BTSendString("Tilted, turning right\n", 30);
		#endif

		FA_Backwards(10);
		FA_Right(5);
		FA_Forwards(10);
		frontLeftIR = FA_ReadIR(1);
	}

	while (frontRightIR >= FRONT_RIGHT_NUMBER) {

		#ifdef TILT_OUTPUTS
		FA_BTSendString("Tilted, turning left\n", 30);
		#endif

		FA_Backwards(10);
		FA_Left(5);
		FA_Forwards(10);
		frontRightIR = FA_ReadIR(3);
	}

	//the robot is now properly aligned and can move forwards to take it into the centre of the square.
	FA_Forwards(105);

	FA_DelayMillis(300);
}

square *createSquare(void) {
	//wall is 0
	//3 is unknown
	//gone through is 1
	//need to go through is 2

	square *returnNode = malloc(sizeof(square));

	returnNode->id = squareId;

	returnNode->north = NULL;
	returnNode->east = NULL;
	returnNode->west = NULL;
	returnNode->south = NULL;

	squareId++;

	return returnNode;
}

void allLEDOn(void) {
	int i;
	for (i = 0; i < 8; i++) {
		FA_LEDOn(i);
	}
}

void allLEDOff(void) {
	int i;
	for (i = 0; i < 8; i++) {
		FA_LEDOff(i);
	}
}

void measureCornerSensors(void) {
	while (1) {
		unsigned short frontLeftIR = FA_ReadIR(1);
		FA_BTSendString("Front Left: ", 20);
		FA_BTSendNumber(frontLeftIR);
		FA_BTSendString("\n", 10);

		unsigned short frontRightIR = FA_ReadIR(3);
		FA_BTSendString("Front Right: ", 20);
		FA_BTSendNumber(frontRightIR);
		FA_BTSendString("\n", 10);

		unsigned short backLeftIR = FA_ReadIR(7);
		FA_BTSendString("Back Left: ", 20);
		FA_BTSendNumber(backLeftIR);
		FA_BTSendString("\n", 10);

		unsigned short backRightIR = FA_ReadIR(5);
		FA_BTSendString("Back Right: ", 20);
		FA_BTSendNumber(backRightIR);
		FA_BTSendString("\n", 10);

		FA_DelaySecs(2);
		FA_BTSendString("\n\n\n\n\n", 20);

		if (FA_ReadSwitch(0) == 1) {

			FA_BTSendString("Stopped\n", 20);
			FA_DelaySecs(5);

			while (1) {
				if (FA_ReadSwitch(0) == 1) break;
			}
		}

		if (FA_ReadSwitch(1) == 1) {

			FA_BTSendString("Stopped\n", 20);
			FA_DelaySecs(5);

			while (1) {
				if (FA_ReadSwitch(1) == 1) break;
			}
		}
	}
}

void measureLight(void) {
	while (1) {
		FA_LCDClear();
		unsigned short lightValue = FA_ReadLight();
		// FA_BTSendUnsigned(lightValue);
		FA_LCDNumber(lightValue, 0, 0, 0, 0);
		FA_DelaySecs(1);
	}
}