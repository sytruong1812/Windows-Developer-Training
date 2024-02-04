// EC1AssignmentDlg.h : header file
//

#if !defined(AFX_EC1ASSIGNMENTDLG_H__8AD95087_4957_11D7_BC1F_00B0D048BACE__INCLUDED_)
#define AFX_EC1ASSIGNMENTDLG_H__8AD95087_4957_11D7_BC1F_00B0D048BACE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CEC1AssignmentDlg dialog
#include <math.h>

class CEC1AssignmentDlg : public CDialog
{
// Construction
private://member variables
	long m_Prime1 ,m_Prime2 ;
	long m_Undef,m_n;
	long m_e,m_d;

	CString m_inFPName_to_encrypt;
	CString m_inFPName_to_decrypt;

	bool m_key_gen_done_flg;
private://functions
	bool CheckKeyGenStatus();
	void GeneratePrimeNumbers();
	void DumpNotes(CString &str);
	void WriteValues(CString &);
	void SelectE();
	bool IsRelativePrime(float X,float Y);
	void Execute();
	void CalculateD();

	int GetPrime1(long &);
	int GetPrime2(long &);

	long FindPrime(long);

	void Encrypt(CString , CString);
	void Decrypt(CString , CString);
	void D_to_B(long no,long noOfBits, char *ou);
	void ChangeFileExtension(CString &str, char changeTo[10]);
	void GetOnlyProperBits(char *bits);
public:
	CEC1AssignmentDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CEC1AssignmentDlg)
	enum { IDD = IDD_EC1ASSIGNMENT_DIALOG };
	CEdit	m_values;
	CEdit	m_ku;
	CEdit	m_kr;
	CEdit	m_Notes;
	UINT	m_prime1;
	UINT	m_prime2;
	BOOL	m_chk_p1;
	BOOL	m_chk_p2;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEC1AssignmentDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CEC1AssignmentDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnGenKey();
	afx_msg void OnEmptyNotes();
	afx_msg void OnEncrypt();
	afx_msg void OnDecrypt();
	afx_msg void OnCompare();
	afx_msg void OnAbstract();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EC1ASSIGNMENTDLG_H__8AD95087_4957_11D7_BC1F_00B0D048BACE__INCLUDED_)
