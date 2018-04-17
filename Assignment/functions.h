typedef struct box {
	int id;

	int searched;

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

void navigate(square *start, square *end);

void lightShow(int counter);

void moveAlongStack(void);

void celebrate(void);

unsigned char anyBranchesNorth(square *start);

unsigned char anyBranchesSouth(square *start);

unsigned char anyBranchesEast(square *start);

unsigned char anyBranchesWest(square *start);

int numBlockedSides(square *block);