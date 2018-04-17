#include "allcode_api.h"
#include "functions.h"
#include <stdio.h>
#include <stdlib.h>

//if these are defined then debug data is outputted via bluetooth
// #define SENSOR__OUTPUTS
// #define TILT_OUTPUTS
// #define COMPASS_OUTPUTS
// #define MAP_ID_OUTPUTS
// #define SQUARE_SEPERATOR
#define SQUARE_EXPLORATION_OUTPUTS
// #define SQUARE_MOVEMENT_OUTPUTS
#define NAVIGATIONS_OUTPUTS

square *activeSquare;
square *startSquare;
square *darkSquare;

//if the IR sensors report numbers greater than these then a wall is detected
static int RIGHT_NUMBER = 30;
static int LEFT_NUMBER = 30;
static int FRONT_NUMBER = 50;
static int BACK_NUMBER = 595;
static int FRONT_LEFT_NUMBER = 130;
static int FRONT_RIGHT_NUMBER = 115;

//if the light level is lower than this the square is marked as a dark square
static int LIGHT_LEVEL = 300;

//when the square id is equal to this number the entire maze has been ignored
static int MAX_SQUARE_ID = 16;

static int NORTH_COMPASS_HEADING = 0;
static int SOUTH_COMPASS_HEADING = 180;
static int EAST_COMPASS_HEADING = 90;
static int WEST_COMPASS_HEADING = 270;

int compass;
int squareId;

unsigned char stackLocation;

unsigned char compassStack[16];

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
			// break;
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
	celebrate();

	return 0;
}

void navigate(square*start, square*end, int directionFrom) {
//go north until you either find what you want or reach a dead end

	if (start->id == end->id) {
		#ifdef NAVIGATIONS_OUTPUTS
		FA_BTSendString("Already at the end square.\n", 40);
		#endif

		FA_LCDBacklight(100);
		FA_LCDPrint("Found Square.", 16, 20, 25, FONT_NORMAL, LCD_OPAQUE);
		return;
	}

	square *movement = start;
	int wallCheck;

	stackLocation = 0;

	int westCheced;
	int eastChecked;
	int southChecked;
	int northChecked;

	if (numBlockedSides(movement) => 2) {//if you have 2 or more blocked routes
		//search through the only unblocked way
		if (movement->north != NULL && directionFrom != 180) wallCheck = 0;
		else if (movement->east != NULL && directionFrom != 270) wallCheck = 1;
		else if (movement->south != NULL && directionFrom != 0) wallCheck = 2;
		else if (movement->west != NULL && directionFrom != 90) wallCheck = 3;

		switch (wallCheck) {
		case 0:
			//go north and check that block
			movement = movement->north;

			compassStack[stackLocation] = NORTH_COMPASS_HEADING;

			navigate(movement, end, NORTH_COMPASS_HEADING);
			break;
		case 1:
			movement = movement->east;

			compassStack[stackLocation] = EAST_COMPASS_HEADING;

			navigate(movement, end, EAST_COMPASS_HEADING);
			break;

		case 2:
			movement = movement->south;

			compassStack[stackLocation] = SOUTH_COMPASS_HEADING;

			navigate(movement, end, SOUTH_COMPASS_HEADING);
			break;

		case 3:
			movement = movement->west;

			compassStack[stackLocation] = WEST_COMPASS_HEADING;

			navigate(movement, end, WEST_COMPASS_HEADING);
			break;

		}

	} else {

	}


}

int numBlockedSides(square *block) {
	int blockedSides = 0;

	if (block->north == NULL) blockedSides++;
	if (block->south == NULL) blockedSides++;
	if (block->east == NULL) blockedSides++;
	if (block->west == NULL) blockedSides++;

	return blockedSides;
}

/**
returns 0 if there are no branches otherwise will return number of branches
will go along all the squares to the north of this square and see if there are branches to the east or west
**/
unsigned char anyBranchesNorth(square *start) {
	square *movement;

	unsigned char branchNumber = 0;

	while (start->north != NULL) {
		movement = start->north;

		if (movement->west != NULL) {
			branchNumber++;
		}

		if (movement->east != NULL) {
			branchNumber++;
		}
	}

	return branchNumber;
}

unsigned char anyBranchesEast(square *start) {
	// want to check north and south
	square *movement;

	unsigned char branchNumber = 0;

	while (start->east != NULL) {
		movement = start->east;

		if (movement->north != NULL) {
			branchNumber++;
		}

		if (movement->south != NULL) {
			branchNumber++;
		}
	}

	return branchNumber;

}

unsigned char anyBranchesWest(square *start) {
	square *movement;

	unsigned char branchNumber = 0;

	while (start->west != NULL) {
		movement = start->east;

		if (movement->north != NULL) {
			branchNumber++;
		}

		if (movement->south != NULL) {
			branchNumber++;
		}
	}

	return branchNumber;

}

unsigned char anyBranchesSouth(square *start) {
	square *movement;

	unsigned char branchNumber = 0;

	while (start->north != NULL) {
		movement = start->north;

		if (movement->west != NULL) {
			branchNumber++;
		}

		if (movement->east != NULL) {
			branchNumber++;
		}
	}

	return branchNumber;
}

void discoverMaze(void) {

	while (1) {

		#ifdef SQUARE_SEPERATOR
		FA_BTSendString("\n\n\n\n", 20);
		#endif

		if (squareId >= MAX_SQUARE_ID) {
			//the maze has now been fully explored so need to find the way back to the dark square

			#ifdef SQUARE_EXPLORATION_OUTPUTS
			FA_BTSendString("maze has been explored.\n", 40);
			FA_BTSendString("In Square ", 20);
			FA_BTSendNumber(activeSquare->id);
			FA_BTSendString(" dark square is square ", 40);
			FA_BTSendNumber(darkSquare->id);
			FA_BTSendString("\n", 20);
			#endif

			allLEDOn();
			FA_Right(180);

			break;
		}

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

		//check for the dark square
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

		//while there is a wall to the right and forwards is clear move forwards
		if (rightIR >= RIGHT_NUMBER && frontIR < FRONT_NUMBER) {
			moveUntillOverLine();
		} else if (leftIR <= LEFT_NUMBER) {
			//if left is clear go left
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

void moveAlongStack(void) {
	if (stackLocation == 0) {
		//already at the square so dont do anything
		#ifdef SQUARE_EXPLORATION_OUTPUTS
		FA_BTSendString("Done!\n", 20);
		#endif

		return;

	} else {
		int i;
		for (i = 0; i <= stackLocation; i++) {
			if (compassStack[i] == 0) {
				//move to north and advance
				if (compass != 0) {
					unsigned char distanceFromCompass = compass % 360;
					while (distanceFromCompass > 0) {
						FA_Right(90);
						distanceFromCompass - 90;
					}
				}

				while (activeSquare->id != darkSquare->id) {
					moveUntillOverLine();
				}

				allLEDOn();

				while (1) {
					FA_LCDBacklight(100);
					FA_DelayMillis(300);
					FA_LCDBacklight(0);
				}

			} else if (compassStack[i] == 90) {
				//move to east and advance
				if (compass != 90) {
					unsigned char distanceFromCompass = compass % 360;
					while (distanceFromCompass > 0) {
						FA_Right(90);
						distanceFromCompass - 90;
					}
				}

			} else if (compassStack[i] == 180) {
				//move to south and advance
				if (compass != 180) {
					unsigned char distanceFromCompass = compass % 360;
					while (distanceFromCompass > 0) {
						FA_Right(90);
						distanceFromCompass - 90;
					}
				}

			} else {
				//move to the west and advance
				if (compass != 270) {
					unsigned char distanceFromCompass = compass % 360;
					while (distanceFromCompass > 0) {
						FA_Right(90);
						distanceFromCompass - 90;
					}
				}

			}
		}
	}
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

		//also need to go newSquare->back is activeSquare

		if (activeSquare->north != NULL) activeSquare = activeSquare->north;
		else {
			square *moveSquare = createSquare();

			moveSquare->south = activeSquare;

			activeSquare->north = moveSquare;
			activeSquare = moveSquare;

			#ifdef SQUARE_MOVEMENT_OUTPUTS
			FA_BTSendString("Going north and linking square\n", 40);
			#endif
		}

	} else if (compass == 90) {
		//east

		if (activeSquare->east != NULL) activeSquare = activeSquare->east;
		else {
			square *moveSquare = createSquare();

			moveSquare->west = activeSquare;

			activeSquare->east = moveSquare;
			activeSquare = moveSquare;


			#ifdef SQUARE_MOVEMENT_OUTPUTS
			FA_BTSendString("Going east and linking square\n", 40);
			#endif
		}

	} else if (compass == 180) {
		//south

		if (activeSquare->south != NULL) activeSquare = activeSquare->south;
		else {
			square *moveSquare = createSquare();

			moveSquare->north = activeSquare;

			activeSquare->south = moveSquare;
			activeSquare = moveSquare;


			#ifdef SQUARE_MOVEMENT_OUTPUTS
			FA_BTSendString("Going south and linking square\n", 40);
			#endif
		}

	} else {
		//west

		if (activeSquare->west != NULL) activeSquare = activeSquare->west;
		else {
			square *moveSquare = createSquare();

			moveSquare->east = activeSquare;

			activeSquare->west = moveSquare;
			activeSquare = moveSquare;


			#ifdef SQUARE_MOVEMENT_OUTPUTS
			FA_BTSendString("Going west and linking square\n", 40);
			#endif
		}
	}

	#ifdef MAP_ID_OUTPUTS
	FA_BTSendString("now in square ", 20);
	FA_BTSendNumber(activeSquare->id);
	FA_BTSendString("\n", 20);
	#endif


	#ifdef TILT_OUTPUTS
	FA_BTSendString("Front left is ", 30);
	FA_BTSendNumber(frontLeftIR);
	FA_BTSendString("\n", 20);
	#endif

	//these two while loops check to make sure that wheels aren't going to hit the wall
	while (frontLeftIR >= 200) {

		#ifdef TILT_OUTPUTS
		FA_BTSendString("Front left is too close\n", 30);
		FA_BTSendString("Front left is ", 30);
		FA_BTSendNumber(frontLeftIR);
		FA_BTSendString("\n", 20);
		#endif

		FA_Backwards(10);
		FA_Right(5);
		FA_Forwards(10);
		frontLeftIR = FA_ReadIR(1);

		#ifdef TILT_OUTPUTS
		FA_BTSendString("Front left is ", 30);
		FA_BTSendNumber(frontLeftIR);
		FA_BTSendString("\n", 20);
		#endif
	}

	#ifdef TILT_OUTPUTS
	FA_BTSendString("Front right is ", 30);
	FA_BTSendNumber(frontRightIR);
	FA_BTSendString("\n", 20);
	#endif

	while (frontRightIR >= 200) {

		#ifdef TILT_OUTPUTS
		FA_BTSendString("Front right is too close\n", 30);
		FA_BTSendString("Front right is ", 30);
		FA_BTSendNumber(frontRightIR);
		FA_BTSendString("\n", 20);
		#endif

		FA_Backwards(10);
		FA_Left(5);
		FA_Forwards(10);
		frontRightIR = FA_ReadIR(3);

		#ifdef TILT_OUTPUTS
		FA_BTSendString("Front right is ", 30);
		FA_BTSendNumber(frontRightIR);
		FA_BTSendString("\n", 20);
		#endif
	}

	FA_SetMotors(53, 58);

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
	FA_SetDriveSpeed(60);

	frontLeftIR = FA_ReadIR(1);
	frontRightIR = FA_ReadIR(3);

	#ifdef TILT_OUTPUTS
	FA_BTSendString("Front left is ", 30);
	FA_BTSendNumber(frontLeftIR);
	FA_BTSendString("\n", 20);
	#endif

	if (frontLeftIR == 0 && frontRightIR < FRONT_RIGHT_NUMBER) {

		#ifdef TILT_OUTPUTS
		FA_BTSendString("Front left is an empty square so moving slightly\n", 70);
		#endif

		FA_Backwards(10);
		FA_Right(6);
		FA_Forwards(10);
		frontRightIR = FA_ReadIR(3);
	}

	if (frontRightIR == 0 && frontLeftIR < FRONT_LEFT_NUMBER) {

		#ifdef TILT_OUTPUTS
		FA_BTSendString("Front right is an empty square so moving slightly\n", 70);
		#endif

		FA_Backwards(10);
		FA_Left(6);
		FA_Forwards(10);
		frontRightIR = FA_ReadIR(3);
	}

	frontLeftIR = FA_ReadIR(1);
	frontRightIR = FA_ReadIR(3);

	//the robot is now poitioned on the line and this checks to make sure that the robot won't hit anyhting when it moves forwards
	while (frontLeftIR >= FRONT_LEFT_NUMBER) {

		#ifdef TILT_OUTPUTS
		FA_BTSendString("Tilted, turning right\n", 30);
		FA_BTSendString("Front left is ", 30);
		FA_BTSendNumber(frontLeftIR);
		FA_BTSendString("\n", 20);
		#endif

		FA_Backwards(10);
		FA_Right(3);
		FA_Forwards(10);
		frontLeftIR = FA_ReadIR(1);

		#ifdef TILT_OUTPUTS
		FA_BTSendString("Front left is ", 30);
		FA_BTSendNumber(frontLeftIR);
		FA_BTSendString("\n", 20);
		#endif
	}

	#ifdef TILT_OUTPUTS
	FA_BTSendString("front right is ", 30);
	FA_BTSendNumber(frontRightIR);
	FA_BTSendString("\n", 20);
	#endif

	while (frontRightIR >= FRONT_RIGHT_NUMBER) {

		#ifdef TILT_OUTPUTS
		FA_BTSendString("Tilted, turning left ", 30);
		FA_BTSendString("front right is ", 30);
		FA_BTSendNumber(frontRightIR);
		FA_BTSendString("\n", 20);
		#endif

		FA_Backwards(10);
		FA_Left(3);
		FA_Forwards(10);
		frontRightIR = FA_ReadIR(3);

		#ifdef TILT_OUTPUTS
		FA_BTSendString("Front right is ", 30);
		FA_BTSendNumber(frontRightIR);
		FA_BTSendString("\n", 20);
		#endif
	}

	//the robot is now properly aligned and can move forwards to take it into the centre of the square.
	FA_Forwards(105);

	// FA_DelayMillis(300);
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

square *createSquare(void) {
	//wall is 0
	//3 is unknown
	//gone through is 1
	//need to go through is 2

	square *returnNode = malloc(sizeof(square));

	returnNode->id = squareId;

	returnNode->searched = 0;

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

void lightShow(int counter) {
	int i;
	for (i = 0; i < counter; i++) {
		FA_LCDBacklight(100);
		allLEDOn();
		FA_DelayMillis(100);
		FA_LCDBacklight(0);
		allLEDOff();
		FA_DelayMillis(75);
	}
}

void celebrate(void) {
	FA_SetDriveSpeed(100);
	lightShow(10);
	FA_Right(30);
	FA_Left(30);
	FA_Right(720);
	FA_Left(720);
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