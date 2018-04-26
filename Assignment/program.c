#include "allcode_api.h"
#include "functions.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

//if these are defined then debug data is outputted via bluetooth
#define SENSOR__OUTPUTS
#define TILT_OUTPUTS
// #define COMPASS_OUTPUTS
// #define MAP_ID_OUTPUTS
#define SQUARE_SEPERATOR
// #define SQUARE_EXPLORATION_OUTPUTS
// #define SQUARE_MOVEMENT_OUTPUTS
// #define ADJANCY_OUTPUTS
// #define STACK_OUTPUTS
// #define DIJKSTRA_OUTPUT
// #define MOVING_BACK_OUTPUTS

square *activeSquare;
square *startSquare;
square *darkSquare;

//if the IR sensors report numbers greater than these then a wall is detected
static int RIGHT_NUMBER = 15;
static int LEFT_NUMBER = 30;
static int FRONT_NUMBER = 50;
static int BACK_NUMBER = 595;
static int FRONT_LEFT_NUMBER = 130;
static int FRONT_RIGHT_NUMBER = 135;

//if the light level is lower than this the square is marked as a dark square
static int LIGHT_LEVEL = 300;

static int NORTH_COMPASS_HEADING = 0;
static int SOUTH_COMPASS_HEADING = 180;
static int EAST_COMPASS_HEADING = 90;
static int WEST_COMPASS_HEADING = 270;
static int NULL_COMPASS_HEADING = -1;

static int SQUARE_SEARCHED = 0;
static int SQUARE_UNSEARCHED = 1;

static int INT_MAX = 100;

static int RIGHT_ANGLE = 83;
static int LEFT_ANGLE = 83;
static int TURN_ANGLE = 173;

//when the square id is equal to this number the entire maze has been ignored
static int MAX_SQUARE_ID = 16;

int compass;
int squareId;

unsigned char adjancy[16][16];
//this acts us a adjacency matrix
//0 is not connected 1 is connected

int distance[16];
//distance outputs of dijkstras

//acts as a map to see which squares are connected from your current square
//for example if you want to get to square 10 then it will return the square next to it that
//you have to reach before visiting that square
int visit[16];

//these two act as a stack of locations to visit when the robot is navigating back to the dark square
char movement[16];
int movementCounter;


int main() {
	FA_RobotInit();
	FA_LCDBacklight(50);

	int i, j;
	for (i = 0; i < 16; i++) {
		for (j = 0; j < 16; j++) {
			adjancy[i][j] = 0;
		}
	}
	//initalize adjacency matrix to all disconnected

	#ifdef ADJANCY_OUTPUTS
	printAdjList();
	#endif


	square *startSquare = createSquare();

	activeSquare = startSquare;

	while (FA_BTConnected() == 0) {
		FA_LCDPrint("BT Not Connected", 16, 20, 25, FONT_NORMAL, LCD_OPAQUE);
		FA_DelaySecs(2);
		FA_LCDClear();
		if (FA_ReadSwitch(0) == 1) {
			FA_DelaySecs(2);
			break;
		}
		if (FA_ReadSwitch(1) == 1) {
			FA_DelaySecs(2);
			break;
		}
	}
	FA_BTSendString("Connected\n", 15);
	FA_LCDBacklight(0);

	compass = 0;
	squareId = 0;
	startSquare = createSquare();

	discoverMaze();
	FA_BTSendString("Square explored!\n", 25);
	celebrate();

	FA_BTSendString("Going to dark square!\n", 35);

	if (activeSquare->id == darkSquare->id) {
		FA_BTSendString("Already in dark square!\n", 25);
		celebrate();
		return 0;
	}

	//perform movement to dark square
	for (i = 0; i < 16; i++) {
		movement[i] = -1;
	}


	movementCounter = 0;


	dijkstra(activeSquare->id);
	moveToSquare(darkSquare->id);
	moveOverStack(darkSquare->id);

	FA_BTSendString("Finished!\n", 35);

	celebrate();

	FA_LCDBacklight(100);
	FA_LCDPrint("Finished!", 16, 20 , 25, FONT_NORMAL, LCD_OPAQUE);

	return 0;
}




/*
This moves over the movement[] array whihc is being used as a stack, this array contains a list of squares to visit with closest square latest in the array
so goes over the array with latest square first
*/
void moveOverStack(int dest) {
	movementCounter--;
	//need to take one away because it is incremented at the end before quitting the function
	int moves = distance[dest];

	#ifdef MOVING_BACK_OUTPUTS
	FA_BTSendString("moves to take ", 20);
	FA_BTSendNumber(moves);
	FA_BTSendString(".\n", 6);
	#endif

	int i;
	for (i = 0; i < moves; i++) {
		#ifdef SQUARE_SEPERATOR
		FA_BTSendString("\n\n\n\n", 10);
		#endif

		int squareId = movement[movementCounter];

		#ifdef MOVING_BACK_OUTPUTS
		FA_BTSendString("in square  ", 20);
		FA_BTSendNumber(activeSquare->id);
		FA_BTSendString(".\n", 6);
		#endif

		#ifdef MOVING_BACK_OUTPUTS
		FA_BTSendString("want to go to square ", 25);
		FA_BTSendNumber(squareId);
		FA_BTSendString(".\n", 6);
		#endif

		#ifdef MOVING_BACK_OUTPUTS
		FA_BTSendString("movementCounter = ", 25);
		FA_BTSendNumber(movementCounter);
		FA_BTSendString(".\n", 6);
		#endif

		square * north = activeSquare->north;
		square * east = activeSquare->east;
		square * south = activeSquare->south;
		square * west = activeSquare->west;

		if (north != NULL && (north->id == squareId || north->id == dest)) {
			//move north and go forwards
			#ifdef MOVING_BACK_OUTPUTS
			FA_BTSendString("going north.\n", 20);
			#endif

			faceNorth();
			moveUntillOverLine();

		} else if (east != NULL && (east->id == squareId || east->id == dest)) {
			#ifdef MOVING_BACK_OUTPUTS
			FA_BTSendString("going east.\n", 20);
			#endif

			faceEast();
			moveUntillOverLine();

		} else if (south != NULL && (south->id == squareId || south->id == dest)) {
			#ifdef MOVING_BACK_OUTPUTS
			FA_BTSendString("going south.\n", 20);
			#endif

			faceSouth();
			moveUntillOverLine();

		} else if (west != NULL && (west->id == squareId || west->id == dest)) {
			#ifdef MOVING_BACK_OUTPUTS
			FA_BTSendString("going west.\n", 20);
			#endif
			faceWest();
			moveUntillOverLine();

		} else {
			while (1) {
				FA_BTSendNumber(squareId);
				FA_BTSendString(" move over stack failed.\n", 30);
				FA_DelaySecs(2);
			}
		}
		movementCounter--;
	}

}

/*
move the robot to face the north side
*/
void faceNorth(void) {
	while (1) {
		if (compass == 0) return;
		if (compass == 90) {
			FA_Left(90);
			compassLeft();
			continue;
		}
		FA_Right(RIGHT_ANGLE);
		compassRight();
	}
}

/*
move the robot to face the east side
*/
void faceEast(void) {
	while (1) {
		if (compass == 90) return;
		if (compass == 180) {
			FA_Left(90);
			compassLeft();
			continue;
		}
		FA_Right(RIGHT_ANGLE);
		compassRight();
	}
}


/*
move the robot to face the west side
*/
void faceWest(void) {
	while (1) {
		if (compass == 270) return;
		if (compass == 0) {
			FA_Left(90);
			compassLeft();
			continue;
		}
		FA_Right(RIGHT_ANGLE);
		compassRight();
	}
}

/*
move the robot to face the south side
*/
void faceSouth(void) {
	while (1) {
		if (compass == 180) return;
		if (compass == 270){
			FA_Left(90);
			compassLeft();
			continue;
		}
		FA_Right(RIGHT_ANGLE);
		compassRight();
	}
}

//fill out the movement stack using the visit map to find out how to visit the destination square
void moveToSquare(int destination) {
	int moveThrough = distance[destination];
	//knows how many squares it has to go to

	int nextSquare = destination;

	int i;
	for (i = 0; i < moveThrough; i++) {
		if (visit[nextSquare] != activeSquare->id) {
			movement[movementCounter] = visit[nextSquare];
			nextSquare = visit[nextSquare];

			#ifdef STACK_OUTPUTS
			FA_BTSendNumber(nextSquare);
			FA_BTSendString(" is next square to move to\n", 30);
			FA_BTSendNumber(movementCounter);
			FA_BTSendString("\n", 5);
			#endif

			movementCounter++;
		} else {
			#ifdef STACK_OUTPUTS
			FA_BTSendNumber(nextSquare);
			FA_BTSendString(" is not being put on the stack\n", 35);
			#endif
		}
	}
}

//discover thr maze using right hand side wall following
void discoverMaze(void) {

	while (1) {

		#ifdef SQUARE_SEPERATOR
		FA_BTSendString("\n\n\n\n", 20);
		#endif

		unsigned short lightLevel = FA_ReadLight();

		//check for the dark square
		if (lightLevel < LIGHT_LEVEL) {
			FA_BTSendString("Found dark square\n", 30);

			allLEDOn();
			FA_PlayNote(261, 250);
			FA_LCDBacklight(100);

			darkSquare = activeSquare;


			//this is needed here iscase the last square is the dark sqaure
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
				FA_PlayNote(261, 250);
				FA_Right(TURN_ANGLE);
				compass180();

				break;
			}
			FA_Right(TURN_ANGLE);
			compass180();
			moveUntillOverLine();
			continue;
		}

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
			FA_PlayNote(261, 250);
			FA_Right(TURN_ANGLE);
			compass180();

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



		//if left front and right are all blocked do a 180 and go forwards
		if (leftIR >= LEFT_NUMBER && rightIR >= RIGHT_NUMBER && frontIR >= FRONT_NUMBER) {
			//need to go backwards
			FA_Right(TURN_ANGLE);
			compass180();

			moveUntillOverLine();
			continue;
		}

		//if left and right is free go right
		if (leftIR < LEFT_NUMBER && rightIR < RIGHT_NUMBER) {
			FA_Right(RIGHT_ANGLE);
			compassRight();

			moveUntillOverLine();
			continue;
		}

		//while there is a wall to the right and forwards is clear move forwards
		if (rightIR >= RIGHT_NUMBER && frontIR < FRONT_NUMBER) {
			moveUntillOverLine();
		} else if (leftIR <= LEFT_NUMBER) {
			//if left is clear go left
			FA_Left(LEFT_ANGLE);
			compassLeft();

			moveUntillOverLine();
		} else {
			FA_Right(RIGHT_ANGLE);
			compassRight();

			moveUntillOverLine();
		}
	}
}

//connect the two supplied ids to be connected on the adjacency matrix
void connectAdj(int id1, int id2) {
	adjancy[id1][id2] = 1;
	adjancy[id2][id1] = 1;

	#ifdef ADJANCY_OUTPUTS
	FA_BTSendNumber(id1);
	FA_BTSendString(" is connected to ", 20);
	FA_BTSendNumber(id2);
	FA_BTSendString("\n", 20);
	printAdjList();
	#endif
}

//used by dijkstras to find the next square to try and move yo
int minDistance(bool sptSet[]) {
	//initialize min values
	int min = INT_MAX;
	int min_index;

	int v;
	for (v = 0; v < MAX_SQUARE_ID; v++)
		if (sptSet[v] == false && distance[v] <= min)
			min = distance[v], min_index = v;
	//if that square is not visited and the distance to it is less than the current minium
	//update the min and the index location of that value

	return min_index;
}

//this performs dijkstras on the src square,
//this will fill the distance[] array with the distances to any square in the maze
//this also fills the visit map to allow pathfinding to work.
void dijkstra(int src) {
	#ifdef DIJKSTRA_OUTPUT
	FA_BTSendString("dijkstra started\n", 20);
	#endif


	bool sptSet[MAX_SQUARE_ID];

	//set all distances as INFINITE and stpSet[] as false
	//100 is used as infinite for this because it's an unweighted graph
	int i;
	for (i = 0; i < MAX_SQUARE_ID; i++) {
		distance[i] = INT_MAX;
		sptSet[i] = false;
	}

	//distance of source square from itself is always 0
	distance[src] = 0;

	//go over all the items in the graph finding the shortest path to them
	int count;
	for (count = 0; count < MAX_SQUARE_ID - 1; count++) {
		//pick the minimum distance square from the set of squares not yet seen
		//u is always equal to src in first iteration.
		int u = minDistance(sptSet);

		//mark the picked square as processed
		sptSet[u] = true;

		//update distance value of the adjacent squares of the picked square.
		int v;
		for (v = 0; v < MAX_SQUARE_ID; v++)

			// Update distance[v] only if is not in sptSet, there is an edge from
			// u to v, and total weight of path from src to  v through u is
			// smaller than current value of distance[v]
			if (!sptSet[v] && adjancy[u][v] && distance[u] != INT_MAX
			        && distance[u] + adjancy[u][v] < distance[v]) {
				distance[v] = distance[u] + adjancy[u][v];
				visit[v] = u;
				//to get to square v you have to be in square u
				//so update the map to fill in this information
			}
	}

	#ifdef DIJKSTRA_OUTPUT
	//print the distance array
	printDistances(MAX_SQUARE_ID);
	#endif
}

//print the distance[] array
void printDistances(int n) {
	FA_BTSendString("Vertex   Distance from Source\n", 30);
	int i;
	for (i = 0; i < MAX_SQUARE_ID; i++) {
		FA_BTSendNumber(i);
		FA_BTSendString("\t\t", 6);
		FA_BTSendNumber(distance[i]);
		FA_BTSendString("\n", 3);
	}
}

//move the robot forwards untill it crosses a line
//this will also link the squares together to form a graph.
void moveUntillOverLine(void) {
	unsigned short frontLeftIR = FA_ReadIR(1);
	unsigned short frontRightIR = FA_ReadIR(3);
	unsigned short frontIR = FA_ReadIR(2);

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

			connectAdj(activeSquare->id, moveSquare->id);

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

			connectAdj(activeSquare->id, moveSquare->id);

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

			connectAdj(activeSquare->id, moveSquare->id);

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

			connectAdj(activeSquare->id, moveSquare->id);

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
		if (FA_ReadLine(0) < 30 || FA_ReadLine(1) < 30) {
			allLEDOn();
			break;
			//gone over the line
		}

		//while the robot is moving check the ir sensors to ensure the robot won't hit the wall
		frontLeftIR = FA_ReadIR(1);
		frontRightIR = FA_ReadIR(3);
		frontIR = FA_ReadIR(2);

		if (frontLeftIR >= 300) {
			FA_SetMotors(0, 0);
			FA_SetDriveSpeed(60);

			#ifdef TILT_OUTPUTS
			FA_BTSendString("while loop Front left is too close\n", 45);
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

			FA_SetMotors(53, 58);
		}

		if (frontRightIR >= 300) {

			FA_SetMotors(0, 0);
			FA_SetDriveSpeed(60);

			#ifdef TILT_OUTPUTS
			FA_BTSendString("while loop Front right is too close\n", 45);
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

			FA_SetMotors(53, 58);
		}

		if (frontIR >= 300) {
			FA_SetMotors(0, 0);
			FA_SetDriveSpeed(60);

			#ifdef TILT_OUTPUTS
			FA_BTSendString("while loop Front  is too close\n", 45);
			FA_BTSendString("Front is ", 30);
			FA_BTSendNumber(frontIR);
			FA_BTSendString("\n", 20);
			#endif

			FA_Backwards(15);
			FA_Right(5);
			FA_Forwards(15);
			frontIR = FA_ReadIR(2);

			#ifdef TILT_OUTPUTS
			FA_BTSendString("Front is ", 30);
			FA_BTSendNumber(frontIR);
			FA_BTSendString("\n", 20);
			#endif

			FA_SetMotors(53, 58);
		}
	}

	FA_LCDBacklight(0);
	allLEDOff();
	//have the light turn off for when it exits the dark square

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

	//the robot is now poitioned on the line and this checks to make sure that the robot won't hit anything when it moves forwards
	while (frontLeftIR >= FRONT_LEFT_NUMBER) {

		#ifdef TILT_OUTPUTS
		FA_BTSendString("Tilted, turning right\n", 30);
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
		FA_Left(5);
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

	frontLeftIR = FA_ReadIR(1);
	frontRightIR = FA_ReadIR(3);
	unsigned char backRightIR = FA_ReadIR(5);
	unsigned char backLeftIR = FA_ReadIR(7);

	int difference = backRightIR - backLeftIR;
	if (difference > 0) difference *= -1;


	while (difference > 200 ) {

		#ifdef TILT_OUTPUTS
		FA_BTSendString("Robot is unbalanced correcting\n", 30);
		FA_BTSendString("difference is \n", 20);
		FA_BTSendNumber(difference);
		FA_BTSendString("\n", 20);
		#endif

		FA_Right(5);
		backRightIR = FA_ReadIR(5);
		backLeftIR = FA_ReadIR(7);

		difference = backRightIR - backLeftIR;
		if (difference > 0) difference *= -1;
	}

	FA_DelayMillis(300);
}

//change the compass 90 degrees to the left
void compassLeft(void) {

	#ifdef COMPASS_OUTPUTS
	FA_BTSendString("compass was ", 20);
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
	FA_BTSendString("compass now ", 20);
	FA_BTSendNumber(compass);
	FA_BTSendString("\n", 5);
	#endif
}

//change the compass 90 degrees to the right
void compassRight(void) {

	#ifdef COMPASS_OUTPUTS
	FA_BTSendString("compass was ", 20);
	FA_BTSendNumber(compass);
	FA_BTSendString("\n", 5);
	#endif

	compass += 90;
	compass %= 360;

	#ifdef COMPASS_OUTPUTS
	FA_BTSendString("compass now ", 20);
	FA_BTSendNumber(compass);
	FA_BTSendString("\n", 5);
	#endif
}

//move the compass 180 degrees
void compass180(void) {

	#ifdef COMPASS_OUTPUTS
	FA_BTSendString("compass was ", 20);
	FA_BTSendNumber(compass);
	FA_BTSendString("\n", 5);
	#endif

	compass += 180;
	compass %= 360;

	#ifdef COMPASS_OUTPUTS
	FA_BTSendString("compass now ", 20);
	FA_BTSendNumber(compass);
	FA_BTSendString("\n", 5);
	#endif
}

//malloc out a new square struct and return a pointer to it
square *createSquare(void) {
	//wall is 0
	//3 is unknown
	//gone through is 1
	//need to go through is 2

	square *returnNode = malloc(sizeof(square));

	returnNode->id = squareId;

	returnNode->searched = SQUARE_UNSEARCHED;

	returnNode->north = NULL;
	returnNode->east = NULL;
	returnNode->west = NULL;
	returnNode->south = NULL;

	squareId++;

	return returnNode;
}

//switch all the leds on
void allLEDOn(void) {
	int i;
	for (i = 0; i < 8; i++) {
		FA_LEDOn(i);
	}
}

//switch all the leds off
void allLEDOff(void) {
	int i;
	for (i = 0; i < 8; i++) {
		FA_LEDOff(i);
	}
}

//flash all the lights counter number of times
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

//the end finished dance
void celebrate(void) {
	FA_SetDriveSpeed(100);
	FA_PlayNote(261, 250);
	lightShow(10);
	FA_Right(30);
	FA_Left(30);
	FA_Right(30);
	FA_Left(30);
	lightShow(10);
	// FA_Right(720);
}

//measure all four corner sensors and print their values out to bluetooth
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

		unsigned short leftIR = FA_ReadIR(0);
		FA_BTSendString("left: ", 20);
		FA_BTSendNumber(leftIR);
		FA_BTSendString("\n", 10);

		unsigned short rightIR = FA_ReadIR(4);
		FA_BTSendString("Right: ", 20);
		FA_BTSendNumber(rightIR);
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

//measure the light sensor and print its values out to bluetooth
void measureLight(void) {
	while (1) {
		FA_LCDClear();
		unsigned short lightValue = FA_ReadLight();
		// FA_BTSendUnsigned(lightValue);
		FA_LCDNumber(lightValue, 0, 0, 0, 0);
		FA_DelaySecs(1);
	}
}

#ifdef ADJANCY_OUTPUTS
//print the adjacency matrix out via bluetooth
void printAdjList(void) {
	int rows, cols;

	for (rows = 0; rows < 16; rows++) {
		for (cols = 0; cols < 16; cols++) {
			FA_BTSendNumber(adjancy[rows][cols]);
			FA_BTSendString(" ", 20);
		}
		FA_BTSendString("\n", 20);
	}
}
#endif