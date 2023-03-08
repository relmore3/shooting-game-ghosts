/*___________________________________________________________________
|
|	File: main.h
|
| Note: Program_Init(), Program_Free() and Program_Immediate_Key_Handler()
|   happen on the main window thread of execution.  Program_Run() happens 
|   on a separate worker thread.
|
| (C) Copyright 2013 Abonvita Software LLC.
| Licensed under the GX Toolkit License, Version 1.0.
|___________________________________________________________________*/

int  Program_Get_User_Preferences (void **preferences);
int  Program_Init (void *preferences, int *generate_keypress_events);
void Program_Run ();
void Program_Free ();
void Program_Immediate_Key_Handler (int keycode);
