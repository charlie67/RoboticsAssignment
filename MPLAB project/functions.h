typedef struct box {
	int front;
	int back;
	int left;
	int right;

	int isLowLight;
	int currentSquare;

	struct box *frontSquare;
	struct box *backSquare;
	struct box *leftSquare;
	struct box *rightSquare;
} square;
