#ifndef XL_C_DECIMAL_CNVS_H
#define XL_C_DECIMAL_CNVS_H

//����vbΪ�����ַ�����λ��������123������λ����23, Ĭ��-1Ϊ����ȫ��λ
char* longtostr(unsigned long l, int bw, char* outstr, int vb);
//bwΪ�����ַ��������͡�����,bw=2,bw=8,bw=16 �ֱ�Ϊ�����ơ��˽��ơ�ʮ������
unsigned long strtolong(const char* str, int bw);

#endif