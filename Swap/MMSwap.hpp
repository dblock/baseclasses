/*

  MMAP Simple Virtual Memory Implementation
  ==================================================
  part of the Alkaline Search Engine
  © Vestris Inc., Geneva Switzerland
  http://www.vestris.com, 1998, All Rights Reserved
  __________________________________________________

  written by Daniel Doubrovkine - dblock@vestris.com

*/

#ifndef BASE_MMSWAP_HPP
#define BASE_MMSWAP_HPP

#include <platform/include.hpp>

#include <Object/Object.hpp>
#include <Mutex/Mutex.hpp>
#include <Mutex/RWMutex.hpp>

class CMMBlock;

#ifdef _WIN32
#define MM_MAX_DESC 64
#endif

#define MM_MAX_CACHE 64
 
class CMMSwap {
private:
#ifdef _WIN32
  HANDLE                   m_Descriptors[MM_MAX_DESC];
  HANDLE                   m_Mappings[MM_MAX_DESC];
  char                     m_szFilenames[MM_MAX_DESC][MAXPATHLEN];
  int                      m_DescCount;
#else
  char                     m_szFilename[MAXPATHLEN];
  int                      m_Descriptor;
#endif
  CMMBlock *               m_CacheBlocks[MM_MAX_CACHE];
  CMMBlock *               m_pvBlocks;
  CRWMutex                 m_IrlMutex;
  long                     m_TailOffset;
  long                     m_LastMultiple;
  size_t                   m_PageSize;
  readonly_property(bool,  Enabled);
  void FreeBlocks(void);
public:
  static CMMSwap *         m_pSwap;
  static void InitializeSwap(void);
  static void UnInitializeSwap(void);
  void SetEnabled(bool bValue, char * pszFilename = NULL);
  long GetSize(void) const;
  void * MMAllocate(size_t Bytes);
  void MMFree(void * Pointer);
  CMMSwap(void);
  ~CMMSwap(void);
};

#endif
