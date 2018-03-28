typedef struct box {
	int front;
	int back;
	int left;
	int right;

	int isLowLight;
	int discovered;

	struct box *frontSquare;
	struct box *backSquare;
	struct box *leftSquare;
	struct box *rightSquare;
} square;

square createSquare();

void moveUntillOverLine();

void movement();

void allLEDOn();

void allLEDOff();

void discover(square *activeSquare);