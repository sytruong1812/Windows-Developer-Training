//Decrypt.cpp
#include "stdafx.h"
#include "EC1Assignment.h"
#include "EC1AssignmentDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void CEC1AssignmentDlg::OnDecrypt() 
{
	if(!CheckKeyGenStatus()) 
	{
		DumpNotes(CString("\nERROR: Generate public and private keys first !!"));
		return;
	}

	// TODO: Add your control notification handler code here
	CString filter;
	filter = "Encrypted files (*.enc) |*.enc||";
	CFileDialog dlgFI
		(
			true, 
			".bin", 
			NULL, 
			OFN_HIDEREADONLY,
			filter
		);
	dlgFI.m_ofn.lpstrTitle = "Select a file to decrypt using RSA algorithm";	
	dlgFI.m_ofn.Flags |= OFN_OVERWRITEPROMPT;
	if (dlgFI.DoModal() != IDOK)
		return;

	m_inFPName_to_decrypt=dlgFI.GetFileName(); //in-file name
	DumpNotes(CString("\n"));
	filter.Format("\n****'%s'****",m_inFPName_to_decrypt);
	DumpNotes(filter);
	

	m_inFPName_to_decrypt=dlgFI.GetPathName(); //in-file path name
	CString opFName=m_inFPName_to_decrypt;
	ChangeFileExtension(opFName,"dec");

	///
	filter.Format("\nDecrypting file: %s",m_inFPName_to_decrypt);
	DumpNotes(filter);
	filter.Format("\nSaving to file: %s",opFName);
	DumpNotes(filter);
	Decrypt(m_inFPName_to_decrypt,opFName);		
}

void CEC1AssignmentDlg::Decrypt(CString infile, CString outfile)
{
	int NO_BITS;
	double c=0,d=1;
	char bits[100];
	double n=(double)m_n;
	double data;//19;
	long i,k;
	CString str;
	int sizeof_d=sizeof(double);


	FILE *inFP, *outFP;
	inFP=fopen(infile,"rb");
	if(inFP==NULL)
	{
		str.Format("\nERROR: Couldn't open input file: %s\nAborting operation",infile);
		DumpNotes(str);
		return;
		
	}

	outFP=fopen(outfile,"wt+");
	if(outFP==NULL)
	{
		str.Format("\nERROR: Couldn't create output file: %s\nAborting operation",outfile);
		DumpNotes(str);
		fcloseall();
		return;
		
	}
	

	//change integer 'd' to binary 
	D_to_B(m_d,32,bits);
	GetOnlyProperBits(bits);
	k=NO_BITS = strlen(bits)-1;

	while(1)
	{
		//read from ip file into 'data'
		if(fread(&data,sizeof_d,1,inFP)==0)
			break;

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
		unsigned char ch=(unsigned char)d;
		fprintf(outFP,"%c",ch);
		//write(&ch,1,1,outFP);
		
	}//end of while loop 

	DumpNotes(CString("\nDecryption over..."));
	fcloseall();
	
}//end of encrypt