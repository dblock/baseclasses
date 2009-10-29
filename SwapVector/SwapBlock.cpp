/*
    © Vestris Inc., Geneva, Switzerland
    http://www.vestris.com, 1994-1999 All Rights Reserved
    ______________________________________________

    written by Hassan Sultan - hsultan@vestris.com

*/

#include <baseclasses.hpp>
#include "SwapBlock.hpp"

CSwapBlock::CSwapBlock(void)
{
	m_UsedCount=0;
	m_NextBlock=NULL;
}

CSwapBlock::~CSwapBlock(void)
{
	if (m_NextBlock)
		delete m_NextBlock;
}



void CSwapBlock::Display()
{
	unsigned int i=0;
	
	for(i=0;i<m_UsedCount;i++)
		cout<<(int)m_Block[i]<<" ";
	cout<<endl<<"[-- end of block --]"<<endl;
	if (m_NextBlock)
		m_NextBlock->Display();
}

void CSwapBlock::Balance(CSwapBlock *PreviousBlock)
{
	//We will copy from the prv and next block, one third of the available space in the current block
	unsigned int EltsToCopy=(INT_BLOCK_SIZE-m_UsedCount)/3;
	register unsigned int i;
	
	if (PreviousBlock->m_UsedCount>EltsToCopy*2) //If the previous block is almost full, balance it
	{		
		int count=0;
		for (i = PreviousBlock->m_UsedCount-EltsToCopy ; i < PreviousBlock->m_UsedCount; i++)
			{m_Block[m_UsedCount++] = PreviousBlock->m_Block[i];count++;}
		PreviousBlock->m_UsedCount-=EltsToCopy;
	}	

	if ( (m_NextBlock) && (m_NextBlock->m_UsedCount>EltsToCopy*2) )//If the next block is almost full, balance it
	{	
		int count=0;	
		for (i = 0; i < EltsToCopy; i++)
			{count++;m_Block[m_UsedCount++] = m_NextBlock->m_Block[i];}
		memmove(&(m_NextBlock->m_Block[0]),&(m_NextBlock->m_Block[EltsToCopy]),(m_NextBlock->m_UsedCount-EltsToCopy)*sizeof(int));
		m_NextBlock->m_UsedCount-=EltsToCopy;
	}	
}


int CSwapBlock::FindAbs(const int Element, const int Min, const int Max) const 
{

    register int AbsElt = CSABS(Element);
    
    if ( CSABS(m_Block[Min]) == AbsElt ) 
        return Min;
    
    if ( CSABS(m_Block[Max]) == AbsElt ) 
        return Max;
    
    if ( Max <= Min + 1 ) 
        return -1;
    
    int Middle = Min + (Max - Min) /2 ;
    register int AbsMiddle = CSABS(m_Block[Middle]);
    
    if ( AbsElt > AbsMiddle )
        return FindAbs(Element, Middle, Max);
    else if ( AbsElt == AbsMiddle ) 
        return Middle;
    else 
        return FindAbs(Element , Min, Middle);
}


int CSwapBlock::FindElt(const int Element, const int Min, const int Max) const 
{
	if ( m_Block[Min] == Element ) 
        return Min;
    
    if ( m_Block[Max] == Element ) 
        return Max;
    
    if ( Max <= Min + 1 ) 
        return -1;
    
    int Middle = Min + (Max - Min) /2 ;
    
    if ( Element > m_Block[Middle] )
        return FindElt(Element, Middle, Max);
    else if ( Element == m_Block[Middle] ) 
        return Middle;
    else 
        return FindElt(Element, Min, Middle);
}

int CSwapBlock::FindIndex(const int Element, const int Min, const int Max, bool * pInsert) const {
    register int AbsElt, AbsMax, AbsMin, AbsMiddle, Middle;

    * pInsert = true;

    AbsElt = CSABS(Element);
    AbsMax = CSABS(m_Block[Max]);
    AbsMin = CSABS(m_Block[Min]);
    
    if ( AbsElt > AbsMax ) 
        return Max + 1;
    
    if ( AbsElt < AbsMin ) 
        return Min;
    
    if (Min == Max - 1) {
        
        if ( AbsElt == AbsMax ) {
            * pInsert = false;
            return -Max;
        }

        if ( AbsElt == AbsMin ) {
            * pInsert = false;
            return -Min;
        }

        return Max;
    }
    
    Middle = Min + (Max - Min) / 2;
    AbsMiddle = CSABS(m_Block[Middle]);
    
    if ( AbsElt == AbsMiddle ) {
        * pInsert = false;
        return - Middle;
    } else if ( AbsElt < AbsMiddle ) {
        return FindIndex(Element, Min, Middle, pInsert);
    } else {
        return FindIndex(Element, Middle, Max, pInsert);  
	}
}

void CSwapBlock::RemoveAt(const int Pos, CSwapBlock **FirstBlock, CSwapBlock **LastBlock)
{
	int MyPos=Pos;
	CSwapBlock *CurrentBlock=this, *PreviousBlock=NULL;
	while(MyPos>(int)CurrentBlock->m_UsedCount)
		{
			MyPos-=CurrentBlock->m_UsedCount;
			if (CurrentBlock->m_NextBlock)
			{
				PreviousBlock=CurrentBlock;
				CurrentBlock=CurrentBlock->m_NextBlock;
			}
		}
		
	if (CurrentBlock)
	{
		CurrentBlock->PrivRemoveAt(MyPos);
		if (!CurrentBlock->m_UsedCount) //If block is empty, delete it
			{
				if (NULL==CurrentBlock->m_NextBlock) //If it's the last block, then now the last block is the previous block
					*LastBlock=PreviousBlock;
				if (PreviousBlock) //If we're not the initial block, update the previous block
					PreviousBlock->m_NextBlock=CurrentBlock->m_NextBlock;
				else
					if (CurrentBlock->m_NextBlock)
						*FirstBlock=CurrentBlock->m_NextBlock;
				CurrentBlock->m_NextBlock=NULL; //Needed to avoid deleting the whole list when deleting the block
				if (PreviousBlock) //Should not delete the initial block
					delete CurrentBlock;
			}
	}
}

bool CSwapBlock::RemoveEx(const int Value, CSwapBlock **FirstBlock, CSwapBlock **LastBlock)
{	
	bool bWasRemoved=false;
	CSwapBlock *CurrentBlock=this, *PreviousBlock=NULL;
	while ( (CurrentBlock->m_NextBlock) && (CurrentBlock->m_NextBlock->SmallestElt() <= CSABS(Value)) )
		{
			PreviousBlock=CurrentBlock;
			CurrentBlock=CurrentBlock->m_NextBlock;
		}
		
	if (CurrentBlock->SmallestElt() <= CSABS(Value))
	{
		int Pos = CurrentBlock->m_UsedCount ? CurrentBlock->FindAbs(Value, 0, CurrentBlock->m_UsedCount - 1) : -1;
		if (Pos >= 0) 
		{
			if (CurrentBlock->m_Block[Pos] > 0) 
			{
				CurrentBlock->PrivRemoveAt(Pos);
				bWasRemoved=true;
			}
			else 
			{
				CurrentBlock->m_Block[Pos] = -CurrentBlock->m_Block[Pos];
				bWasRemoved=false;
			}
	        
		}	
		if (!CurrentBlock->m_UsedCount) //If block is empty, delete it
			{
				if (NULL==CurrentBlock->m_NextBlock) //If it's the last block, then now the last block is the previous block
					*LastBlock=PreviousBlock;
				if (PreviousBlock) //If we're not the initial block, update the previous block
					PreviousBlock->m_NextBlock=CurrentBlock->m_NextBlock;
				else
					if (CurrentBlock->m_NextBlock)			
						*FirstBlock=CurrentBlock->m_NextBlock;
				CurrentBlock->m_NextBlock=NULL; //Needed to avoid deleting the whole list when deleting the block
				if (PreviousBlock) //Should not delete the initial block
					delete CurrentBlock;
			}
	}	
	return bWasRemoved;
}

bool CSwapBlock::Remove(const int Value, CSwapBlock **FirstBlock, CSwapBlock **LastBlock) 
{

	bool bWasRemoved=false;
	CSwapBlock *CurrentBlock=this, *PreviousBlock=NULL;
	while ( (CurrentBlock->m_NextBlock) && (CurrentBlock->m_NextBlock->SmallestElt() <= CSABS(Value)) )
		{
			PreviousBlock=CurrentBlock;
			CurrentBlock=CurrentBlock->m_NextBlock;
		}
		
	if (CurrentBlock->SmallestElt() <= CSABS(Value))
	{
		int Pos = CurrentBlock->m_UsedCount ? CurrentBlock->FindAbs(Value, 0, CurrentBlock->m_UsedCount-1) : -1;
		if (Pos >= 0)  
		{
			CurrentBlock->PrivRemoveAt(Pos);		
			bWasRemoved=true;
		} 	
		if (!CurrentBlock->m_UsedCount) //If block is empty, delete it
			{
				if (NULL==CurrentBlock->m_NextBlock) //If it's the last block, then now the last block is the previous block
					*LastBlock=PreviousBlock;
				if (PreviousBlock) //If we're not the initial block, update the previous block
					PreviousBlock->m_NextBlock=CurrentBlock->m_NextBlock;
				else
					if (CurrentBlock->m_NextBlock)
						*FirstBlock=CurrentBlock->m_NextBlock;
				CurrentBlock->m_NextBlock=NULL; //Needed to avoid deleting the whole list when deleting the block
				if (PreviousBlock) //Should not delete the initial block
					delete CurrentBlock;
			}
	}	
	return bWasRemoved;
}	

void CSwapBlock::Append(CSwapBlock *OtherBlock, unsigned int Pos, CSwapBlock **LastBlock, unsigned int *Count)
{
	if (m_NextBlock)
		m_NextBlock->Append(OtherBlock, Pos, LastBlock, Count);
	else if (INT_BLOCK_SIZE==m_UsedCount)
	{
		m_NextBlock=new CSwapBlock;
		*LastBlock=m_NextBlock;
		m_NextBlock->Append(OtherBlock, Pos, LastBlock, Count);
	}
	else
	{
		unsigned int OldPos=Pos;
		while ( (INT_BLOCK_SIZE!=m_UsedCount) && (Pos<OtherBlock->m_UsedCount) )
			m_Block[m_UsedCount++]=OtherBlock->m_Block[Pos++];
		*Count+=Pos-OldPos;
		if ( (INT_BLOCK_SIZE==m_UsedCount) && (Pos<OtherBlock->m_UsedCount) )
			{
				m_NextBlock=new CSwapBlock;
				*LastBlock=m_NextBlock;
				m_NextBlock->Append(OtherBlock,Pos, LastBlock, Count);
			}
		else if ( (Pos==OtherBlock->m_UsedCount) && (OtherBlock->m_NextBlock) )
		{
			Append(OtherBlock->m_NextBlock, 0, LastBlock, Count);
		}
	}
}

void CSwapBlock::Append(const int Elt, CSwapBlock **LastBlock)
{
	if (m_NextBlock)
		m_NextBlock->Append(Elt, LastBlock);
	else if (INT_BLOCK_SIZE==m_UsedCount)
	{
		m_NextBlock=new CSwapBlock;
		*LastBlock=m_NextBlock;
		m_NextBlock->Append(Elt, LastBlock);
	}
	else
		m_Block[m_UsedCount++]=Elt;
}

void CSwapBlock::Append(const CSVector& Vector, unsigned int Pos, CSwapBlock **LastBlock)
{
	if (m_NextBlock)
		m_NextBlock->Append(Vector, Pos, LastBlock);
	else if (INT_BLOCK_SIZE==m_UsedCount)
	{
		m_NextBlock=new CSwapBlock;
		*LastBlock=m_NextBlock;
		m_NextBlock->Append(Vector, Pos, LastBlock);
	}
	else
	{
		while ( (INT_BLOCK_SIZE!=m_UsedCount) && (Pos<Vector.GetSize()) )
			m_Block[m_UsedCount++]=Vector[Pos++];

		if ( (INT_BLOCK_SIZE==m_UsedCount) && (Pos<Vector.GetSize()) )
			{
				m_NextBlock=new CSwapBlock;
				*LastBlock=m_NextBlock;
				m_NextBlock->Append(Vector,Pos, LastBlock);
			}
	}
}





void CSwapBlock :: Add(unsigned int i, unsigned int j, unsigned int t, const CSVector& Vector, CSwapBlock *OtherBlock, CSwapBlock **LastBlock, unsigned int *Count) 
{  
	if (!Vector.GetSize())
	{
		Append(OtherBlock, j, LastBlock, Count);
		return;
	}
	
	if (!OtherBlock->m_UsedCount) 
	{
		Append(Vector, i, LastBlock);
		*Count+=Vector.GetSize()-i;
		return;
	}

	while ( (i < Vector.GetSize()) && (j < OtherBlock->m_UsedCount) && (t<INT_BLOCK_SIZE) )
	{
		if (CSABS(Vector[i]) == CSABS(OtherBlock->m_Block[j])) 
		{      
			m_Block[t++] = CSNEG(OtherBlock->m_Block[j], Vector[i]);
			i++;
			j++;
		} 
		else if (CSABS(Vector[i]) < CSABS(OtherBlock->m_Block[j])) 
		{
			m_Block[t++] = Vector[i];
			i++;
		} 
		else 
		{
			m_Block[t++] = OtherBlock->m_Block[j];
			j++;    
		}
	} 
	m_UsedCount=t;
	*Count+=m_UsedCount;
	if (INT_BLOCK_SIZE==t)
	{
		m_NextBlock=new CSwapBlock;
		*LastBlock=m_NextBlock;
		m_NextBlock->Add(i,j,0,Vector,OtherBlock,LastBlock, Count);
	}
	else if (Vector.GetSize()==i)
	{
		Append(OtherBlock,j,LastBlock, Count);
	}
	else if (j==OtherBlock->m_UsedCount)
	{
		if (OtherBlock->m_NextBlock)
			Add(i,0,t,Vector,OtherBlock->m_NextBlock,LastBlock,Count);
		else
		{
			Append(Vector,i,LastBlock);
			*Count+=Vector.GetSize()-i;
		}
	}
}
