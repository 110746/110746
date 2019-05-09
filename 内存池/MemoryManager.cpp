#include "MemoryManager.h"
#include <cstdio>
#include <cstdlib>
#define  MPM(o) ((MemoryManager*)(o))

extern ptrdiff_t MPIdxFind(MemPool *mp, size_t size1, size_t size2);

namespace XDataBaseEngine
{
	namespace Engine
	{
		MemoryManager::MemoryManager(void *mem, size_t mSize, size_t idxNumber) :
			m_mp(MPCreate(mem, mSize, idxNumber)) 
		{
		}

		MemoryManager::~MemoryManager()
		{
		}

		MemoryManager::iterator MemoryManager::begin(size_t size)
		{
			return MPIterator(this, size, IterBegin);
		}
		MemoryManager::iterator MemoryManager::end(size_t size)
		{
			return MPIterator(this, size, IterEnd);
		}

		MemoryManager::iterator  MemoryManager::rbegin(size_t size)
		{
			return MPIterator(this, size, IterRBegin);
		}
		MemoryManager::iterator  MemoryManager::rend(size_t size)
		{
			return MPIterator(this, size, IterREnd);
		}

///////////////////////////////////////////////////////////////////
		MemoryStack::MemoryStack(void *mem, size_t mSize, size_t idxNumber) :
			MemoryManager(mem, mSize, idxNumber)
		{
		}
		MemoryStack::~MemoryStack()
		{
		}

		bool MemoryStack::ReSize(size_t newSize)
		{
			return false;
		}
		bool MemoryStack::ReSize(void *newMem, size_t newSize)
		{
			if (newMem == m_mp || newSize <= m_mp->nSize)
				return false;

			memcpy(newMem, m_mp, m_mp->iUse);
			m_mp = (MemPool*)newMem;
			m_mp->nSize = newSize;

			return true;
		}
///////////////////////////////////////////////////////////////////
		MemoryHeap::MemoryHeap(size_t mSize, size_t idxNumber) :
			MemoryManager(malloc(mSize), mSize, idxNumber)
		{
		}
		MemoryHeap::~MemoryHeap()
		{
			free(m_mp);
		}
		bool MemoryHeap::ReSize(size_t newSize)
		{
			if (newSize <= m_mp->nSize)
				return false;

			void *newMem = realloc(m_mp, newSize);

			//只有realloc返回的新的内存块
			//才需要设置，否则使用扩展的旧
			//内存块就行
			if (newMem == NULL)
			{
				mpLastErrorCode = RunOutOfMemory;
				return false;
			} 
			if (newMem != m_mp)
				m_mp = (MemPool*)newMem;
			
			m_mp->nSize = newSize;

			mpLastErrorCode = OK;
			return true;
		}
///////////////////////////////////////////////////////////////////
#if  defined( _WIN32) || defined( _WIN64)
		MemoryFile::MemoryFile(LPCTSTR path, size_t mSize, size_t idxNumber) :
			MemoryManager(NULL, mSize, idxNumber)
		{
			m_path = (LPTSTR)malloc((_tcslen(path) + 1) * sizeof(TCHAR));
			_tcscpy(m_path, path);

			FromFile(mSize, idxNumber);
		}
		MemoryFile::~MemoryFile()
		{
			UnmapViewOfFile(m_mp);
			CloseHandle(m_hFMp);
			CloseHandle(m_hFile);
			//DeleteFile(m_path);
			free(m_path);
		}

		bool MemoryFile::ReSize(size_t newSize)
		{
			if (newSize <= m_mp->nSize)
				return false;

			UnmapViewOfFile(m_mp);
			//CloseHandle(m_hFMp);

			m_hFMp = CreateFileMapping(m_hFile,
				NULL,
				PAGE_READWRITE,
				0,
				newSize,
				NULL
			);

			m_mp = (MemPool*)MapViewOfFile(m_hFMp,
				FILE_MAP_WRITE | FILE_MAP_READ,
				0,
				0,
				0);

			m_mp->nSize = newSize;
			return true;
		}

		void MemoryFile::FromFile(size_t size, size_t idxNumber)
		{
			if (size == -1)
				return;

			FILE *fTmp = _tfopen(m_path, _T("rb"));
			bool bExist = (bool)fTmp;


			if (bExist)
				fclose(fTmp);

			m_hFile = CreateFile(m_path,
				GENERIC_READ | GENERIC_WRITE,
				0,
				NULL,
				OPEN_ALWAYS,
				FILE_ATTRIBUTE_NORMAL,
				NULL);

			if (m_hFile == INVALID_HANDLE_VALUE)
				return;

			LARGE_INTEGER fileSize = { 0 };

			if (bExist)
				fileSize.LowPart = GetFileSize(m_hFile, (LPDWORD)&fileSize.HighPart);
			else //文件不存在使用自定义大小
				fileSize.QuadPart = size;

			m_hFMp = CreateFileMapping(m_hFile,
				NULL,
				PAGE_READWRITE,
				fileSize.HighPart,
				fileSize.LowPart,
				NULL);

			if (m_hFMp == NULL)
				return;

			 
			m_mp = (MemPool*)MapViewOfFile(m_hFMp,
				FILE_MAP_WRITE | FILE_MAP_READ,
				0,
				0,
				0);

			if (!bExist)
				m_mp = MPCreate(m_mp, (size_t)fileSize.QuadPart, idxNumber);
		}
#endif 
///////////////////////////////////////////////////////////////////
		MPIterator::MPIterator(const MPIterator& iter):
			m_iter(NULL)
		{
			iter.m_iter->Copy(&m_iter);
		}
		MPIterator::MPIterator(MemoryManager *mpm, size_t size, MPEnum type):
			m_iter(NULL)
		{
			if (mpm == NULL)
			{
				mpLastErrorCode = PtrNull;
				return;
			}
			switch (type)
			{
			case IterBegin:
			case IterEnd:
				m_iter = new MPIterSizeNext(mpm, size, type);
				break;
			case IterRBegin:
			case IterREnd:
				m_iter = new MPIterSizeBack(mpm, size, type);
				break;
			}

			mpLastErrorCode = OK;
		}
		 MPIterator::~MPIterator()
		 {
			delete m_iter; 
	     }

		 MPIterator& MPIterator::operator++()
		 {
			 m_iter->Next();
			 return *this;
		 }
		MPIterator MPIterator::operator++(int)
		{
			MPIterator i(*this);
			++*this;
			return  i;
		}
		MPIterator& MPIterator::operator--()
		{
			m_iter->Back();
			return *this;
		}
		MPIterator MPIterator::operator--(int)
		{
			MPIterator i(*this);
			--*this;
			return  i;
		}
		bool MPIterator::operator==(const MPIterator& my)
		{
			return m_iter->m_iNode == my.m_iter->m_iNode;
		}
		bool MPIterator::operator!=(const MPIterator& my)
		{
			return !(*this == my);
		}
		MPIterator& MPIterator::operator=(const MPIterator& my)
		{
			my.m_iter->Copy(&m_iter);
			 
			return *this;
		}


		void *MPIterator::Ptr()
		{
			if (m_iter->m_mp == NULL)
				return NULL;

			return NodePtr(m_iter->m_mp, m_iter->m_iNode)->data;

		}
		 

///////////////////////////////////////////////////////////////////
		MPIterObj::MPIterObj(MemoryManager *mpm):
			m_mp(mpm->m_mp),m_iNode(0)
		{
		 
		}
///////////////////////////////////////////////////////////////////
	
		 MPIterSizeNext::MPIterSizeNext(MemoryManager *mpm, size_t size, MPEnum type):
			 MPIterObj(mpm)
		 {	
			 static size_t iCacheSize;
			 static ptrdiff_t iFind;
			 
			 if (iCacheSize != size)
			 {
				 iCacheSize = size;
				 iFind = MPIdxFind(m_mp, size, size);
			 }

			 if (iFind != -1)
			 {
				 MPList *ls = ListPtr(m_mp, m_mp->idxs[iFind].idx) + 1;
				 switch (type)
				 {
				 case IterBegin:
					 m_iNode = NodePtr(m_mp, ls->iREnd)->iNext;
					 break;
				 case IterEnd:
					 m_iNode = ls->iEnd;
					 break;
				 }				
			 }
		 }

		 void MPIterSizeNext::Back()
		 {
			 if (m_iNode == 0)
			 {
				 mpLastErrorCode = MemSizeIllegal;
				 return;
			 }
			 MPListNode *node = NodePtr(m_mp, m_iNode);
			 if (node->iBack == 0)
			 {
				 mpLastErrorCode = IterEnd;
				 return;
			 }
			 m_iNode = node->iBack;

			 mpLastErrorCode = OK;
		 }

		void MPIterSizeNext::Next()
		 {
			 if (m_iNode == 0)
			 {
				 mpLastErrorCode = MemSizeIllegal;
				 return;
			 }
			 MPListNode *node = NodePtr(m_mp, m_iNode);
			 if (node->iNext == 0)
			 {
				 mpLastErrorCode = IterEnd;
				 return ;
			 }
			 m_iNode = node->iNext;

			 mpLastErrorCode = OK;
		 }

		void MPIterSizeNext::Copy(MPIterObj** iter)
		{
			if (*iter == NULL)
				*iter = new MPIterSizeNext(*this);
			else
				**iter = *this;
		}

	
///////////////////////////////////////////////////////////////////
		 MPIterSizeBack::MPIterSizeBack(MemoryManager *mpm, size_t size, MPEnum type ) :
			 MPIterObj(mpm)
		 {
			 static size_t iCacheSize;
			 static ptrdiff_t iFind;

			 if (iCacheSize != size)
			 {
				 iCacheSize = size;
				 iFind = MPIdxFind(m_mp, size, size);
			 }

			 if (iFind != -1)
			 {
				 MPList *ls = ListPtr(m_mp, m_mp->idxs[iFind].idx) + 1;
				 switch (type)
				 {
				 case IterRBegin:
					 m_iNode = NodePtr(m_mp, ls->iEnd)->iBack;
					 break;
				 case IterREnd:
					 m_iNode = ls->iREnd;
					 break;
				 }
			 }
		 }

		 void MPIterSizeBack::Back()
		 {
			 if (m_iNode == 0)
			 {
				 mpLastErrorCode = MemSizeIllegal;
				 return;
			 }
			 MPListNode *node = NodePtr(m_mp, m_iNode);
			 if (node->iNext == 0)
			 {
				 mpLastErrorCode = IterEnd;
				 return;
			 }
			 m_iNode = node->iNext;

			 mpLastErrorCode = OK;
		 }

		 void MPIterSizeBack::Next()
		 {
			 if (m_iNode == 0)
			 {
				 mpLastErrorCode = MemSizeIllegal;
				 return;
			 }
			 MPListNode *node = NodePtr(m_mp, m_iNode);
			 if (node->iBack == 0)
			 {
				 mpLastErrorCode = IterREnd;
				 return;
			 }
			 m_iNode = node->iBack;

			 mpLastErrorCode = OK;
		 }
		
 
		 void MPIterSizeBack::Copy(MPIterObj** iter)
		 {
			 if (*iter == NULL)
				 *iter = new MPIterSizeBack(*this);
			 else
				 **iter = *this;			
		 }
	}
}


