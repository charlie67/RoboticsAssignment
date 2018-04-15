typedef struct box {
	int dark;
	int id;

	struct box * north;
	struct box * east;
	struct box * south;
	struct box * west;

} square;

square *createSquare(void);

void moveUntillOverLine(void);

void turnRight(void);

void turnLeft(void);

void turnRound(void);

void measureCornerSensors(void);

void allLEDOn(void);

void allLEDOff(void);

void movement(void);

void discoverMaze(void);

void measureLight(void);

void compassRight(void);

void compassLeft(void);

void compass180(void);