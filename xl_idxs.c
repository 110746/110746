#include "xl_idxs.h"
#include <malloc.h>
//无效下标
#define IDXS_INVALID_IDX ((size_t)-1)
//已分配内存标记
#define IDXS_ALLOC_YES 1
//未分配内存标记
#define IDXS_ALLOC_NO 0


idxs_head* idxs_create(size_t size)
{
	idxs_head* ph = (idxs_head*)malloc(sizeof(idxs_head));
	//可用表为空
	ph->tyf = IDXS_INVALID_IDX;
	ph->tyl = IDXS_INVALID_IDX;
	ph->tnf = IDXS_INVALID_IDX;
	ph->tnl = IDXS_INVALID_IDX;

	ph->size = sizeof(idxs_ip) + size;
	ph->n = 0;

	return ph;
}



//flag为1则添加到可用表中，0则添加到不可用表中
//添加，从表头添加
static void idxs_addobj(idxs_head* ph, void* obj, int flag)
{  
	char* pf = (char*)(ph+1);

	size_t *tf = flag ? &ph->tnf : &ph->tyf;
	size_t *tl = flag ? &ph->tnl : &ph->tyl;

	char* pip =  (char*)obj - sizeof(idxs_ip);

	//获取成员偏移量
	size_t ofnext = flag ? offsetof(idxs_ip, inn) : offsetof(idxs_ip, iyn);
	size_t ofback = flag ? offsetof(idxs_ip, inb) : offsetof(idxs_ip, iyb);
	size_t pipidx =  *(size_t*)(pip + offsetof(idxs_ip, idx));

	if (*tf != IDXS_INVALID_IDX)
	{
		//获得列表中第一个ip
		char* pip1 = pf + *tf;
		//将pip插入到pip1前面
		*(size_t*)(pip + ofnext) = *tf;

		*(size_t*)(pip1 + ofback) =  pipidx;
	}
	else
	{
		//如果列表为空
		//那么两个"指针"都指向这个被删除的ip
		*tl =  pipidx;
	}

	*(size_t*)(pip + ofback) = IDXS_INVALID_IDX;

	*tf = pipidx;
}

//删除，从表尾删除
static void idxs_delobj(idxs_head* ph,void* obj, int flag)
{
	char* pip =  (char*)obj - sizeof(idxs_ip);

	char* pf = (char*)(ph+1);

	size_t *tf = flag ? &ph->tnf : &ph->tyf;
	size_t *tl = flag ? &ph->tnl : &ph->tyl;
	
	//获取成员偏移量
	size_t ofnext = flag ? offsetof(idxs_ip, inn) : offsetof(idxs_ip, iyn);
	size_t ofback = flag ? offsetof(idxs_ip, inb) : offsetof(idxs_ip, iyb);
	size_t pipnext =  *(size_t*)(pip + ofnext);
	size_t pipback =  *(size_t*)(pip + ofback);

	if (pipback == IDXS_INVALID_IDX)
		*tf = pipnext;
	else
		*(size_t*)(pf + pipback + ofnext) = pipnext;

	if (pipnext == IDXS_INVALID_IDX)
		*tl = pipback;
	else
		*(size_t*)(pf + pipnext + ofback) = pipback;

}

void*  idxs_create_obj(idxs_head** idxs)
{
	idxs_head* ph = *idxs;
	char* pf= (char*)(ph+1);

	//没有对象可用的时候
	if (ph->tyf == IDXS_INVALID_IDX)
	{
		//增长一倍容量
		int n = ph->n ? ph->n*2 : 1;
		*idxs = (idxs_head*)realloc(ph, sizeof(idxs_head) + ph->size * n);
		ph = *idxs;

		//列表起始位置
		pf = (char*)(ph+1);

		//指向新列表中第1个节点
		ph->tyf = ph->n * ph->size;
		//指向倒数第1个节点
		ph->tyl = (n-1) * ph->size;

		//新列表起始位置
		char* pnlf = pf + ph->tyf;
		//新列表结束位置
		char* pnll = pf  + n * ph->size;

		idxs_ip* pip0 = NULL;
		idxs_ip* pip1 = NULL;

		/////////初始化新表/////////////
		((idxs_ip*)pnlf)->iyb = IDXS_INVALID_IDX;
		((idxs_ip*)(pnll-ph->size))->iyn = IDXS_INVALID_IDX;
		while (pnlf != pnll)
		{
			pip1 = (idxs_ip*)pnlf;
			pip1->idx = pnlf - pf;

			if (pip0)
			{
				pip0->iyn = pip1->idx;
				pip1->iyb = pip0->idx;
			}

			pip1->inb = IDXS_INVALID_IDX;
			pip1->inn = IDXS_INVALID_IDX;
			pip1->alloc = IDXS_ALLOC_NO;

			pip0 = pip1;
			pnlf += ph->size;
		}

		ph->n = n;
	}
	
	idxs_ip* pip = (idxs_ip*)(pf + ph->tyl);
	//从未分配表中删除对象
	idxs_delobj(ph, pip+1, IDXS_ALLOC_NO);
	//添加对象到已分配表中
	idxs_addobj(ph, pip+1, IDXS_ALLOC_YES);
	//标记已被使用
	pip->alloc = IDXS_ALLOC_YES;

	return  pip + 1;

}

void idxs_del_idx(idxs_head* ph, size_t idx)
{
	void* obj = idxs_idx_getobj(ph, idx);

	//已经回收过一次的,无需再回收
	//否则出现无限循环
	if (obj == NULL)
		return;

	idxs_ip* pip =  (idxs_ip*)((char*)obj - sizeof(idxs_ip));

	//添加对象到未分配表中
	idxs_addobj(ph, obj, IDXS_ALLOC_NO);
	//从已分配表中删除对象
	idxs_delobj(ph, obj, IDXS_ALLOC_YES);
	//标记未被使用
	pip->alloc = IDXS_ALLOC_NO;

}


size_t idxs_obj_getidx(idxs_head* ph, void* obj)
{
	idxs_ip* pip = (idxs_ip*)((char*)obj - sizeof(idxs_ip));

	//如果此块内存已经被回收了
	if (pip->alloc == IDXS_ALLOC_NO)
		return IDXS_INVALID_IDX;

	return pip->idx;

}

void* idxs_idx_getobj(idxs_head* ph, size_t idx)
{
	if (idx == IDXS_INVALID_IDX)
		return NULL;

	idxs_ip* pip = (idxs_ip*)((char*)(ph+1) + idx);

	//如果此块内存已经被回收了
	if (pip->alloc == IDXS_ALLOC_NO)
		return NULL;

	return pip + 1;
}

