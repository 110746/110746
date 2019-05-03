#ifndef XL_C_ARRAY_H
#define XL_C_ARRAY_H
#include <stdlib.h>

typedef struct _array
{
	void *ary;      //数组指针
	unsigned size;  //每个元素大小
	unsigned n;    //已使用的数目
	unsigned cn;   //容量数目
	//如何重新分配元素个数
	//on为旧的元素数目，
	//nn为新的元素数目
	//void*为自定义数据
	//返回重新分配后的元素数目
	unsigned (*f_realloc)(unsigned on,unsigned nn,void*);
	void *param;
}array,*parray;

//创建具有size个字节大小的n个元素的数组,并用obj初始化
array  ary_create(unsigned n,unsigned size,void *obj);
void   ary_destroy(parray ary);
//把obj插入到pos的前面或后面
//isFront为真则插入前面,否则后面
//int  ary_insert(parray ary,void* pos, void* obj,int isFront);
//把begin和end范围内的元素插入到obj的前面或后面
//返回新插入的数组的首地址 
void*  ary_inserts(parray ary, void* pos, void* begin, void* end, int isFront);
//从数组中删除begin和end范围内的所有元素
//返回begin
void*  ary_erases(parray ary, void* begin, void* end);
//设置元素如何重新分配，param为传给f_realloc的自定义数据
void  ary_set_realloc(parray ary, unsigned (*f_realloc)(unsigned, unsigned, void*), void* param);

#define ary_ary(pary)   ((pary)->ary)
#define ary_size(pary)  ((pary)->size)
#define ary_length(pary)  ((pary)->n)
#define ary_capacity(pary)  ((pary)->cn)
#define ary_ptr_first(pary) ary_ary((pary))
#define ary_ptr_last(pary)   ((char*)ary_ary((pary)) + (ary_length((pary)) ? ary_length((pary)) -1 : 0) * ary_size((pary)))
#define ary_ptr_begin(pary) ary_ptr_first((pary))
#define ary_ptr_end(pary)    ((char*)ary_ptr_last((pary)) +  ary_size((pary)))
#define ary_insert(pary,pos, obj,isFront) ary_inserts((pary), (pos), (obj), (char*)(obj) + (pary)->size, (isFront))
#define ary_push_front(pary, obj) ary_insert((pary),ary_ary((pary)) ,(obj), 1)
#define ary_push_back(pary, obj)   ary_insert((pary),ary_ptr_last((pary)),(obj), 0) 
#define ary_erase(pary,pos) ary_erases((pary),(pos), ((pos) + 1))


#endif