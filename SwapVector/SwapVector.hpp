#include <platform/include.hpp>
/*
    © Vestris Inc., Geneva, Switzerland
    http://www.vestris.com, 1994-1999 All Rights Reserved
    ______________________________________________

    written by Hassan Sultan - hsultan@vestris.com

*/

#include <Vector/SVector.hpp>
#include <Vector/Vector.hpp>
#include <Vector/IntVector.hpp>
#include <File/MMapFile.hpp>
#include <BitSet/BitSet.hpp>
#include <SwapVector/SwapBlock.hpp>
    
#ifndef H_SWAPVECTOR_HPP
#define H_SWAPVECTOR_HPP

class CSwapVector
{
	public:
		CSwapVector(void);
		~CSwapVector(void);
	
		inline bool Add(const int Elt);
		inline void Add(const CSVector &OtherVector);
		
		inline void Append(const int Elt);
		inline void Append(CSwapVector& Vector);
		inline void Append(const CSVector& Vector);
		inline CSwapBlock* GetBegin() {return m_FirstBlock;}
		
		inline void SetAlreadyIndexed(CBitSet *Array) {m_FirstBlock->SetAlreadyIndexed(Array);}
		
		inline void GetDispTable(CVector<int>& Pages, const CVector<int>& DispTable) { m_FirstBlock->GetDispTable(Pages, DispTable); }
		
		inline void GetAsCSVector(CSVector *Vector);
		inline void GetAsCIntVector(CIntVector& Vector);
		inline bool DumpTo(CMMapFile& File) {return m_FirstBlock->DumpTo(File);}
		inline bool DumpTo(FILE *File) {return m_FirstBlock->DumpTo(File);}
		inline int ReadFrom(FILE *File, const unsigned int Size) { if (m_FirstBlock->ReadFrom(File,Size, &m_LastBlock)!=-1) {m_Count=Size;return Size;} else return -1;}
		inline int ReadFrom(CMMapFile& File, const unsigned int Size) {if (m_FirstBlock->ReadFrom(File,Size, &m_LastBlock)!=-1) {m_Count=Size;return Size;} else return -1;}
		inline void ConvertIndianness() { m_FirstBlock->ConvertIndianness();}
		inline int FindElt(const int Elt) const {return m_FirstBlock->FindElt(Elt);}
		inline int FindAbs(const int Elt) const {return m_FirstBlock->FindAbs(Elt);}
		
		inline void PopulateSortArray(CVector<int>& Array) {m_FirstBlock->PopulateSortArray(Array);}
		inline void PopulateRankArray(CVector<short int>& Array, const int Weight) {m_FirstBlock->PopulateRankArray(Array, Weight);}
		inline void PopulateSortArray(CSVector& Array) {m_FirstBlock->PopulateSortArray(Array);}
		
		inline int GetLastValue() {return m_LastBlock->GetLastValue();}
		inline bool Remove(const int Elt);
		inline void RemoveAt(const int Pos); 
		inline bool RemoveEx(const int Elt); 
		inline void RemoveAll();
		inline void Display() {m_FirstBlock->Display();}
		inline const unsigned int GetSize() const {return m_Count;}
		void CheckVector();
	private:
		CSwapBlock *m_FirstBlock;
		CSwapBlock *m_LastBlock;
		unsigned int m_Count;
	
};


inline void CSwapVector::RemoveAll()
{
	m_FirstBlock->RemoveAll(); 
	m_Count=0; 
	m_LastBlock=m_FirstBlock;
}

inline void CSwapVector::GetAsCSVector(CSVector *Vector)
{
	Vector->SetDim(m_Count,true,false);
	m_FirstBlock->GetAsCSVector(Vector,0);
}

inline void CSwapVector::GetAsCIntVector(CIntVector& Vector)
{
	m_FirstBlock->GetAsCIntVector(Vector);
}

inline void CSwapVector::Append(CSwapVector& Vector)
{ 
	unsigned int Count=0;
	m_FirstBlock->Append(Vector.m_FirstBlock,0, &m_LastBlock, &Count); 
	m_Count+=Count;
}

inline void CSwapVector::Append(const CSVector& Vector)
{ 
	m_FirstBlock->Append(Vector,0, &m_LastBlock); 
	m_Count+=Vector.GetSize();
}

inline void CSwapVector::Append(const int Elt)
{
	m_LastBlock->Append(Elt, &m_LastBlock);
	m_Count++;
}

inline bool CSwapVector::Add(const int Elt)
{
	if (m_FirstBlock->Add(Elt, &m_LastBlock)) 
		{
			m_Count++;
			return true;
		}
	return false;
}

inline void CSwapVector::Add(const CSVector& OtherVector)
{
	if (OtherVector.GetSize()==0)
		return;
		
	if (OtherVector.GetSize()<30)
	{
		for(unsigned int i=0;i<OtherVector.GetSize();i++)
			Add(OtherVector[i]);
	}
	else
	{
		unsigned int Count=0;
		CSwapBlock *OtherBlock=m_FirstBlock;
		m_FirstBlock=new CSwapBlock;
		m_LastBlock=m_FirstBlock;
		m_Count=0;
		m_FirstBlock->Add(0,0,0, OtherVector, OtherBlock, &m_LastBlock, &Count);

		m_Count+=Count;
		OtherBlock->RemoveAll();
		delete OtherBlock;
	}
}


inline bool CSwapVector::Remove(const int Elt)
{
	if (m_FirstBlock->Remove(Elt, &m_FirstBlock, &m_LastBlock)) 
	{
		m_Count--;
		return true;
	}
	return false;
}

inline void CSwapVector::RemoveAt(const int Pos)
{
	if (m_FirstBlock->Remove(Pos, &m_FirstBlock, &m_LastBlock))
		m_Count--;
}

inline bool CSwapVector::RemoveEx(const int Elt)
{
	if (m_FirstBlock->RemoveEx(Elt, &m_FirstBlock, &m_LastBlock)) 
	{
		m_Count--;
		return true;
	}
	return false;
}
#endif
