/*___________________________________________________________________
|
|	File: CMainApp.cpp
|
|	Description: CMainApp class implementation file.
|
| Functions:	InitInstance
|							Run
|             PreTranslateMessage
|							ExitInstance
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
#include "resource.h"
#include "getdxver.h"
#include "version.h"
#include "CMainFrame.h"

#include "CMainApp.h"
#include "splash.h"

/*____________________
|
|	Constants
|___________________*/

// All messages handled by message handlers
BEGIN_MESSAGE_MAP(CMainApp, CWinApp)
	//{{AFX_MSG_MAP(CMainApp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/*____________________
|
|	Global variables
|___________________*/

// The application object
CMainApp The_application;
// The application window
CMainFrame *The_window;

/*___________________________________________________________________
|
|	Function: InitInstance
| 
|	Input: Called by MFC when application starts.
| Output: Creates application window and makes it visible.
|___________________________________________________________________*/

BOOL CMainApp::InitInstance (void)
{
#ifdef SPLASH_SCREEN
  // Enable splash screen
  CCommandLineInfo cmdInfo;
	ParseCommandLine (cmdInfo);
 	SplashScreen::EnableSplashScreen (cmdInfo.m_bShowSplash);
#endif

  DWORD dwDxVersion;
  char str[80];
  BOOL initialized = FALSE;

  CoInitialize (0); 

  // Check for the correct version of DX or greater
  dwDxVersion = GetDXVersion ();
  if (dwDxVersion < DIRECTX_VERSION_REQUIRED) {
    sprintf (str, "This program requires DirectX %s or greater.", DIRECTX_VERSION_STR);
    MessageBox (NULL, str, "Error", MB_OK | MB_ICONSTOP);
  }
  else {
    // Instantiate and create application window.
	  The_window = new CMainFrame;
    if (The_window) {
      sprintf (str, "%s %s", APPLICATION_NAME, APPLICATION_VERSION);
      if (The_window->Init (str, IDI_ICON)) {
			  // Tell base class about the window created for this application
			  m_pMainWnd = The_window;
			  // Make sure m_pMainWnd is not NULL
			  DEBUG_ASSERT (m_pMainWnd);
			  // Display the application window
			  m_pMainWnd->ShowWindow (SW_SHOWNORMAL);
			  m_pMainWnd->UpdateWindow ();
			  // Turn off cursor
        ShowCursor (FALSE);
        // Call base class version of this function
			  initialized = CWinApp::InitInstance ();
      }
    }
  }

  return (initialized);
}

/*___________________________________________________________________
|
|	Function: Run
| 
|	Input: Called by MFC 
| Output: Starts processing msg loop for window.
|___________________________________________________________________*/

int CMainApp::Run (void)
{
  return (CWinApp::Run ());
}

/*___________________________________________________________________
|
|	Function: PreTranslateMessage
| 
|	Input: Called by MFC 
| Output: Used to translate windows messages before they are dispatched
|   to the TranslateMesage() and DispatchMessage() Windows functions.
|___________________________________________________________________*/

BOOL CMainApp::PreTranslateMessage (MSG *pMsg)
{
  switch (pMsg->message) {
    case USER_START_PROGRAM_THREAD_MSG:
      The_window->Start_Program_Thread ();
      return (1);
    case USER_CALLBACK_MSG:
      The_window->CallbackQueue_Process ();
      return (1);
    default: 
      return (CWinApp::PreTranslateMessage (pMsg));
  }
}                                                                  

/*___________________________________________________________________
|
|	Function: ExitInstance
| 
|	Input: Called by MFC when application ends
| Output: Performs any cleanup before application terminates.
|___________________________________________________________________*/

int CMainApp::ExitInstance (void)
{
  // Delete the application window
	delete The_window;

//#ifdef _DEBUG
//  system ("c:\\dev2012w7\\bin\\debug_file_reader.exe");
//#endif

  CoUninitialize ();

	return (CWinApp::ExitInstance ());
}
