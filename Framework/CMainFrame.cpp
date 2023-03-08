/*___________________________________________________________________
|
|	File: CMainFrame.cpp
|
|	Description:
|
| Functions:	CMainFrame::CMainFrame (Constructor)            
|             CMainFrame::~CMainFrame (Destructor)
|							CMainFrame::Init
|
|             CMainFrame::Get_Window_Handle
|             CMainFrame::Get_Instance_Handle
|             CMainFrame::Get_Window_Rectangle
|             CMainFrame::Show_Message_Box
|             CMainFrame::Abort_Program
|
|							CMainFrame::EventQueue_Add
|             CMainFrame::EventQueue_Remove                      
|             CMainFrame::EventQueue_Flush
|             CMainFrame::CallbackQueue_Add
|             CMainFrame::CallbackQueue_Flush
|             CMainFrame::CallbackQueue_Process
|
|             CMainFrame::Start_Program_Thread
|              Program_Thread
|
|							CMainFrame::OnCreate
|							CMainFrame::OnClose
|							CMainFrame::OnActivateApp
|							CMainFrame::OnKeyDown                
|             CMainFrame::OnSysKeyDown
|							CMainFrame::OnChar   
|              CMainFrame::On_Key_Press
|
|             CMainFrame::Show_MFC_Cursor
|             CMainFrame::Hide_MFC_Cursor
|
| (C) Copyright 2013 Abonvita Software LLC.
| Licensed under the GX Toolkit License, Version 1.0.
|___________________________________________________________________*/

//#define DEBUG_EVENTS

/*____________________
|
| Include files
|___________________*/

#include <first_header.h>
#include "wdp.h"
#include <time.h>
#include "resource.h"
#include "version.h"

#include "..\Application\main.h"

#include "CMainFrame.h"
#include "splash.h"

/*____________________
|
| Type definitions
|___________________*/

struct CallbackQueueEntry {
  void (*callback) (void *params); 
  void *params;
};

/*____________________
|
| Function prototypes
|___________________*/

UINT Program_Thread (LPVOID pParam);

/*____________________
|
|	Constants
|___________________*/

// List all messages that will be handled by message handlers
BEGIN_MESSAGE_MAP (CMainFrame, CWnd)
	ON_WM_CONTEXTMENU()
//	{{AFX_MSG_MAP(CWnd)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_WM_ERASEBKGND()
	ON_WM_ACTIVATEAPP()
	ON_WM_KEYDOWN()
  ON_WM_SYSKEYDOWN()
	ON_WM_CHAR()
//  }}AFX_MSG_MAP 
END_MESSAGE_MAP()

#define SIZE_EVENT_QUEUE  512
#define SIZE_CALLBACK_QUEUE 64

/*____________________
|
|	Global variables
|___________________*/

static volatile byte program_initialized = FALSE;

/*___________________________________________________________________
|
|	Function: CMainFrame::CMainFrame (Constructor)
| 
|	Input: 
| Output: Initializes variables in CMainFrame class.
|___________________________________________________________________*/

CMainFrame::CMainFrame ()
{
  // Init event queue
  InitializeCriticalSection (&event_critsection);
  event_queue = queue_Init (SIZE_EVENT_QUEUE, sizeof (EventQueueEntry));
  generate_keypress_events = FALSE;
  preferences = NULL;

  // Init callback queue
  InitializeCriticalSection (&callback_critsection);
  callback_queue = queue_Init (SIZE_CALLBACK_QUEUE, sizeof (CallbackQueueEntry));

  // Init cursor state (visible by default)
  cursor_state = 1;
  // Init abort string to empty
  abort_string[0] = 0;
}

/*___________________________________________________________________
|
|	Function: CMainFrame::~CMainFrame (Destructor)
| 
|	Input: 
| Output: Frees variables in CMainFrame class.
|___________________________________________________________________*/

CMainFrame::~CMainFrame ()
{
  // Free callback queue
  queue_Free (callback_queue);
  DeleteCriticalSection (&callback_critsection);

  // Free event queue
  queue_Free (event_queue);
  DeleteCriticalSection (&event_critsection);

  // Print abort string?
  if (abort_string[0]) {
    for (int i=0; i<10; i++)   // shouldn't have to do this so many times!
      ShowCursor (TRUE);        
    Show_Message_Box (abort_string);
  }
}

/*___________________________________________________________________
|
|	Function: CMainFrame::Init
| 
|	Input: Called from CMainApp::InitInstance() 
| Output: Creates main application window, which causes a WM_CREATE
|	  msg to be posted to msg queue.
|___________________________________________________________________*/

BOOL CMainFrame::Init (const CString& title, int icon)
{
	CString sClassName;
  int created = 0;

/*
  CString filename;
  TCHAR szFilters [] = 
    _T ("Text files (*.txt)|*.txt|All files (*.*)|*.*||");

  CFileDialog dlg (TRUE, _T ("txt"), _T ("*.txt"),
    OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters);

  if (dlg.DoModal () == IDOK) {
    filename = dlg.GetPathName ();
  };
*/

  // Get any user preferences
  if (Program_Get_User_Preferences (&preferences)) {
    // Register the window
	  sClassName = AfxRegisterWndClass (CS_HREDRAW | CS_VREDRAW,
																		  LoadCursor(0, IDC_ARROW),
			                                (HBRUSH)(COLOR_WINDOW + 1),
																		  LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(icon)));
    // Hide the cursor
    Hide_MFC_Cursor ();
    // Create the window
	  created = CWnd::CreateEx (WS_EX_TOPMOST, sClassName, title, WS_POPUP | WS_SYSMENU, 0, 0, 1, 1, 0, 0);
  }

	return (created);
}

/*___________________________________________________________________
|
|	Function: CMainFrame::Get_Window_Handle
| 
|	Input: Called from ____
| Output: Returns window handle.
|___________________________________________________________________*/

HWND CMainFrame::Get_Window_Handle ()
{
  return (GetSafeHwnd ());
}

/*___________________________________________________________________
|
|	Function: CMainFrame::Get_Instance_Handle
| 
|	Input: Called from ____
| Output: Returns instance handle.
|___________________________________________________________________*/

HINSTANCE CMainFrame::Get_Instance_Handle ()
{
  return (AfxGetInstanceHandle ());
}

/*___________________________________________________________________
|
|	Function: CMainFrame::Get_Window_Rectangle
| 
|	Input: Called from ____
| Output: Returns screen-relative coordinates of window.
|___________________________________________________________________*/

void CMainFrame::Get_Window_Rectangle (int *left, int *top, int *right, int *bottom)
{
  RECT rect;

  GetWindowRect (&rect);
  *left   = rect.left;
  *top    = rect.top;
  *right  = rect.right;
  *bottom = rect.bottom;
}

/*___________________________________________________________________
|
|	Function: CMainFrame::Show_Message_Box
| 
|	Input: Called from ____
| Output: Opens a message box and waits for user to close it.
|___________________________________________________________________*/

void CMainFrame::Show_Message_Box (char *str)
{
  if (str)
    AfxMessageBox (str);
}

/*___________________________________________________________________
|
|	Function: CMainFrame::Abort_Program
| 
|	Input: Called from ____
| Output: Send close message to abort program and sets string (if any)
|   to write to a MFC message box upon program termination.
|___________________________________________________________________*/

void CMainFrame::Abort_Program (char *str)
{
  // Save string, if any
  if (str) {
    strncpy (abort_string, str, 255);
    abort_string[255] = 0;
  }
  // Send a message to primary thread to indicate program thread is ending
	::PostMessage (m_hWnd, WM_CLOSE, 0, 0);
}

/*___________________________________________________________________
|
|	Function: CMainFrame::EventQueue_Add
| 
|	Output: Adds an entry to the event queue.
|___________________________________________________________________*/

void CMainFrame::EventQueue_Add (EventQueueEntry *qentry)
{
	EnterCriticalSection (&event_critsection);
	queue_Add (event_queue, qentry);
	LeaveCriticalSection (&event_critsection);
}

/*___________________________________________________________________
|
|	Function: CMainFrame::EventQueue_Remove
| 
|	Output: Removes an entry from the event queue.
|___________________________________________________________________*/

int CMainFrame::EventQueue_Remove (EventQueueEntry *qentry)
{
	int event_ready;

  EnterCriticalSection (&event_critsection);
	event_ready = queue_Remove (event_queue, qentry);
	LeaveCriticalSection (&event_critsection);

  return (event_ready);
}

/*___________________________________________________________________
|
|	Function: CMainFrame::EventQueue_Flush
| 
|	Input: Called from ____
| Output: Flushes all events contained in the eventmask from the event queue.
|___________________________________________________________________*/

// Callback function
static int Entry_Is_Type (void *event, void *event_type_mask)
{
  int same;

  if (((EventQueueEntry *)event)->type & *((unsigned *)event_type_mask))
    same = TRUE;
  else
    same = FALSE;

  return (same);
}

void CMainFrame::EventQueue_Flush (unsigned event_type_mask)
{
  EnterCriticalSection (&event_critsection);
	queue_Remove_Selected_Entries (event_queue, &event_type_mask, Entry_Is_Type);
	LeaveCriticalSection (&event_critsection);
}

/*___________________________________________________________________
|
|	Function: CMainFrame::CallbackQueue_Add
| 
|	Output: Adds an entry to the callback queue.
|___________________________________________________________________*/

void CMainFrame::CallbackQueue_Add (void (*callback) (void *params), void *params, unsigned size_params)
{
  int error = FALSE;

  CallbackQueueEntry data;

  // Create a struct for the data
  data.callback = callback;
  data.params   = NULL;
  if (size_params) {
    data.params = (void *) malloc (size_params);
    if (data.params) 
      memcpy (data.params, params, size_params);
    else 
      error = TRUE;
  }

  if (NOT error) {
    // Add this to the callback queue
    EnterCriticalSection (&callback_critsection);
    queue_Add (callback_queue, (byte *)&data);
    LeaveCriticalSection (&callback_critsection);
  	  
    // Send a message to indicate an entry has been entered in the callback queue
	  ::PostMessage (m_hWnd, USER_CALLBACK_MSG, 0, 0);
  }
}

/*___________________________________________________________________
|
|	Function: CMainFrame::CallbackQueue_Flush
| 
|	Input: Called from ____
| Output: Flushes all events contained in the callback queue.
|___________________________________________________________________*/

void CMainFrame::CallbackQueue_Flush (void)
{
  CallbackQueueEntry data;

  EnterCriticalSection (&callback_critsection);
  while (queue_Remove (callback_queue, (byte *)&data))
    if (data.params)
      free (data.params);
	LeaveCriticalSection (&callback_critsection);
}

/*___________________________________________________________________
|
|	Function: CMainFrame::CallbackQueue_Process
| 
|	Input: Called from ____
| Output: Processes an entry in the callback queue, if any.
|___________________________________________________________________*/

void CMainFrame::CallbackQueue_Process (void)
{
  CallbackQueueEntry data;

  EnterCriticalSection (&callback_critsection);
  // If any entry in the queue
  if (queue_Remove (callback_queue, (byte *)&data)) {
    // Call back the user function
    (*data.callback) (data.params);   
    // Free memory for this queue entry
    if (data.params)
      free (data.params);
  }
    
  LeaveCriticalSection (&callback_critsection);
}

/*___________________________________________________________________
|
|	Function: CMainFrame::Start_Program_Thread
| 
|	Input: Called from CMainApp::PreTranslateMessage()
| Output: Starts program thread.
|___________________________________________________________________*/

void CMainFrame::Start_Program_Thread (void)
{
  ShowCursor (FALSE);

  if (Program_Init (preferences, &generate_keypress_events)) { 
    program_initialized = TRUE;
    ShowCursor (FALSE);
    // Start a new thread here to run application program 
		CWinThread *cwt;
  	cwt = AfxBeginThread (Program_Thread, (void *)this);
		program_thread_handle = cwt->m_hThread;
	}
  else 
    // Send a message to primary thread to indicate program thread is ending
	  ::PostMessage (m_hWnd, WM_CLOSE, 0, 0);
}

/*___________________________________________________________________
|
|	Function: Program_Thread
| 
|	Input: Called from OnCreate()
| Output: This function is a seperate worker thread that calls the main
|					application program and upon exit from the application, sets
|					a flag to notify the primary thread that it has finished.
|___________________________________________________________________*/

unsigned Program_Thread (LPVOID pParam)
{            
  ShowCursor (FALSE);
	Program_Run ();

	// Send a message to primary thread to indicate program thread is ending
  PostMessage (((CMainFrame *)pParam)->m_hWnd, WM_CLOSE, 0, 0);

  return (0);                 
}

/*___________________________________________________________________
|
|	Function: CMainFrame::OnCreate
| 
|	Input: Called from MFC msg loop on WM_CREATE msg.
| Output: Returns to MFC 0 on success, else -1 on any error.
|___________________________________________________________________*/

afx_msg int CMainFrame::OnCreate (LPCREATESTRUCT lpCreateStruct)
{
	int rc = 0; // success

#ifdef SPLASH_SCREEN
  // Display a splash screen
#ifdef SPLASH_SOUND
  PlaySound (MAKEINTRESOURCE(IDR_WAVE1), NULL, SND_RESOURCE | SND_ASYNC);
#endif
  SplashScreen::ShowSplashScreen (this);
  Sleep (SPLASH_TIME);
#endif

  ::PostMessage (m_hWnd, USER_START_PROGRAM_THREAD_MSG, 0, 0);

//	CWnd::OnCreate(lpCreateStruct);

  return (rc);
}

/*___________________________________________________________________
|
|	Function: CMainFrame::OnClose
| 
|	Input: Called from MFC msg loop on WM_CLOSE msg.
|___________________________________________________________________*/

afx_msg void CMainFrame::OnClose ()
{
	if (program_initialized) {
	  // Generate a close event for program thread
    static EventQueueEntry qentry;
	  qentry.type = evTYPE_WINDOW_CLOSE;
    EnterCriticalSection (&event_critsection);
    queue_Add (event_queue, &qentry);
    LeaveCriticalSection (&event_critsection);

    // Wait until program thread closes (or 10 seconds max)
		WaitForSingleObject (program_thread_handle, 10*1000);

    // Perform any cleanup
    Program_Free ();
    program_initialized = FALSE;
  }

  CWnd::OnClose ();
}

/*___________________________________________________________________
|
|	Function: CMainFrame::OnActivateApp
| 
|	Input: Called from MFC msg loop.
| Output:
|___________________________________________________________________*/

afx_msg void CMainFrame::OnActivateApp (BOOL bActive, DWORD hTask) 
{
	int window_active;
  static bool first_time = true;

#ifdef DEBUG_EVENTS
  DEBUG_WRITE ("OnActivateApp()")
#endif

  if (bActive AND (GetForegroundWindow () == (CWnd *)this)) {
		window_active = TRUE;
    Hide_MFC_Cursor ();
    // TURN IT OFF!!!!!!!!!!!!!!!
    for (int i=0; i<10; i++)
      ShowCursor (FALSE);
  }
  else { 
    // Ignore first inactive event
    if (first_time) {
#ifdef DEBUG_EVENTS
      DEBUG_WRITE ("Discarding first Inactive window message")
#endif
      first_time = false;
      return;
    }
		window_active = FALSE;
	  Show_MFC_Cursor ();
  }

	// Generate an event for program thread
	static EventQueueEntry qentry;
	if (window_active)
		qentry.type = evTYPE_WINDOW_ACTIVE;
  else 
		qentry.type = evTYPE_WINDOW_INACTIVE;

#ifdef DEBUG_EVENTS
  if (window_active)
    DEBUG_WRITE ("Inside OnActivateApp() - generating evTYPE_WINDOW_ACTIVE")
  else
    DEBUG_WRITE ("Inside OnActivateApp() - generating evTYPE_WINDOW_INACTIVE")
#endif

  EnterCriticalSection (&event_critsection);
  queue_Add (event_queue, &qentry);
  LeaveCriticalSection (&event_critsection);
}

/*___________________________________________________________________
|
|	Function: CMainFrame::OnKeyDown
| 
|	Input: Called from MFC msg loop on WM_KEYDOWN msg.
| Output:
|___________________________________________________________________*/

afx_msg void CMainFrame::OnKeyDown (UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	int i, keycode;
	
	static struct {
		UINT virtual_key;
		int	 event_keycode;
	} xlat [] = {
		{ VK_UP,				evKY_UP_ARROW			},
		{ VK_DOWN,			evKY_DOWN_ARROW		},
		{ VK_LEFT,			evKY_LEFT_ARROW		},
		{ VK_RIGHT,			evKY_RIGHT_ARROW	},
		{ VK_PRIOR,			evKY_PAGE_UP			},
		{ VK_NEXT,			evKY_PAGE_DOWN		},
		{ VK_RETURN,		evKY_ENTER				},
		{ VK_HOME,			evKY_HOME					},
		{ VK_END,				evKY_END					},
		{ VK_INSERT,		evKY_INSERT				},
		{ VK_DELETE,		evKY_DELETE				},
		{ VK_PAUSE,			evKY_PAUSE				},
		{ VK_CONTROL,		evKY_CONTROL			},
		{ VK_SHIFT,			evKY_SHIFT				},
		{ VK_ESCAPE,		evKY_ESC					},
		{ VK_BACK,			evKY_BACKSPACE		},
		{ VK_TAB,				evKY_TAB					},
		{ VK_F1,				evKY_F1						},
		{ VK_F2,				evKY_F2						},
		{ VK_F3,				evKY_F3						},
		{ VK_F4,				evKY_F4						},
		{ VK_F5,				evKY_F5						},
		{ VK_F6,				evKY_F6						},
		{ VK_F7,				evKY_F7						},
		{ VK_F8,				evKY_F8						},
		{ VK_F9,				evKY_F9						},
		{ VK_F11,				evKY_F11					},
		{ VK_F12,				evKY_F12					},
		{ 0,						0									}	// last entry must be 0,0
	};
	
  if (generate_keypress_events) {
    // Is this first time key was pressed (not just being held down)?
    if ((nFlags & BIT_14) == 0) {
      // Look for this virtual key in table
  	  for (i=0, keycode=0; (xlat[i].event_keycode) AND (keycode == 0); i++)
	  	  if (xlat[i].virtual_key == nChar)
		  	  keycode = xlat[i].event_keycode;
      // Is this a recognized key?
      if (keycode) 
        On_Key_Press (keycode);
    }
  }
}

/*___________________________________________________________________
|
|	Function: CMainFrame::OnSysKeyDown
| 
|	Input: Called from MFC msg loop on WM_SYSKEYDOWN msg.
| Output:
|___________________________________________________________________*/

afx_msg void CMainFrame::OnSysKeyDown (UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	int i, keycode;
	
	static struct {
		UINT virtual_key;
		int	 event_keycode;
	} xlat [] = {
		{ VK_MENU, evKY_ALT	},
		{ VK_F10,	 evKY_F10	},
		{ 0,			 0				}	// last entry must be 0,0
	};
	
  if (generate_keypress_events) {
    // Is this first time key was pressed (not just being held down)?
    if ((nFlags & BIT_14) == 0) {
      // Look for this virtual key in table
	    for (i=0, keycode=0; (xlat[i].event_keycode) AND (keycode == 0); i++)
		    if (xlat[i].virtual_key == nChar)
			    keycode = xlat[i].event_keycode;
  	  // Is this a recognized key?
      if (keycode) 
		    On_Key_Press (keycode);
    }
  }
}

/*___________________________________________________________________
|
|	Function: CMainFrame::OnChar
| 
|	Input: Called from MFC msg loop on WM_CHAR msg.
| Output:
|___________________________________________________________________*/

afx_msg void CMainFrame::OnChar (UINT nChar, UINT nRepCnt, UINT nFlags) 
{
  if (generate_keypress_events) {
    // Is this a valid ASCII key?
    if ((nChar >= 32) AND (nChar <= 127)) 
	    On_Key_Press ((int)nChar);
  }

  // Call base class of this function
  CWnd::OnChar (nChar, nRepCnt, nFlags);
}

/*___________________________________________________________________
|
|	Function: CMainFrame::On_Key_Press
| 
|	Input: Receives a event keycode from Windows msg loop.
| Output: Puts the key press in event queue.
|___________________________________________________________________*/

void CMainFrame::On_Key_Press (int event_keycode) 
{
  // Generate an event
  static EventQueueEntry qentry;

  qentry.type		 = evTYPE_KEY_PRESS;
  qentry.keycode = event_keycode;
  EnterCriticalSection (&event_critsection);
  queue_Add (event_queue, &qentry);
  LeaveCriticalSection (&event_critsection);
}

/*___________________________________________________________________
|
|	Function: CMainFrame::Show_MFC_Cursor
| 
|	Input: 
| Output: Makes the MFC mouse cursor visible.
|___________________________________________________________________*/

void CMainFrame::Show_MFC_Cursor ()
{
  while (cursor_state < 1) {  
    ShowCursor (TRUE);        
    cursor_state++;           
  }
}

/*___________________________________________________________________
|
|	Function: CMainFrame::Hide_MFC_Cursor
| 
|	Input: 
| Output: Hides the MFC mouse cursor.
|___________________________________________________________________*/

void CMainFrame::Hide_MFC_Cursor ()
{
  while (cursor_state > 0) { 
    ShowCursor (FALSE);       
    cursor_state--;        
  }
}
