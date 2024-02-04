//Encrypt.cpp

#include "stdafx.h"
#include "EC1Assignment.h"
#include "EC1AssignmentDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

bool CEC1AssignmentDlg::CheckKeyGenStatus()
{
	if(m_key_gen_done_flg) return 1;
	else return 0;
}
///this function will encrypt the input file
void CEC1AssignmentDlg::OnEncrypt() 
{	

	if(!CheckKeyGenStatus()) 
	{
		DumpNotes(CString("\nERROR: Generate public and private keys first !!"));
		return;
	}

	CString filter;
	filter = "All Files (*.*) |*.*||";
	CFileDialog dlgFI
		(
			true, 
			".bin", 
			NULL, 
			OFN_HIDEREADONLY,
			filter
		);
	dlgFI.m_ofn.lpstrTitle = "Select a file to encrypt using RSA algorithm";	
	dlgFI.m_ofn.Flags |= OFN_OVERWRITEPROMPT;
	if (dlgFI.DoModal() != IDOK)
		return;
	
	m_inFPName_to_encrypt=dlgFI.GetFileName(); //in-file name
	DumpNotes(CString("\n"));
	filter.Format("\n****'%s'****",m_inFPName_to_encrypt);
	DumpNotes(filter);

	m_inFPName_to_encrypt=dlgFI.GetPathName(); //in-file path name
	CString opFName=m_inFPName_to_encrypt;
	ChangeFileExtension(opFName,"enc");

	///
	filter.Format("\nEncrypting file: %s",m_inFPName_to_encrypt);
	DumpNotes(filter);
	filter.Format("\nSaving to file: %s",opFName);
	DumpNotes(filter);
	Encrypt(m_inFPName_to_encrypt,opFName);	
}

void CEC1AssignmentDlg::Encrypt(CString infile, CString outfile)
{
	int NO_BITS=32;
	double c=0,d=1;
	char bits[100];
	double n=(double)m_n;
	unsigned char ch;
	double data;//19;
	long i,k=NO_BITS;
	CString str;
	int sizeof_d=sizeof(double);


	FILE *inFP, *outFP;
	inFP=fopen(infile,"rt");
	if(inFP==NULL)
	{
		str.Format("\nERROR: Couldn't open input file: %s\nAborting operation",infile);
		DumpNotes(str);
		return;
		
	}

	outFP=fopen(outfile,"wb");
	if(outFP==NULL)
	{
		str.Format("\nERROR: Couldn't create output file: %s\nAborting operation",outfile);
		DumpNotes(str);
		fcloseall();
		return;
		
	}
	

	//change integer 'm' to binary 
	D_to_B(m_e,32,bits);
	GetOnlyProperBits(bits);
	k=NO_BITS = strlen(bits)-1;

	while(1)
	{
		//read from ip file into 'data'
		if(fread(&ch,1,1,inFP)==0)
			break;
		data =(double)ch;

		//calculate ((data)^e mod n) .i.e the remainder
		c=0;d=1;
		for(i=k;i>=0;i--)
		{
			c=2*c;
			d=fmod(d*d,n);
	
			if(bits[NO_BITS-i] == '1')
			{
				c=c+1;
				d=fmod(data*d,n);
			}//end of IF		
		}//end of for loop 

		//write to op file from 'd'
		fwrite(&d,sizeof_d,1,outFP);
		
	}//end of while loop 

	DumpNotes(CString("\nEncryption over..."));
	
	fcloseall();
}//end of encrypt
