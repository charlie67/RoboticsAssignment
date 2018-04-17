
/**
this retunrs an array of squares you need to go through to find the end square or NULL if you're already in that square
**/
void navigate(square *start, square *end, int compass) {
	//from start go through all routes to find the end square

	if (start->id == end->id) {
		//already at the desired position
		return;
	}

	//0	north
	//90 east
	//180 south
	//270 west

	stackLocation = 0;

	square *movement;

	unsigned char northConnections = 0;
	unsigned char lastNorthConnection;

	unsigned char westConnections = 0;
	unsigned char lastWestConnection;

	unsigned char eastConnections = 0;
	unsigned char 

	if (start->north != NULL) {
		movement = start->north;
		compassStack[stackLocation] = 0;
		stackLocation++;
		//gone north so add that to the stack

		while (movement->north != NULL) {
			if (movement->id == end->id) {
				return;
			}

			//if movement has other connection note those down to come abck to later
			if (movement->east != NULL) {
				northConnections++;
				lastNorthConnection = movement->id;

			}
			if (movement->west != NULL) {
				northConnections++;
				lastNorthConnection = movement->id;
			}
			//only need to check east and west because it came from the south and has already checked the north

			movement = movement->north;
			compassStack[stackLocation] = 0;
			stackLocation++;
		}
		//going only north has failed so check any branches off north
		//the number of branches are in northConnections and if there are multiple then the last one is in lastNorthConnection

		stackLocation = 0;
		//reset the stack counter

		while (northConnections > 0) {

			//if there are no north connection branches to check cancel the while loop and move on

			//reset the movement square to be the start north connection
			movement = start->north;

			while (movement->id != lastNorthConnection) {
				movement = movement->north;
			}
			//movement is now at the last connection square

			northConnections--;

			if (movement->east != NULL) {
				//go along the east path checking making notes of any branches off along the way

				while (movement->east != NULL) {
					if (movement->id == end->id) {
						return;
					}

					//if movement has other connection note those down to come abck to later
					if (movement->east != NULL) {
						westConnections++;
						lastNorthConnection = movement->id;

					}
					if (movement->west != NULL) {
						northConnections++;
						lastNorthConnection = movement->id;
					}
					//only need to check east and west because it came from the south and has already checked the north

					movement = movement->east;
					compassStack[stackLocation] = 0;
					stackLocation++;
				}
			}

			movement = start->north;

			while (movement->id != lastNorthConnection) {
				movement = movement->north;
			}

			if (movement->west != NULL) {

			}

		}
	}



	//if east
	//if south
	//etc

}