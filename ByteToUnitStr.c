#include <tchar.h>
#include <windows.h>

VOID ByteToUnitStr(LPTSTR strSize, DWORD nSizeHigh, DWORD nSizeLow)
{
	static const DWORD PBH = 268435456;
	static const DWORD EBH = 262144;
	static const DWORD TBH = 256;
	static const DWORD GB = 1073741824;
	static const DWORD MB = 1048576;
	static const DWORD KB = 1024;

	LPCTSTR strUnit = 0;
	double rv = 0;

	//计算PB
	if (nSizeHigh >= PBH)
	{
		rv = (double)nSizeHigh / PBH +
			(double)nSizeLow / ~0u / PBH;
		strUnit = _T("PB");
	}//计算EB
	else if (nSizeHigh >= EBH)
	{
		rv = (double)nSizeHigh / EBH +
			(double)nSizeLow / ~0u / EBH;
		strUnit = _T("EB");
	}
	//生成TB字符串
	else if (nSizeHigh >= TBH)
	{
		rv = (double)nSizeHigh / TBH +
			(double)nSizeLow / ~0u / TBH;
		strUnit = _T("TB");
	}
	//生成GB字符串
	else if (nSizeHigh > 0 || nSizeLow >= GB)
	{
		rv = (double)nSizeHigh / TBH * 1024 +
			(double)nSizeLow / GB;
		strUnit = _T("GB");

	}//生成MB字符串
	else if (nSizeLow >= MB)
	{
		rv = (double)nSizeLow / MB;
		strUnit = _T("MB");

	}//生成KB字符串
	else if (nSizeLow >= KB)
	{
		rv = (double)nSizeLow / KB;
		strUnit = _T("KB");
	}
	//生成B字符串
	else
	{
		rv = nSizeLow;
		strUnit = _T("B");
	}

	_stprintf(strSize, _T("%0.2lf %s"), rv, strUnit);
}
