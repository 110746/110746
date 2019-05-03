#ifndef  XL_IDXS_H
#define  XL_IDXS_H
#include <stddef.h>
#include <malloc.h>

typedef struct
{
	size_t tyf;       //δ��������±�����ͷ
	size_t tyl;       //δ��������±�����β
	size_t tnf;       //�ѷ����ȥ���������ͷ
	size_t tnl;       //�ѷ����ȥ���������β
	size_t size;      //ÿ������Ĵ�С������idxs_ip�Ĵ�С)
	size_t n;         //�ѷ�����ڴ�����ɵĶ�����Ŀ
}idxs_head;
 
typedef struct
{
	size_t iyn; //"nextָ��"
	size_t iyb; //"back"ָ��
	size_t inn;
	size_t inb;     
	size_t idx;   //ip�����±�
	int alloc; //�Ƿ��Ѿ���������
}idxs_ip;


//����һ���±��,sizeΪÿ������Ĵ�С
idxs_head* idxs_create(size_t size);
//����һ���±��
#define idxs_destroy(ph) free(ph)

//���±���д���һ��size��С�Ķ���
void*  idxs_create_obj(idxs_head** idxs);

//ͨ���±�ɾ��һ������
void idxs_del_idx(idxs_head* ph, size_t idx);

//��ȡ�� �����±���е�����,��������ѱ����գ��򷵻�-1
size_t idxs_obj_getidx(idxs_head* ph, void* obj);
//��ȡ�±�������Ķ���,��������ѱ����գ��򷵻�NULL
void* idxs_idx_getobj(idxs_head* ph, size_t idx);

//��ȡ��һ��������
#define idxs_iter_frist(ph) (ph->tnl)
//��ȡ���һ��������
#define idxs_iter_last(ph)  (ph->tnf)
//������һ��������
#define idxs_iter_next(obj)  ((idxs_ip*)((char*)obj-sizeof(idxs_ip)))->inb
//������һ��������
#define idxs_iter_back(obj)  ((idxs_ip*)((char*)obj-sizeof(idxs_ip)))->inn

/*
 ������ʹ��ʾ��:

 ////˳�����/////
 size_t idx = idxs_iter_frist(ph);

 while (idx != -1)
 {
 int* obj = idxs_idx_getobj(ph, idx);
 printf("%-3d", *obj);

 idx = idxs_iter_next(obj);
 }


 ////�������/////
 size_t idx = idxs_iter_last(ph);

 while (idx != -1)
 {
 int* obj = idxs_idx_getobj(ph, idx);
 printf("%-3d", *obj);

 idx = idxs_iter_back(obj);
 }
*/

#endif
