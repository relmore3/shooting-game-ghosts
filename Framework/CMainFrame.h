/*___________________________________________________________________
|
|	File: CMainFrame.h
|
| (C) Copyright 2013 Abonvita Software LLC.
| Licensed under the GX Toolkit License, Version 1.0.
|___________________________________________________________________*/

/*____________________
|
|	Include files
|___________________*/
                                                          
#include <events.h>

/*____________________
|
|	Constants
|___________________*/

#define USER_START_PROGRAM_THREAD_MSG (WM_USER + 10)
#define USER_CALLBACK_MSG             (WM_USER + 11)
                                    
/*____________________
|
|	Class: CMainFrame
|___________________*/

class CMainFrame : public CWnd
{                                                      
public:
	CMainFrame ();	// Constructor
  ~CMainFrame (); // Destructor                               
  BOOL Init (const CString& title, int icon);

  HWND Get_Window_Handle ();                       
  HINSTANCE Get_Instance_Handle ();
  void Get_Window_Rectangle (int *left, int *top, int *right, int *bottom);
  void Show_Message_Box (char *str);
  void Abort_Program (char *str);

  void EventQueue_Add    (EventQueueEntry *qentry);
  int  EventQueue_Remove (EventQueueEntry *qentry);
  void EventQueue_Flush  (unsigned event_type_mask);

  void CallbackQueue_Add     (void (*callback) (void *params), void *params, unsigned size_params);
  void CallbackQueue_Flush   (void);
  void CallbackQueue_Process (void);
  
  void Start_Program_Thread (void);

protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int  OnCreate (LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose ();
	afx_msg void OnActivateApp (BOOL bActive, DWORD hTask);
	afx_msg void OnKeyDown (UINT nChar, UINT nRepCnt, UINT nFlags);
  afx_msg void OnSysKeyDown (UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnChar (UINT nChar, UINT nRepCnt, UINT nFlags);

  //}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
  void On_Key_Press (int event_keycode);
  void Show_MFC_Cursor ();
  void Hide_MFC_Cursor ();

	// Event queue
  Queue event_queue;
  CRITICAL_SECTION event_critsection;
  int generate_keypress_events;
  void *preferences;

  // Callback queue
  Queue	callback_queue;
  CRITICAL_SECTION callback_critsection;

	// Program thread object
	HANDLE program_thread_handle;
  // cursor state - MFC cursor only
  int cursor_state;
  // abort string
  char abort_string [256];
};
