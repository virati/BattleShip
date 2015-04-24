//Vineet Tiruvadi
//Hour 1
//FIRST COLOR battleship EVER at East

#include <iostream.h>					//Libraries
#include <windows.h>					
#include <stdio.h>
#include <winbase.h>
#include <time.h>

#define SHIPTHERE 1						//Values for terrain
#define SEATHERE 0
#define HITTHERE 2

#define DEST 2							//Dimensions of each ship, can be altered
#define SUB 3
#define CRUS 3
#define BESP 4
#define AIRC 5

//Background Colors
#define BLUE		0x0010
#define GREEN		0x0020
#define RED			0x0040
#define INTENS		0x0080

//Foreground Colors
#define fBLUE		0x0001
#define fGREEN		0x0002
#define fRED		0x0004
#define fINTENS		0x0008

//Structure that contains the coordinates for the entire game
typedef struct {
	int ID;
	int status;							//0= nothing there, 1= ship there, 2= hit ship there
	char type;
	WORD color;
} unit;

//Coordinate Structure
typedef struct {
	int x;
	int y;
} COOR;

//Direction Structure
typedef struct {
	int degr;							//The number of degrees, either 90, 180, or 270
	char orion;
} DIRECT;

//Ship structure
typedef struct {
	COOR front;
	DIRECT dir;
	char name[16];							//THe name, Battleship, sub, etc.
	char type;								//The Type of ship
	int dim;
	bool active;
	WORD color;
} ship;

//Fleet structure, one for each player
typedef struct {
	ship dest;
	ship sub;
	ship crus;
	ship battle;
	ship carr;
	int admiral;
	int destnum;
	int subnum;
	int crusnum;
	int battnum;
	int airnum;
	char name[16];
} fleet;

//Function Prototypes
int menu();									//The Menu function that displays the main menu
void menuFunc(int option);					//"Menu Function" function
char changeOp();							//"Change Option" function
void about();								//The About function that displays the information
void makeBoard(unit board[10][10]);							//Function that creates the board
int setPiece(ship *ship1, fleet fleet1, unit board[10][10], COOR start, char dir);	//The function that sets the pieces on the board
void refreshBoard(unit Disp[10][10]);		//Refreshes the board
void setup();
void game();								//Main function, for game
void key();
void aisetup();
int target(fleet *fleet1, COOR trget, unit yourBoard[10][10], unit trgetBoard[10][10], unit gboard[10][10]);	//Checks to see if the target was hit
void showGuesses(unit gb[10][10]);
void autoSet();
void piecesLeft();
void checkDone();
void endGame();

//Global Variables
bool cont;									//The variable that is for the main while loop
bool end;									//Variable for main while loop
unit board1[10][10];						//The Player 1 Board for Game
unit board1g[10][10];
unit board2[10][10];						//The Player 2 Board for Game
unit board2g[10][10];
fleet play[2];								//The fleets of each player, 0=ai, 1=player
WORD color;
WORD wAttributesOld;
CONSOLE_SCREEN_BUFFER_INFO csbi;	
HANDLE hConsole;
int keyTemp;
bool keydone;
char shipnames[5][12] = {"Destroyer" , "Submarine" , "Cruiser" , "Battleship" , "Aircarrier"};	//Ship names
bool set;
int winner;									//Final winner
int play1moves;
int play2moves;

//The function that
void endGame() {
	char yon;
	int x;

	system("CLS");
	if(winner == 0) {								//Checks to see who is the winner
		PlaySound("jabba.wav", NULL, SND_SYNC);
		MessageBox(NULL, "The NME won!!! How could you LOSE???", "LOSER", MB_OK);
	}
	else {
		PlaySound("dest.wav", NULL, SND_SYNC);
		MessageBox(NULL, "YOU WON!!", "WINNER!", MB_OK);
	}
	PlaySound("drum.wav", NULL, SND_SYNC);
	cout << "Winner is...";		
	SetConsoleTextAttribute(hConsole, fRED | fINTENS);
	printf("Winner is........ %s", play[winner].name);
	Sleep(5000);
	SetConsoleTextAttribute(hConsole, fRED | fGREEN | fBLUE);
	refreshBoard(board2);
	cout << "\nNme's board... Doncha LOVE it??? (y or n)\n\n";
	cin >> yon;
	if(yon == 'y') {								//Just for no reason
		cout << "\n\nWhat the! How could you love the NME's board???";
	}
	else {
		cout << "\n\nGASP! How could you NOT like the NME's board!!!";
	}
	system("CLS");
	cout << "It took you " << play1moves << " moves in this game to finish";
	cout << "\n\nIt took the computer " << play2moves << " moves to finish";
	about();
	for (x=0; x<25; x++) {							//Output for no reason
		SetConsoleTextAttribute(hConsole, RED | GREEN | BLUE | INTENS);
		printf(" ");
	}

	exit(0);
}

void checkDone() {
	int x;

	for(x=0; x<=1; x++) {				//Checks if the game is over
		if(play[x].battnum + play[x].destnum + play[x].subnum + play[x].crusnum + play[x].airnum == 0) {
			winner = x;
			cont = true;
			end = true;
		}
	}
}

main() {

	if( ( hConsole = CreateFile("CONOUT$", GENERIC_WRITE | GENERIC_READ,FILE_SHARE_READ | FILE_SHARE_WRITE,0L, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0L) )
                   == (HANDLE) -1 ) {
        printf("\nError: Unable to open console.\n");
        return( -1 );
    }
	wAttributesOld = csbi.wAttributes;

	srand((unsigned)time(NULL));

	COOR UrEnt;								//The multiuse varable for Usr entered Coordinates
	char name[16];							//The name of the player
	int option;								//The option variable that holds the value of the option the user selected

	setup();
	makeBoard(board1);
	makeBoard(board2);

	end = false;
	cont = false;
	cout << "Welcome to Battleship\n\n";
	cout << "Enter your name: ";
	cin.get(name, 16);

	//Setup
	play[1].admiral = 1;						//Sets the First fleet to player 1
	strcpy(play[1].name, name);
	play[0].admiral = 0;						//Sets the second fleet to computer
	strcpy(play[0].name, "Computer");

	cout << "\n";
	aisetup();
	while(!end &&  !cont) {
		option = menu();
		menuFunc(option);
	}

	return 0;
}

void game() {
	int x;
	cont = false;
	char option;
	int z;
	COOR ships;
	char dir;

	PlaySound("wipeout.wav",NULL, SND_SYNC);			//Starting sound

	if (!set) {											//Setting loop, loops to set all ships
		for(z=0; z<5; z++) {
			refreshBoard(board1);
			cout << "\nEnter the Location you want to put your " << shipnames[z] << ": ";
			cin >> ships.x;
			cin >> ships.y;

			if(board1[ships.x][ships.y].status == 0) {
				cout << "\nEnter the Direction (u, d, l, r) ";
				cin >> dir;

				switch(z) {
					case 0:
						if((x = setPiece(&play[1].dest, play[1], board1, ships, dir)) == 0) {
							cout << '\7';
							z--;
						}
						break;

					case 1:
						if((x = setPiece(&play[1].sub, play[1], board1, ships, dir)) == 0) {
							cout << '\7';
							z--;
						}
						break;
					case 2:
						if((x = setPiece(&play[1].crus, play[1], board1, ships, dir)) == 0) {
							cout << '\7';
							z--;
						}
						break;

					case 3:
						if((x = setPiece(&play[1].battle, play[1], board1, ships, dir)) == 0) {
							cout << '\7';
							z--;
						}
						break;

					case 4:
						if((x = setPiece(&play[1].carr, play[1], board1, ships, dir)) == 0) {
							cout << '\7';
							z--;
						}
						break;


				}
			}
			else {
				z--;
			}
		}
	}

	COOR hit;

	while (!end && !cont) {									//Main Loop... All command occur here
		refreshBoard(board1);
		cout << "\n\nEnter your command (h for help):";
		//refreshBoard(board2);
		cin >> option;
		if(option == 'h') {
			system("CLS");
			cout << "Help\n--------------------------\n";
			cout << "h- help\nt- target\nc- Check gwGuesses/Hits\nx- exit\n\n";

		}
		else if(option == 't') {
			refreshBoard(board1);
			cout << "\nWhat coordinates do you want to hit? ";
			cin >> hit.x;
			cin >> hit.y;
			x = target(&play[1], hit,board1, board2, board1g);
			hit.x = (rand()%10) + 1;
			hit.y = (rand()%10) + 1;
			x = target(&play[0], hit, board2, board1, board2g);
		}
		else if(option == 'c') {
			system("CLS");
			showGuesses(board1g);
			piecesLeft();
			cout << "\n\n\nPress \"g\" to continue\n";
			cin >> option;

			//refreshBoard(board2);
		}
		else if(option == 'x') {			//exits
			exit(0);
		}
		else if(option == 'k') {
			MessageBox(NULL, "0 - Location where you have tried to hit\nX - Location where your nme tried to shoot","Key", MB_OK);

		}
		checkDone();		//CHecks to see if done
	}
	endGame();				//WHen loop exits, game is done


}

void piecesLeft() {									//Displays how many more pieces are left
	cout << "\n\t\tPlayer\t\t\tComputer\n";
	cout << "Destroyer\t" << play[1].destnum << "\t\t\t" << play[0].destnum;
	cout << "\nSubmarines\t" << play[1].subnum << "\t\t\t" << play[0].subnum;
	cout << "\nCrusier\t\t" << play[1].crusnum << "\t\t\t" << play[0].crusnum;
	cout << "\nBattleship\t" << play[1].battnum << "\t\t\t" << play[0].battnum;
	cout << "\nCarrier\t\t" << play[1].airnum << "\t\t\t" << play[0].airnum;
}

void showGuesses(unit gb[10][10]) {					//Shows the guesses so far
	int x, y;
	system("CLS");
	cout << "Guessed Spots\n------------------------\n";
	for(x=0; x<10; x++) {
		//cout << x << "|";
		cout << x << "|";
		for(y=0; y<10; y++) {
			if(gb[x][y].status == 0) {
				cout << "0|";
			}
			else if(gb[x][y].status == 1) {
				cout << "-|";
			}
			else if(gb[x][y].status == 2) {
				cout << "X|";
			}
		}
		cout << "\n";
	}
	cout << " |0|1|2|3|4|5|6|7|8|9|\n";



}

//Targetting function, that aims for 
int target(fleet * fleet1, COOR trget, unit yourBoard[10][10] ,unit trgetBoard[10][10], unit gboard[10][10]) {
	if(fleet1->admiral == 1) {
		play1moves++;
	}
	else if(fleet1->admiral == 0) {
		play2moves++;
	}
	if(trgetBoard[trget.x][trget.y].status == 1) {
		trgetBoard[trget.x][trget.y].status = 2;
		if(fleet1->admiral == 1) {
			MessageBox(NULL, "You hit a ship!", "HIT!", MB_OK );
			PlaySound("hit.wav", NULL, SND_SYNC);
			gboard[trget.x][trget.y].status = 2;
			yourBoard[trget.x][trget.y].color = RED | GREEN | INTENS;
			trgetBoard[trget.x][trget.y].status = 2;
			
			
		}
		else if(fleet1->admiral == 0){
			gboard[trget.x][trget.y].status = 2;
			MessageBox(NULL, "The NME hit your ship!", "OH NO!", MB_OK);
			PlaySound("hit.wav", NULL, SND_SYNC);
		}
		yourBoard[trget.x][trget.y].type = '^';
		trgetBoard[trget.x][trget.y].type = 'X';

		if(trgetBoard[trget.x][trget.y].color == fleet1->dest.color) {
			fleet1->destnum--;
		}
		else if(trgetBoard[trget.x][trget.y].color == fleet1->sub.color) {
			fleet1->subnum--;
		}
		else if(trgetBoard[trget.x][trget.y].color == fleet1->crus.color) {
			fleet1->crusnum--;
		}
		else if(trgetBoard[trget.x][trget.y].color == fleet1->battle.color) {
			fleet1->battnum--;
		}
		else if(trgetBoard[trget.x][trget.y].color == fleet1->carr.color) {
			fleet1->airnum--;
		}
	}
	else if(trgetBoard[trget.x][trget.y].status == 0) {
		if(fleet1->admiral == 1) {
			PlaySound("nohit.wav", NULL, SND_SYNC);
		}
		gboard[trget.x][trget.y].status = 0;
		yourBoard[trget.x][trget.y].type = '0';
		trgetBoard[trget.x][trget.y].color = fRED | fGREEN | fBLUE | fINTENS;
		trgetBoard[trget.x][trget.y].type = 'X';
	}

	return 1;
}


//Set Pieces Function
int setPiece(ship *ship1, fleet fleet1, unit board[10][10], COOR start, char dir) {
	int x, y;
	bool goodPoint = true;
	int z, w;

	if(start.x < 10 && start.y >= 0) {
		x = start.x;
		
	}
	else {
		x = 9;
	}
	if(start.y < 10 && start.y >= 0) {
		y = start.y;
	}
	else {
		y = 9;
	}

	ship1->front.x = x;
	ship1->front.y = y;
	board[x][y].type = ship1->type;
	board[x][y].status = 1;
	ship1->dir.orion = dir;
	if(dir == 'u' || dir == 'U') {
		ship1->dir.degr = 0;
		board[ship1->front.x][ship1->front.y].type=ship1->type;
		board[ship1->front.x][ship1->front.y].status = 1;
		board[ship1->front.x][ship1->front.y].color = ship1->color;
		for(z=x-1; z > x - ship1->dim; z--) {
			if (board[z][ship1->front.y].status == 0 && board[x][y].type == ship1->type && z > 0) {
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
						board[z][ship1->front.y].color = NULL;
						board[ship1->front.x][ship1->front.y].color = NULL;
						board[ship1->front.x][ship1->front.y].type = ' ';
						board[ship1->front.x][ship1->front.y].status = 0;
						//board[x-ship1->dim+1][y].status = 0;
						//board[x-ship1->dim+1][y].type = ' ';
					}
				}

				goodPoint = false;
				return 0;
				break;
			}
		}
	}
	else if(dir == 'd' || dir == 'D') {
		ship1->dir.degr = 180;
		board[ship1->front.x][ship1->front.y].type=ship1->type;
		board[ship1->front.x][ship1->front.y].status = 1;
		board[ship1->front.x][ship1->front.y].color = ship1->color;
		for(z=x+1; z < x + ship1->dim; z++) {
			if (board[z][ship1->front.y].status == 0 && board[x][y].type == ship1->type && z < 10) {
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
						board[z][ship1->front.y].color = NULL;
						board[ship1->front.x][ship1->front.y].color = NULL;
						board[ship1->front.x][ship1->front.y].type = ' ';
						board[ship1->front.x][ship1->front.y].status = 0;

						//board[x+ship1->dim-1][y].status = 0;
						//board[x+ship1->dim-1][y].type = ' ';
					}
				}

				goodPoint = false;
				return 0;
				break;

			}
		}

	}
	else if(dir == 'l' || dir == 'L') {
		ship1->dir.degr = 270;
		board[ship1->front.x][ship1->front.y].type=ship1->type;
		board[ship1->front.x][ship1->front.y].status = 1;
		board[ship1->front.x][ship1->front.y].color = ship1->color;
		for(z=y-1; z > y - ship1->dim; z--) {
			if (board[ship1->front.x][z].status == 0 && board[x][y].type == ship1->type &&  z > 0) {
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
						board[ship1->front.x][ship1->front.y].color = NULL;
						board[ship1->front.x][ship1->front.y].type = ' ';
						board[ship1->front.x][ship1->front.y].status = 0;
						//board[x][y-ship1->dim+1].status = 0;
						//board[x][y-ship1->dim+1].type = ' ';

					}
				}

				goodPoint = false;
				return 0;
				break;

			}
		}

	}
	else if(dir == 'r' || dir == 'R') {
		ship1->dir.degr = 90;
		board[ship1->front.x][ship1->front.y].type=ship1->type;
		board[ship1->front.x][ship1->front.y].status = 1;
		board[ship1->front.x][ship1->front.y].color = ship1->color;
		for(z=y+1; z < y + ship1->dim; z++) {
			if (board[ship1->front.x][z].status == 0 && board[x][y].type == ship1->type && z < 10) {
				board[ship1->front.x][z].type= ship1->type;
				board[ship1->front.x][z].status = 1;
				board[ship1->front.x][z].color = ship1->color;

				//board[x][y+ship1->dim-1].status = 1;
				//board[x][y+ship1->dim-1].type = ship1->type;
			}
			else {
				for(z=y; z < y + ship1->dim; z++) {
					if(board[ship1->front.x][z].type == ship1->type) {
						board[ship1->front.x][z].status = 0;
						board[ship1->front.x][z].type = ' ';
						board[ship1->front.x][z].color = NULL;
						board[ship1->front.x][ship1->front.y].color = NULL;
						board[ship1->front.x][ship1->front.y].type = ' ';
						board[ship1->front.x][ship1->front.y].status = 0;
						//board[x][y+ship1->dim-1].status = 0;
						//board[x][y+ship1->dim-1].type = ' ';
					}
				}

				goodPoint = false;
				return 0;
				break;

			}
		}

	}

	system("CLS");
	return 1;
}

//The codes for the board matrix, that translates the numbers into Data

//Functions

//Refresh Board
void refreshBoard(unit Disp[10][10]) {
	int x;
	int y;

	keyTemp = 1;
	keydone = false;
	system("CLS");

	printf("\nYour Board\n----------------------------------------\n\n");
	for(x=0; x<10; x++) {
		//cout << x << "|";
		printf("%d|",x);
		for(y=0; y<10; y++) {
			SetConsoleTextAttribute(hConsole, Disp[x][y].color | fRED | fGREEN | fBLUE);
			if(Disp[x][y].status == 2) {
				SetConsoleTextAttribute(hConsole, Disp[x][y].color | INTENS | fINTENS);
				printf("ø");
			}
			else if(Disp[x][y].status == 2) {

			}
			else {
				printf("%c", Disp[x][y].type);
				
			}
			SetConsoleTextAttribute(hConsole, fRED|fGREEN|fBLUE);
			printf("|");							//Should be .type
		}
		key();
		printf("\n");
	}
	printf(" |0|1|2|3|4|5|6|7|8|9|\n");
}

void key() {
	if (!keydone){
		switch (keyTemp) {
		case 1:
				printf("\t\t\t");
				SetConsoleTextAttribute(hConsole, play[1].dest.color|fRED|fGREEN|fBLUE|fINTENS);
				printf("--Destroyer");
				SetConsoleTextAttribute(hConsole, fRED|fGREEN|fBLUE);

				break;
		case 2:
				printf("\t\t\t");
				SetConsoleTextAttribute(hConsole, play[1].sub.color|fRED|fGREEN|fBLUE|fINTENS);
				printf("--Submarine");
				SetConsoleTextAttribute(hConsole, fRED|fGREEN|fBLUE);

				break;
		case 3:
				printf("\t\t\t");
				SetConsoleTextAttribute(hConsole, play[1].crus.color|fRED|fGREEN|fBLUE|fINTENS);
				printf("--Crusier");
				SetConsoleTextAttribute(hConsole, fRED|fGREEN|fBLUE);

				break;
		case 4:
				printf("\t\t\t");
				SetConsoleTextAttribute(hConsole, play[1].battle.color|fRED|fGREEN|fBLUE|fINTENS);
				printf("--Battleship");
				SetConsoleTextAttribute(hConsole, fRED|fGREEN|fBLUE);

				break;
		case 5:
				printf("\t\t\t");
				SetConsoleTextAttribute(hConsole, play[1].carr.color|fRED|fGREEN|fBLUE|fINTENS);
				printf("--Air Carrier");
				SetConsoleTextAttribute(hConsole, fRED|fGREEN|fBLUE);
				keydone = true;
				break;
		}
		keyTemp++;
	}
}

//Initial Setup
void setup() {
	int x, y;
	//1 = player 1
	keydone = false;
	keyTemp = 1;

	for (x=0; x<=1; x++) {
		play[x].dest.dim = DEST;
		play[x].dest.type = 'd';
		strcpy(play[x].dest.name,"Destroyer");
		play[x].dest.color = BLUE;
		play[x].destnum = 2;
		play[x].sub.dim = SUB;
		play[x].sub.type='s';
		strcpy(play[x].sub.name,"Submarine");
		play[x].sub.color = RED;
		play[x].subnum = 3;
		play[x].crus.dim = CRUS;
		play[x].crus.type = 'c';
		strcpy(play[x].crus.name,"Cruiser");
		play[x].crus.color = GREEN;
		play[x].crusnum = 3;
		play[x].battle.dim = BESP;
		play[x].battle.type = 'b';
		strcpy(play[x].battle.name,"Battleship");
		play[x].battle.color = RED | GREEN;
		play[x].battnum = 4;
		play[x].carr.dim = AIRC;
		play[x].carr.type = 'a';
		strcpy(play[x].carr.name,"Carrier");
		play[x].carr.color = RED | BLUE;
		play[x].airnum = 5;
	}

	for(x=0; x<10; x++) {
		for(y=0; y<10; y++) {
			board1[x][y].status = 0;
			board2[x][y].status = 0;
			board1[x][y].color = fRED | fGREEN | fBLUE;
			board2[x][y].color = fRED | fGREEN | fBLUE;

		}
	}


	for(x=0; x<10; x++) {
		for(y=0; y<10; y++) {
			board1g[x][y].status = 1;
			board2g[x][y].status = 1;
		}
	}
	set = false;
	play1moves = 0;
	play2moves = 0;

}

//Menu function
int menu() {
	system("CLS");
	int option;
	cout << "Menu\n------------------------------\n";
	cout << "1.Start/Continue\n2.Quick Start\n3.Change Options\n4.About\n0.Exit\n--------------------------\n";
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
			set = false;
			game();
			break;

		case 2:
			cont=true;
			autoSet();
			set = true;
			game();
			break;

		case 4:
			about();
			break;

		case 0:
			exit(0);
			break;
	}

	system("CLS");				//Clears screen

}

void autoSet() {
	COOR start;
	char dir;
	int x;
	int y;
	int z;

	for(x=0; x<5; x++) {
		start.x = (rand()%9) + 1;
		start.y = (rand()%9) + 1;
		if(board1[start.x][start.y].status == 0) {
			y = (rand()%4) + 0;
			switch(y) {
				case 0:
					dir = 'r';
					break;
				case 1:
					dir = 'd';
					break;
				case 2:
					dir = 'u';
					break;
				case 3:
					dir = 'l';
					break;
			}
			switch(x) {

				case 0:
					if((z = setPiece(&play[1].dest, play[1], board1, start, dir) == 0)) {
						x--;
					}
					break;
				case 1:
					if((z = setPiece(&play[1].sub, play[1], board1, start, dir) == 0)) {
						x--;
					}
					break;

				case 2:
					if((z = setPiece(&play[1].crus, play[1], board1, start, dir) == 0)) {
						x--;
					}
					break;

				case 3:
					if((z = setPiece(&play[1].battle, play[1], board1, start, dir) == 0)) {
						x--;
					}
					break;

				case 4:
					if((z = setPiece(&play[1].carr, play[1], board1, start, dir) == 0)) {
						x--;
					}
					break;

			}
		}
		else {
			x--;
		}
	}
}

void aisetup() {
	COOR start;
	char dir;
	int x;
	int y;
	int z;

	for(x=0; x<5; x++) {
		start.x = (rand()%9) + 1;
		start.y = (rand()%9) + 1;
		if(board2[start.x][start.y].status == 0) {
			y = (rand()%4) + 0;
			switch(y) {
				case 0:
					dir = 'r';
					break;
				case 1:
					dir = 'd';
					break;
				case 2:
					dir = 'u';
					break;
				case 3:
					dir = 'l';
					break;
			}
			switch(x) {
				case 0:
					if((z = setPiece(&play[0].dest, play[0], board2, start, dir) == 0)) {
						x--;
					}
					break;
				case 1:
					if((z = setPiece(&play[0].sub, play[0], board2, start, dir) == 0)) {
						x--;
					}
					break;

				case 2:
					if((z = setPiece(&play[0].crus, play[0], board2, start, dir) == 0)) {
						x--;
					}
					break;

				case 3:
					if((z = setPiece(&play[0].battle, play[0], board2, start, dir) == 0)) {
						x--;
					}
					break;

				case 4:
					if((z = setPiece(&play[0].carr, play[0], board2, start, dir) == 0)) {
						x--;
					}
					break;

			}
		}
		else {
			x--;
		}
	}

}

void about() {
	MessageBox(NULL, "Created by Vineet Tiruvadi", "About", MB_OK);
	MessageBox(NULL, "Eyesight Friendly Battleship!" , "WOHO!", MB_OK);
	PlaySound("obi.wav", NULL, SND_SYNC);
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