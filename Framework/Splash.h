/*___________________________________________________________________
|
|	File: splash.h
|
| (C) Copyright 2013 Abonvita Software LLC.
| Licensed under the GX Toolkit License, Version 1.0.
|___________________________________________________________________*/

/*____________________
|
|	Class: SplashScreen
|___________________*/

class SplashScreen : public CWnd
{
// Construction
protected:
	SplashScreen ();

// Attributes:
public:
	CBitmap m_bitmap;

// Operations
public:
	static void EnableSplashScreen (BOOL bEnable = TRUE);
	static void ShowSplashScreen (CWnd* pParentWnd = NULL);
	static BOOL PreTranslateAppMessage (MSG* pMsg);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SplashScreen)
	//}}AFX_VIRTUAL

// Implementation
public:
	~SplashScreen ();
	virtual void PostNcDestroy ();

protected:
	BOOL Create (CWnd* pParentWnd = NULL);
	void HideSplashScreen ();

  static BOOL c_bShowSplashWnd;
	static SplashScreen* c_pSplashWnd;

// Generated message map functions
protected:
	//{{AFX_MSG(SplashScreen)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
