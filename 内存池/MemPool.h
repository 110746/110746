#ifndef MEM_POOL_H
#define MEM_POOL_H

 #if  defined( _WIN32) || defined( _WIN64)
	#define _CRT_NONSTDC_NO_DEPRECATE
	#define _CRT_SECURE_NO_WARNINGS
#endif

#include <cstddef>
#include <cstring>
#include <stdbool.h>
#define  abs_(v)   (((v) < 0) ? -(v) : (v))

#define  PB(p) ((MPByte*)(p))
#define  NodePtr(addr,i)        ((MPListNode*)(PB(addr) + (i)))
#define  ListPtr(addr,i)        ((MPList*)(PB(addr) + (i)))
#define  Offset(addr1,addr2)    (PB(addr2) - PB(addr1))
#define  OffsetMember(s,m)      ((ptrdiff_t)((&((s*)0)->m)))

#define MPIterPtr(iter)        MPIterPtr_ (&(iter))
#define MPIterNext(iter)       MPIterNext_(&(iter))
#define MPIterBack(iter)       MPIterBack_ (&(iter))
#define MPIterEq(it1, it2)     MPIterEq_(&(it1),&(it2))

//具有动态数组成员的对象所需的内存
#define  ObjSize(cname,mname,size)   (OffsetMember(cname,mname) + size)
//获取最后一次错误代码
#define  MPGetLastEorrCode()  mpLastErrorCode
 
typedef  unsigned char MPByte;

typedef enum _MPEnum
{
	OK,
	PtrNull,
	IdxMemGtrAllMem,    //索引内存大于总内存
	IdxIllegal,          //非法索引
	RunOutOfMemory,     //内存已耗尽
	RunOutOfIdxs,       //索引已用完
	MemSizeIllegal,     //非法的内存块大小
//////////////////
	IterBegin,          //迭代器指向首元素
	IterEnd,            //迭代器指向尾元素的下一位置
	null,               //分隔符，用于区分迭代器类型(千万不能删除)
	IterRBegin,         //迭代器指向尾元素
	IterREnd,           //迭代器指向首元素的上一位置
//////////////////
	IterDifferent,      //迭代器类型不同
}MPEnum;
extern _MPEnum mpLastErrorCode;

///////n 表示大小首字母,i表示索引首字母///////////

typedef struct _MPIdx
{
	size_t size;
	ptrdiff_t idx;

}MPIdx;

typedef struct _MPHead
{
	size_t nSize;      //内存块大小
	ptrdiff_t iUse;    //已使用的字节数

	size_t   iNum;     //已使用的索引数
	ptrdiff_t iMax;     //idxs的数目
	MPIdx idxs[1];     //链表索引数组(动态数组)

}MPHead,MemPool;

typedef struct _MPList
{ 
	union 
	{
		ptrdiff_t  iFirst;  //未分配的链表的首节点偏移
		ptrdiff_t  iREnd;   //rend
	};
	union
	{
		ptrdiff_t  iLast;   //未分配的链表的尾节点偏移
		ptrdiff_t  iEnd;   //end
	};
}MPList;

////////节点////////////
typedef struct _MPListNode
{
	ptrdiff_t  iNext;    //指向下一节点
	ptrdiff_t  iBack;    //指向上一个节点
	ptrdiff_t  iList;    //节点所属链表位于内存池中的位置
	MPByte       isUse;    //是否正在使用中(针对重复释放而做标记的)

	MPByte  data[1];       //用户数据(动态数组)

}MPListNode;

////////rend/end节点////////////
typedef struct _MPListEnd
{
	ptrdiff_t  iNext;    //指向下一节点
	ptrdiff_t  iBack;    //指向上一个节点

}MPListEnd;

typedef struct _MPIter
{
	ptrdiff_t iNode;  //下一节点指针
	MemPool *mp;
	MPEnum type;
}MPIter;


//////////////////////////////////////////////////


//把mSize大小的内存mem作为内存池
//idxNumber可以指定索引数目(链表数目)
MemPool* MPCreate(void *mem, size_t mSize, size_t idxNumber);
//分配一个内存
void* MPAlloc(MemPool *mp, size_t size);
//释放内存mem
void MPFree(MemPool *mp, void *mem);

//遍历size大小块的所有对象
//设置iterType可以从头或尾开始遍历
MPIter  MPIterBegin(MemPool *mp,size_t size);
MPIter  MPIterEnd(MemPool *mp, size_t size);
MPIter  MPIterRBegin(MemPool *mp, size_t size);
MPIter  MPIterREnd(MemPool *mp, size_t size);


//取得当前节点的指针
void* MPIterPtr_(MPIter* iter);
//前进遍历
void MPIterNext_(MPIter* iter);
//后退遍历
void MPIterBack_(MPIter* iter);
//比较两个迭代器是否相等
bool MPIterEq_(MPIter *it1, MPIter* itr2);


#endif // !MEM_POOL_H

