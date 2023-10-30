/*
* This program allows display and editing of text art (also called ASCII art).
*/

#include <iostream>
#include <fstream>
#include <cctype>
#include <windows.h>
#include <conio.h>
#include <ctype.h>
#include "Definitions.h"
using namespace std;


int main()
{
	//current holds the current canvas
	Node* current;
	current = newCanvas();

	/*
	* menuChoice hold's the user's choice from the menu
	* old and newCharChoice hold the characters that the user chooses for the replace function
	* animateToggle holds the 'y' or 'n' to be displayed in the menu
	* name holds the user's input for the name of a file
	* fileName holds the properly formatted name for a file
	* fileType hold the users choice of canvas or animation for load and save functions
	*/
	char menuChoice = ' ', oldCharChoice = ' ', newCharChoice = ' ', animateToggle;
	char name[FILENAMESIZE - CANVASNAMEBUFFER];
	char fileName[FILENAMESIZE];
	char fileType;

	//lists so the user can undo, redo, or store clips
	List undoList, redoList, clips;

	//initialize the lists with NULL
	undoList.head = NULL;
	redoList.head = NULL;
	clips.head = NULL;

	//rowMoved and colMoved hold the values for the moveCanvas function
	int rowMoved, colMoved;

	//initialize animate with false
	bool animate = false;

	//initializes the canvas with spaces
	initCanvas(current->item);

	//tolower function makes the user's character choice always lowercase
	//loop continues until user enters q to quit the program
	while (tolower(menuChoice) != 'q')
	{
		system("cls");
		displayCanvas(current->item);

		animateToggle = animate ? 'Y' : 'N';

		//clears any lines used by previous function output
		clearLine(MAXROWS + 1, MAXMENUSIZE);
		clearLine(MAXROWS + 2, MAXMENUSIZE);
		clearLine(MAXROWS + 3, MAXMENUSIZE);

		//moves cursor to the line below the bottom canvas border
		gotoxy(MAXROWS + 1, 0);

		printf("<A>nimate: %c / <U>ndo: %d / ", animateToggle, undoList.count);

		//checks if there are possible redos
		if (redoList.count > 0)
			printf("Red<O>: %d / ", redoList.count);

		printf("Cl<I>p: %d", clips.count);

		//checks if any clips are stored
		if (clips.count > 1)
			cout << " / <P>lay";

		cout << "\n<E>dit / <M>ove / <R>eplace / <D>raw / <C>lear / <L>oad / <S>ave / <Q>uit: ";
		cin >> menuChoice;

		//decides which functions to execute based on user's menu choice
		switch (tolower(menuChoice))
		{
		//toggles the animate variable
		case 'a':
			animate = !animate;
			animateToggle = animate ? 'Y' : 'N';
			break;

		case 'o':
			//returns to menu if redoList is empty
			if (redoList.count == 0)
				break;
			//moves the first node from redo list to current canvas, puts current canvas in undoList
			restore(redoList, undoList, current);
			break;

		case 'i':
			//adds a copy of the canvas to the clips list
			addNode(clips, newCanvas(current));
			break;

		case 'p':
			//clears the menu line so new text can be displayed
			clearLine(MAXROWS + 1, MAXMENUSIZE);
			clearLine(MAXROWS + 2, MAXMENUSIZE);

			//resets the cursor
			gotoxy(MAXROWS + 1, 0);

			cout << "Hold <ESC> to stop \t Clip: ";

			//plays the stored clips
			play(clips);
			break;

		case 'e':
			//adds a copy of the canvas to undoList
			addUndoState(undoList, redoList, current);

			editCanvas(current->item);
			break;

		case 'm':
			addUndoState(undoList, redoList, current);

			clearLine(MAXROWS + 1, MAXMENUSIZE);
			clearLine(MAXROWS + 2, MAXMENUSIZE);
			gotoxy(MAXROWS + 1, 0);

			//stores column units to move
			cout << "Enter column units to move: ";
			cin >> colMoved;

			clearLine(MAXROWS + 1, MAXMENUSIZE);
			clearLine(MAXROWS + 2, MAXMENUSIZE);
			gotoxy(MAXROWS + 1, 0);

			//stores row units to move
			cout << "Enter row units to move: ";
			cin >> rowMoved;

			moveCanvas(current->item, rowMoved, colMoved);
			break;

		case 'r':
			addUndoState(undoList, redoList, current);

			clearLine(MAXROWS + 1, MAXMENUSIZE);
			clearLine(MAXROWS + 2, MAXMENUSIZE);
			gotoxy(MAXROWS + 1, 0);

			cin.clear();
			cin.ignore((numeric_limits<streamsize>::max)(), '\n');

			//stores the character to be replaced
			cout << "Choose the character to be replaced: ";
			oldCharChoice = cin.get();

			//checks if the user wanted to replace a space
			if (oldCharChoice == '\n')
			{
				oldCharChoice = ' ';
			}

			cin.clear();
			cin.ignore((numeric_limits<streamsize>::max)(), '\n');

			clearLine(MAXROWS + 1, MAXMENUSIZE);
			clearLine(MAXROWS + 2, MAXMENUSIZE);
			gotoxy(MAXROWS + 1, 0);

			//stores character to be inserted
			cout << "Choose a new character to insert: ";
			newCharChoice = cin.get();

			//checks if the user wanted to insert a space
			if (newCharChoice == '\n')
			{
				newCharChoice = ' ';
			}

			replace(current->item, oldCharChoice, newCharChoice);
			break;

		case 'd':
			menuTwo(current, undoList, redoList, clips, animate);
			break;

		case 'c':
			addUndoState(undoList, redoList, current);
			initCanvas(current->item);
			break;

		case 'u':
			//returns to the menu if undoList is empty
			if (undoList.count == 0)
				break;

			//moves the first node from undoList to current canvas, puts current canvas in redoList
			restore(undoList, redoList, current);
			break;

		case 'l':
			//clears the menu line so new text can be displayed
			clearLine(MAXROWS + 1, MAXMENUSIZE);
			clearLine(MAXROWS + 2, MAXMENUSIZE);
			gotoxy(MAXROWS + 1, 0);

			cout << "<C>anvas or <A>nimation ? ";

			cin.clear();
			cin.ignore((numeric_limits<streamsize>::max)(), '\n');

			//stores user's choice of file type
			cin >> fileType;

			//changes fileType char to lowercase
			switch (tolower(fileType))
			{
			case 'c':
				clearLine(MAXROWS + 1, MAXMENUSIZE);
				cout << "Enter the filename. (Don't include .txt): ";

				cin.clear();
				cin.ignore((numeric_limits<streamsize>::max)(), '\n');

				//stores the user's input in an array, subtract CANVASNAMEBUFFER to leave space for SavedFiles and .txt
				cin.getline(name, FILENAMESIZE - CANVASNAMEBUFFER);

				//combines SavedFiles, name, and .txt to create a properly formated file name (SavedFiles\\name.txt)
				snprintf(fileName, FILENAMESIZE, "%s%s%s", "SavedFiles\\", name, ".txt");

				//checks if the file failed to load
				if (!loadCanvas(current->item, fileName))
				{
					clearLine(MAXROWS + 1, MAXMENUSIZE);
					clearLine(MAXROWS + 2, MAXMENUSIZE);
					gotoxy(MAXROWS + 1, 0);

					//displays error message before returning to the menu
					cerr << "ERROR: File could not be read.\n";
					system("pause");
				}
				break;

			case 'a':
				clearLine(MAXROWS + 1, MAXMENUSIZE);
				cout << "Enter the filename. (Don't include .txt): ";

				cin.clear();
				cin.ignore((numeric_limits<streamsize>::max)(), '\n');

				//stores the user's input in an array, subtract CLIPSNAMEBUFFER to leave space for SavedFiles, the clip number, and .txt
				cin.getline(name, FILENAMESIZE - CLIPSNAMEBUFFER);

				//combines SavedFiles and name to create a formatted name that needs the clip number and .txt (SavedFiles\\name)
				snprintf(fileName, FILENAMESIZE, "%s%s", "SavedFiles\\", name);

				//checks if no clips were able to be loaded
				if (!loadClips(clips, fileName))
				{
					clearLine(MAXROWS + 1, MAXMENUSIZE);
					clearLine(MAXROWS + 2, MAXMENUSIZE);
					gotoxy(MAXROWS + 1, 0);

					//displays error message before returning to the menu
					cerr << "ERROR: File(s) could not be loaded.\n";
					system("pause");
				}
				else
				{
					clearLine(MAXROWS + 1, MAXMENUSIZE);
					clearLine(MAXROWS + 2, MAXMENUSIZE);
					gotoxy(MAXROWS + 1, 0);

					//displays message before returning to menu
					cout << "File(s) loaded!\n";
					system("pause");
				}
				break;

			default:
				break;
			}
			break;

		case 's':
			//clears the menu line so new text can be displayed
			clearLine(MAXROWS + 1, MAXMENUSIZE);
			clearLine(MAXROWS + 2, MAXMENUSIZE);
			gotoxy(MAXROWS + 1, 0);

			cout << "<C>anvas or <A>nimation ? ";
			
			cin.clear();
			cin.ignore((numeric_limits<streamsize>::max)(), '\n');
			
			//stores user's choice of file type
			cin >> fileType;

			//changes fileType char to lowercase
			switch (tolower(fileType))
			{
			case 'c':
				clearLine(MAXROWS + 1, MAXMENUSIZE);
				cout << "Enter the filename. (Don't include .txt): ";

				cin.clear();
				cin.ignore((numeric_limits<streamsize>::max)(), '\n');

				//stores the user's input in an array, subtract CANVASNAMEBUFFER to leave space for SavedFiles and .txt
				cin.getline(name, FILENAMESIZE - CANVASNAMEBUFFER);

				//combines SavedFiles, name, and .txt to create a properly formated file name (SavedFiles\\name.txt)
				snprintf(fileName, FILENAMESIZE, "%s%s%s", "SavedFiles\\", name, ".txt");

				//checks if canvas failed to save
				if (!saveCanvas(current->item, fileName))
				{
					clearLine(MAXROWS + 1, MAXMENUSIZE);
					clearLine(MAXROWS + 2, MAXMENUSIZE);
					gotoxy(MAXROWS + 1, 0);

					//displays error message before returning to the menu
					cerr << "ERROR: File could not be created.\n";
					system("pause");
				}
				else
				{
					clearLine(MAXROWS + 1, MAXMENUSIZE);
					clearLine(MAXROWS + 2, MAXMENUSIZE);
					gotoxy(MAXROWS + 1, 0);

					//displays message before returning to menu
					cout << "File saved!\n";
					system("pause");
				}
				break;

			case 'a':
				clearLine(MAXROWS + 1, MAXMENUSIZE);
				cout << "Enter the filename. (Don't include .txt): ";

				cin.clear();
				cin.ignore((numeric_limits<streamsize>::max)(), '\n');

				//stores the user's input in an array, subtract CLIPSNAMEBUFFER to leave space for SavedFiles, the clip number, and .txt
				cin.getline(name, FILENAMESIZE - CLIPSNAMEBUFFER);

				//combines SavedFiles and name to create a formatted name that needs the clip number and .txt (SavedFiles\\name)
				snprintf(fileName, FILENAMESIZE, "%s%s", "SavedFiles\\", name);

				//checks if all clips failed to save
				if (!saveClips(clips, fileName))
				{
					clearLine(MAXROWS + 1, MAXMENUSIZE);
					clearLine(MAXROWS + 2, MAXMENUSIZE);
					gotoxy(MAXROWS + 1, 0);

					//displays error message before returning to the menu
					cerr << "ERROR: File(s) could not be created.\n";
					system("pause");
				}
				else
				{
					clearLine(MAXROWS + 1, MAXMENUSIZE);
					clearLine(MAXROWS + 2, MAXMENUSIZE);
					gotoxy(MAXROWS + 1, 0);

					//displays message before returning to menu
					cout << "File(s) saved!\n";
					system("pause");
				}
				break;

			default:
				break;
			}
			break;

		case 'q':
			//exits the switch statement so the while loop and the program can end
			break;

		default:
			clearLine(MAXROWS + 1, MAXMENUSIZE);
			clearLine(MAXROWS + 2, MAXMENUSIZE);
			gotoxy(MAXROWS + 1, 0);

			//gives error message before redisplaying the menu for another choice
			cout << "'" << menuChoice << "' is not a valid selection. \n";
			system("pause");
		}
	}

	//deallocates the memory used for the lists
	deleteList(undoList);
	deleteList(redoList);
	deleteList(clips);

	return 0;
}


void gotoxy(short row, short col)
{
	COORD pos = { col, row };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}


void clearLine(int lineNum, int numOfChars)
{
	// Move cursor to the beginning of the specified line on the console
	gotoxy(lineNum, 0);

	// Write a specified number of spaces to overwrite characters
	for (int x = 0; x < numOfChars; x++)
		cout << " ";

	// Move cursor back to the beginning of the line
	gotoxy(lineNum, 0);
}


void replace(char canvas[][MAXCOLS], char oldCh, char newCh)
{
	//for loops loop through canvas
	for (int row = 0; row < MAXROWS; row++)
	{
		for (int col = 0; col < MAXCOLS; col++)
		{
			/*if statement checks the old character and replaces
			it with the new character if valid*/
			if (canvas[row][col] == oldCh)
			{
				canvas[row][col] = newCh;
			}
		}
	}
}


void editCanvas(char canvas[][MAXCOLS])
{
	char moveInput;
	int row = 0, col = 0;

	clearLine(MAXROWS + 1, MAXMENUSIZE);
	clearLine(MAXROWS + 2, MAXMENUSIZE);
	gotoxy(MAXROWS + 1, 0);
	cout << "Press <ESC> to stop editing";

	// Move cursor to row,col and then get
	// a single character from the keyboard
	gotoxy(row, col);
	moveInput = _getch();


	while (moveInput != ESC)
	{
		//if statement to move the cursor using arrowkeys 
		if (moveInput == SPECIAL)
		{
			moveInput = _getch();
			switch (moveInput)
			{
			case LEFTARROW:
				if (col > 0)
				{
					col--;
				}
				break;
			case RIGHTARROW:
				if (col < MAXCOLS - 1)
				{
					col++;
				}
				break;
			case UPARROW:
				if (row > 0)
				{
					row--;
				}
				break;
			case DOWNARROW:
				if (row < MAXROWS - 1)
				{
					row++;
				}
				break;
			}
		}
		//if key is a special condition 
		else if (moveInput == '\0')
		{
			moveInput = _getch();
		}
		//puts character where cursor is 
		else if (32 <= moveInput && moveInput <= 126)
		{
			canvas[row][col] = moveInput;
			printf("%c", moveInput);
		}

		gotoxy(row, col);
		moveInput = _getch();
	}
}


void moveCanvas(char canvas[][MAXCOLS], int rowValue, int colValue)
{
	//movedCanvas holds a copy of the canvas that has been moved based on row and col value
	char movedCanvas[MAXROWS][MAXCOLS];

	//initializes movedCanvas with spaces
	initCanvas(movedCanvas);

	//loops through each row of canvas
	for (int row = 0; row < MAXROWS; row++)
	{
		//loops through each column of canvas
		for (int col = 0; col < MAXCOLS; col++)
		{
			//checks if new row position is in the array
			//goes to next iteration if it goes out of canvas
			if (row + rowValue < 0 || row + rowValue >= MAXROWS)
			{
				continue;
			}
			//checks if new column position is in the array
			//goes to next iteration if it goes out of canvas
			else if (col + colValue < 0 || col + colValue >= MAXCOLS)
			{
				continue;
			}

			movedCanvas[row + rowValue][col + colValue] = canvas[row][col];
		}
	}

	initCanvas(canvas);

	copyCanvas(canvas, movedCanvas);
}


void initCanvas(char canvas[][MAXCOLS])
{
	//loops through each row
	for (int row = 0; row < MAXROWS; row++)
	{
		//loops through each column
		for (int col = 0; col < MAXCOLS; col++)
		{
			//initializes each array position with a space
			canvas[row][col] = ' ';
		}
	}
}


void displayCanvas(char canvas[][MAXCOLS])
{
	gotoxy(0, 0);
	//displays the canvas, one character at a time 
	for (int row = 0; row < MAXROWS; row++)
	{
		for (int col = 0; col < MAXCOLS; col++)
		{
			cout << canvas[row][col];
		}

		//displays the '|' character for the right canvas border at the end of each row
		cout << "|" << endl;
	}

	for (int i = 0; i < MAXCOLS; i++)
	{
		//displays the bottom canvas border
		cout << "-";
	}
}


void copyCanvas(char to[][MAXCOLS], char from[][MAXCOLS])
{
	//Loops through canvas and copys to a new array
	for (int row = 0; row < MAXROWS; row++)
	{
		for (int col = 0; col < MAXCOLS; col++)
		{
			to[row][col] = from[row][col];
		}
	}
}


bool saveCanvas(char canvas[][MAXCOLS], char filename[])
{
	/*
	* outFile holds the file that the canvas is saved to
	* input array holds the user's input for the name of the file
	*/
	ofstream outFile;
	outFile.open(filename);

	//checks if the file was opened
	if (!outFile)
		return false;

	//loops through each row
	for (int row = 0; row < MAXROWS; row++)
	{
		//loops through each column
		//copies characters from the canvas to the output file one at a time
		for (int col = 0; col < MAXCOLS; col++)
		{
			outFile << canvas[row][col];
		}

		//new line to move to the next row on the document
		outFile << endl;
	}

	outFile.close();

	return true;
}

/*
* Gets a filename from the user. If file can be opened for reading,
* this function loads the file's contents into canvas.
* File is a TXT file located in the SavedFiles folder.
* If file cannot be opened, error message is displayed and
* canvas is left unchanged.
*/
bool loadCanvas(char canvas[][MAXCOLS], char filename[]) 
{
	//file to load
	ifstream inFile;

	//holds the current character to be loaded from the file
	char fileChar;

	//tracks the current row and column of the canvas
	int row = 0;
	int col = 0;

	inFile.open(filename);

	//checks to see if file was opened
	if (!inFile)
	{
		return false;
	}

	initCanvas(canvas);

	//loops through each row of the canvas, checks for eof
	for (row = 0; row < MAXROWS && !inFile.eof(); row++)
	{
		//stores a character from the document
		//after first iteration, this gets a character to override the \n that is stored
		fileChar = inFile.get();

		//loops through each column of the canvas 
		//puts the character from the file in the canvas if it isn't a \n or end of file
		for (col = 0; col < MAXCOLS && !inFile.eof() && fileChar != '\n'; col++)
		{
			canvas[row][col] = fileChar;
			fileChar = inFile.get();
		}

		//reads the remaining characters on the line without storing them if the canvas row is already full
		if (col == MAXCOLS)
		{
			while (fileChar != '\n' && !inFile.eof())
			{
				fileChar = inFile.get();
			}
		}
	}
	return true;
}