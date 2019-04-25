#include "../lib/xl_c_hexdump.h"
#include "../lib/xl_c_deciaml_cnvs.h"

#define  _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#define MSG "hello world!"
#define N 100000000
int main()
{

	const size_t sz = 1024 * 1024;
	const size_t szStr = sizeof(MSG) + 1;
	char *buf = (char*)malloc(sz), *pbuf = buf;
	const char *pi;

	const size_t nMStr = sz / szStr;
	const size_t nStr = N / nMStr;
	const size_t nSg = N % nMStr;

	for (unsigned i = 0; i < nMStr; ++i)
	{
		pi = MSG;
		while (*pbuf++ = *pi++);
		*pbuf++ = '\n';

	}

	//FILE *f;
	//fopen_s(&f, "1.txt", "wb");

	//Ð´ÈëStr±í
	for (unsigned i = 0; i < nStr; ++i)
	{
		fwrite(buf, szStr, nMStr, stdout);
		
	}
	//Ð´ÈëÊ£Óàstr
	fwrite(buf, szStr, nSg, stdout);
	//fclose(f);
	

	return 0;
}


 