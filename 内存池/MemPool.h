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

//���ж�̬�����Ա�Ķ���������ڴ�
#define  ObjSize(cname,mname,size)   (OffsetMember(cname,mname) + size)
//��ȡ���һ�δ������
#define  MPGetLastEorrCode()  mpLastErrorCode
 
typedef  unsigned char MPByte;

typedef enum _MPEnum
{
	OK,
	PtrNull,
	IdxMemGtrAllMem,    //�����ڴ�������ڴ�
	IdxIllegal,          //�Ƿ�����
	RunOutOfMemory,     //�ڴ��Ѻľ�
	RunOutOfIdxs,       //����������
	MemSizeIllegal,     //�Ƿ����ڴ���С
//////////////////
	IterBegin,          //������ָ����Ԫ��
	IterEnd,            //������ָ��βԪ�ص���һλ��
	null,               //�ָ������������ֵ���������(ǧ����ɾ��)
	IterRBegin,         //������ָ��βԪ��
	IterREnd,           //������ָ����Ԫ�ص���һλ��
//////////////////
	IterDifferent,      //���������Ͳ�ͬ
}MPEnum;
extern _MPEnum mpLastErrorCode;

///////n ��ʾ��С����ĸ,i��ʾ��������ĸ///////////

typedef struct _MPIdx
{
	size_t size;
	ptrdiff_t idx;

}MPIdx;

typedef struct _MPHead
{
	size_t nSize;      //�ڴ���С
	ptrdiff_t iUse;    //��ʹ�õ��ֽ���

	size_t   iNum;     //��ʹ�õ�������
	ptrdiff_t iMax;     //idxs����Ŀ
	MPIdx idxs[1];     //������������(��̬����)

}MPHead,MemPool;

typedef struct _MPList
{ 
	union 
	{
		ptrdiff_t  iFirst;  //δ�����������׽ڵ�ƫ��
		ptrdiff_t  iREnd;   //rend
	};
	union
	{
		ptrdiff_t  iLast;   //δ����������β�ڵ�ƫ��
		ptrdiff_t  iEnd;   //end
	};
}MPList;

////////�ڵ�////////////
typedef struct _MPListNode
{
	ptrdiff_t  iNext;    //ָ����һ�ڵ�
	ptrdiff_t  iBack;    //ָ����һ���ڵ�
	ptrdiff_t  iList;    //�ڵ���������λ���ڴ���е�λ��
	MPByte       isUse;    //�Ƿ�����ʹ����(����ظ��ͷŶ�����ǵ�)

	MPByte  data[1];       //�û�����(��̬����)

}MPListNode;

////////rend/end�ڵ�////////////
typedef struct _MPListEnd
{
	ptrdiff_t  iNext;    //ָ����һ�ڵ�
	ptrdiff_t  iBack;    //ָ����һ���ڵ�

}MPListEnd;

typedef struct _MPIter
{
	ptrdiff_t iNode;  //��һ�ڵ�ָ��
	MemPool *mp;
	MPEnum type;
}MPIter;


//////////////////////////////////////////////////


//��mSize��С���ڴ�mem��Ϊ�ڴ��
//idxNumber����ָ��������Ŀ(������Ŀ)
MemPool* MPCreate(void *mem, size_t mSize, size_t idxNumber);
//����һ���ڴ�
void* MPAlloc(MemPool *mp, size_t size);
//�ͷ��ڴ�mem
void MPFree(MemPool *mp, void *mem);

//����size��С������ж���
//����iterType���Դ�ͷ��β��ʼ����
MPIter  MPIterBegin(MemPool *mp,size_t size);
MPIter  MPIterEnd(MemPool *mp, size_t size);
MPIter  MPIterRBegin(MemPool *mp, size_t size);
MPIter  MPIterREnd(MemPool *mp, size_t size);


//ȡ�õ�ǰ�ڵ��ָ��
void* MPIterPtr_(MPIter* iter);
//ǰ������
void MPIterNext_(MPIter* iter);
//���˱���
void MPIterBack_(MPIter* iter);
//�Ƚ������������Ƿ����
bool MPIterEq_(MPIter *it1, MPIter* itr2);


#endif // !MEM_POOL_H

