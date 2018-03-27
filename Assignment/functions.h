typedef struct box {
	int front;
	int back;
	int left;
	int right;

	int isLowLight;
	int currentSquare;
	int wall;

	struct box *frontSquare;
	struct box *backSquare;
	struct box *leftSquare;
	struct box *rightSquare;
} square;

square createSquare(int l, int r, int b, int f);

void movement();