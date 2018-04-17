typedef struct head {
	int capacity;
	int size;
	int front;
	int rear;
	int *elements;
} queue;

queue * createQueue(int maxElements);

int isQueueEmpty(queue *queueHead);

void dequeue(queue *queueHead);

int front(queue *queueHead);

void enqueue(queue *queueHead, int element);


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

void lightShow(int counter);

void moveAlongStack(void);

void celebrate(void);