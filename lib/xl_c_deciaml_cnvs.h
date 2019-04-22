#ifndef XL_C_DECIMAL_CNVS_H
#define XL_C_DECIMAL_CNVS_H

//参数vb为保留字符串的位数，例如123保留两位就是23, 默认-1为保留全部位
char* longtostr(unsigned long l, int bw, char* outstr, int vb);
//bw为解释字符串的类型。例如,bw=2,bw=8,bw=16 分别为二进制、八进制、十六进制
unsigned long strtolong(const char* str, int bw);

#endif