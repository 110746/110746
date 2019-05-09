#pragma once

#include "MemPool.h"

#if  defined( _WIN32) || defined( _WIN64)
#include <tchar.h>
 #include <Windows.h>
#endif 

using namespace std;

namespace XDataBaseEngine
{
	namespace Engine
	{
		class MPIterator;
		class MPIterObj;
		class MPIterSizeNext;
		class MemoryManager
		{
			friend class MPIterator;
			friend class MPIterObj;
			friend class MPIterSizeNext;
		public:
			typedef MPIterator iterator;
		protected:
			//��mSize��С���ڴ�mem��Ϊ�ڴ��
			//Ĭ�Ͼ���32�����ʹ�С�Ķ�������
			MemoryManager(void *mem = NULL, size_t mSize = -1, size_t idxNumber = 0);
			virtual ~MemoryManager();

		public:
			//�����ڴ��
			 bool ReSet() { return (bool)MPCreate(m_mp, m_mp->nSize, m_mp->iMax); }
			//����һ��size��С�Ķ���
			 void* Alloc(size_t size) { return MPAlloc(m_mp, size); }
			//�ͷ�
			 void Free(void* mem){ MPFree(m_mp, mem); }
			//�����ڴ�
			virtual bool ReSize(size_t newSize) = 0;
			//��ȡ��ָ��
			MPByte* BasePtr() { return PB(m_mp); }
			//��ȡoffsetλ�õ�ָ��
			MPByte* GetPtr(ptrdiff_t offset) { return PB(m_mp) + offset; }
		public: 
			/////���������/////
			iterator  begin(size_t size);
			iterator  end(size_t size);
			/////���������/////
			iterator  rbegin(size_t size);
			iterator  rend(size_t size);
		
		//protected:
			MemPool *m_mp;
		};
		
		/////////////ջ�ڴ��////////////////
		class MemoryStack : public MemoryManager
		{
		public:
			MemoryStack(void *mem, size_t mSize, size_t idxNumber = 32u);
			~MemoryStack();
		private:
			bool ReSize(size_t newSize);		
		public:
			bool ReSize(void *newMem, size_t newSize);
		};
/////////////////////////���ڴ��////////////////
		class MemoryHeap : public MemoryManager
		{
		public:
			MemoryHeap(size_t mSize, size_t idxNumber = 32u);
			~MemoryHeap();
		public:
			bool ReSize(size_t newSize);
		};

/////////////////////////�ļ��ڴ��////////////////
#if  defined( _WIN32) || defined( _WIN64)
		class MemoryFile : public MemoryManager
		{
		public:
			MemoryFile(LPCTSTR path, size_t mSize = -1, size_t idxNumber = 32u);
			~MemoryFile();
		public:
			bool ReSize(size_t newSize);
		private:
			void FromFile(size_t size, size_t idxNumber);

		private:
			TCHAR *m_path;
			HANDLE m_hFile;
			HANDLE m_hFMp;
		};
#endif
 
/////////////////////�ڴ�ض��������////////////////
        class MPIterObj;
		class MPIterator 
		{
			friend class MemoryManager;	
		public:
			//��������size��С�����ж���
			MPIterator(MemoryManager *mpm = NULL, size_t size = 0, MPEnum type = IterBegin);
			MPIterator(const MPIterator& iter);
			~MPIterator();
		public:
			MPIterator& operator++();
			MPIterator operator++(int);
			MPIterator& operator--();
			MPIterator operator--(int);
			bool operator==(const MPIterator& my);
			bool operator!=(const MPIterator& my);
			MPIterator& operator=(const MPIterator& my);
		public:
			void *Ptr();
		private:	     
			MPIterObj *m_iter;
	    };

//////////////////���������////////////////
		class MPIterObj
		{
			friend class MPIterator;
		protected:
			MPIterObj(MemoryManager *mpm);
			virtual ~MPIterObj() {}
		public:
			virtual void Back() = 0;
			virtual void Next() = 0;
		protected:
			virtual void Copy(MPIterObj** iter) = 0;
		protected:
			MemPool  *m_mp;
			ptrdiff_t m_iNode;
		};

///////////////////�ض������С������////////////////
		class MPIterSizeNext : public MPIterObj
		{
			friend class MPIterator;
		private:
			MPIterSizeNext(MemoryManager *mpm , size_t size = 0, MPEnum type = IterBegin);
		public:
			virtual void Back();
			virtual void Next();
			virtual void Copy(MPIterObj** iter);
		};

///////////////////�ض������С���������////////////////
		class MPIterSizeBack: public MPIterObj
		{
			friend class MPIterator;
		private:
			MPIterSizeBack(MemoryManager *mpm , size_t size = 0, MPEnum type = IterBegin);
		public:
			virtual void Back();
			virtual void Next();
			virtual void Copy(MPIterObj** iter);
		};
 
	}
	 
}