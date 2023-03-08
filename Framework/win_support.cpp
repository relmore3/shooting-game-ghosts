/*___________________________________________________________________
|
|	File: win_support.cpp
|
|	Description:
|
| Functions:	win_Get_Instance_Handle
|             win_Get_Window_Handle
|             win_Get_Window_Rectangle
|             win_Show_Message_Box
|             win_Abort_Program
|             win_ListBox_Select
|							win_EventQueue_Add
|							win_EventQueue_Remove
|							win_EventQueue_Flush
|							win_CallbackQueue_Add
|							win_CallbackQueue_Flush
|
| (C) Copyright 2013 Abonvita Software LLC.
| Licensed under the GX Toolkit License, Version 1.0.
|___________________________________________________________________*/

/*____________________
|
| Include files
|___________________*/

#include <first_header.h>
#include "wdp.h"
#include <time.h>
#include "resource.h"
#include "listbox.h"

#include "CMainFrame.h"
#include "CMainApp.h"

#include "win_support.h"

/*____________________
|
| Global variables
|___________________*/

extern CMainApp    The_application;
extern CMainFrame *The_window;

/*___________________________________________________________________
|
|	Function: win_Get_Instance_Handle
| 
|	Input: Called from ____
| Output: Returns instance handle.
|___________________________________________________________________*/

HINSTANCE win_Get_Instance_Handle ()
{
  return (AfxGetInstanceHandle());
}

/*___________________________________________________________________
|
|	Function: win_Get_Window_Handle
| 
|	Input: Called from ____
| Output: Returns window handle.
|___________________________________________________________________*/

HWND win_Get_Window_Handle (void)
{
  return (The_window->Get_Window_Handle ());
}

/*___________________________________________________________________
|
|	Function: win_Get_Window_Rectangle
| 
|	Input: Called from ____
| Output: Returns screen-relative coordinates of the application window.
|___________________________________________________________________*/

void win_Get_Window_Rectangle (int *left, int *top, int *right, int *bottom)
{
  The_window->Get_Window_Rectangle (left, top, right, bottom);
}

/*___________________________________________________________________
|
|	Function: win_Show_Message_Box
| 
|	Input: Called from ____
| Output: Opens a message box and waits for user to close it.
|___________________________________________________________________*/

void win_Show_Message_Box (char *str)
{
  if (str)
    AfxMessageBox (str);
}

/*___________________________________________________________________
|
|	Function: win_Abort_Program
| 
|	Input: Called from ____
| Output: Aborts program abnormally, optionally prints a string to a 
|   message box.
|___________________________________________________________________*/

void win_Abort_Program (char *str)
{
	// Notify the MFC window thread to close down
  The_window->Abort_Program (str);
	// Exit this worker thread immediately
  _endthreadex (0);
}

/*___________________________________________________________________
|
|	Function: win_ListBox_Select
| 
|	Input: Called from ____
| Output: Presents an MFC dialog box to allow user to select from a 
|         listbox.
|
|					Returns selected line in listbox or -1 if user selects none.
|___________________________________________________________________*/

int win_ListBox_Select (
  char *title,
  char **lines,       // array of string descriptions
  int    num_lines )  // # in array
{
	int i, selection;

	// If only 1 line available, return index to this line
	if (num_lines == 1)
		selection = 0;
	else {
		// Allow user to select from a MFC dialog box
		CArray <CString, CString> list;
	  for (i=0; i<num_lines; i++) 		
		  list.Add (lines[i]);
    ListBox dialog (NULL, title);
	  dialog.SetContents (&list);
//ShowCursor (TRUE);
	  if (dialog.DoModal () != IDOK)
		  selection = -1;
		else
			selection = dialog.GetSelection ();
  }

  return (selection);
}

/*___________________________________________________________________
|
|	Function: win_EventQueue_Add
| 
|	Input: Called from ____
| Output: Adds an entry to the event queue.
|___________________________________________________________________*/

void win_EventQueue_Add (EventQueueEntry *qentry)
{
	The_window->EventQueue_Add (qentry);
}

/*___________________________________________________________________
|
|	Function: win_EventQueue_Remove
| 
|	Input: Called from ____
| Output: Removes an entry from the event queue.
|___________________________________________________________________*/

int win_EventQueue_Remove (EventQueueEntry *qentry)
{
  return (The_window->EventQueue_Remove (qentry));
}

/*___________________________________________________________________
|
|	Function: win_EventQueue_Flush
| 
|	Input: Called from ____
| Output: Flushes the event queue.
|___________________________________________________________________*/

void win_EventQueue_Flush (unsigned event_type_mask)
{
  The_window->EventQueue_Flush (event_type_mask);
}

/*___________________________________________________________________
|
|	Function: win_CallbackQueue_Add
| 
|	Input: Called from ____
| Output: Adds an entry to the callback queue.
|___________________________________________________________________*/

void win_CallbackQueue_Add (void (*callback) (void *params), void *params, unsigned size_params)
{
	The_window->CallbackQueue_Add (callback, params, size_params);
}

/*___________________________________________________________________
|
|	Function: win_CallbackQueue_Flush
| 
|	Input: Called from ____
| Output: Flushes the callback queue.
|___________________________________________________________________*/

void win_CallbackQueue_Flush (void)
{
  The_window->CallbackQueue_Flush ();
}
