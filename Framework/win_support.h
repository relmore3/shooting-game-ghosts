/*____________________________________________________________________
|
| File: win_support.h
|
| (C) Copyright 2013 Abonvita Software LLC.
| Licensed under the GX Toolkit License, Version 1.0.
|___________________________________________________________________*/

#ifndef _WIN_SUPPORT_H_
#define _WIN_SUPPORT_H_

#include <events.h>

/*___________________
|
| Functions
|__________________*/

// Returns the application instance handle
HINSTANCE win_Get_Instance_Handle ();

// Returns the window handle
HWND win_Get_Window_Handle ();

// Returns screen-relative coordinates of the application window
void win_Get_Window_Rectangle (int *left, int *top, int *right, int *bottom);

// Displays a popup message box with string
void win_Show_Message_Box (char *str);
                                                          
// Aborts program abnormally, optionally prints a string to a message box
void win_Abort_Program (char *str);

// Returns selected entry, displaying a dialog box if more than one for user to choose from
int win_ListBox_Select (
  char  *title,       // title of dialog
  char **lines,       // array of string descriptions
  int    num_lines ); // # in array

// Adds an entry to the event queue
void win_EventQueue_Add (EventQueueEntry *qentry);

// Removes an entry from the event queue
int win_EventQueue_Remove (EventQueueEntry *qentry);

// Flushes the event queue
void win_EventQueue_Flush (unsigned event_type_mask);

// Adds an entry to the callback queue
void win_CallbackQueue_Add (void (*callback) (void *params), void *params, unsigned size_params);

// Flushes the callback queue
void win_CallbackQueue_Flush (void);

#endif
