#include "allcode_api.h"
#include "functions.h"
#include <stdio.h>
#include <stdlib.h>

using namespace std;


int main() {
	FA_RobotInit();
	FA_LCDBacklight(50);

	while (FA_BTConnected() == 0) {
		FA_LCDPrint("BT Not Connected", 16, 20, 25, FONT_NORMAL, LCD_OPAQUE);
		FA_DelaySecs(2);
		FA_LCDClear();
	}


	FA_BTSendUnsigned(sizeof(square));

	square *activeSquare = malloc(20);

	if (activeSquare==NULL){
		FA_BTSendString("malloc failed null pointer\n", 50);
	}

	while(1){

	}


	return 0;
}
