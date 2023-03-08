/*___________________________________________________________________
|
|	File: splash.cpp
|
|	Description: SplashScreen class implementation file.
|
| Functions:  SplashScreen
|             ~SplashScreen
|             EnableSplashScreen
|             ShowSplashScreen
|             PreTranslateAppMessage
|             Create
|             HideSplashScreen
|             PostNcDestroy
|             OnCreate
|             OnPaint
|             OnTimer
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
#include "splash.h"

/*____________________
|
|	Constants
|___________________*/

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(SplashScreen, CWnd)
	//{{AFX_MSG_MAP(SplashScreen)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/*____________________
|
|	Global variables
|___________________*/

BOOL SplashScreen::c_bShowSplashWnd;
SplashScreen* SplashScreen::c_pSplashWnd;

/*___________________________________________________________________
|
|	Function: SplashScreen (constructor)
|___________________________________________________________________*/

SplashScreen::SplashScreen ()
{
}

/*___________________________________________________________________
|
|	Function: ~SplashScreen (destructor)
|___________________________________________________________________*/

SplashScreen::~SplashScreen ()
{
	// Clear the static window pointer.
	DEBUG_ASSERT (c_pSplashWnd == this);
	c_pSplashWnd = NULL;
}

/*___________________________________________________________________
|
|	Function: EnableSplashScreen
|___________________________________________________________________*/

void SplashScreen::EnableSplashScreen (BOOL bEnable /*= TRUE*/)
{
	c_bShowSplashWnd = bEnable;
}

/*___________________________________________________________________
|
|	Function: ShowSplashScreen
|___________________________________________________________________*/

void SplashScreen::ShowSplashScreen (CWnd* pParentWnd /*= NULL*/)
{
	if (!c_bShowSplashWnd || c_pSplashWnd != NULL)
		return;

	// Allocate a new splash screen, and create the window.
	c_pSplashWnd = new SplashScreen;
	if (!c_pSplashWnd->Create(pParentWnd))
		delete c_pSplashWnd;
	else
		c_pSplashWnd->UpdateWindow();
}

/*___________________________________________________________________
|
|	Function: PreTranslateAppMessage
|___________________________________________________________________*/

BOOL SplashScreen::PreTranslateAppMessage (MSG* pMsg)
{
	if (c_pSplashWnd == NULL)
		return FALSE;

	// If we get a keyboard or mouse message, hide the splash screen.
	if (pMsg->message == WM_KEYDOWN ||
	    pMsg->message == WM_SYSKEYDOWN ||
	    pMsg->message == WM_LBUTTONDOWN ||
	    pMsg->message == WM_RBUTTONDOWN ||
	    pMsg->message == WM_MBUTTONDOWN ||
	    pMsg->message == WM_NCLBUTTONDOWN ||
	    pMsg->message == WM_NCRBUTTONDOWN ||
	    pMsg->message == WM_NCMBUTTONDOWN)
	{
		c_pSplashWnd->HideSplashScreen();
		return TRUE;	// message handled here
	}

	return FALSE;	// message not handled
}

/*___________________________________________________________________
|
|	Function: PreTranslateAppMessage
|___________________________________________________________________*/

BOOL SplashScreen::Create (CWnd* pParentWnd /*= NULL*/)
{
	if (!m_bitmap.LoadBitmap(IDB_SPLASH))
		return FALSE;

	BITMAP bm;
	m_bitmap.GetBitmap(&bm);

	return CreateEx(0,
		AfxRegisterWndClass(0, AfxGetApp()->LoadStandardCursor(IDC_ARROW)),
		NULL, WS_POPUP | WS_VISIBLE, 0, 0, bm.bmWidth, bm.bmHeight, pParentWnd->GetSafeHwnd(), NULL);
}

/*___________________________________________________________________
|
|	Function: HideSplashScreen
|___________________________________________________________________*/

void SplashScreen::HideSplashScreen ()
{
	// Destroy the window, and update the mainframe.
	DestroyWindow();
	AfxGetMainWnd()->UpdateWindow();
}

/*___________________________________________________________________
|
|	Function: PostNcDestroy
|___________________________________________________________________*/

void SplashScreen::PostNcDestroy ()
{
	// Free the C++ class.
	delete this;
}

/*___________________________________________________________________
|
|	Function: OnCreate
|___________________________________________________________________*/

int SplashScreen::OnCreate (LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Center the window.
	CenterWindow();

	// Set a timer to destroy the splash screen.
	SetTimer (1, SPLASH_TIME, NULL);

	return 0;
}

/*___________________________________________________________________
|
|	Function: OnPaint
|___________________________________________________________________*/

void SplashScreen::OnPaint ()
{
	CPaintDC dc(this);

	CDC dcImage;
	if (!dcImage.CreateCompatibleDC(&dc))
		return;

	BITMAP bm;
	m_bitmap.GetBitmap(&bm);

	// Paint the image.
	CBitmap* pOldBitmap = dcImage.SelectObject(&m_bitmap);
	dc.BitBlt(0, 0, bm.bmWidth, bm.bmHeight, &dcImage, 0, 0, SRCCOPY);
	dcImage.SelectObject(pOldBitmap);
}

/*___________________________________________________________________
|
|	Function: OnTimer
|___________________________________________________________________*/

void SplashScreen::OnTimer (UINT nIDEvent)
{
	// Destroy the splash screen window.
	HideSplashScreen();
}
