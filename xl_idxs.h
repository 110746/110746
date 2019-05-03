#ifndef  XL_IDXS_H
#define  XL_IDXS_H
#include <stddef.h>
#include <malloc.h>

typedef struct
{
	size_t tyf;       //未分配对象下标链表头
	size_t tyl;       //未分配对象下标链表尾
	size_t tnf;       //已分配出去对象的链表头
	size_t tnl;       //已分配出去对象的链表尾
	size_t size;      //每个对象的大小（包括idxs_ip的大小)
	size_t n;         //已分配的内存块容纳的对象数目
}idxs_head;
 
typedef struct
{
	size_t iyn; //"next指针"
	size_t iyb; //"back"指针
	size_t inn;
	size_t inb;     
	size_t idx;   //ip所在下标
	int alloc; //是否已经被分配了
}idxs_ip;


//创建一个下标表,size为每个对象的大小
idxs_head* idxs_create(size_t size);
//销毁一个下标表
#define idxs_destroy(ph) free(ph)

//从下标表中创建一个size大小的对象
void*  idxs_create_obj(idxs_head** idxs);

//通过下标删除一个对象
void idxs_del_idx(idxs_head* ph, size_t idx);

//获取对 象在下标表中的索引,如果对象已被回收，则返回-1
size_t idxs_obj_getidx(idxs_head* ph, void* obj);
//获取下标所代表的对象,如果对象已被回收，则返回NULL
void* idxs_idx_getobj(idxs_head* ph, size_t idx);

//获取第一个迭代器
#define idxs_iter_frist(ph) (ph->tnl)
//获取最后一个迭代器
#define idxs_iter_last(ph)  (ph->tnf)
//返回下一个迭代器
#define idxs_iter_next(obj)  ((idxs_ip*)((char*)obj-sizeof(idxs_ip)))->inb
//返回上一个迭代器
#define idxs_iter_back(obj)  ((idxs_ip*)((char*)obj-sizeof(idxs_ip)))->inn

/*
 迭代器使用示例:

 ////顺序遍历/////
 size_t idx = idxs_iter_frist(ph);

 while (idx != -1)
 {
 int* obj = idxs_idx_getobj(ph, idx);
 printf("%-3d", *obj);

 idx = idxs_iter_next(obj);
 }


 ////逆序遍历/////
 size_t idx = idxs_iter_last(ph);

 while (idx != -1)
 {
 int* obj = idxs_idx_getobj(ph, idx);
 printf("%-3d", *obj);

 idx = idxs_iter_back(obj);
 }
*/

#endif
