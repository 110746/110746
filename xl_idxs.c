#include "xl_idxs.h"
#include <malloc.h>
//��Ч�±�
#define IDXS_INVALID_IDX ((size_t)-1)
//�ѷ����ڴ���
#define IDXS_ALLOC_YES 1
//δ�����ڴ���
#define IDXS_ALLOC_NO 0


idxs_head* idxs_create(size_t size)
{
	idxs_head* ph = (idxs_head*)malloc(sizeof(idxs_head));
	//���ñ�Ϊ��
	ph->tyf = IDXS_INVALID_IDX;
	ph->tyl = IDXS_INVALID_IDX;
	ph->tnf = IDXS_INVALID_IDX;
	ph->tnl = IDXS_INVALID_IDX;

	ph->size = sizeof(idxs_ip) + size;
	ph->n = 0;

	return ph;
}



//flagΪ1����ӵ����ñ��У�0����ӵ������ñ���
//��ӣ��ӱ�ͷ���
static void idxs_addobj(idxs_head* ph, void* obj, int flag)
{  
	char* pf = (char*)(ph+1);

	size_t *tf = flag ? &ph->tnf : &ph->tyf;
	size_t *tl = flag ? &ph->tnl : &ph->tyl;

	char* pip =  (char*)obj - sizeof(idxs_ip);

	//��ȡ��Աƫ����
	size_t ofnext = flag ? offsetof(idxs_ip, inn) : offsetof(idxs_ip, iyn);
	size_t ofback = flag ? offsetof(idxs_ip, inb) : offsetof(idxs_ip, iyb);
	size_t pipidx =  *(size_t*)(pip + offsetof(idxs_ip, idx));

	if (*tf != IDXS_INVALID_IDX)
	{
		//����б��е�һ��ip
		char* pip1 = pf + *tf;
		//��pip���뵽pip1ǰ��
		*(size_t*)(pip + ofnext) = *tf;

		*(size_t*)(pip1 + ofback) =  pipidx;
	}
	else
	{
		//����б�Ϊ��
		//��ô����"ָ��"��ָ�������ɾ����ip
		*tl =  pipidx;
	}

	*(size_t*)(pip + ofback) = IDXS_INVALID_IDX;

	*tf = pipidx;
}

//ɾ�����ӱ�βɾ��
static void idxs_delobj(idxs_head* ph,void* obj, int flag)
{
	char* pip =  (char*)obj - sizeof(idxs_ip);

	char* pf = (char*)(ph+1);

	size_t *tf = flag ? &ph->tnf : &ph->tyf;
	size_t *tl = flag ? &ph->tnl : &ph->tyl;
	
	//��ȡ��Աƫ����
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

	//û�ж�����õ�ʱ��
	if (ph->tyf == IDXS_INVALID_IDX)
	{
		//����һ������
		int n = ph->n ? ph->n*2 : 1;
		*idxs = (idxs_head*)realloc(ph, sizeof(idxs_head) + ph->size * n);
		ph = *idxs;

		//�б���ʼλ��
		pf = (char*)(ph+1);

		//ָ�����б��е�1���ڵ�
		ph->tyf = ph->n * ph->size;
		//ָ������1���ڵ�
		ph->tyl = (n-1) * ph->size;

		//���б���ʼλ��
		char* pnlf = pf + ph->tyf;
		//���б����λ��
		char* pnll = pf  + n * ph->size;

		idxs_ip* pip0 = NULL;
		idxs_ip* pip1 = NULL;

		/////////��ʼ���±�/////////////
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
	//��δ�������ɾ������
	idxs_delobj(ph, pip+1, IDXS_ALLOC_NO);
	//��Ӷ����ѷ������
	idxs_addobj(ph, pip+1, IDXS_ALLOC_YES);
	//����ѱ�ʹ��
	pip->alloc = IDXS_ALLOC_YES;

	return  pip + 1;

}

void idxs_del_idx(idxs_head* ph, size_t idx)
{
	void* obj = idxs_idx_getobj(ph, idx);

	//�Ѿ����չ�һ�ε�,�����ٻ���
	//�����������ѭ��
	if (obj == NULL)
		return;

	idxs_ip* pip =  (idxs_ip*)((char*)obj - sizeof(idxs_ip));

	//��Ӷ���δ�������
	idxs_addobj(ph, obj, IDXS_ALLOC_NO);
	//���ѷ������ɾ������
	idxs_delobj(ph, obj, IDXS_ALLOC_YES);
	//���δ��ʹ��
	pip->alloc = IDXS_ALLOC_NO;

}


size_t idxs_obj_getidx(idxs_head* ph, void* obj)
{
	idxs_ip* pip = (idxs_ip*)((char*)obj - sizeof(idxs_ip));

	//����˿��ڴ��Ѿ���������
	if (pip->alloc == IDXS_ALLOC_NO)
		return IDXS_INVALID_IDX;

	return pip->idx;

}

void* idxs_idx_getobj(idxs_head* ph, size_t idx)
{
	if (idx == IDXS_INVALID_IDX)
		return NULL;

	idxs_ip* pip = (idxs_ip*)((char*)(ph+1) + idx);

	//����˿��ڴ��Ѿ���������
	if (pip->alloc == IDXS_ALLOC_NO)
		return NULL;

	return pip + 1;
}

