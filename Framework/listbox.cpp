/*___________________________________________________________________
|
|	File: listbox.cpp
|
|	Description: ListBox class implementation file.
|
| Functions:  ListBox
|             DoDataExchange
|             OnInitDialog
|             SetContents
|             OnOK
|             OnDblclkModelist
|             OnSetFocus  
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
#include "listbox.h"

/*____________________
|
|	Constants
|___________________*/

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(ListBox, CDialog)
	//{{AFX_MSG_MAP(ListBox)
	ON_LBN_DBLCLK(IDC_LIST, OnDblclkModelist)
//  ON_LBN_SETFOCUS(IDD_LISTBOXDIALOG, OnSetFocus)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/*___________________________________________________________________
|
|	Function: ListBox (constructor)
|___________________________________________________________________*/

ListBox::ListBox(CWnd* pParent /*=NULL*/, char *title)
	: CDialog(ListBox::IDD, pParent)
{
	selection=0;
	//{{AFX_DATA_INIT(ListBox)
  m_title = title;
	//}}AFX_DATA_INIT
}

/*___________________________________________________________________
|
|	Function: DoDataExchange
|___________________________________________________________________*/

void ListBox::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ListBox)
	DDX_Control(pDX, IDC_LIST, driverlist);
	DDX_Text(pDX, IDC_TITLE, m_title);
	//}}AFX_DATA_MAP
}

/*___________________________________________________________________
|
|	Function: OnInitDialog
|___________________________________________________________________*/

BOOL ListBox::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	int size=(int)list->GetSize();
	for (int i=0;i<size;i++)
		driverlist.InsertString( i, list->GetAt(i) );
	driverlist.SetCurSel(0);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/*___________________________________________________________________
|
|	Function: DoDataExchange
|___________________________________________________________________*/

BOOL ListBox::SetContents( CArray<CString, CString>* drivers)
{
	list=drivers;

	return TRUE;
}

/*___________________________________________________________________
|
|	Function: OnOK
|___________________________________________________________________*/

void ListBox::OnOK() 
{
	selection=driverlist.GetCurSel();
	
	CDialog::OnOK();
}

/*___________________________________________________________________
|
|	Function: OnDblclkModelist
|___________________________________________________________________*/

void ListBox::OnDblclkModelist() 
{
	OnOK();
}

/*___________________________________________________________________
|
|	Function: OnSetFocus
|___________________________________________________________________*/

/*
void ListBox::OnSetFocus (CWnd *pOldWnd)
{
//  ShowCursor (TRUE);
//  CDialog::OnSetFocus (pOldWnd);
}
*/
