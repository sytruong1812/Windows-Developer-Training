#if !defined(AFX_ABSTRACT_H__264EF981_4D65_11D7_BC25_00B0D048BACE__INCLUDED_)
#define AFX_ABSTRACT_H__264EF981_4D65_11D7_BC25_00B0D048BACE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Abstract.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAbstract dialog

class CAbstract : public CDialog
{
// Construction
public:
	CAbstract(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAbstract)
	enum { IDD = IDD_ABSTRACT };
	CRichEditCtrl	m_text;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAbstract)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAbstract)
	afx_msg void OnClose();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ABSTRACT_H__264EF981_4D65_11D7_BC25_00B0D048BACE__INCLUDED_)
