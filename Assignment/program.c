#include "allcode_api.h"
#include "functions.h"
#include <stdio.h>
#include <stdlib.h>


int main() {
	FA_RobotInit();
	FA_LCDBacklight(50);

	while (FA_BTConnected() == 0) {
		FA_LCDPrint("BT Not Connected", 16, 20, 25, FONT_NORMAL, LCD_OPAQUE);
		FA_DelaySecs(2);
		FA_LCDClear();
	}

	FA_SetDriveSpeed(50);

	square startSquare = createSquare();
	square *activeSquare = &startSquare;
	square *lowLightSquare;


	if (activeSquare == NULL) {
		FA_BTSendString("NULL Pointer", 15);
		FA_DelaySecs(1000);
	}
	FA_LCDClear();


	while (1) {
		FA_BTSendUnsigned(activeSquare->discovered);
		FA_BTSendUnsigned(activeSquare->front);
		FA_BTSendUnsigned(activeSquare->left);
		FA_BTSendUnsigned(activeSquare->right);
		FA_BTSendUnsigned(activeSquare->back);
		if (activeSquare->frontSquare == NULL) {
			FA_BTSendString("fuck\n", 20);
		} else {
			FA_BTSendString("Not fuck\n", 20);
		}
		FA_BTSendString("\n", 3);


		//check all of the walls to see which ones you can move to
		discover(activeSquare);


		//now need to decide which direction to go
		movement(activeSquare);

		if (FA_ReadIR(1) > 100 && activeSquare->right != 0) {
			FA_SetDriveSpeed(50);
			FA_BTSendString("Turning right slightly\n", 15);
			while (FA_ReadIR(1) > 100) {
				FA_Right(1);
			}
		}

		if (FA_ReadIR(3) > 90 && activeSquare->left != 0) {
			FA_SetDriveSpeed(50);
			FA_BTSendString("Turning left slightly\n", 15);
			while (FA_ReadIR(3) > 90) {
				FA_Left(1);
			}
		}

		FA_DelaySecs(2);
		FA_BTSendString("\n\n\n", 15);
	}



	return 0;
}

void discover(square *activeSquare) {

	if (activeSquare->discovered == 0) {
		//if the square hasn't been discovered yet then go through the measuring process

		FA_BTSendString("Moved to an undiscovered square\n", 100);

		unsigned short frontIR = FA_ReadIR (2);
		FA_BTSendString("frontIR is \n", 15);
		FA_BTSendUnsigned(frontIR);
		FA_BTSendString("\n", 2);

		unsigned short rearIR = FA_ReadIR (6);
		FA_BTSendString("rearIR is \n", 15);
		FA_BTSendUnsigned(rearIR);
		FA_BTSendString("\n", 2);

		unsigned short leftIR = FA_ReadIR (0);
		FA_BTSendString("leftIR is \n", 15);
		FA_BTSendUnsigned(leftIR);
		FA_BTSendString("\n", 2);

		unsigned short rightIR = FA_ReadIR (4);
		FA_BTSendString("rightIR is \n", 15);
		FA_BTSendUnsigned(rightIR);
		FA_BTSendString("\n", 2);

		if (leftIR >= 170) {
			//left is blocked set it's priority to 0
			activeSquare->left = 0;

			FA_BTSendString("Left Blocked\n", 15);
			FA_BTSendString("Left is now ", 15);
			FA_BTSendUnsigned(activeSquare->left);
			FA_BTSendString("\n", 15);

			FA_LEDOn(0);
		} else {
			activeSquare->left = 2;

			FA_BTSendString("Left is now ", 15);
			FA_BTSendUnsigned(activeSquare->left);
			FA_BTSendString("\n", 15);

			FA_LEDOff(0);
		}

		if (frontIR >= 313) {
			activeSquare->front = 0;

			FA_BTSendString("Front Blocked\n", 15);
			FA_BTSendString("Front is now ", 15);
			FA_BTSendUnsigned(activeSquare->front);
			FA_BTSendString("\n", 15);

			FA_LEDOn(2);
		} else {
			activeSquare->front = 2;

			FA_BTSendString("Front is now ", 15);
			FA_BTSendUnsigned(activeSquare->front);
			FA_BTSendString("\n", 15);

			FA_LEDOff(2);
		}

		if (rightIR >= 242) {
			activeSquare->right = 0;

			FA_BTSendString("Right Blocked\n", 15);
			FA_BTSendString("Right is now ", 15);
			FA_BTSendUnsigned(activeSquare->right);
			FA_BTSendString("\n", 15);

			FA_LEDOn(4);
		} else {
			activeSquare->right = 2;

			FA_BTSendString("Right is now ", 15);
			FA_BTSendUnsigned(activeSquare->right);
			FA_BTSendString("\n", 15);

			FA_LEDOff(4);
		}

		if (rearIR >= 595) {
			activeSquare->back = 0;

			FA_BTSendString("Back Blocked\n", 15);
			FA_BTSendString("Back is now ", 15);
			FA_BTSendUnsigned(activeSquare->back);
			FA_BTSendString("\n", 15);

			FA_LEDOn(6);
		} else {
			activeSquare->back = 2;

			FA_BTSendString("Back is now ", 15);
			FA_BTSendUnsigned(activeSquare->back);
			FA_BTSendString("\n", 15);

			FA_LEDOff(6);
		}
	} else {
		FA_BTSendString("Moved back to a discovered square\n", 100);
	}

	activeSquare->discovered = 1;
}


void movement(square *activeSquare) {
	square moveTo = createSquare();

	if (activeSquare->front >= 1 && activeSquare->back >= 1 && activeSquare->left >= 1 && activeSquare->right >= 1) {
		//need to find an undiscovered square
		FA_BTSendString("currently in a discovered square\n", 100);
		FA_BTSendString("Move to the next undiscovered square\n", 100);
		//start at active and see if you can find a square with a connection of 2
		while (1) {

		}

	}

	if (activeSquare->front == 2) {
		FA_BTSendString("Going forwards\n", 15);

		activeSquare->front = 1;
		activeSquare->frontSquare = &moveTo;
		activeSquare = &moveTo;
		FA_BTSendUnsigned(activeSquare->front);
		FA_BTSendUnsigned(activeSquare->discovered);
		FA_BTSendUnsigned(moveTo.front);
		FA_BTSendUnsigned(moveTo.discovered);

		moveUntillOverLine();
		return 1;
	}

	if (activeSquare->right == 2) {
		FA_BTSendString("Going right\n", 15);

		activeSquare->right = 1;
		activeSquare->rightSquare = &moveTo;
		activeSquare = &moveTo;


		FA_Right(90);
		moveUntillOverLine();
		return 2;
	}

	if (activeSquare->left == 2) {
		FA_BTSendString("Going left\n", 15);

		activeSquare->left = 1;
		activeSquare->leftSquare = &moveTo;
		activeSquare = &moveTo;


		FA_Left(90);
		moveUntillOverLine();
		return 3;
	}

	if (activeSquare->back == 2) {
		FA_BTSendString("Going back\n", 15);

		activeSquare->back = 1;
		activeSquare->backSquare = &moveTo;
		activeSquare = &moveTo;


		FA_Right(180);
		moveUntillOverLine();
		return 4;
	}

	if (activeSquare->front == 1) {
		FA_BTSendString("Going forwards\n", 15);

		activeSquare->frontSquare = &moveTo;
		activeSquare = &moveTo;

		moveUntillOverLine();
		return 1;
	}

	if (activeSquare->right == 1) {
		FA_BTSendString("Going right\n", 15);

		activeSquare->rightSquare = &moveTo;
		activeSquare = &moveTo;


		FA_Right(90);
		moveUntillOverLine();
		return 2;
	}

	if (activeSquare->left == 1) {
		FA_BTSendString("Going left\n", 15);
		activeSquare->leftSquare = &moveTo;
		activeSquare = &moveTo;


		FA_Left(90);
		moveUntillOverLine();
		return 3;
	}

	if (activeSquare->back == 1) {
		FA_BTSendString("Going back\n", 15);
		activeSquare->backSquare = &moveTo;
		activeSquare = &moveTo;


		FA_Right(180);
		moveUntillOverLine();
		return 4;
	}


	while (1) {
		FA_LCDPrint("Error", 5, 20, 25, FONT_NORMAL, LCD_OPAQUE);
		FA_BTSendString("No where to move to\n", 100);
		while (1) {

		}
	}
}

void moveUntillOverLine() {
	FA_SetMotors(20, 25);
	while (1) {
		if (FA_ReadLine(0) < 10) {
			break;
			//gone over the line
		}
	}
	FA_DelayMillis(375);
	FA_SetMotors(0, 0);

	FA_DelaySecs(2);
}

void allLEDOn() {
	int i;
	for (i = 0; i < 8; i++) {
		FA_LEDOn(i);
	}
}

void allLEDOff() {
	int i;
	for (i = 0; i < 8; i++) {
		FA_LEDOff(i);
	}
}


square createSquare() {
	//wall is 0
	//3 is unknown
	//gone through is 1
	//need to go through is 2

	square returnNode;

	returnNode.discovered = 0;
	returnNode.isLowLight = 0;

	returnNode.right = 3;
	returnNode.left = 3;
	returnNode.back = 3;
	returnNode.front = 3;

	returnNode.frontSquare = NULL;
	returnNode.backSquare = NULL;
	returnNode.leftSquare = NULL;
	returnNode.rightSquare = NULL;

	return returnNode;
}