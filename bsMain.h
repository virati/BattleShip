/*//The codes for the board matrix, that translates the numbers into Data
#define SHIPTHERE 1
#define SEATHERE 0
#define HITTHERE 2

#define DEST 2
#define SUB 3
#define CRUS 3
#define BESP 4
#define AIRC 5

//Foreground Colors 
#define BLUE		0x0010
#define GREEN		0x0020
#define RED			0x0040
#define INTENS		0x0080
//Structure that contains the coordinates for the entire game
typedef struct {
	int ID;
	int status;							//0= nothing there, 1= ship there, 2= hit ship there
	char type;
	WORD color;
} unit;

typedef struct {
	char x;
	int y;
} COOR;

typedef struct {
	int degr;							//The number of degrees, either 90, 180, or 270
	char orion;
} DIRECT;

typedef struct {
	COOR front;
	DIRECT dir;
	char name[16];							//THe name, Battleship, sub, etc.
	char type;								//The Type of ship
	int dim;
	bool active;
	WORD color;
} ship;

typedef struct {
	ship dest;
	ship sub;
	ship crus;
	ship battle;
	ship carr;
	int admiral;
} fleet;

//Function Prototypes
int menu();									//The Menu function that displays the main menu
void menuFunc(int option);					//"Menu Function" function
char changeOp();							//"Change Option" function
void about();								//The About function that displays the information
void makeBoard();							//Function that creates the board
int setPiece(ship *ship1, fleet fleet1);	//The function that sets the pieces on the board
void refreshBoard(unit Disp[10][10], WORD wAttributesOld);		//Refreshes the board
void setup();
void game();

//Global Variables
bool cont;									//The variable that is for the main while loop
bool end;									//Variable for main while loop
unit board1[10][10];						//The Player 1 Board for Game
unit board2[10][10];						//The Player 2 Board for Game
fleet play[2];								//The fleets of each player
WORD color;
WORD wAttributesOld;
CONSOLE_SCREEN_BUFFER_INFO csbi;
HANDLE hConsole;

//Functions

//Refresh Board
void refreshBoard(unit Disp[10][10], WORD wAttributesOld) {
	int x;
	int y;
	system("CLS");
	cout << "Your Board\n----------------------------------------\n\n";
	for(x=0; x<10; x++) {
		cout << x << "|";
		for(y=0; y<10; y++) {
			SetConsoleTextAttribute( hConsole, BLUE);
			cout << "-" ;
			SetConsoleTextAttribute( hConsole, wAttributesOld);
			cout << "|";							//Should be .type
		}
		cout << "\n";
	}
	//cout << " |0|1|2|3|4|5|6|7|8|9|\n";
}

//Inition Setup
void setup() {
	int x, y;
	//1 = player 1
	play[1].dest.dim = DEST;
	play[1].dest.type = 'd';
	strcpy(play[1].dest.name,"Destroyer");
	play[1].dest.color = BLUE;
	play[1].sub.dim = SUB;
	play[1].sub.type='s';
	strcpy(play[1].sub.name,"Submarine");
	play[1].sub.color = RED;
	play[1].crus.dim = CRUS;
	play[1].crus.type = 'c';
	strcpy(play[1].crus.name,"Cruiser");
	play[1].crus.color = GREEN;
	play[1].battle.dim = BESP;
	play[1].battle.type = 'b';
	strcpy(play[1].battle.name,"Battleship");
	play[1].battle.color = RED | GREEN;
	play[1].carr.dim = AIRC;
	play[1].carr.type = 'a';
	strcpy(play[1].carr.name,"Carrier");
	play[1].carr.color = GREEN | BLUE;

	//0 = AI
	play[0].dest.dim = DEST;
	play[0].dest.type = 'd';
	strcpy(play[0].dest.name,"Destroyer");
	play[0].dest.color = BLUE;
	play[0].sub.dim = SUB;
	play[0].sub.type = 's';
	strcpy(play[0].sub.name,"Submarine");
	play[0].sub.color = RED;
	play[0].crus.dim = CRUS;
	play[0].crus.type = 'c';
	strcpy(play[0].crus.name,"Cruiser");
	play[0].crus.color = GREEN;
	play[0].battle.dim = BESP;
	play[0].battle.type = 'b';
	strcpy(play[0].battle.name,"Battleship");
	play[0].battle.color = RED | GREEN;
	play[0].carr.dim = AIRC;
	play[0].carr.type = 'a';
	strcpy(play[0].carr.name,"Carrier");
	play[0].carr.color = GREEN | BLUE;

	for(x=0; x<10; x++) {
		for(y=0; y<10; y++) {
			board1[x][y].status = 0;
			board2[x][y].status = 0;

		}
	}
}

//Set Pieces Function
int setPiece(ship *ship1, fleet fleet1, unit board[10][10], HANDLE * hConsole) {
	int x, y;
	char dir;
	bool goodPoint = true;
	int z, w;

	system("CLS");
	refreshBoard(board, wAttributesOld);
	switch(fleet1.admiral) {
		case 1:
			do {
				x = y = 0;
				dir = ' ';
				cout << "Enter the Location you want to put your " << ship1->name << "? ";
				cin >> x;
				cin >> y;
				if (board[x][y].status == 0) {
					goodPoint = true;
					ship1->front.x = x;
					ship1->front.y = y;
					board[x][y].type = ship1->type;
					board[x][y].status = 1;
					cout << "\nEnter the Direction (u, d, l, r) ";
					cin >> dir;
					ship1->dir.orion = dir;
					if(dir == 'u' || dir == 'U') {
						ship1->dir.degr = 0;
						board[ship1->front.x][ship1->front.y].type=ship1->type;
						board[ship1->front.x][ship1->front.y].status = 1;
						for(z=x-1; z > x - ship1->dim; z--) {
							if (board[z][ship1->front.y].status == 0 && board[x][y].type == ship1->type) {
								board[z][ship1->front.y].type= ship1->type;
								board[z][ship1->front.y].status = 1;
								board[z][ship1->front.y].color = ship1->color;

								//board[x-ship1->dim+1][y].status = 1;
								//board[x-ship1->dim+1][y].type = ship1->type;
							}
							else {
								for(z=x; z > x - ship1->dim; z--) {
									if(board[z][ship1->front.y].type == ship1->type) {
										board[z][ship1->front.y].status = 0;
										board[z][ship1->front.y].type = ' ';
										board[ship1->front.x][z].color = NULL;
										//board[x-ship1->dim+1][y].status = 0;
										//board[x-ship1->dim+1][y].type = ' ';
									}
								}
								goodPoint = false;
								break;
							}
						}
					}
					else if(dir == 'd' || dir == 'D') {
						ship1->dir.degr = 180;
						board[ship1->front.x][ship1->front.y].type=ship1->type;
						board[ship1->front.x][ship1->front.y].status = 1;
						for(z=x+1; z < x + ship1->dim; z++) {
							if (board[z][ship1->front.y].status == 0 && board[x][y].type == ship1->type) {
								board[z][ship1->front.y].type= ship1->type;
								board[z][ship1->front.y].status= 1;
								board[z][ship1->front.y].color = ship1->color;

								//board[x+ship1->dim-1][y].status = 1;
								//board[x+ship1->dim-1][y].type = ship1->type;
							}
							else {
								for(z=x; z < x + ship1->dim; z++) {
									if(board[z][ship1->front.y].type == ship1->type) {
										board[z][ship1->front.y].status = 0;
										board[z][ship1->front.y].type = ' ';
										board[ship1->front.x][z].color = NULL;
										//board[x+ship1->dim-1][y].status = 0;
										//board[x+ship1->dim-1][y].type = ' ';
									}
								}
								goodPoint = false;
								break;
							}
						}

					}
					else if(dir == 'l' || dir == 'L') {
						ship1->dir.degr = 270;
						board[ship1->front.x][ship1->front.y].type=ship1->type;
						board[ship1->front.x][ship1->front.y].status = 1;
						for(z=y-1; z > y - ship1->dim; z--) {
							if (board[ship1->front.x][z].status == 0 && board[x][y].type == ship1->type && z+1 > 0) {
								board[ship1->front.x][z].type= ship1->type;
								board[ship1->front.x][z].status = 1;
								board[ship1->front.x][z].color = ship1->color;

								//board[x][y-ship1->dim+1].status = 1;
								//board[x][y-ship1->dim+1].type = ship1->type;
							}
							else {
								for(z=y; z > y - ship1->dim; z--) {
									if(board[ship1->front.x][z].type == ship1->type) {
										board[ship1->front.x][z].status = 0;
										board[ship1->front.x][z].type = ' ';
										board[ship1->front.x][z].color = NULL;
										//board[x][y-ship1->dim+1].status = 0;
										//board[x][y-ship1->dim+1].type = ' ';
									}
								}
								goodPoint = false;
								break;
							}
						}

					}
					else if(dir == 'r' || dir == 'R') {
						ship1->dir.degr = 90;
						board[ship1->front.x][ship1->front.y].type=ship1->type;
						board[ship1->front.x][ship1->front.y].status = 1;
						for(z=y+1; z < y + ship1->dim; z++) {
							if (board[ship1->front.x][z].status == 0 && board[x][y].type == ship1->type) {
								board[ship1->front.x][z].type= ship1->type;
								board[ship1->front.x][z].status = 1;
								board[ship1->front.x][z].color = ship1->color;

								//board[x][y+ship1->dim-1].status = 1;
								//board[x][y+ship1->dim-1].type = ship1->type;
							}
							else {
								for(w=y; w < y + ship1->dim; w++) {
									if(board[ship1->front.x][z].type == ship1->type) {
										board[ship1->front.x][z].status = 0;
										board[ship1->front.x][z].type = ' ';
										board[ship1->front.x][z].color = NULL;
										//board[x][y+ship1->dim-1].status = 0;
										//board[x][y+ship1->dim-1].type = ' ';
									}
								}
								goodPoint = false;
								break;
							}
						}

					}
				}
				else {
					goodPoint = false;
				}
			} while (goodPoint == false);

			break;
	}
	//system("CLS");
	refreshBoard(board, wAttributesOld);
	return 1;
}

//Menu function
int menu() {
	int option;
	cout << "Menu\n------------------------------\n";
	cout << "1.Start/Continue\n2.Change Options\n3.About\n0.Exit\n--------------------------\n";
	cout << "Command: ";
	cin >> option;

	return option;
}

//Function that recieves the
void menuFunc(int option) {
	int x;
	switch (option) {
		case 1:

			cont=true;
			game();
			break;

		case 2:

			break;

		case 3:
			about();
			break;

		case 0:
			exit(0);
			break;
	}

	system("CLS");				//Clears screen

}

void about() {
	MessageBox(NULL, "Created by Vineet Tiruvadi", "About", MB_OK);
}

void makeBoard(unit board[10][10]) {
	int x;
	int y;
	for (x=0; x<10; x++) {
		for(y=0; y<10; y++) {
			board[x][y].type = ' ';
		}
	}

}
*/