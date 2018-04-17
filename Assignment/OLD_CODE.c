
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







int navigate(square*start, square*end, int directionFrom) {
	//go north until you either find what you want or reach a dead end

	#ifdef NAVIGATIONS_OUTPUTS
	FA_BTSendString("navigate called.\n", 40);
	FA_BTSendString("In square ", 40);
	FA_BTSendNumber(start->id);
	FA_BTSendString("\n",20);
	#endif

	if (start->id == end->id) {
		#ifdef NAVIGATIONS_OUTPUTS
		FA_BTSendString("Already at the end square.\n", 40);
		#endif

		FA_LCDBacklight(100);
		FA_LCDPrint("Found Square.", 16, 20, 25, FONT_NORMAL, LCD_OPAQUE);
		return 0;
	} else {
		start->searched = SQUARE_SEARCHED;
	}

	square *movement = start;
	int wallCheck;

	int westCheced;
	int eastChecked;
	int southChecked;
	int northChecked;

	if (numBlockedSides(movement) == 3) {
		//in a blocked square
		#ifdef NAVIGATIONS_OUTPUTS
		FA_BTSendString("Square has one way out.\n", 40);
		#endif

		// if (compass ==)
	}

	if (numBlockedSides(movement) >= 2) {//if you have 2 or more blocked routes
		//search through the only unblocked way

		#ifdef NAVIGATIONS_OUTPUTS
		FA_BTSendString("square has 2 or more exits.\n", 40);
		#endif

		if (movement->north != NULL && directionFrom != NORTH_COMPASS_HEADING) return 1;
		else if (movement->east != NULL && directionFrom != EAST_COMPASS_HEADING) return 1;
		else if (movement->south != NULL && directionFrom != SOUTH_COMPASS_HEADING) return 1;
		else if (movement->west != NULL && directionFrom != WEST_COMPASS_HEADING) return 1;

		if (movement->north != NULL && directionFrom != SOUTH_COMPASS_HEADING) wallCheck = 0;
		else if (movement->east != NULL && directionFrom != WEST_COMPASS_HEADING) wallCheck = 1;
		else if (movement->south != NULL && directionFrom != NORTH_COMPASS_HEADING) wallCheck = 2;
		else if (movement->west != NULL && directionFrom != EAST_COMPASS_HEADING) wallCheck = 3;

		switch (wallCheck) {

			#ifdef NAVIGATIONS_OUTPUTS
			FA_BTSendString("switching on .\n", 40);
			FA_BTSendNumber(wallCheck);
			FA_BTSendString("\n", 20);
			#endif

		case 0:
			//go north and check that block
			movement = movement->north;

			compassStack[stackLocation] = NORTH_COMPASS_HEADING;
			stackLocation++;

			if (navigate(movement, end, NORTH_COMPASS_HEADING) == 1) {
				compassStack[stackLocation] = NULL_COMPASS_HEADING;
				stackLocation--;
			} else {
				return 0;
			}
			break;
		case 1:
			movement = movement->east;

			compassStack[stackLocation] = EAST_COMPASS_HEADING;
			stackLocation++;

			navigate(movement, end, EAST_COMPASS_HEADING);
			break;

		case 2:
			movement = movement->south;

			compassStack[stackLocation] = SOUTH_COMPASS_HEADING;
			stackLocation++;

			navigate(movement, end, SOUTH_COMPASS_HEADING);
			break;

		case 3:
			movement = movement->west;

			compassStack[stackLocation] = WEST_COMPASS_HEADING;
			stackLocation++;

			navigate(movement, end, WEST_COMPASS_HEADING);
			break;

		}

	} else if (numBlockedSides(movement) == 1) {
		//need to check two branches
		#ifdef NAVIGATIONS_OUTPUTS
		FA_BTSendString("There are two branches to check\n", 40);
		#endif
	}
}

int numBlockedSides(square *block) {
	int blockedSides = 0;

	if (block->north == NULL) blockedSides++;
	if (block->south == NULL) blockedSides++;
	if (block->east == NULL) blockedSides++;
	if (block->west == NULL) blockedSides++;

	return blockedSides;
}

/**
returns 0 if there are no branches otherwise will return number of branches
will go along all the squares to the north of this square and see if there are branches to the east or west
**/
unsigned char anyBranchesNorth(square *start) {
	square *movement;

	unsigned char branchNumber = 0;

	while (start->north != NULL) {
		movement = start->north;

		if (movement->west != NULL) {
			branchNumber++;
		}

		if (movement->east != NULL) {
			branchNumber++;
		}
	}

	return branchNumber;
}

unsigned char anyBranchesEast(square *start) {
	// want to check north and south
	square *movement;

	unsigned char branchNumber = 0;

	while (start->east != NULL) {
		movement = start->east;

		if (movement->north != NULL) {
			branchNumber++;
		}

		if (movement->south != NULL) {
			branchNumber++;
		}
	}

	return branchNumber;

}

unsigned char anyBranchesWest(square *start) {
	square *movement;

	unsigned char branchNumber = 0;

	while (start->west != NULL) {
		movement = start->east;

		if (movement->north != NULL) {
			branchNumber++;
		}

		if (movement->south != NULL) {
			branchNumber++;
		}
	}

	return branchNumber;

}

unsigned char anyBranchesSouth(square *start) {
	square *movement;

	unsigned char branchNumber = 0;

	while (start->north != NULL) {
		movement = start->north;

		if (movement->west != NULL) {
			branchNumber++;
		}

		if (movement->east != NULL) {
			branchNumber++;
		}
	}

	return branchNumber;
}

void moveAlongStack(void) {
	if (stackLocation == 0) {
		//already at the square so dont do anything
		#ifdef SQUARE_EXPLORATION_OUTPUTS
		FA_BTSendString("Done!\n", 20);
		#endif

		return;

	} else {
		int i;
		for (i = 0; i <= stackLocation; i++) {
			if (compassStack[i] == 0) {
				//move to north and advance
				if (compass != 0) {
					unsigned char distanceFromCompass = compass % 360;
					while (distanceFromCompass > 0) {
						FA_Right(90);
						distanceFromCompass - 90;
					}
				}

				while (activeSquare->id != darkSquare->id) {
					moveUntillOverLine();
				}

				allLEDOn();

				while (1) {
					FA_LCDBacklight(100);
					FA_DelayMillis(300);
					FA_LCDBacklight(0);
				}

			} else if (compassStack[i] == 90) {
				//move to east and advance
				if (compass != 90) {
					unsigned char distanceFromCompass = compass % 360;
					while (distanceFromCompass > 0) {
						FA_Right(90);
						distanceFromCompass - 90;
					}
				}

			} else if (compassStack[i] == 180) {
				//move to south and advance
				if (compass != 180) {
					unsigned char distanceFromCompass = compass % 360;
					while (distanceFromCompass > 0) {
						FA_Right(90);
						distanceFromCompass - 90;
					}
				}

			} else {
				//move to the west and advance
				if (compass != 270) {
					unsigned char distanceFromCompass = compass % 360;
					while (distanceFromCompass > 0) {
						FA_Right(90);
						distanceFromCompass - 90;
					}
				}

			}
		}
	}
}