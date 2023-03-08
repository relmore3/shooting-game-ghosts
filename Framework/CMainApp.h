/*___________________________________________________________________
|
|	File: CMainApp.h
|
| (C) Copyright 2013 Abonvita Software LLC.
| Licensed under the GX Toolkit License, Version 1.0.
|___________________________________________________________________*/

/*____________________
|
|	Class: CMainApp
|___________________*/

class CMainApp : public CWinApp
{
public:
	BOOL InitInstance ();
	int  Run ();
  BOOL PreTranslateMessage (MSG *pMsg);
	int  ExitInstance ();

protected:
	//{{AFX_MSG(CMainApp)
  //}}AFX_MSG
	DECLARE_MESSAGE_MAP ()
};
