/*___________________________________________________________________
|
|	File: listbox.h
|
| (C) Copyright 2013 Abonvita Software LLC.
| Licensed under the GX Toolkit License, Version 1.0.
|___________________________________________________________________*/

/*____________________
|
|	Class: ListBox
|___________________*/

class ListBox : public CDialog
{
// Construction
public:
  ListBox(CWnd* pParent /*=NULL*/, char *title);
// -  ListBox(CWnd* pParent = NULL);   // standard constructor
	BOOL SetContents( CArray<CString, CString>* );
	int GetSelection()  { return selection; }
// Dialog Data
	// {{AFX_DATA(ListBox)
	enum { IDD = IDD_LISTBOXDIALOG };
	CListBox	driverlist;
	CString	  m_title;
	// }}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	// {{AFX_VIRTUAL(ListBox)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	// }}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	// {{AFX_MSG(ListBox)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnDblclkModelist();
//  afx_msg void OnSetFocus (CWnd *pOldWnd);
  // }}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CArray<CString, CString>* list;
	int selection;
};
