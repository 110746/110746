// MemPool.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
#include "MemPool.h"



#define MPAddNode0(mp,list,node)    if((list)->iFirst == 0)\
									{\
									   (list)->iFirst = Offset((mp),(node));\
									   (list)->iLast = (list)->iFirst;\
									}else\
									{\
									    NodePtr((mp),(list)->iLast)->iNext = Offset((mp),(node)); \
										(node)->iBack = (list)->iLast; \
										(list)->iLast = Offset((mp), (node)); \
									}
								

#define MPDelNode0(mp,list,node)  if ((node)->iBack == 0)\
							          (list)->iFirst = NodePtr((mp),(list)->iFirst)->iNext;\
							       else\
							         NodePtr((mp), (node)->iBack)->iNext = (node)->iNext;\
							      if ((node)->iNext == 0)\
							          (list)->iLast = NodePtr((mp),(list)->iLast)->iBack;\
							       else\
							          NodePtr((mp), (node)->iNext)->iBack = (node)->iBack;\
									 (node)->iBack = 0;\
									 (node)->iNext = 0;

#define MPAddNode1(mp,list,node)  (node)->iBack = NodePtr((mp),(list)->iLast)->iBack;\
                                  (node)->iNext = (list)->iLast;\
                                  NodePtr((mp),(node)->iBack)->iNext = Offset((mp), (node));\
                                  NodePtr((mp),(list)->iLast)->iBack = Offset((mp), (node));

#define MPDelNode1(mp,list,node)  NodePtr((mp),(node)->iBack)->iNext = (node)->iNext;\
                                  NodePtr((mp),(node)->iNext)->iBack = (node)->iBack;\
                                  (node)->iBack = 0; \
								  (node)->iNext = 0;
                                       
 
////////////////////////////////////////////////////
ptrdiff_t MPIdxFind(MemPool *mp, size_t size1, size_t size2);

////////////////////////////////////////////////////
MPEnum mpLastErrorCode;  //最后一次错误代码
////////////////////////////////////////////////////

MemPool* MPCreate(void *mem, size_t mSize, size_t idxNumber)
{
	if (mem == NULL || mSize == 0 || idxNumber == 0)
	{  
		mpLastErrorCode = MemSizeIllegal;
		return NULL;
	}
	size_t idxSize = idxNumber * sizeof(MPIdx);
	  
	if (idxSize >= mSize)
	{
		mpLastErrorCode = IdxMemGtrAllMem;
		return NULL;
	}
		
	MPHead *mph = (MPHead *)mem;

	mph->nSize = mSize;
	mph->iUse = ObjSize(MPHead, idxs, idxSize);
	mph->iNum = 0;
	mph->iMax = idxNumber;
	//清0索引
	memset(mph->idxs, 0, idxSize);

	return mph;
}
////////////////////////////////////////////////////
void* MPAlloc(MemPool *mp, size_t size)
{
	static ptrdiff_t iFind;
	static size_t iCacheSize;
	MPList *ls = NULL;
	MPListNode *node = NULL;
	
	if (iCacheSize != size)
	{
		iCacheSize = size;
		iFind = MPIdxFind(mp, size, size);
	}

	//查找完全匹配size大小的链表下标
	//iFind = MPIdxFind(mp, size, size);

	//如果未找到size大小的内存块链表
	if (iFind == -1)
	{
		//如果下标数组已经用完
		if (mp->iNum == mp->iMax)
		{
			mpLastErrorCode = RunOutOfIdxs;
			return NULL;
		}
		else
		{
			const size_t szList = sizeof(MPList) * 2;
			const size_t szNode = sizeof(MPListEnd) * 2;
			const size_t nSize = szList + szNode + ObjSize(MPListNode, data, size);

			//如果所剩内存不够分配:链表、rend/end节点
			//则停止分配
			if (mp->iUse + nSize > mp->nSize)
			{
				mpLastErrorCode = RunOutOfMemory;
				return NULL;
			}

		   iFind = MPIdxFind(mp, size, 0);
		   //没有找到合适的索引(哈希失败)
		   if (iFind == -1)
		   {
			   mpLastErrorCode = RunOutOfIdxs;
			   return  NULL;
		   }

		   //设置idx的大小
		   mp->idxs[iFind].size = size;

		   //把当前空闲块分割出来作为链表头
		   mp->idxs[iFind].idx = mp->iUse;

		   //增加索引数
		   ++mp->iNum;

		   ls = ListPtr(mp, mp->iUse);
		   //清零空闲链表
		   ls[0].iFirst = 0;
		   ls[0].iLast = 0;
		   //使用表分别指向rend和end
		   const ptrdiff_t iREnd = mp->iUse + szList;
		   const ptrdiff_t iEnd = iREnd + sizeof(MPListEnd);
		   ls[1].iREnd = iREnd;
		   ls[1].iEnd  = iEnd;

		   //分配两个end节点
		   MPListNode *rend = NodePtr(mp, iREnd);
		   MPListNode *end =  NodePtr(mp, iEnd);
		   rend->iBack = 0;
		   rend->iNext = iEnd;
		   end->iBack  = iREnd;
		   end->iNext  = 0;

		   //内存使用量增加
		   mp->iUse += szList + szNode;
		}
	}
	else
	{
		ls = ListPtr(mp, mp->idxs[iFind].idx);
		//第一个空闲节点
		if (ls->iFirst != 0)
		{
			node = NodePtr(mp, ls->iFirst);
			//从空闲链表中删除
			MPDelNode0(mp, ls, node);
			//加入已使用的链表
			MPAddNode1(mp, ls + 1, node);
		}
			
	}

	//如果链表中没有空闲节点
	//则创建新的节点
	if (node == NULL)
	{
		const size_t nSize = ObjSize(MPListNode, data, size);

		//假设分配容量大于总容量，则分配失败
		if (mp->iUse + nSize > mp->nSize)
		{
			mpLastErrorCode = RunOutOfMemory;
			return NULL;
		}
		
		node = NodePtr(mp, mp->iUse);
		node->iBack = 0;
		node->iNext = 0;
		//指向链表首地址
		node->iList = Offset(mp, ls);

		//加入到使用链表中
		MPAddNode1(mp, ls + 1, node);

		//内存使用量增加
		mp->iUse += nSize;
	}

	//标记为已使用
	node->isUse = 1;

	mpLastErrorCode = OK;
	//返回用户内存
	return PB(node) + OffsetMember(MPListNode, data);
}

void MPFree(MemPool *mp, void *mem)
{
	MPListNode *node = NodePtr(mem ,-OffsetMember(MPListNode, data));
	MPList *ls = ListPtr(mp, node->iList);

	//已经释放过了，无需再次释放
	if (node->isUse == 0)
		return;

	//从已使用链表中删除
	MPDelNode1(mp, ls + 1, node);
	
	//加入空闲链表中
	MPAddNode0(mp, ls, node);

	//标记为未使用
	node->isUse = 0;

	mpLastErrorCode = OK;
}
 
////////////////////////////////////////////////////
static void MPIterCreate(MemPool *mp, MPIter *iter, size_t size, _MPEnum iterType)
{
	static size_t iCacheSize;
	static ptrdiff_t iFind;	
	MPList *ls;	

	if (iCacheSize != size)
	{
		iFind = MPIdxFind(mp, size, size);
		iCacheSize = size;
	}
		
	if (iFind == -1)
	{
		mpLastErrorCode = MemSizeIllegal;
		return ;
	}

	ls = ListPtr(mp, mp->idxs[iFind].idx) + 1;
	iter->mp = mp;
	iter->type = iterType;
	switch (iterType)
	{
	case IterBegin:
		iter->iNode =  NodePtr(mp,ls->iREnd)->iNext;
		break;
	case IterEnd:
		iter->iNode = ls->iEnd;
		break;
	case IterRBegin:
		iter->iNode = NodePtr(mp, ls->iEnd)->iBack;
		break;
	case IterREnd:
		iter->iNode = ls->iREnd;
		break;
	}
	mpLastErrorCode = OK;
}
 

MPIter  MPIterBegin(MemPool *mp, size_t size)
{
	MPIter it;

	MPIterCreate(mp, &it, size, IterBegin);

	return it;
}
MPIter  MPIterEnd(MemPool *mp, size_t size)
{
	MPIter it;

	MPIterCreate(mp, &it, size, IterEnd);

	return it;
}
MPIter  MPIterRBegin(MemPool *mp, size_t size)
{
	MPIter it;

	MPIterCreate(mp, &it, size, IterRBegin);

	return it;
}
MPIter  MPIterREnd(MemPool *mp, size_t size)
{
	MPIter it;

	MPIterCreate(mp, &it, size, IterREnd);

	return it;
}

void* MPIterPtr_(MPIter* iter)
{
	if (iter->mp == NULL)
	{
		mpLastErrorCode = PtrNull;
		return NULL;
	}
	else
	{
		MPListNode *node = NodePtr(iter->mp, iter->iNode);

		if (node->iBack == 0)
		{
			mpLastErrorCode = IterREnd;
			return NULL;

		}
		else if (node->iNext == 0)
		{
			mpLastErrorCode = IterEnd;
			return NULL;
		}

		mpLastErrorCode = OK;

		return node->data;
	}
		
}

void MPIterNext_(MPIter* iter)
{
	MPListNode *node;

	if (iter->mp == NULL)
	{
		mpLastErrorCode = PtrNull;
		return;
	}
	node = NodePtr(iter->mp, iter->iNode);

	if (iter->type == IterBegin || iter->type == IterEnd)
	{
		if (node->iNext == 0)
		{
			mpLastErrorCode = IterEnd;
			return;
		}
		iter->iNode = node->iNext;

	}
	else if (iter->type == IterRBegin || iter->type == IterREnd)
	{
		if (node->iBack == 0)
		{
			mpLastErrorCode = IterREnd;
			return;
		}
		iter->iNode = node->iBack;
	}
	mpLastErrorCode = OK;
 
}

void MPIterBack_(MPIter* iter)
{
	MPListNode *node;

	if (iter->mp == NULL)
	{
		mpLastErrorCode = PtrNull;
		return;
	}

	node = NodePtr(iter->mp, iter->iNode);

	if (iter->type == IterBegin || iter->type == IterEnd)
	{
		if (node->iBack == 0)
		{
			mpLastErrorCode = IterREnd;
			return;
		}
		iter->iNode = node->iBack;

	}
	else if (iter->type == IterRBegin || iter->type == IterREnd)
	{
		if (node->iNext == 0)
		{
			mpLastErrorCode = IterEnd;
			return;
		}
		iter->iNode = node->iNext;
	}
	mpLastErrorCode = OK;
}

bool MPIterEq_(MPIter *it1, MPIter* it2)
{  
	const MPByte v = abs_(it1->type - it2->type);

	//迭代器类型不同
	if (v > 1)
	{
		mpLastErrorCode = IterDifferent;
		return false;
	}

	//是一对[b,e]区间迭代器的时候
 	if( v > 0)
		return it1->iNode != it2->iNode;

	mpLastErrorCode = OK;

	return it1->iNode == it2->iNode;
}

///////////////索引处理//////////////////////
typedef  unsigned int uint32_t;
uint32_t hash1(uint32_t);
uint32_t hash2(uint32_t);
uint32_t hash3(uint32_t);
uint32_t hash4(uint32_t);
uint32_t hash5(uint32_t);
uint32_t hash6(uint32_t);
uint32_t hash7(uint32_t);

ptrdiff_t MPIdxFind(MemPool *mp, size_t size1,size_t size2)
{
	static  uint32_t(*const hash[])(uint32_t) =
	{
		hash1,
		hash2,
		hash3,
		hash4,
		hash5,
		hash6,
		hash7,
		NULL
	};
	uint32_t(*const *pHash)(uint32_t) = hash;
	ptrdiff_t idx;

	for (; *pHash; ++pHash)
	{
		 idx = (*pHash)(size1) % mp->iMax;
		 		 
		//如果找到下标则返回
		if (mp->idxs[idx].size == size2)
			return idx;
		 
	}
	return -1;
}

///////////////hash函数表//////////////////////
 uint32_t hash1(uint32_t a)
{
	a = (a ^ 61) ^ (a >> 16);
	a = a + (a << 3);
	a = a ^ (a >> 4);
	a = a * 0x27d4eb2d;
	a = a ^ (a >> 15);
	return a;
}

 uint32_t hash2(uint32_t a)
{
	a = ~a + (a << 15); 
	a = a ^ (a >> 12);
	a = a + (a << 2);
	a = a ^ (a >> 4);
	a = a * 2057; 
	a = a ^ (a >> 16);
	return a;
}


 uint32_t hash3(uint32_t a)
{
	a -= (a << 6);
	a ^= (a >> 17);
	a -= (a << 9);
	a ^= (a << 4);
	a -= (a << 3);
	a ^= (a << 10);
	a ^= (a >> 15);
	return a;
}

 uint32_t hash4(uint32_t a)
{
	a += ~(a << 15);
	a ^= (a >> 10);
	a += (a << 3);
	a ^= (a >> 6);
	a += ~(a << 11);
	a ^= (a >> 16);

	return a;
}


 uint32_t hash5(uint32_t h)
{
	h ^= (h >> 20) ^ (h >> 12);
	return h ^ (h >> 7) ^ (h >> 4);
}

 uint32_t hash6(uint32_t a)
{
	a = a ^ (a >> 4);
	a = (a ^ 0xdeadbeef) + (a << 5);
	a = a ^ (a >> 11);
	return a;
}
uint32_t hash7(uint32_t a)
 {
	 a ^= (a >> 16);
	 return ((a >> 8) ^ a);
 }