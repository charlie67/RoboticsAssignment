#include "allcode_api.h"
#include "functions.h"
#include <stdio.h>
#include <stdlib.h>


square activeSquare;
// square startSquare;

int main() {
	FA_RobotInit();
	FA_SetDriveSpeed(50);
	FA_LCDBacklight(50);

	activeSquare = createSquare(3, 3, 3, 3);
	// activesquare = &startSquare;


	while (FA_BTConnected() == 0) {
		FA_LCDPrint("BT Not Connected", 16, 20, 25, FONT_NORMAL, LCD_OPAQUE);
		FA_DelaySecs(2);
		FA_LCDClear();
	}

	FA_LCDClear();

	if (activeSquare.front != 10) {
		FA_BTSendUnsigned(activeSquare.front);
		FA_BTSendString("\n\n", 15);
	}



	while (1) {


		//check all of the walls to see which ones you can move to
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


		//priority of 0 is wall, 2 is new place to go and 1 is a place that you have been

		if (leftIR >= 170) {
			//left is blocked set it's priority to 0
			activeSquare.left = 0;

			FA_BTSendString("Left Blocked\n", 15);
			FA_BTSendString("Left is now ", 15);
			FA_BTSendUnsigned(activeSquare.left);
			FA_BTSendString("\n", 15);

			FA_LEDOn(0);
		} else {
			//this direction is unblocked so set it's prority to be 2 to reflect that
			activeSquare.left = 2;

			FA_BTSendString("Left is now ", 15);
			FA_BTSendUnsigned(activeSquare.left);
			FA_BTSendString("\n", 15);

			FA_LEDOff(0);
		}

		if (frontIR >= 313) {
			activeSquare.front = 0;

			FA_BTSendString("Front Blocked\n", 15);
			FA_BTSendString("Front is now ", 15);
			FA_BTSendUnsigned(activeSquare.front);
			FA_BTSendString("\n", 15);

			FA_LEDOn(2);
		} else {
			activeSquare.front = 2;

			FA_BTSendString("Front is now ", 15);
			FA_BTSendUnsigned(activeSquare.front);
			FA_BTSendString("\n", 15);

			FA_LEDOff(2);
		}

		if (rightIR >= 242) {
			activeSquare.right = 0;

			FA_BTSendString("Right Blocked\n", 15);
			FA_BTSendString("Right is now ", 15);
			FA_BTSendUnsigned(activeSquare.right);
			FA_BTSendString("\n", 15);

			FA_LEDOn(4);
		} else {
			activeSquare.right = 2;

			FA_BTSendString("Right is now ", 15);
			FA_BTSendUnsigned(activeSquare.right);
			FA_BTSendString("\n", 15);

			FA_LEDOff(4);
		}

		if (rearIR >= 595) {
			activeSquare.back = 0;

			FA_BTSendString("Back Blocked\n", 15);
			FA_BTSendString("Back is now ", 15);
			FA_BTSendUnsigned(activeSquare.back);
			FA_BTSendString("\n", 15);

			FA_LEDOn(6);
		} else {
			activeSquare.back = 2;

			FA_BTSendString("Back is now ", 15);
			FA_BTSendUnsigned(activeSquare.back);
			FA_BTSendString("\n", 15);

			FA_LEDOff(6);
		}

		// FA_DelaySecs(4);


		//now need to decide which direction to go
		movement(activeSquare);

		//set activesquare to the moved to square

		FA_DelaySecs(2);
		FA_BTSendString("\n\n\n", 15);
	}



	return 0;
}

void movement(square activeSquare) {
	square moveTo = createSquare(3, 3, 3, 3);

	if (activeSquare.front == 2) {
		FA_BTSendString("Going forwards\n", 15);
		activeSquare.front = 1;
		activeSquare.frontSquare = &moveTo;

		FA_Forwards(150);
		return;
	}

	if (activeSquare.right == 2) {
		FA_BTSendString("Going right\n", 15);
		activeSquare.right = 1;
		activeSquare.rightSquare = &moveTo;

		FA_Right(90);
		FA_Forwards(150);
		return;
	}

	if (activeSquare.left == 2) {
		FA_BTSendString("Going left\n", 15);
		activeSquare.left = 1;
		activeSquare.leftSquare = &moveTo;

		FA_Left(90);
		FA_Forwards(150);
		return;
	}

	if (activeSquare.back == 2) {
		FA_BTSendString("Going back\n", 15);
		activeSquare.back = 1;
		activeSquare.backSquare = &moveTo;

		FA_Right(180);
		FA_Forwards(150);
		return;
	}

	if (activeSquare.front == 1) {
		A_BTSendString("Going forwards\n", 15);
		activeSquare.frontSquare = &moveTo;

		FA_Forwards(150);
		return;
	}

	if (activeSquare.right == 1) {
		FA_BTSendString("Going right\n", 15);
		activeSquare.rightSquare = &moveTo;

		FA_Right(90);
		FA_Forwards(150);
		return;
	}

	if (activeSquare.left == 1) {
		FA_BTSendString("Going left\n", 15);
		activeSquare.leftSquare = &moveTo;

		FA_Left(90);
		FA_Forwards(150);
		return;
	}

	if (activeSquare.back == 1) {
		FA_BTSendString("Going back\n", 15);
		activeSquare.backSquare = &moveTo;

		FA_Right(180);
		FA_Forwards(150);
		return;
	}


	while (1) {
		FA_LCDPrint("Error", 5, 20, 25, FONT_NORMAL, LCD_OPAQUE);
		FA_BTSendString("No where to move to\n", 15);
		FA_DelaySecs(1000);
	}
}



square createSquare(int l, int r, int b, int f) {
	//wall is 0
	//need to go through is 3
	//gone through is 1

	square newNode;

	newNode.right = r;
	newNode.left = l;
	newNode.back = b;
	newNode.front = f;

	newNode.frontSquare = NULL;
	newNode.backSquare = NULL;
	newNode.leftSquare = NULL;
	newNode.rightSquare = NULL;

	return newNode;
}