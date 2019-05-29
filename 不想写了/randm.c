#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define N 100
#define GIVE_UP_MOD N / 2

///////////////hash函数表//////////////////////
int hash1(int a)
{
	a = (a ^ 61) ^ (a >> 16);
	a = a + (a << 3);
	a = a ^ (a >> 4);
	a = a * 0x27d4eb2d;
	a = a ^ (a >> 15);
	return a;
}

int hash2(int a)
{
	a = ~a + (a << 15);
	a = a ^ (a >> 12);
	a = a + (a << 2);
	a = a ^ (a >> 4);
	a = a * 2057;
	a = a ^ (a >> 16);
	return a;
}


int hash3(int a)
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

int hash4(int a)
{
	a += ~(a << 15);
	a ^= (a >> 10);
	a += (a << 3);
	a ^= (a >> 6);
	a += ~(a << 11);
	a ^= (a >> 16);

	return a;
}


int hash5(int h)
{
	h ^= (h >> 20) ^ (h >> 12);
	return h ^ (h >> 7) ^ (h >> 4);
}

int hash6(int a)
{
	a = a ^ (a >> 4);
	a = (a ^ 0xdeadbeef) + (a << 5);
	a = a ^ (a >> 11);
	return a;
}
int hash7(int a)
{
	a ^= (a >> 16);
	return ((a >> 8) ^ a);
}
static int (*hashs[])(int) = {hash1,hash2,hash3,hash4,hash5,hash6,hash7};
int main()
{
	static unsigned char bAry[N / 8 + 1];
	static int  ary[N];
	int i,n;
	char bOk = 1;
	int nGiveUp; //放弃系数
	unsigned nRdm;
	
	
	for (n = 0; bOk && n != N; n += bOk)
	{	
		bOk = 0;
		for (int i = 0; !bOk && i < 2; ++i)
		{
			//尝试使用方案3
			srand((unsigned)time(NULL));

			nGiveUp = 0;
			for (;!bOk && nGiveUp < GIVE_UP_MOD; ++nGiveUp)
			{
				//尝试使用方案1
				nRdm = rand() % N + 1;
				int nRdOld = nRdm;
				unsigned nAvg = nRdm / 8;
				unsigned char nBit = 1 << (nRdm % 8 ? nRdm % 8 - 1u : 0);

				if (!ary[n] && !(bAry[nAvg] & nBit))
					bOk = 1;

				//尝试使用方案2
				else
				{
					for (int i = 0; !bOk && i < 7; ++i)
					{
						nRdm = hashs[i](nRdOld) % N + 1;
						nAvg = nRdm / 8;
						nBit = 1 << (nRdm % 8 ? nRdm % 8 - 1u : 0);

						bOk = !ary[n] && !(bAry[nAvg] & nBit);
						
					}
				}
				//方案尝试成功
				if (bOk)
				{
					ary[n] = nRdm;
					bAry[nAvg] |= nBit;
				}
				
			}
	    }
		
		
	}

	for (i = 0; i != n; ++i)
		printf("%d\n", ary[i]);



	return 0;
}
