/*____________________________________________________________________
|
| File: dp.h
|
| (C) Copyright 2013 Abonvita Software LLC.
| Licensed under the GX Toolkit License, Version 1.0.
|___________________________________________________________________*/

#ifdef _MAIN_
#define GLOBAL /* */
#else
#define GLOBAL extern
#endif

/*____________________
|                                      
| Include files
|___________________*/
                     
#include <mfc_linkfix.h>
                           
#include <windows.h>
#include <winbase.h>

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <ctype.h>
#include <dos.h>
#include <math.h>
#include <conio.h>
#include <direct.h>
#include <time.h>
#include <mmsystem.h>
#include <assert.h>

#include <iostream>		// include these 4 lines if you need to use C++ stream IO
#include <fstream>
#include <string>
using namespace std;

#include <defines.h>
                                                 
#include <clib.h>
#include <gx_w7.h>
#include <ms_w7.h>
#include <ev_w7.h>
#include <gxt_w7.h>
#include <snd8.h>				
//#include <tts_w7.h>
//#include <sr_w7.h>

#include "..\Framework\version.h"

/*___________________
|
| Defines
|__________________*/

//#define MAX_VRAM_PAGES          16      /* 16 pages of 640x400x256 in 2MB       */
//#define MAX_USER_PAGES          2
//#define MIN_USER_PAGES          2
#define XOR_COLOR               WHITE

#define BEEP {sound(5000);delay(20);nosound();delay(50);}

#define delay Sleep

#define NO_COMBINE  0
#define COMBINE     1

#define CHARACTER_HEIGHT  5

/*___________________
|
| Global variables
|__________________*/

GLOBAL gxFont     *Pgm_system_font;
GLOBAL gxRectangle Pgm_screen;
GLOBAL int         Pgm_num_pages;

GLOBAL char Pgm_debug_str1[256];
GLOBAL char Pgm_debug_str2[256];
GLOBAL char Pgm_debug_str3[256];
GLOBAL char Pgm_debug_str4[256];
GLOBAL char Pgm_debug_str5[256];
GLOBAL char Pgm_debug_str6[256];
GLOBAL char Pgm_debug_str7[256];
GLOBAL char Pgm_debug_str8[256];
GLOBAL char Pgm_debug_str9[256];
GLOBAL char Pgm_debug_str10[256];
GLOBAL char Pgm_debug_str11[256];
GLOBAL char Pgm_debug_str12[256];
GLOBAL char Pgm_debug_str13[256];
GLOBAL char Pgm_debug_str14[256];
GLOBAL char Pgm_debug_str15[256];

#ifdef DEBUG
static char debug_str [256];
#endif
