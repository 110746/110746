#ifndef  XL_C_HEX_DUMP_H
#define  XL_C_HEX_DUMP_H
#include <stdio.h>
#include <stdlib.h>

//block �ڴ���ַ
//size  ���С
//cc    ÿ����ʾ������(����)
//isofst �Ƿ���ʾoffset
void hexdumpf(const void *block, int size, int cc, char isofst,FILE *pf);
#define hexdump(block,size,cc,isofst) hexdumpf((block),(size),(cc),(isofst),stdout)


#endif // !HEX_DUMP_H
