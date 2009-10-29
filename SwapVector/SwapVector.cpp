/*
    © Vestris Inc., Geneva, Switzerland
    http://www.vestris.com, 1994-1999 All Rights Reserved
    ______________________________________________

    written by Hassan Sultan - hsultan@vestris.com

*/

#include <baseclasses.hpp>
#include "SwapVector.hpp"

CSwapVector::CSwapVector(void)
{
		m_Count=0;
		m_FirstBlock=new CSwapBlock;
		m_LastBlock=m_FirstBlock;
}

CSwapVector::~CSwapVector(void)
{
		CSwapBlock *NextBlock, *CurrentBlock;
		CurrentBlock=m_FirstBlock;
			
		while(CurrentBlock)
		{
			NextBlock=CurrentBlock->m_NextBlock;
			CurrentBlock->m_NextBlock=NULL; //Destroy blocks one by one in a loop to avoid stack overflow with big vectors
			delete CurrentBlock;			//when using default destructor
			CurrentBlock=NextBlock;				
		}
}


 void CSwapVector::CheckVector()
{
	CSwapBlock *Current=m_FirstBlock;
	int iCurVal;
	int iPrevVal=0;
	int i=0;
	while(Current)
	{
		iCurVal=Current->GetLastValue();
		assert(CSABS(iCurVal)>CSABS(iPrevVal));		
		Current->CheckBlock();
		iPrevVal=Current->GetLastValue();
		Current=Current->m_NextBlock;
	}
	

}
