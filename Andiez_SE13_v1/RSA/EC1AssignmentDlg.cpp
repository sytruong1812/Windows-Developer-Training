// EC1AssignmentDlg.cpp : implementation file
//

#include "stdafx.h"
#include "EC1Assignment.h"
#include "EC1AssignmentDlg.h"
#include "abstract.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEC1AssignmentDlg dialog

CEC1AssignmentDlg::CEC1AssignmentDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEC1AssignmentDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEC1AssignmentDlg)
	m_prime1 = 7;
	m_prime2 = 17;
	m_chk_p1 = TRUE;
	m_chk_p2 = TRUE;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CEC1AssignmentDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEC1AssignmentDlg)
	DDX_Control(pDX, IDC_VALUES, m_values);
	DDX_Control(pDX, IDC_PUBLIC_KEY, m_ku);
	DDX_Control(pDX, IDC_PRIVATE_KEY, m_kr);
	DDX_Control(pDX, IDC_NOTES, m_Notes);
	DDX_Text(pDX, IDC_PRIME1, m_prime1);
	DDX_Text(pDX, IDC_PRIME2, m_prime2);
	DDX_Check(pDX, IDC_CHECK_P1, m_chk_p1);
	DDX_Check(pDX, IDC_CHECK_P2, m_chk_p2);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CEC1AssignmentDlg, CDialog)
	//{{AFX_MSG_MAP(CEC1AssignmentDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_GEN_KEY, OnGenKey)
	ON_BN_CLICKED(IDC_EMPTY_NOTES, OnEmptyNotes)
	ON_BN_CLICKED(IDC_ENCRYPT, OnEncrypt)
	ON_BN_CLICKED(IDC_DECRYPT, OnDecrypt)
	ON_BN_CLICKED(IDC_COMPARE, OnCompare)
	ON_BN_CLICKED(IDC_ABSTRACT, OnAbstract)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEC1AssignmentDlg message handlers

BOOL CEC1AssignmentDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	AfxInitRichEdit();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	

	static CFont fnt;
	fnt.CreatePointFont(90,"Courier New");
	m_Notes.SetFont(&fnt, true);
	m_values.SetFont(&fnt, true);
	// TODO: Add extra initialization here

	m_inFPName_to_encrypt = "\\\\";
	m_inFPName_to_decrypt = "\\\\";
	m_key_gen_done_flg = false;
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CEC1AssignmentDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CEC1AssignmentDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CEC1AssignmentDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CEC1AssignmentDlg::OnGenKey() 
{
	
	Execute();
	m_key_gen_done_flg = true;
	
}


void CEC1AssignmentDlg::DumpNotes(CString &str)
{
	CString winStr;

	m_Notes.GetWindowText(winStr);
	m_Notes.SetSel(winStr.GetLength(),winStr.GetLength());

	winStr = "";
	if(str[0] == 0x0A)
		winStr = 0x0D;

	winStr += str;
	m_Notes.ReplaceSel(winStr);
}

void CEC1AssignmentDlg::WriteValues(CString &str)
{
	CString winStr;

	m_values.GetWindowText(winStr);
	m_values.SetSel(winStr.GetLength(),winStr.GetLength());

	winStr = "";
	if(str[0] == 0x0A)
		winStr = 0x0D;

	winStr += str;
	m_values.ReplaceSel(winStr);
}

void CEC1AssignmentDlg::OnEmptyNotes() 
{
	m_Notes.SetWindowText("");	
}

void CEC1AssignmentDlg::ChangeFileExtension(CString &str, char changeTo[10])
{
	str.MakeReverse();
	
	char ext[5]="ABC";
	for(int i=0;i<4 && i<str.GetLength();i++)
	{
		if(str[i] == '.') break;
		ext[i] = str[i];
	}

	strrev(ext);
	
	str.SetAt(0, changeTo[2]);
	str.SetAt(1, changeTo[1]);
	str.SetAt(2, changeTo[0]);
	str.MakeReverse();
		
}


void CEC1AssignmentDlg::OnCompare() 
{	
	FILE *inFP1, *inFP2;
	CString str;

	DumpNotes(CString("\n"));
	DumpNotes(CString("\n********************************************"));
	DumpNotes(CString("\nComparing text files..."));

	if(m_inFPName_to_encrypt == "\\\\") 
	{
		DumpNotes(CString("\nNote: Select and encrypt a text file first..."));
		return;
	}
	if(m_inFPName_to_decrypt == "\\\\") 
	{
		DumpNotes(CString("\nNote: Select and decrypt an encrypted (.enc) file first..."));
		return;
	}
	
	//open text file
	inFP1=fopen(m_inFPName_to_encrypt,"rt");
	if(inFP1==NULL)
	{
		str.Format("\nERROR: Couldn't open input file: %s\nAborting operation",m_inFPName_to_encrypt);
		DumpNotes(str);
		return;
		
	}

	//open encrypted file
	CString tmp=m_inFPName_to_decrypt;
	ChangeFileExtension(tmp,"dec");
	inFP2=fopen(tmp,"rt");
	if(inFP2==NULL)
	{
		str.Format("\nERROR: Couldn't open input file: %s\nAborting operation",tmp);
		DumpNotes(str);
		fcloseall();
		return;
	}

	str.Format("\nfile 1: %s",m_inFPName_to_encrypt);
	DumpNotes(str);
	str.Format("\nfile 2: %s",m_inFPName_to_decrypt);
	DumpNotes(str);

	str = "\nFile Compare in progress...";
	DumpNotes(str);
	
	unsigned char data1,data2;
	int sizeof_d=sizeof(data1);

	while(1)
	{
		//read from file 1
		if(fread(&data1,sizeof_d,1,inFP1)==0)
			break;

		//read from file 2
		if(fread(&data2,sizeof_d,1,inFP2)==0)
			break;

		if(data1 != data2)
		{
			DumpNotes(CString("\nRESULT: Mismatching byte detected..."));
			fcloseall();
			return;
		}

	}//end of while loop

	DumpNotes(CString("\nRESULT: Both the files has matching data..."));
	fcloseall();		

}//end of compare

void CEC1AssignmentDlg::OnAbstract() 
{
	CAbstract ab;	
	ab.DoModal();
}
