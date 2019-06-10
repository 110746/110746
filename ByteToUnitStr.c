#include <stdlib.h>

typedef char tchar;
typedef unsigned long ulong;
void ByteToUnitStr (tchar *strSize, ulong nSizeHigh, ulong nSizeLow )
{
    static const ulong PBH = 268435456;
	static const ulong EBH = 262144;
	static const ulong TBH = 256;
	static const ulong GB = 1073741824;
	static const ulong MB = 1048576;
	static const ulong KB = 1024;

	tchar strUnit[8] = "@B";
	tchar strFmt[] = "%0.@lf %s";
	double rv = 0;

	//计算PB
	if (nSizeHigh >= PBH)
	{
		rv = (double)nSizeHigh / PBH +
			(double)nSizeLow / ~0u / PBH;

		strUnit[0] = 'P';
	}//计算EB
	else if (nSizeHigh >= EBH)
	{
		rv = (double)nSizeHigh / EBH  +
			(double)nSizeLow / ~0u / EBH;
		strUnit[0] = 'E';
	}
	//生成TB字符串
	else if (nSizeHigh >= TBH)
	{
		rv = (double)nSizeHigh / TBH +
			(double)nSizeLow / ~0u / TBH;
		strUnit[0] = 'T';
	}
	//生成GB字符串
	else if (nSizeHigh > 0 || nSizeLow >= GB)
	{
		rv = (double)nSizeHigh / TBH * 1024 +
			(double)nSizeLow / GB;
		strUnit[0] = 'G';

	}//生成MB字符串
	else if (nSizeLow >= MB)
	{
		rv = (double)nSizeLow / MB;
		strUnit[0] = 'M';

	}//生成KB字符串
	else if (nSizeLow >= KB)
	{
		rv = (double)nSizeLow / KB;
		strUnit[0] = 'K';
	}
	//生成B字符串
	else
	{
		rv = nSizeLow;
        *(ulong*)strUnit = *(ulong*)"字节";
        
	}

	if (rv - 0.1 < (ulong)rv)
	{
		strFmt[3] = '0';

	}else
	{
		if (rv * 10 - 0.1 < (ulong)(rv * 10))
			strFmt[3] = '1';
		else 
			strFmt[3] = '2';
			
	}
  
	 sprintf(strSize,strFmt, rv, strUnit);
}
