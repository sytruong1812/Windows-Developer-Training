// Abstract.cpp : implementation file
//

#include "stdafx.h"
#include "EC1Assignment.h"
#include "Abstract.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAbstract dialog


CAbstract::CAbstract(CWnd* pParent /*=NULL*/)
	: CDialog(CAbstract::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAbstract)
	//}}AFX_DATA_INIT
}


void CAbstract::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAbstract)
	DDX_Control(pDX, IDC_TEXT, m_text);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAbstract, CDialog)
	//{{AFX_MSG_MAP(CAbstract)
	ON_BN_CLICKED(IDC_CLOSE, OnClose)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAbstract message handlers

void CAbstract::OnClose() 
{
	EndDialog(NULL);	
}

BOOL CAbstract::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CString str;
	char *buff;

	FILE *fp;

	fp=fopen("abstract.txt","rt");
	if(fp==NULL) return TRUE;

	int len;
	fseek(fp,0,SEEK_END);
	len=ftell(fp);
	fseek(fp,0,SEEK_SET);
	buff = new char [len+1];
	fread(buff,1,len,fp);
	buff[len]='\0';
	
	str = buff;
	delete buff;
	m_text.SetWindowText(str);
	
	fclose(fp);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE

}
