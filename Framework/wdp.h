/*___________________________________________________________________
|
|	File: wdp.h
|
| (C) Copyright 2013 Abonvita Software LLC.
| Licensed under the GX Toolkit License, Version 1.0.
|___________________________________________________________________*/

/*____________________
|
| Include files
|___________________*/

#include <AfxWin.h>
#include <AfxExt.h>
#include <AfxTempl.h>
#include <AfxMt.h>
#include <mmsystem.h>
//#include <iostream>   // include these 3 to use C++ I/O
//#include <fstream>
//using namespace std;
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <process.h>

#include <defines.h>
#include <clib.h>
#include <queue.h>

/*____________________
|
| Libraries
|___________________*/

// These are Microsoft libraries
#pragma comment (lib, "version.lib")  // For routines needed by getdxver.cpp
#pragma comment (lib, "winmm.lib")
#pragma comment (lib, "dxguid.lib")
#pragma comment (lib, "d3d9.lib")     // needed for dx9 compatibility
#pragma comment (lib, "d3dx9.lib")    // needed for dx9 compatibility
#pragma comment (lib, "dsound.lib")   
#pragma comment (lib, "dinput8.lib")  // needed for dx8-9 compatibility

// These are custom (non-Microsoft) libraries
#pragma comment (lib, "clib.lib")
#pragma comment (lib, "queue.lib")
#pragma comment (lib, "dx9.lib")      // needed for dx9 compatibility
#pragma comment (lib, "gx_w7.lib")
#pragma comment (lib, "ms_w7.lib")
#pragma comment (lib, "ev_w7.lib")   
#pragma comment (lib, "gxt_w7.lib")
#pragma comment (lib, "snd8.lib")
//#pragma comment (lib, "tts_w7.lib") // needed for text-to-speech (Microsoft SAPI 5.1 must be installed)

/*____________________
|
| Constants
|___________________*/

// Enables splash screen - in framework.rc define the filename for the splash screen
#define SPLASH_SCREEN     
// Time (in milliseconds) for splash screen to display
#define SPLASH_TIME 2000
// define this symbol to play a splash sound "splash_sound.wav"
//#define SPLASH_SOUND
