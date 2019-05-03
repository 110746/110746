#include "xl_c_array.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

array  ary_create(unsigned n, unsigned size, void *obj)
{
	array ary = { 0 };

	ary.size = size;
	if (n > 0 && size > 0)
	{
		char *mem = (char*)malloc(size * n);
		void *val = obj;
		//如果obj为NULL,分配一个为0的对象
		if (obj == NULL)
			val = calloc(1, size);

		
		//使用obj初始化n个对象
		for (unsigned i = 0; i < n; ++i)
			memcpy(mem + i * size, val, size);
		
		if (obj == NULL)
			free(val);

		ary.ary = mem;
		
		ary.n = n;
		ary.cn = n;
		
    }
	return ary;
}

void ary_destroy(parray ary)
{
	static array temp = { 0 };
	free(ary->ary);
	*ary = temp;
}


void*  ary_inserts(parray ary, void* pos,void* begin, void* end, int isFront)
{
	char *aary = (char*)ary->ary;
	unsigned size = ary->size;
	unsigned n = ary->n;

	if (n != 0)
	{
		if (!(pos >= aary && pos < aary + size * ary->n))
			return NULL;
    }


	//记录pos的位置
	unsigned npos = (char*)pos - aary;
	//计算begin~end之间的元素数目
	unsigned nr = ((char*)end - (char*)begin) / size;


	if (n + nr > ary->cn)
	{
		if (ary->f_realloc)
			ary->cn = ary->f_realloc(n, n + nr,ary->param);
		else
			ary->cn = ary->cn ? (ary->cn + nr) * 2 : 4;

		ary->ary = realloc(aary, ary->cn * size);

		aary = (char*)ary->ary;
	}

	//把数据往后移
	char *newpos = n ? (isFront ? aary + npos : aary + npos + size) : aary;
	//当前已使用的空间最后一个元素地址
	char *last0 = aary + size * (n - 1);
	//插入数组后的的空间最后一个元素地址
	char *last1 = last0 + size * nr;
	//复制
	while (last0 >= newpos)
	{
		memcpy(last1, last0, size);
		last0 -= size;
		last1 -= size;
	}
		 
	//插入到空位置中
	memcpy(newpos, begin, size * nr);
	ary->n += nr;

	return newpos;
}

void*  ary_erases(parray ary, void* begin, void* end)
{
	char *aary = (char*)ary->ary;
	unsigned size = ary->size;
	void *enda = aary + size * ary->n;
	void *posdel;
	unsigned nr;
	

	if (!((begin >= aary && begin < enda) &&
		(end > aary && end <= enda) &&
		begin < end))
		return NULL;

	nr = ((char*)end - (char*)begin)/size;
	posdel = begin;

	while (end != enda)
	{
		memcpy(begin,end, size);
		begin = (char*)begin + size;
		end = (char*)end + size;
	}

	ary->n -= nr;

	return posdel;
}

void  ary_set_realloc(parray ary, unsigned (*f_realloc)(unsigned, unsigned, void*), void* param)
{
	ary->f_realloc = f_realloc;
	ary->param = param;
}