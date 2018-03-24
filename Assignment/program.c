#include "allcode_api.h"
#include "functions.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
	FA_RobotInit();
	FA_SetDriveSpeed(50);
	FA_LCDBacklight(50);

	square activeSquare = createBox(6,6,6,6);

	while (FA_BTConnected() == 0){
		FA_LCDPrint("BT Not Connected", 16, 20,25,FONT_NORMAL, LCD_OPAQUE);
		FA_DelaySecs(2);
		FA_LCDClear();
	}

	FA_LCDClear();

	FA_BTSendUnsigned(activeSquare.ahead);
	FA_BTSendString("\n", 15);

	

	while (1) {


		//check all of the walls to see which ones you can move to
		unsigned short front = FA_ReadIR (2);
		FA_BTSendString("front is\n", 9);
		FA_BTSendUnsigned(front);
		FA_BTSendString("\n", 2);

		unsigned short rear = FA_ReadIR (6);
		FA_BTSendString("rear is\n", 9);
		FA_BTSendUnsigned(rear);
		FA_BTSendString("\n", 2);

		unsigned short left = FA_ReadIR (0);
		FA_BTSendString("left is\n", 9);
		FA_BTSendUnsigned(left);
		FA_BTSendString("\n", 2);

		unsigned short right = FA_ReadIR (4);
		FA_BTSendString("right is\n", 9);
		FA_BTSendUnsigned(right);
		FA_BTSendString("\n", 2);


		//priority of 0 is wall, 2 is new place to go and 1 is a place that you have been
		
		if (left>=170) {
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
		
		if (front>=313) {	
			activeSquare.ahead = 0;

			FA_BTSendString("Front Blocked\n", 15);
			FA_BTSendString("Front is now ", 15);
			FA_BTSendUnsigned(activeSquare.ahead);
			FA_BTSendString("\n", 15);

			FA_LEDOn(2);
		} else {
			activeSquare.ahead = 2;

			FA_BTSendString("Front is now ", 15);
			FA_BTSendUnsigned(activeSquare.ahead);
			FA_BTSendString("\n", 15);

			FA_LEDOff(2);
		}
		
		if (right>=242) {	
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
		
		if (rear>=595) {	
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
	}



	return 0;	
}

void movement(square activeSquare){
	if (activeSquare.ahead == 2) {
			//go forwards
			FA_Forwards(160);
			return;
	} 

	if (activeSquare.right == 2){
			FA_Right(90);
			FA_Forwards(160);
			return;
	} 

	if (activeSquare.left == 2){
			FA_Left(90);
			FA_Forwards(160);
			return;
	}  

	if (activeSquare.back == 2) {
			FA_Right(180);
			FA_Forwards(160);
			return;
	} 

	while (1){
		FA_LCDPrint("Error", 5, 20,25,FONT_NORMAL, LCD_OPAQUE);
	}
	
}

square createBox(int l, int r, int b, int f) {
	
	square newNode = malloc(sizeof(square));
	
	newNode.right = r;
	newNode.left = l;
	newNode.back = b;
	newNode.ahead = f;
	
	newNode.frontSquare =NULL;
	newNode.behindSquare =NULL;
	newNode.leftSquare =NULL;
	newNode.rightSquare =NULL;
	
	return newNode;
}