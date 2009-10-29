/*
    © Vestris Inc., Geneva, Switzerland
    http://www.vestris.com, 1994-1999 All Rights Reserved
    ______________________________________________

    written by Hassan Sultan - hsultan@vestris.com

*/

#include <platform/include.hpp>
#include <Vector/SVector.hpp>
#include <Vector/Vector.hpp>
#include <Vector/IntVector.hpp>
#include <File/MMapFile.hpp>
#include <BitSet/BitSet.hpp>

#ifndef H_SWAPBLOCK_HPP
#define H_SWAPBLOCK_HPP

#define INT_BLOCK_SIZE 30//8190//12286//4094
#define MAX_VAL_INT 0xEFFFFFFF

#define BEGIN_USEDCOUNT(X) if (m_UsedCount>INT_BLOCK_SIZE) {cout<<X<<": "<<"BEGIN FAILED: "<<m_UsedCount<<endl;exit(0);}
#define MIDDLE_USEDCOUNT(X,Y) if (Y>INT_BLOCK_SIZE) {cout<<X<<": "<<"MIDDLE FAILED: "<<Y<<endl;exit(0);}
#define END_USEDCOUNT(X) if (m_UsedCount>INT_BLOCK_SIZE) {cout<<X<<": "<<"END FAILED: "<<m_UsedCount<<endl;exit(0);}

#define CSABS(X) abs(X) 
// abs() is inlined and asm ((X>0)?X:-X)
#define CSNEG(X,Y) ((X<0)?X:Y)
#define Flip_int32(type) (((type >>24) & 0x000000ff) | ((type >> 8) & 0x0000ff00) | ((type << 8) & 0x00ff0000) | ((type <<24) & 0xff000000))

class CSwapBlock;

class CSwapVector;

//REMEMBER: IF BLOCK BECOMES EMPTY, IT MUST BE REMOVED EXCEPT 1st BLOCK !!!! OTHERWISE ADD WILL FAIL IF EMPTY BLOCKS ARE IN THE LIST
class CSwapBlock 
{
	friend class CSwapVector;
public:
	CSwapBlock(void);
	~CSwapBlock(void);

	inline bool Add(const int Elt, CSwapBlock **m_LastBlock);
	void Add(unsigned int i, unsigned int j, unsigned int t, const CSVector& Vector, CSwapBlock *OtherBlock, CSwapBlock **LastBlock, unsigned int *Count);
	
	void Append(CSwapBlock *OtherBlock, unsigned int Pos, CSwapBlock **LastBlock, unsigned int *Count);
	void Append(const int Elt, CSwapBlock **LastBlock);
	void Append(const CSVector& Vector, unsigned int Pos, CSwapBlock **LastBlock);
	
	inline void GetDispTable(CVector<int>& Pages, const CVector<int>& DispTable);
	inline void SetAlreadyIndexed(CBitSet *Array);
	
	inline bool DumpTo(CMMapFile& File);
	inline bool DumpTo(FILE *File);
	inline int ReadFrom(FILE *File, const unsigned int Size, CSwapBlock **LastBlock);
	inline int ReadFrom(CMMapFile& File, const unsigned int Size, CSwapBlock **LastBlock);
	inline void ConvertIndianness();
	inline void GetAsCSVector(CSVector *Vector, const int Pos);
	inline void GetAsCIntVector(CIntVector& Vector);
	
	inline void PopulateSortArray(CVector<int>& Array);
	inline void PopulateRankArray(CVector<short int>& Array, const int Weight);
	inline void PopulateSortArray(CSVector& Array);

	inline void InsertElt(const int Elt, const unsigned int Pos, CSwapBlock **m_LastBlock);

	inline int FindElt(const int Elt) const;
	inline int FindAbs(const int Elt) const;

	inline int GetLastValue() {return m_UsedCount ? m_Block[m_UsedCount-1] : 0;}
	
	inline bool CheckBlock();
	
	bool Remove(const int Elt, CSwapBlock **FirstBlock, CSwapBlock **LastBlock);
	void RemoveAt(const int Pos, CSwapBlock **FirstBlock, CSwapBlock **LastBlock);
	bool RemoveEx(const int Elt, CSwapBlock **FirstBlock, CSwapBlock **LastBlock);
	inline void RemoveAll();

	void Display();
private:
	
	inline int FindElt(const int Element, int *CurrentPos) const;
	inline int FindAbs(const int Element, int *CurrentPos) const;
	int FindElt(const int Element, const int Min, const int Max) const;
	int FindAbs(const int Element, const int Min, const int Max) const;
	int FindIndex(const int Elt, const int Min, const int Max, bool *pInsert) const;
	
	inline void PrivRemoveAt(const int Pos);
	void Balance(CSwapBlock *PreviousBlock);
	
	inline int SmallestElt() {return (m_UsedCount) ? CSABS(m_Block[0]) : MAX_VAL_INT;}
	int m_Block[INT_BLOCK_SIZE];
	unsigned int m_UsedCount;
	CSwapBlock *m_NextBlock;
};

inline void CSwapBlock::SetAlreadyIndexed(CBitSet *Array)
{
	for(unsigned int i=0;i<m_UsedCount;i++)
	{
		Array->SetBit(CSABS(m_Block[i]));
	}
	if (m_NextBlock)
		m_NextBlock->SetAlreadyIndexed(Array);
}

inline void CSwapBlock::GetDispTable(CVector<int>& Pages, const CVector<int>& DispTable)
{
	for(unsigned int i=0;i<m_UsedCount;i++)
	{
		if (m_Block[i]>0)
			Pages+=DispTable[m_Block[i]];
	}
	if (m_NextBlock)
		m_NextBlock->GetDispTable(Pages,DispTable);
}


inline void CSwapBlock::ConvertIndianness()
{
	register int TempVal;
	for(unsigned int i=0;i<m_UsedCount;i++)
	{
		TempVal = m_Block[i];
		m_Block[i] = Flip_int32(TempVal);	
	}
	if (m_NextBlock)
		m_NextBlock->ConvertIndianness();
}


inline int CSwapBlock::ReadFrom(CMMapFile& File, const unsigned int Size, CSwapBlock **LastBlock)
{
	CSwapBlock *CurrentBlock=this;
	
	if (0==Size)
		return 0;

	unsigned int SizeToRead=Size;
	unsigned int ReadSize=0;		
	int Result;

	while(SizeToRead>0)
	{
		if (SizeToRead<INT_BLOCK_SIZE)
			ReadSize=SizeToRead;
		else
			ReadSize=INT_BLOCK_SIZE;
		Result=File.Read(CurrentBlock->m_Block,ReadSize*sizeof(int));
		if ((int) (ReadSize*sizeof(int))!=Result)
		{
				CurrentBlock=this;
				delete CurrentBlock->m_NextBlock;
				CurrentBlock->m_NextBlock=NULL;
				*LastBlock=NULL;
				return -1;
		}
		CurrentBlock->m_UsedCount=ReadSize;
		if (SizeToRead-ReadSize>0)
		{
			CurrentBlock->m_NextBlock=new CSwapBlock;
			*LastBlock=CurrentBlock->m_NextBlock;
			CurrentBlock=CurrentBlock->m_NextBlock;
		}
		SizeToRead-=ReadSize;
	}
	return Size;
}


inline int CSwapBlock::ReadFrom(FILE *File, const unsigned int Size, CSwapBlock **LastBlock)
{
	unsigned int ReadSize;
	if (0==Size)
		return 0;
	if (Size<INT_BLOCK_SIZE)
		ReadSize=Size;
	else
		ReadSize=INT_BLOCK_SIZE;
	if (ReadSize!=fread(m_Block,ReadSize,sizeof(int),File))
		return -1;
	m_UsedCount=ReadSize;
	if (Size-ReadSize>0)
	{
		m_NextBlock=new CSwapBlock;
		*LastBlock=m_NextBlock;
		int BlockRes=m_NextBlock->ReadFrom(File,Size-ReadSize,LastBlock);
		if (-1==BlockRes)
		{
			delete m_NextBlock;
			m_NextBlock=NULL;
			*LastBlock=NULL;
			return -1;
		}
		else
			return ReadSize+BlockRes;
	}
	else
		return ReadSize;
}


inline void CSwapBlock::GetAsCSVector(CSVector *Vector, const int Pos)
{
	for(unsigned int i=0;i<m_UsedCount;i++)
		(*Vector).Append(m_Block[i]);
	if (m_NextBlock)
		m_NextBlock->GetAsCSVector(Vector,Pos+m_UsedCount);
}

inline void CSwapBlock::GetAsCIntVector(CIntVector& Vector)
{
	for(unsigned int i=0;i<m_UsedCount;i++)
		Vector+=CSABS(m_Block[i]);
	if (m_NextBlock)
		m_NextBlock->GetAsCIntVector(Vector);
}

inline bool CSwapBlock::DumpTo(CMMapFile& File)
{
	if (0==m_UsedCount)
        return true;
	bool bRetVal=File.Write(m_Block,m_UsedCount*sizeof(int));
	if (!bRetVal)
		return false;
	else
	{
		if (m_NextBlock)
			return m_NextBlock->DumpTo(File);
		else
			return true;
	}
}


inline bool CSwapBlock::DumpTo(FILE *File)
{
	if (0==m_UsedCount)
        return true;
	int iResult=fwrite(m_Block,sizeof(int),m_UsedCount,File);
	if (iResult!=(int)m_UsedCount)
		return false;
	else
	{
		if (m_NextBlock)
			return m_NextBlock->DumpTo(File);
		else
			return true;
	}
}
 
inline void CSwapBlock::PopulateSortArray(CVector<int>& Array)
{
	for(unsigned int i=0;i<m_UsedCount;i++)
		Array[CSABS(m_Block[i])]=1;
	if (m_NextBlock)
		m_NextBlock->PopulateSortArray(Array);
}

inline void CSwapBlock::PopulateRankArray(CVector<short int>& Array, const int Weight)
{
	for(unsigned int i=0;i<m_UsedCount;i++)
		Array[CSABS(m_Block[i])]+=Weight;
	if (m_NextBlock)
		m_NextBlock->PopulateRankArray(Array, Weight);
}

inline void CSwapBlock::PopulateSortArray(CSVector& Array)
{
	for(unsigned int i=0;i<m_UsedCount;i++)
		Array[CSABS(m_Block[i])]=1;
	if (m_NextBlock)
		m_NextBlock->PopulateSortArray(Array);
}

inline int CSwapBlock::FindElt(const int Element) const 
{
	int Pos=0;
	return FindElt(Element,&Pos);
}

inline int CSwapBlock::FindAbs(const int Element) const 
{
	int Pos=0;
	return FindAbs(Element,&Pos);
}

inline int CSwapBlock::FindAbs(const int Element, int *CurrentPos) const
{
	if (m_NextBlock)
	{
		if (m_NextBlock->SmallestElt()<=CSABS(Element))
		{
			*CurrentPos+=m_UsedCount;
			return m_NextBlock->FindAbs(Element, CurrentPos);
		}
		else
			return m_UsedCount ? FindAbs(Element, 0, (int) m_UsedCount - 1) + *CurrentPos: -1;
	}
    else
		return m_UsedCount ? FindAbs(Element, 0, (int) m_UsedCount - 1) + *CurrentPos: -1;
}

inline int CSwapBlock::FindElt(const int Element, int *CurrentPos) const
{
	if (m_NextBlock)
	{
		if (m_NextBlock->SmallestElt()<=Element)
		{
			*CurrentPos+=m_UsedCount;
			return m_NextBlock->FindElt(Element, CurrentPos);
		}
		else
			return m_UsedCount ? FindElt(Element, 0, (int) m_UsedCount - 1) + *CurrentPos: -1;
	}
	else
	  	return m_UsedCount ? FindElt(Element, 0, (int) m_UsedCount - 1) + *CurrentPos: -1;
}



inline void CSwapBlock::PrivRemoveAt(const int Pos)
{
    m_UsedCount--;	
	for (register int i=Pos;i<(int)m_UsedCount;i++)
      m_Block[i] = m_Block[i+1];	
}


inline void CSwapBlock::InsertElt(const int Elt, const unsigned int Pos, CSwapBlock **LastBlock)
{
	if (m_UsedCount==INT_BLOCK_SIZE) //If we need to create space for an elt
	{
		if (!m_NextBlock) //if we're the last block, simply add another block at the end of the list
		{
			m_NextBlock=new CSwapBlock;
			*LastBlock=m_NextBlock;
		}
		else //otherwise, store the next block pointer, add a new block inbetween and restore the pointers for the list
		{
			CSwapBlock *TempBlock=m_NextBlock;
			m_NextBlock=new CSwapBlock;
			m_NextBlock->m_NextBlock=TempBlock;	
		}
		m_NextBlock->Balance(this); //We create space in our block by moving some of the stuff to the next block
	}
	
	if (Pos>m_UsedCount)
		m_NextBlock->InsertElt(Elt,Pos-m_UsedCount,LastBlock);
	else
	{
		for (register int i = (int) m_UsedCount; i > (int) Pos; i--)
			m_Block[i] = m_Block[i-1];
		m_Block[Pos] = Elt;
		m_UsedCount++;	
	}
}

inline bool CSwapBlock::Add(const int Elt, CSwapBlock **LastBlock)
{
	CSwapBlock *CurrentBlock=this, *PreviousBlock=NULL;
	while ( (CurrentBlock->m_NextBlock) && (CurrentBlock->m_NextBlock->SmallestElt() <= CSABS(Elt)) )
		{
			PreviousBlock=CurrentBlock;
			CurrentBlock=CurrentBlock->m_NextBlock;
		}

	bool bInsert = true;
	int Pos = CurrentBlock->m_UsedCount ? CurrentBlock->FindIndex(Elt, 0, (int) CurrentBlock->m_UsedCount - 1, &bInsert) : 0;
	    
	if (bInsert)
	{
		CurrentBlock->InsertElt(Elt,Pos, LastBlock);
		return true;
	}
	else
	{
		CurrentBlock->m_Block[-Pos]= Elt;
		return false;
	}
}

inline void CSwapBlock::RemoveAll()
{
	m_UsedCount=0;
	if (m_NextBlock)
	{
		delete m_NextBlock;	
		m_NextBlock=NULL;
	}

}

inline bool CSwapBlock::CheckBlock()
{

	if (m_UsedCount)
		for(unsigned int i=1;i<m_UsedCount;i++)
			if (CSABS(m_Block[i-1])>=CSABS(m_Block[i]))
				return false;
	return true;
}

#endif 
