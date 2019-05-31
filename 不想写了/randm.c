#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//把b~e(不包含e)之间的随机数生成在具有n个元素数组的ary中
//返回实际生成的随机数，数目
//错误则返回-1
int rand_range(int *ary, int n, int b, int e)
{
	if (b >= e)
		return -1;

	int dst = e - b;
	int min = n < dst ? n : dst;
	unsigned char *bs;

	bs = (unsigned char *)calloc((dst >> 3) + 1, 1);

	srand((unsigned)time(NULL));

	for (int i = 0; i < min; )
	{
		int rdm = rand() % dst;
		unsigned char *pbs = bs + (rdm >> 3) ;
		unsigned char bv = 1 << rdm % 8;

		//判断第rdm位是否被占用
		if (!(*pbs & bv))
		{
			ary[i++] = b + rdm;
			*pbs |= bv;
		}

	}

	free(bs);

	return min;
}

int main()
{
	 
	int ary[50];
	int n = sizeof(ary) / sizeof(*ary);
	int n1;
	
	//获取-5~-3之间的所有随机数
	n1 = rand_range(ary, n, -5, -3); 
	for (int i = 0; i < n1; ++i)
		printf("%d ", ary[i]);

	printf("\n------------\n");

	//获取11123~11144之间的所有随机数
	n1 = rand_range(ary, n, 11123, 11144);
	for (int i = 0; i < n1; ++i)
		printf("%d ", ary[i]);

	printf("\n------------\n");

	//获取-50~-16之间的所有随机数
	n1 = rand_range(ary, n, -50, -16);
	for (int i = 0; i < n1; ++i)
		printf("%d ", ary[i]);

	puts("");
}

