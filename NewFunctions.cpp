#include <iostream>
#include <windows.h>
#include <conio.h>
#include "Definitions.h"
using namespace std;


Point::Point(DrawPoint p)
{
	row = (int)round(p.row);
	col = (int)round(p.col);
}

// https://math.stackexchange.com/questions/39390/determining-end-coordinates-of-line-with-the-specified-length-and-angle
DrawPoint findEndPoint(DrawPoint start, int len, int angle)
{
	DrawPoint end;
	end.col = start.col + len * cos(degree2radian(angle));
	end.row = start.row + len * sin(degree2radian(angle));
	return end;
}


// Use this to draw characters into the canvas, with the option of performing animation
void drawHelper(char canvas[][MAXCOLS], Point p, char ch, bool animate)
{
	// Pause time between steps (in milliseconds)
	const int TIME = 50;

	// Make sure point is within bounds
	if (p.row >= 0 && p.row < MAXROWS && p.col >= 0 && p.col < MAXCOLS)
	{
		// Draw character into the canvas
		canvas[p.row][p.col] = ch;

		// If animation is enabled, draw to screen at same time
		if (animate)
		{
			gotoxy(p.row, p.col);
			printf("%c", ch);
			Sleep(TIME);
		}
	}
}


// Fills gaps in a row caused by mismatch between match calculations and screen coordinates
// (i.e. the resolution of our 'canvas' isn't very good)
void drawLineFillRow(char canvas[][MAXCOLS], int col, int startRow, int endRow, char ch, bool animate)
{
	// determine if we're counting up or down
	if (startRow <= endRow)
		for (int r = startRow; r <= endRow; r++)
		{
			Point point(r, col);
			drawHelper(canvas, point, ch, animate);
		}
	else
		for (int r = startRow; r >= endRow; r--)
		{
			Point point(r, col);
			drawHelper(canvas, point, ch, animate);
		}
}


// Draw a single line from start point to end point
void drawLine(char canvas[][MAXCOLS], DrawPoint start, DrawPoint end, bool animate)
{
	char ch;

	Point scrStart(start);
	Point scrEnd(end);

	// vertical line
	if (scrStart.col == scrEnd.col)
	{
		ch = '|';

		drawLineFillRow(canvas, scrStart.col, scrStart.row, scrEnd.row, ch, animate);
	}
	// non-vertical line
	else
	{
		int row = -1, prevRow;

		// determine the slope of the line
		double slope = (start.row - end.row) / (start.col - end.col);

		// choose appropriate characters based on 'steepness' and direction of slope
		if (slope > 1.8)  ch = '|';
		else if (slope > 0.08)  ch = '`';
		else if (slope > -0.08)  ch = '-';
		else if (slope > -1.8) ch = '\'';
		else ch = '|';

		// determine if columns are counting up or down
		if (scrStart.col <= scrEnd.col)
		{
			// for each column from start to end, calculate row values
			for (int col = scrStart.col; col <= scrEnd.col; col++)
			{
				prevRow = row;
				row = (int)round(slope * (col - start.col) + start.row);

				// draw from previous row to current row (to fill in row gaps)
				if (prevRow > -1)
				{
					drawLineFillRow(canvas, col, prevRow, row, ch, animate);
				}
			}
		}
		else
		{
			// for each column from start to end, calculate row values
			for (int col = scrStart.col; col >= scrEnd.col; col--)
			{
				prevRow = row;
				row = (int)round(slope * (col - start.col) + start.row);

				// draw from previous row to current row (to fill in row gaps)
				if (prevRow > -1)
				{
					drawLineFillRow(canvas, col, prevRow, row, ch, animate);
				}
			}
		}
	}
}


// Draws a single box around a center point
void drawBox(char canvas[][MAXCOLS], Point center, int height, bool animate)
{
	int sizeHalf = height / 2;
	int ratio = (int)round(MAXCOLS / (double)MAXROWS * sizeHalf);

	// Calculate where the four corners of the box should be
	DrawPoint points[4];
	points[0] = DrawPoint(center.row - sizeHalf, center.col - ratio);
	points[1] = DrawPoint(center.row - sizeHalf, center.col + ratio);
	points[2] = DrawPoint(center.row + sizeHalf, center.col + ratio);
	points[3] = DrawPoint(center.row + sizeHalf, center.col - ratio);

	// Draw the four lines of the box
	for (int x = 0; x < 3; x++)
	{
		drawLine(canvas, points[x], points[x + 1], animate);
	}
	drawLine(canvas, points[3], points[0], animate);

	// Replace the corners with a better looking character
	for (int x = 0; x < 4; x++)
	{
		drawHelper(canvas, points[x], '+', animate);
	}
}


// Menu for the drawing tools
void menuTwo(Node*& current, List& undoList, List& redoList, List& clips, bool& animate)
{
	/*
	* menuChoice stores the user's input for the menu
	* animateToggle stores either t or f to output in the menu according to animate value
	* pointChar holds the character that is returned by the getPoint function
	* oldCh holds the character to be replaced in the fill/line functions
	*/
	char menuChoice, animateToggle, pointChar, oldCh;

	//height variable stores the height for a box or a tree, depending on the function
	//branchAngle stores the angle of branches
	int height = 0, branchAngle;

	//startPoint holds the starting point for any of the drawing functions
	//endPoint holds the ending point for any drawing functions that need it
	Point startPoint, endPoint;

	do
	{
		//checks if animate is true or false, assigns 'y' or 'n' accordingly
		animateToggle = animate ? 'Y' : 'N';

		//displays current canvas
		displayCanvas(current->item);

		//clears any lines used by previous output
		clearLine(MAXROWS + 1, MAXMENUSIZE);
		clearLine(MAXROWS + 2, MAXMENUSIZE);
		clearLine(MAXROWS + 3, MAXMENUSIZE);

		//moves cursor to the line below the bottom canvas border
		gotoxy(MAXROWS + 1, 0);

		printf("<A>nimate: %c / <U>ndo: %d / ", animateToggle, undoList.count);

		//checks if there are any possible redos
		if (redoList.count > 0)
			printf("Red<O>: %d / ", redoList.count);

		printf("Cl<I>p: %d", clips.count);

		//checks if there are any stored clips
		if (clips.count > 1)
			cout << " / <P>lay";

		printf("\n<F>ill / <L>ine / <B>ox / <N>ested Boxes / <T>ree / <M>ain Menu: ");

		//clears cin buffer/error state
		cin.clear();
		cin.ignore((numeric_limits<streamsize>::max)(), '\n');

		//stores user's menu choice
		cin >> menuChoice;

		//changes user's input to a lowercase letter and checks the character
		switch (tolower(menuChoice))
		{
		//toggles the animate value
		case 'a':
			animate = !animate;
			animateToggle = animate ? 'Y' : 'N';
			break;

		case 'u':
			//returns to menu if undoList is empty
			if (undoList.count == 0)
				break;

			//moves the first node from undoList to current canvas, puts current canvas in redoList
			restore(undoList, redoList, current);
			break;

		case 'o':
			//returns to menu if redoList is empty
			if (redoList.count == 0)
				break;

			//moves the first node from redo list to current canvas, puts current canvas in undoList
			restore(redoList, undoList, current);
			break;

		case 'i':
			//adds the current canvas to the clip list
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

		case 'f':
			//clears the output line
			clearLine(MAXROWS + 1, MAXMENUSIZE);
			clearLine(MAXROWS + 2, MAXMENUSIZE);
			
			//resets the cursor
			gotoxy(MAXROWS + 1, 0);

			cout << "Enter character to fill with from current location / <ESC> to cancel";

			//gives row and col value to startPoint and stores the character returned by getPoint
			pointChar = getPoint(startPoint);

			//stores the character to be replaced by the fill function
			oldCh = current->item[startPoint.row][startPoint.col];

			//checks if the user wants to cancel
			if (pointChar == ESC)
				break;

			//makes a copy in case the user wants to undo
			addUndoState(undoList, redoList, current);

			fillRecursive(current->item, startPoint.row, startPoint.col, oldCh, pointChar, animate);

			break;

		case 'l':
			clearLine(MAXROWS + 1, MAXMENUSIZE);
			clearLine(MAXROWS + 2, MAXMENUSIZE);
			gotoxy(MAXROWS + 1, 0);
			cout << "Type any letter to choose a start point / <ESC> to cancel";

			//gives row and col value to startPoint and stores the character returned by getPoint
			pointChar = getPoint(startPoint);

			//checks if the user wants to cancel
			if (pointChar == ESC)
				break;

			//stores the character to be replaced by the pointChar
			oldCh = current->item[startPoint.row][startPoint.col];

			//stores the character entered so the user can see where the line will start
			current->item[startPoint.row][startPoint.col] = pointChar;

			//displays the canvas so the user can see the start of the line
			displayCanvas(current->item);

			/*
			reverts the starting point in the canvas back to what the character originally
			was so the character won't be left in the canvas
			*/
			current->item[startPoint.row][startPoint.col] = oldCh;

			clearLine(MAXROWS + 1, MAXMENUSIZE);
			clearLine(MAXROWS + 2, MAXMENUSIZE);
			gotoxy(MAXROWS + 1, 0);

			cout << "Type any letter to choose end point / <ESC> to cancel";

			//gives row and col value to endpoint and stores the character returned by getPoint
			pointChar = getPoint(endPoint);

			//checks if user wants to cancel
			if (pointChar == ESC)
				break;

			//makes a copy in case the user wants to undo
			addUndoState(undoList, redoList, current);

			drawLine(current->item, startPoint, endPoint, animate);
			break;

		case 'b':
			clearLine(MAXROWS + 1, MAXMENUSIZE);
			clearLine(MAXROWS + 2, MAXMENUSIZE);
			gotoxy(MAXROWS + 1, 0);

			cout << "Enter size: ";

			//clears cin buffer/error state
			cin.clear();
			cin.ignore((numeric_limits<streamsize>::max)(), '\n');

			//stores the height of the box
			cin >> height;

			clearLine(MAXROWS + 1, MAXMENUSIZE);
			clearLine(MAXROWS + 2, MAXMENUSIZE);
			gotoxy(MAXROWS + 1, 0);

			cout << "Type any letter to choose box center, or <c> for screen center / <ESC> to cancel";

			//gives row and col value to startPoint and stores the character returned by getPoint
			pointChar = getPoint(startPoint);

			//checks if the user wants the box to be centered in the canvas
			if (pointChar == 'c')
			{
				//sets the startPoint values to be the center of the canvas
				startPoint.row = 11;
				startPoint.col = 40;
			}
			//checks if user wants to cancel
			else if (pointChar == ESC)
				break;

			//makes a copy in case the user wants to undo
			addUndoState(undoList, redoList, current);

			drawBox(current->item, startPoint, height, animate);
			break;

		case 'n':
			clearLine(MAXROWS + 1, MAXMENUSIZE);
			clearLine(MAXROWS + 2, MAXMENUSIZE);
			gotoxy(MAXROWS + 1, 0);

			cout << "Enter size of largest box: ";

			//clears cin buffer/error state
			cin.clear();
			cin.ignore((numeric_limits<streamsize>::max)(), '\n');

			//stores the height of the largest box
			cin >> height;

			clearLine(MAXROWS + 1, MAXMENUSIZE);
			clearLine(MAXROWS + 2, MAXMENUSIZE);
			gotoxy(MAXROWS + 1, 0);

			cout << "Type any letter to choose box center, or <c> for screen center / <ESC> to cancel";

			//gives row and col value to startPoint and stores the character returned by getPoint
			pointChar = getPoint(startPoint);

			//checks if the user wants to center the boxes in the canvas
			if (pointChar == 'c')
			{
				//sets the startPoint values to be the center of the canvas
				startPoint.row = 11;
				startPoint.col = 40;
			}
			//checks if the user wants to cancel
			else if (pointChar == ESC)
				break;

			//makes a copy in case the user wants to undo
			addUndoState(undoList, redoList, current);

			drawBoxesRecursive(current->item, startPoint, height, animate);
			break;

		case 't':
			clearLine(MAXROWS + 1, MAXMENUSIZE);
			clearLine(MAXROWS + 2, MAXMENUSIZE);
			gotoxy(MAXROWS + 1, 0);

			cout << "Enter apporoximate tree height: ";

			//clears cin buffer/error state
			cin.clear();
			cin.ignore((numeric_limits<streamsize>::max)(), '\n');

			//stores the height of the tree
			cin >> height;

			clearLine(MAXROWS + 1, MAXMENUSIZE);
			clearLine(MAXROWS + 2, MAXMENUSIZE);
			gotoxy(MAXROWS + 1, 0);

			cout << "Enter branch angle: ";

			cin.clear();
			cin.ignore((numeric_limits<streamsize>::max)(), '\n');

			//stores the angle for branches
			cin >> branchAngle;

			clearLine(MAXROWS + 1, MAXMENUSIZE);
			clearLine(MAXROWS + 2, MAXMENUSIZE);
			gotoxy(MAXROWS + 1, 0);

			cout << "Type any letter to choose start point, or <c> for bottom center / <ESC> to cancel";

			//gives row and col value to startPoint and stores the character returned by getPoint
			pointChar = getPoint(startPoint);

			//checks if the user wants the tree to be centered in the canvas
			if (pointChar == 'c')
			{
				//sets the startPoint values to be the center of the canvas
				startPoint.row = 22;
				startPoint.col = 40;
			}
			//checks if the user wants to cancel
			else if (pointChar == ESC)
				break;

			//makes a copy in case the user wants to undo
			addUndoState(undoList, redoList, current);

			treeRecursive(current->item, startPoint, height, 270, branchAngle, animate);
			break;

		case 'm':
			//exits the switch statement so the while loop can end and return to menu
			break;

		default:
			clearLine(MAXROWS + 1, MAXMENUSIZE);
			clearLine(MAXROWS + 2, MAXMENUSIZE);
			gotoxy(MAXROWS + 1, 0);

			//gives error message before redisplaying the menu for another choice
			printf("'%c' is not a valid selection. \n", menuChoice);
			system("pause");
		}
	} while (tolower(menuChoice) != 'm');

}


// Get a single point from screen, with character entered at that point
char getPoint(Point& pt)
{

	char moveInput;
	int row = 0, col = 0;

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

		//Pt is updated with current user position and returns input
		else if (32 <= moveInput && moveInput <= 126)
		{
			pt.row = row;
			pt.col = col;
			return moveInput;
		}

		gotoxy(row, col);
		moveInput = _getch();
	}
	return ESC;
}


// Recursively fill a section of the screen
void fillRecursive(char canvas[][MAXCOLS], int row, int col, char oldCh, char newCh, bool animate)
{
	//Checks if the space it is trying to edit is in the array/canvas
	if (row < 0 || row >= MAXROWS || col < 0 || col >= MAXCOLS)
	{
		return;
	}
	//Checks that the character it is trying to change is the correct character 
	if (canvas[row][col] != oldCh)
	{
		return;
	}

	//sets the Point that is being edited for the drawhelper 
	Point fillPoint;
	fillPoint.row = row;
	fillPoint.col = col;
	drawHelper(canvas, fillPoint, newCh, animate);

	//Calls this function to attempt to fill the surrounding cardinal spaces
	fillRecursive(canvas, row + 1, col, oldCh, newCh, animate);
	fillRecursive(canvas, row - 1, col, oldCh, newCh, animate);
	fillRecursive(canvas, row, col + 1, oldCh, newCh, animate);
	fillRecursive(canvas, row, col - 1, oldCh, newCh, animate);

}


// Recursively draw a tree
void treeRecursive(char canvas[][MAXCOLS], DrawPoint start, int height, int startAngle, int branchAngle, bool animate)
{
	// Stops once branch height is less than 3
	if (height < 3)
	{
		return;
	}

	//stores the end of the most recent branch
	//is used as the start point for the next branch
	DrawPoint branchEnd = findEndPoint(start, height / 3, startAngle);

	// Two treeRecursive calls is for branches on each side of the tree  
	drawLine(canvas, start, branchEnd, animate);
	treeRecursive(canvas, branchEnd, height - 2, startAngle + branchAngle, branchAngle, animate);
	treeRecursive(canvas, branchEnd, height - 2, startAngle - branchAngle, branchAngle, animate);
}


// Recursively draw nested boxes
void drawBoxesRecursive(char canvas[][MAXCOLS], Point center, int height, bool animate)
{
	//Base case: If box is big enough to draw 
	if (height > 1)
	{
		//Draws box
		drawBox(canvas, center, height, animate);
		//recalls function to draw box with height - 2
		drawBoxesRecursive(canvas, center, height - 2, animate);
	}
	else
	{
		return;
	}
}