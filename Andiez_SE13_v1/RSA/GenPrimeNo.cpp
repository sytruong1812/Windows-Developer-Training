//genPrimeNo.cpp
#include "stdafx.h"
#include "EC1Assignment.h"
#include "EC1AssignmentDlg.h"
#include <stdlib.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//Step-1 this module will generate two random numbers
void CEC1AssignmentDlg::GeneratePrimeNumbers()
{
	CString str;

	UpdateData(TRUE);

	str.Format("\n>Generating Prime Numbers...");
	DumpNotes(str);
	
	if(!GetPrime1(m_Prime1))
		m_Prime1 = FindPrime(1);

	if(!GetPrime2(m_Prime2))
		m_Prime2 = FindPrime(m_Prime1);
	
	str.Format("\nPrime # pair {%d,%d}...",m_Prime1,m_Prime2);
	DumpNotes(str);
	WriteValues(str);

}

int CEC1AssignmentDlg::GetPrime1(long &prime)
{
	if(m_chk_p1)
	{
		prime = m_prime1;
		return 1;
	}
	else return 0;
}

int CEC1AssignmentDlg::GetPrime2(long &prime)
{
	if(m_chk_p2)
	{
		prime = m_prime2;
		return 1;
	}
	else return 0;
}

long CEC1AssignmentDlg::FindPrime(long ignore_prime)
{
	long odd,tmp_odd;
	double tmp;

	srand( (unsigned)time( NULL ) );
	
	while(1)
	{
		odd = rand();
		if(odd%2==0)
			odd++;

		if(ignore_prime == odd) continue;

		//test whether 'odd' is prime number or not 
		for(int i=2;i<=odd/2;i++)
		{
			tmp_odd=odd%i;
			if(tmp_odd ==0) 
			{
				i=-1;
				break;
			}
		}//x of 'for' loop 
		if(i != -1) 
			break;
	}//x of 'while' loop 
	return odd;
}