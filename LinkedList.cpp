#include <iostream>
#include <Windows.h>
#include "Definitions.h"
using namespace std;

Node* newCanvas()
{
	//temp node to hold the new node
	Node* temp = new Node;

	//initializes the canvas in the new node with spaces
	initCanvas(temp->item);

	//initializes the node's next value with NULL
	temp->next = NULL;

	//returns the new node
	return temp;
};


Node* newCanvas(Node* oldNode)
{
	//temp node to hold the new node
	Node* temp = new Node;

	//copies canvas from old node to new node 
	copyCanvas(temp->item, oldNode->item);

	//initializes the node's pointer as Null
	temp->next = NULL;

	//returns the new node
	return temp;
};


void play(List& clips)
{
	//checks if there are at least 2 clips in the list
	if (clips.count < 2)
		return;

	// loops as long as the ESCAPE key is not currently being pressed
	while (!(GetKeyState(VK_ESCAPE) & 0x8000))
	{
		playRecursive(clips.head, clips.count);
	}
}


void playRecursive(Node* head, int count)
{
	//checks if the end of the list has been reached
	if (count == 0)
		return;

	//moves down the list of clips
	playRecursive(head->next, count - 1);

	//starts displaying clips once the end of the list has been reached, 
	//so they appear in the correct order
	displayCanvas(head->item);

	//places the cursor where the clip number will be displayed
	gotoxy(MAXROWS + 1, CLIPNUMPOS);

	//displays the clip number
	printf("%-4d", count);

	// Pause for 100 milliseconds to slow down animation
	Sleep(100);
}


void addUndoState(List& undoList, List& redoList, Node*& current)
{
	//adds a copy of the current canvas to the undoList
	addNode(undoList, newCanvas(current));

	//deletes the redo list since you can't redo once a change has been made
	deleteList(redoList);
}


void restore(List& undoList, List& redoList, Node*& current)
{
	//adds a copy of the current canvas to the redoList
	addNode(redoList, newCanvas(current));

	//stores the first node from the undoList to the current canvas
	current = removeNode(undoList);
}


void addNode(List& list, Node* nodeToAdd)
{
	//temp node to hold the contents of the list
	Node* temp = list.head;

	//adds the node to the beginning of list
	list.head = nodeToAdd;

	//puts the rest of the list back in after the new node
	list.head->next = temp;

	//increases list count
	list.count++;
}


Node* removeNode(List& list)
{
	//checks to make sure the list isn't empty
	if (list.head != NULL)
	{
		//sets the first node of the list to be the removed node
		Node* removedNode = list.head;

		//moves the rest of the list to the head
		list.head = list.head->next;

		//decreases list count
		list.count--;

		return removedNode;
	}
	//returns NULL if list is empty
	return NULL;
}


void deleteList(List& list)
{
	//Runs until head equals null
	while (list.head != NULL)
	{
		// Creates a temporary Node equal to the pointer of the "last" node,
		// deletes said node. Repeats until only head node remains
		Node* temp = list.head->next;
		delete list.head;
		list.head = temp;
	}
	list.count = 0;
}


bool loadClips(List& clips, char filename[])
{
	//resets the clips linked list as a new list will be created 
	deleteList(clips);

	//holds the properly formatted clip file name
	char clipName[FILENAMESIZE];
	int count = 1;

	//holds the clip being currently loaded
	Node* currentClip;

	//temporary node to store canvas of each clip to clips list
	currentClip = newCanvas();

	snprintf(clipName, FILENAMESIZE, "%s%s%d%s", filename, "-", count, ".txt");

	// Continuously loads clips while loadCanvas returns true
	while (loadCanvas(currentClip->item, clipName))
	{
		addNode(clips, currentClip);
		currentClip = currentClip->next;
		count++;
		snprintf(clipName, FILENAMESIZE, "%s%s%d%s", filename, "-", count, ".txt");
		currentClip = newCanvas();
	}
	
	//returns true if at least one clip was loaded
	if (count > 1)
		return true;
	
	//returns false if no clips  were loaded
	return false;
}


bool saveClips(List& clips, char filename[])
{
	//holds the properly formatted clip file name
	char clipName[FILENAMESIZE];

	//holds the clip being currently saved
	Node* currentClip;

	//sets the fist clip in the list as the current clip
	currentClip = clips.head;
	
	//continuously loads clips until the end of the list is reached
	for (int i = clips.count; i >= 1; i--)
	{
		//creates the clip name with proper formatting
		snprintf(clipName, FILENAMESIZE, "%s%s%d%s", filename, "-", i, ".txt");
		
		// Exits the function if saveCanvas is unable to save the canvas
		if (!saveCanvas(currentClip->item, clipName))
			return false;

		// Changes the clip to be saved to the next clip in the list 
		currentClip = currentClip->next;
	}
	//returns true if all clips were able to be saved
	return true;
}