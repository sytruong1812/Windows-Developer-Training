// EC1Assignment.h : main header file for the EC1ASSIGNMENT application
//

#if !defined(AFX_EC1ASSIGNMENT_H__8AD95085_4957_11D7_BC1F_00B0D048BACE__INCLUDED_)
#define AFX_EC1ASSIGNMENT_H__8AD95085_4957_11D7_BC1F_00B0D048BACE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CEC1AssignmentApp:
// See EC1Assignment.cpp for the implementation of this class
//

class CEC1AssignmentApp : public CWinApp
{
public:
	CEC1AssignmentApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEC1AssignmentApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CEC1AssignmentApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EC1ASSIGNMENT_H__8AD95085_4957_11D7_BC1F_00B0D048BACE__INCLUDED_)
