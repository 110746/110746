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
			//把mSize大小的内存mem作为内存池
			//默认具有32种类型大小的对象链表
			MemoryManager(void *mem = NULL, size_t mSize = -1, size_t idxNumber = 0);
			virtual ~MemoryManager();

		public:
			//重置内存池
			 bool ReSet() { return (bool)MPCreate(m_mp, m_mp->nSize, m_mp->iMax); }
			//分配一个size大小的对象
			 void* Alloc(size_t size) { return MPAlloc(m_mp, size); }
			//释放
			 void Free(void* mem){ MPFree(m_mp, mem); }
			//扩充内存
			virtual bool ReSize(size_t newSize) = 0;
			//获取基指针
			MPByte* BasePtr() { return PB(m_mp); }
			//获取offset位置的指针
			MPByte* GetPtr(ptrdiff_t offset) { return PB(m_mp) + offset; }
		public: 
			/////正序迭代器/////
			iterator  begin(size_t size);
			iterator  end(size_t size);
			/////逆序迭代器/////
			iterator  rbegin(size_t size);
			iterator  rend(size_t size);
		
		//protected:
			MemPool *m_mp;
		};
		
		/////////////栈内存池////////////////
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
/////////////////////////堆内存池////////////////
		class MemoryHeap : public MemoryManager
		{
		public:
			MemoryHeap(size_t mSize, size_t idxNumber = 32u);
			~MemoryHeap();
		public:
			bool ReSize(size_t newSize);
		};

/////////////////////////文件内存池////////////////
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
 
/////////////////////内存池对象迭代器////////////////
        class MPIterObj;
		class MPIterator 
		{
			friend class MemoryManager;	
		public:
			//遍历池中size大小的所有对象
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

//////////////////对象迭代器////////////////
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

///////////////////特定对象大小迭代器////////////////
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

///////////////////特定对象大小逆向迭代器////////////////
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