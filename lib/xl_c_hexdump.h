#ifndef  XL_C_HEX_DUMP_H
#define  XL_C_HEX_DUMP_H
#include <stdio.h>
#include <stdlib.h>

//block 内存块地址
//size  块大小
//cc    每行显示多少列(列数)
//isofst 是否显示offset
void hexdumpf(const void *block, int size, int cc, char isofst,FILE *pf);
#define hexdump(block,size,cc,isofst) hexdumpf((block),(size),(cc),(isofst),stdout)


#endif // !HEX_DUMP_H
