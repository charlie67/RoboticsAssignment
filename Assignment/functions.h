typedef struct box {
	int ahead;
	int back; 
	int left;
	int right;
	
	int isLowLight;
	int currentSquare;
	int wall;
	
	struct box *frontSquare;
	struct box *behindSquare;
	struct box *leftSquare;
	struct box *rightSquare;
}square;

square createBox(int l, int r, int b, int f);

void movement();