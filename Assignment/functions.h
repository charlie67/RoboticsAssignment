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

void discoverMaze(void);

void measureLight(void);

void compassRight(void);

void compassLeft(void);

void compass180(void);

void lightShow(int counter);

void moveAlongStack(void);

void celebrate(void);

void printAdjList(void);

void connectAdj(int id1, int id2);

void printDistances(int n);

void dijkstra(int src);

int minDistance(bool sptSet[]);

void moveOverStack(int dest);

void moveToSquare(int destination);

void faceNorth(void);

void faceEast(void);

void faceWest(void);

void faceSouth(void);