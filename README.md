# text-art
This program allows you to draw, edit, save/load, and animate ASCII text art in the Windows terminal. There are also multiple features for drawing pre-made shapes/designs. This program utilizes linked lists to manage the list of canvases, the undo list, and the redo list.

## Concepts
Linked List Manipulation, Two-Dimensional Arrays, File Input/Output, Header File Creation/Implementation, String Manipulation, Array Manipulation, Output Terminal Manipulation 

## DISCLAIMER
**This program is only compatible with Windows machines since it utilizes windows-specific header files.**

## NOTE
**The SavedFiles folder is necessary for loading and saving text art. The folder already has some art included, which can be loaded into the canvas and freely edited. Also make sure you have all of the necessary .cpp files in the folder/solution. To utilize loading and saving, download and extract the .zip file and open the .sln file in Visual Studio. This program was written and tested using Visual Studio.**

## Menu Navigation 
Navigate the menu by inputting the highlighted letter of your desired feature. Prompts will follow to give further instruction once a choice has been made. The Draw menu provides extra features for drawing and filling designs.

## Main Menu Features
### Edit
Allows user to edit the canvas by moving the cursor around with the arrow keys and entering characters. Editing continues until the ESC key is pressed. 

### Move
Shifts contents of the canvas by a specified number of rows and columns. ***Rows: positive numbers shift downward, negative numbers shift upward. Columns: positive numbers shift right, negative numbers shift left.***

### Replace
Replaces all instances of a character in the canvas.

### Draw
Opens the secondary "Draw" menu which houses several functions for drawing. 

### Clear
Clears the canvas to be empty.

### Load
Gets a file name from the user. This can be either a single canvas or the name of an animation collection from the folder. If the file(s) can be opened for reading, the contents are loaded into the canvas. ***The files are .txt files located in the SavedFiles folder. For animations: enter only the name of the collection (not the "-\#"). Ex.) walk instead of walk-1***

### Save
Gets a file name from the user. If the user decides to save an animation, the current clips will be saved into a collection using a name given by the user. If the file(s) can be opened for writing, the contents of the canvas are written into the file(s). ***The files are .txt files located in the SavedFiles folder. For animations: enter only the name of the collection (not the "-\#"). Ex.) walk instead of walk-1***

### Animate
Toggles animation (N = off, Y = on). If animation is turned on, drawing functions will print characters as they are modified. If animation is turned off, the resulting canvas will be printed all at once. (Only applies to features in the Draw menu)

### Undo
Uses a copy of the canvas to revert the canvas back to its previous state (before the most recent change).

### Redo
Appears once Undo has been used at least once. This will cancel the previous Undo use. The list of canvases saved in Redo is reset once the canvas is modified in any way other than through an Undo.

### Clip
Saves the current canvas to the list of clips. Once there are at least 2 clips, they can be played together as an animation. 

### Play
Appears once at least two clips are stored. This will play the current list of clips in the drawing window repeatedly until ESC is held.

## Draw Menu Features
The Draw menu shares the Animate, Undo, Redo, Clip, and Play features with the Main menu.

### Fill
Fills a section of the canvas with a chosen character.

### Line
Draws a line between two chosen points on the canvas.

### Box
Draws a box into the canvas around a chosen center point. **Size of two or larger works best.**

### Nested Boxes
Draws a series of nested boxes into the canvas around a chosen center point. **Size of four or larger works best.**

### Tree
Draws a fractal tree into the canvas.

### Main Menu
Returns to the main menu screen.
