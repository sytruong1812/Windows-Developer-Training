// RSA.cpp
//

#include "stdafx.h"
#include "EC1Assignment.h"
#include "EC1AssignmentDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


void CEC1AssignmentDlg::Execute()
{
	CString str;

	//clear values window 
	m_values.SetWindowText("");
	
	DumpNotes(CString("\n*********************************"));
	DumpNotes(CString("\nStarting key generation session..."));

	//Step-1 Generating Random Numbers
	GeneratePrimeNumbers();

	//Step-2 calculating n=p*q
	m_n = m_Prime1 * m_Prime2;
	str.Format("\nValue of 'n' is %d",m_n);
	DumpNotes(str);
	WriteValues(str);

	//Step-3 0=(p-1)(q-1)
	m_Undef = (m_Prime1-1) * (m_Prime2-1);

	//Step-4 Selecting 'e'
	SelectE();
	//m_e=17;

	//Step-5 Calculate 'd'
	CalculateD();

	//Displaying private and public keys
	//(1) Public key KU={e,n}
	DumpNotes(CString("\n"));
	str.Format("{%d, %d}",m_e,m_n);
	DumpNotes(CString("\nPublic key KU="));
	WriteValues(CString("\nPublic key KU="));
	DumpNotes(str);
	WriteValues(str);
	m_ku.SetWindowText(str);
	

	//(2) Private key KU={d,n}
	str.Format("{%d, %d}",m_d,m_n);
	DumpNotes(CString("\nPrivate key KR="));
	WriteValues(CString("\nPrivate key KR="));
	DumpNotes(str);
	WriteValues(str);
	m_kr.SetWindowText(str);
	

}

//Step-4 Selecting a value for 'e' such that 'e' is relative prime
//to m_Undef
void CEC1AssignmentDlg::SelectE()
{
	CString str;
	///algo: EUCLID	
		
	str.Format("\n>Selecting Integer 'e'...");
	DumpNotes(str);

	for(float i=2;i<100000;i++)
	{
		if(IsRelativePrime((float)m_Undef,(float)i))
		{
			m_e=(long)i;
			str.Format("\nInteger 'e' is %d",m_e);
			DumpNotes(str);
			WriteValues(str);
			return;
		}
	}
}

bool CEC1AssignmentDlg::IsRelativePrime(float X,float Y)
{
	float R;
	///algo: EUCLID
	//Input: X,Y	

	if(X<Y)//if X is smaller then swap the values
	{
		X=X+Y;
		Y=X-Y;
		X=X-Y;
	}
	
	//X is always great than Y at this point

	for(;;)
	{
		if(Y==0) 
			break;
		R=fmod(X,Y);
		X=Y;
		Y=R;
	}

	if(X != 1) 
		return false;
	else 
		return true;//relatively prime

}

//STEP-5
void CEC1AssignmentDlg::CalculateD()
{
	float d;
	long d_dash;
	CString str;

	str.Format("\n>Calculating 'd'...");
	DumpNotes(str);

	for(float k=1;k<100000;k++)
	{
		d=(m_Undef*k+1)/m_e;
		d_dash = (long)((m_Undef*k+1)/m_e);
		if(d == d_dash)
		{
			m_d=d;
			str.Format("\nValue of 'd' is %d",m_d);
			DumpNotes(str);
			WriteValues(str);
			return;
		}
	}
}



//trancate all MSB which are ZEROS
void CEC1AssignmentDlg::GetOnlyProperBits(char *bits)
{
	char tmpb[40];
	int ti=0;

	//reach till a non zero bit
	for(int i=0;i<strlen(bits);i++)
	{
		if(bits[i]=='1') break;		
	}
	
	for(;i<strlen(bits);i++)
	{
		tmpb[ti++]=bits[i];
		tmpb[ti]='\0';
	}
	strcpy(bits,tmpb);

}





//decimal to binary conversion function
void CEC1AssignmentDlg::D_to_B(long no,long noOfBits, char *ou)
{
	long y = noOfBits;//8 bits
	
	strcpy(ou,"");
	for(long i=0;i<y;i++)
	{
		int rem=no%2;
		no=no/2;
		char st[5];
		sprintf(st,"%d",rem);
		strcat(ou,st);
		//if(no==0) break;
	}//X of for loop...

	strrev(ou);
}