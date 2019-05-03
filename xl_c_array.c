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
		//���objΪNULL,����һ��Ϊ0�Ķ���
		if (obj == NULL)
			val = calloc(1, size);

		
		//ʹ��obj��ʼ��n������
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


	//��¼pos��λ��
	unsigned npos = (char*)pos - aary;
	//����begin~end֮���Ԫ����Ŀ
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

	//������������
	char *newpos = n ? (isFront ? aary + npos : aary + npos + size) : aary;
	//��ǰ��ʹ�õĿռ����һ��Ԫ�ص�ַ
	char *last0 = aary + size * (n - 1);
	//���������ĵĿռ����һ��Ԫ�ص�ַ
	char *last1 = last0 + size * nr;
	//����
	while (last0 >= newpos)
	{
		memcpy(last1, last0, size);
		last0 -= size;
		last1 -= size;
	}
		 
	//���뵽��λ����
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