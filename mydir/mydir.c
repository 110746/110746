#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <tchar.h>
#include <stdbool.h>
#include <locale.h>
#include <windows.h>
#include <crtdbg.h>
#include <Shlwapi.h>
#include "help.h"
#pragma comment(lib,"Shlwapi.lib")
#ifdef __cplusplus
extern "C"
{
#endif
  #include "lib/xl_c_array.h"
#ifdef __cplusplus
}
#endif


 
/////////////////////////////////////////////////
#define  BIT_TRUE(n,bn)       ((n) |= 1 << (bn) )
#define  BIT_FALSE(n,bn)      ((n) &= ~(1 << (bn)))
#define  BIT_SET(n,bn,v)      ((v) ? BIT_TRUE(n,bn) : BIT_FALSE(n,bn))
#define  BIT_GET(n,bn)        ((n) & 1 << (bn) )
#define _mytolower(c)         (((c) >= _T('A') && (c) <= _T('Z')) ? (c) | 0x20 : (c))

//打印属性
#define  BIT_PRINT 1

//过滤属性
#define BIT_FILTER 2
/////////////////////////////////////////////////
//文件名，类型，大小，属性，创建时间，访问时间，修改时间
//fn      ty    sz   ab   ct        at        wt
//列数
#define COLUMNS_NUMBER      7 
//最小列宽为10个字符(以英文字符为单位)
#define COLUMNS_MIN         8 
#define ARY_DIR          0
#define ARY_FILE         1
//默认列间隔为4个字符(以英文字符为单位)
#define COLUMNS_SPACING  2 
#define FILE_NAME        0
#define FILE_TYPE        1
#define FILE_SIZE        2
#define FILE_ATTRIBUTE   3
#define FILE_CREATE_TIME 4
#define FILE_ACCESS_TIME 5
#define FILE_WRITE_TIME  6

//0表示文件所有属性均能使用
#define FILE_ATB_ALL  0
//1表示文件的所有属性均不能使用
#define FILE_ATB_NO  -1

///////////////////////////
#define FS_SZ(n) ( (unsigned)(((LPStrName)0)->str)  + (n))
typedef struct _tagStr
{
	WORD  len;
	WORD  celen; //屏幕实际所占的字节数
	TCHAR str[1];

}StrName, *LPStrName;
 
//////////////////////////////

/////////////////////////////////////////////////
LPTSTR  ErrorCodeToStr(DWORD nCode);
VOID GetColumns();
VOID GetFolderSize(LPStrName dirPath, PDWORD nSizeHigh, PDWORD nSizeLow);
VOID ShortName(LPStrName path, WORD n);
VOID ConfirmFormat();
VOID ByteToUnitStr(LPStrName strSize, DWORD nSizeHigh, DWORD nSizeLow);
BOOL CMDParsing(INT n, LPTSTR param[]);
VOID GetTimeStr(LPStrName strTime, PFILETIME timeFile);
VOID Clear(array *columns,DWORD n);
VOID PrintFormat();

//////////////////////////////
//把Src累加到Dest中
void QWordAdd(PDWORD dwDestHigh, PDWORD dwDestLow, DWORD dwSrcHigh, DWORD dwSrcLow)
{
	DWORD result = *dwDestLow + dwSrcLow;

	*dwDestLow += dwSrcLow;
	*dwDestHigh += dwSrcHigh;
	*dwDestHigh += (result < *dwDestLow || result < dwSrcLow);
}
/////////////////////////
static WORD _myscpy(LPTSTR dest, LPCTSTR src)
{
	WORD n = 0;
	while (*dest++ = *src++)
		++n;
	return n;
}
static WORD _myscelen(LPTSTR str)
{
	WORD n = 0;

	for (; *str; ++str)
	{
		if ((unsigned)*str > 255u)
			n += 2;
		else
			++n;
	}
	return n;
}
//不区分大小写比较，不同返回0，相同返回1
static bool _mysupcmp(LPCTSTR dest, LPCTSTR src)
{	
	while (*dest && *src && 
		_mytolower(*dest) == _mytolower(*src))
		   ++dest, ++src;
 
	return !*dest && !*src;
}
 

/////////////////////////
static array   columns[2][COLUMNS_NUMBER];
//实际需要输出的有效的列,NULL为结束
//默认显示：文件名、类型、大小、属性
static parray  lpCValids[2][COLUMNS_NUMBER + 1] = 
{
	{
		columns[ARY_DIR] + FILE_NAME,
		columns[ARY_DIR] + FILE_TYPE,
		columns[ARY_DIR] + FILE_SIZE,
		columns[ARY_DIR] + FILE_ATTRIBUTE,
	},
    {
		columns[ARY_FILE] + FILE_NAME,
		columns[ARY_FILE] + FILE_TYPE,
		columns[ARY_FILE] + FILE_SIZE,
		columns[ARY_FILE] + FILE_ATTRIBUTE,
	}
};
//每列最长字符串长度
static WORD    nStrMax[COLUMNS_NUMBER]; 
//实际每列的最长字符数
//默认计算：文件名、类型、大小、属性的最大长度
static PWORD   lpStrMaxs[COLUMNS_NUMBER + 1] = 
{
	nStrMax + FILE_NAME,
	nStrMax + FILE_TYPE,
	nStrMax + FILE_SIZE,
	nStrMax + FILE_ATTRIBUTE
};
//行实际长度
WORD nWidthLine;
//记录每个列是否有效
//默认显示文件名、类型、大小、属性的最大长度
static BYTE    bValids = 0xf; 
//记录每个列是否需要短名(默认需要短名)
static BYTE    bSNs = -1;    
//有效的属性(默认全部显示)
static DWORD   dwAtbs = FILE_ATB_ALL;
//扩展名列表
static LPCTSTR sExts; 
//要遍历的文件夹名
static LPCTSTR  sPathDir;

/////////////////////////

int _tmain(INT n, LPTSTR param[])
{ 
 
	setlocale(LC_ALL, "");
	
	if (n == 1)
	{
		//如果没有参数，则模拟一个参数,让程序遍历当前目录
		static TCHAR s[2][3] = { _T(""),_T("./") };
		static LPTSTR param2[3] = { s[0],s[1],NULL };
		param = param2;
		++n;
	}

	if (!CMDParsing(n, param))
		return -1;
   
	for(WORD i = 0; i < 2; ++i)
		for(WORD j = 0; lpCValids[i][j]; ++j)
			*lpCValids[i][j] = ary_create(0, sizeof(void*), 0);
	 
	GetColumns();

	ConfirmFormat();

	PrintFormat();

	Clear(*columns, COLUMNS_NUMBER * 2);
 
	//_CrtDumpMemoryLeaks();
	return 0;
}


VOID GetColumns()
{
	static BYTE bStrFile[sizeof(StrName) + MAX_PATH];
	static BYTE bStrBuf[sizeof(StrName) + MAX_PATH];
	static LPStrName strFilePath = (LPStrName)bStrFile;
	static LPStrName strBuf = (LPStrName)bStrBuf;
	static WIN32_FIND_DATA wfd;
	HANDLE hFindFile;
	LPStrName strFile;
	DWORD nStr,dwFAtbs;
	//0位为文件夹
	//1位为.目录
	//2位为..目录
	char bs;

	strFilePath->len = _myscpy(strFilePath->str, sPathDir);
	_myscpy(strFilePath->str + strFilePath->len, _T("\\*"));

	hFindFile = FindFirstFile(strFilePath->str, &wfd);
	if (hFindFile == INVALID_HANDLE_VALUE)
	{
		LPTSTR strError = ErrorCodeToStr(GetLastError());
		_tprintf(strError);
		LocalFree((HLOCAL)strError);
		return;
	}

	do
	{		
//-------------------------------------------------------------------------	
		dwFAtbs = wfd.dwFileAttributes;
		
		//如果用户指定的属性不包含在文件属性中
		//则忽略指定文件
		if ((dwFAtbs & dwAtbs) != dwAtbs)
			 continue;
//-------------------------------------------------------------------------
		if (sExts != NULL)
		{
			if (dwFAtbs & FILE_ATTRIBUTE_DIRECTORY)
				continue;

			nStr = _tcslen(wfd.cFileName);
			LPCTSTR sPtPos = wfd.cFileName + nStr - 1;
			LPCTSTR sIter;
			LPCTSTR sBegin = NULL;
			bool bEqu = false;

			//查找文件扩展名.位置
			for (; sPtPos >= wfd.cFileName; --sPtPos)
				if (*sPtPos == _T('.'))
					break;
			if (sPtPos > wfd.cFileName)
			{
				for (sIter = sExts; *sIter; ++sIter)
				{
					if (sIter == sExts || *sIter == _T(','))
						sBegin = sPtPos + 1;

					while (*sBegin && *sBegin != _T(',') &&
						*sIter == _mytolower(*sBegin))
						++sIter, ++sBegin;

					if ((*sIter == _T(',') || *sIter == 0) &&
						*sBegin == 0)
					{
						bEqu = true;
						break;
					}
				}
			}

			//不是指定的扩展名的文件则忽略
			if (!bEqu)
				continue;

		}
		

		
//-------------------------------------------------------------------------
		bs = 0;
		if (dwFAtbs & FILE_ATTRIBUTE_DIRECTORY)
		{
			BIT_SET(bs, 0, 1);

			if (!_tcscmp(wfd.cFileName, _T(".")))
				BIT_SET(bs, 1, 1);
			else 
				if (!_tcscmp(wfd.cFileName, _T("..")))
					BIT_SET(bs, 2, 1);
		}
//-------------------------------------------------------------------------	
		//如果文件类型列有效
		if (BIT_GET(bValids, FILE_TYPE))
		{
			if (BIT_GET(bs, 0))
			{
				if (BIT_GET(bs, 1))
				{
					strFile = (LPStrName)malloc(FS_SZ(sizeof(_T("当前目录"))));
					strFile->len = _myscpy(strFile->str, _T("当前目录"));
					strFile->celen = 8;

				}
				else if (BIT_GET(bs,2))
				{
					strFile = (LPStrName)malloc(FS_SZ(sizeof(_T("上级目录"))));
					strFile->len = _myscpy(strFile->str, _T("上级目录"));
					strFile->celen = 8;
				}
				else
				{
					strFile = (LPStrName)malloc(FS_SZ(sizeof(_T("目录"))));
					strFile->len = _myscpy(strFile->str, _T("目录"));
					strFile->celen = 4;
				}

			}
			else
			{
				strFile = (LPStrName)malloc(FS_SZ(sizeof(_T("文件"))));
				strFile->len = _myscpy(strFile->str, _T("文件"));
				strFile->celen = 4;
			}

				ary_push_back(columns[!bs] + FILE_TYPE, &strFile);

//-------------------------------------------------------------------------
			if (nStrMax[FILE_TYPE] < strFile->celen)
				nStrMax[FILE_TYPE] = strFile->celen;
//-------------------------------------------------------------------------
		}
		
//-------------------------------------------------------------------------
		
		if (BIT_GET(bValids, FILE_NAME))
		{
			//文件名列表,上面已经使用了nStr获取了一次长度，直接使用就行了
			strFile = (LPStrName)malloc(FS_SZ(_tcslen(wfd.cFileName) * sizeof(TCHAR) + sizeof(TCHAR)));
			strFile->len = _myscpy(strFile->str, wfd.cFileName);
			strFile->celen = _myscelen(strFile->str);
	
				ary_push_back(columns[!bs] + FILE_NAME, &strFile);
//-------------------------------------------------------------------------
			if (nStrMax[FILE_NAME] < strFile->celen)
				nStrMax[FILE_NAME] = strFile->celen;
//-------------------------------------------------------------------------
	    }
	
//-------------------------------------------------------------------------
		if (BIT_GET(bValids, FILE_SIZE))
		{
			switch (bs)
			{			
			//处理文件
			case 0:
			//处理非.和..目录大小
			case 1:
				nStr = strFilePath->len;
				nStr += _myscpy(strFilePath->str + nStr, _T("\\"));
				_myscpy(strFilePath->str + nStr, wfd.cFileName);
				GetFolderSize(strFilePath, &wfd.nFileSizeHigh, &wfd.nFileSizeLow);
				ByteToUnitStr(strBuf, wfd.nFileSizeHigh, wfd.nFileSizeLow);
				break;
			//处理.目录
			case 3:
			//处理..目录
			case 5:
				strBuf->len = _myscpy(strBuf->str, _T("无"));
				//strBuf->celen = 2;
				break;
			}

			strFile = (LPStrName)malloc(FS_SZ(strBuf->len * sizeof(TCHAR) + sizeof(TCHAR)));
			strFile->len = _myscpy(strFile->str, strBuf->str);
			strFile->celen = _myscelen(strFile->str);

				ary_push_back(columns[!bs] + FILE_SIZE, &strFile);
//-------------------------------------------------------------------------
			if (nStrMax[FILE_SIZE] < strFile->celen)
				nStrMax[FILE_SIZE] = strFile->celen;
//-------------------------------------------------------------------------
	    }
		
//-------------------------------------------------------------------------	
		if (BIT_GET(bValids, FILE_ATTRIBUTE))
		{
			LPTSTR lpBuf = strBuf->str;
			*lpBuf++ = _T('-');
			if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				*lpBuf++ = _T('d'), *lpBuf++ = _T('-');

			if (wfd.dwFileAttributes & FILE_ATTRIBUTE_READONLY)
				*lpBuf++ = _T('r'), *lpBuf++ = _T('-');

			if (wfd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)
				*lpBuf++ = _T('h'), *lpBuf++ = _T('-');

			if (wfd.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE)
				*lpBuf++ = _T('a'), *lpBuf++ = _T('-');

			if (wfd.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM)
				*lpBuf++ = _T('s'), *lpBuf++ = _T('-');

			if (wfd.dwFileAttributes & FILE_ATTRIBUTE_NOT_CONTENT_INDEXED)
				*lpBuf++ = _T('i'), *lpBuf++ = _T('-');

			if (wfd.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)
				*lpBuf++ = _T('l'), *lpBuf++ = _T('-');

			if (wfd.dwFileAttributes & FILE_ATTRIBUTE_OFFLINE)
				*lpBuf++ = _T('o'), *lpBuf++ = _T('-');

			if (wfd.dwFileAttributes & FILE_ATTRIBUTE_ENCRYPTED)
				*lpBuf++ = _T('e'), *lpBuf++ = _T('-');

			*lpBuf = 0;

			strFile = (LPStrName)malloc(FS_SZ((lpBuf - strBuf->str) * sizeof(TCHAR) + sizeof(TCHAR)));
			strFile->len = _myscpy(strFile->str, strBuf->str);
			strFile->celen = strFile->len;

			ary_push_back(columns[!bs] + FILE_ATTRIBUTE, &strFile);

//-------------------------------------------------------------------------
			if (nStrMax[FILE_ATTRIBUTE] < strFile->celen)
				nStrMax[FILE_ATTRIBUTE] = strFile->celen;
//-------------------------------------------------------------------------
	    }
		
//-------------------------------------------------------------------------
		if (BIT_GET(bValids, FILE_CREATE_TIME))
		{
			GetTimeStr(strBuf, &wfd.ftCreationTime);

			strFile = (LPStrName)malloc(FS_SZ(strBuf->len * sizeof(TCHAR) + sizeof(TCHAR)));
			strFile->len = _myscpy(strFile->str, strBuf->str);
			strFile->celen = strFile->len;

				ary_push_back(columns[!bs] + FILE_CREATE_TIME, &strFile);

//-------------------------------------------------------------------------
			if (nStrMax[FILE_CREATE_TIME] < strFile->celen)
				nStrMax[FILE_CREATE_TIME] = strFile->celen;
//-------------------------------------------------------------------------
		}
		
//-------------------------------------------------------------------------
		if (BIT_GET(bValids, FILE_ACCESS_TIME))
		{
			GetTimeStr(strBuf, &wfd.ftLastAccessTime);

			strFile = (LPStrName)malloc(FS_SZ(strBuf->len * sizeof(TCHAR) + sizeof(TCHAR)));
			strFile->len = _myscpy(strFile->str, strBuf->str);
			strFile->celen = strFile->len;

				ary_push_back(columns[!bs] + FILE_ACCESS_TIME, &strFile);
//-------------------------------------------------------------------------
			if (nStrMax[FILE_ACCESS_TIME] < strFile->celen)
				nStrMax[FILE_ACCESS_TIME] = strFile->celen;
		}
		
//-------------------------------------------------------------------------
		if (BIT_GET(bValids, FILE_WRITE_TIME))
		{
			GetTimeStr(strBuf, &wfd.ftLastWriteTime);

			strFile = (LPStrName)malloc(FS_SZ(strBuf->len * sizeof(TCHAR) + sizeof(TCHAR)));
			strFile->len = _myscpy(strFile->str, strBuf->str);
			strFile->celen = strFile->len;

			ary_push_back(columns[!bs] + FILE_WRITE_TIME, &strFile);

//-------------------------------------------------------------------------
			if (nStrMax[FILE_WRITE_TIME] < strFile->celen)
				nStrMax[FILE_WRITE_TIME] = strFile->celen;
//-------------------------------------------------------------------------
		}
		

	} while (FindNextFile(hFindFile, &wfd));

	FindClose(hFindFile);

	 
}

 
VOID GetFolderSize(LPStrName dirPath, PDWORD nSizeHigh, PDWORD nSizeLow)
{
	array aStack = ary_create(0, sizeof(LPCTSTR), 0);
	const DWORD nAlloc = MAX_PATH * sizeof(TCHAR) + sizeof(TCHAR);
	static WIN32_FIND_DATA wfd;
	HANDLE hFindFile;
	LPStrName strFilePath;

	strFilePath = (LPStrName)malloc(FS_SZ(nAlloc));
	strFilePath->len = _myscpy(strFilePath->str, dirPath->str);
	ary_push_back(&aStack, &strFilePath);
	
	while (ary_length(&aStack))
	{
		strFilePath = *(LPStrName*)ary_ptr_first(&aStack);
		_myscpy(strFilePath->str + strFilePath->len, _T("\\*"));
		ary_pop_front(&aStack);

		hFindFile = FindFirstFile(strFilePath->str, &wfd);
		if(hFindFile == INVALID_HANDLE_VALUE)
			continue;

		//保持原路径
		strFilePath->str[strFilePath->len] = 0;
		do
		{
			if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				
				if (_tcscmp(wfd.cFileName, _T(".")) && _tcscmp(wfd.cFileName, _T("..")))
				{
					LPStrName strNewPath = (LPStrName)malloc(FS_SZ(nAlloc));

					//生成下一层目录的路径
					strNewPath->len = _myscpy(strNewPath->str, strFilePath->str);
					strNewPath->len += _myscpy(strNewPath->str + strNewPath->len, _T("\\"));
					strNewPath->len += _myscpy(strNewPath->str + strNewPath->len, wfd.cFileName);

					ary_push_back(&aStack, &strNewPath);	
					 
				}
			}
			else
			{
	            //累加文件大小
				QWordAdd(nSizeHigh, nSizeLow, wfd.nFileSizeHigh,wfd.nFileSizeLow);
			}
			 

		} while (FindNextFile(hFindFile, &wfd));

		FindClose(hFindFile);
		free(strFilePath);
    }
	
	ary_destroy(&aStack);
}

LPTSTR  ErrorCodeToStr(DWORD nCode)
{
	LPTSTR strCode;

	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_IGNORE_INSERTS |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, nCode, 0, (LPTSTR)&strCode, 0, NULL);

	return strCode;
}

VOID ShortName(LPStrName path, WORD n)
{ 	
	const DWORD nTxt = sizeof(_T("...")) / sizeof(TCHAR) - 1;

	if (path->celen <= n )
		return;
	
	LPTSTR b, m, e;
	PBYTE bb, eb;
	DWORD nAvg = (n - nTxt) / 2;
	WORD n1 = 1, n2 = 1;
	 
	b = path->str;
	e = path->str + path->len - 1;


	bb = (PBYTE)b;
	while (n1 < nAvg)
	{	
		if ((unsigned)*b > 255u)
		{
			if (++bb > (PBYTE)b + 1)
				++b;
		}
					
		else
			bb = (PBYTE)++b;

		++n1;
	}
	//如果b没有指向宽字符的尾字节
	//就证明不是一个完全的字
	//则退一格
	if ((unsigned)*b > 255u &&
		(bb != (PBYTE)b + sizeof(TCHAR) - 1))
	{
		--b;
		--n1;
	}

	m = b + nTxt;
	*++b = _T('.');
	*++b = _T('.');
	*++b = _T('.');

	eb = (PBYTE)e + 1 ;
	while (n2 < nAvg)
	{		
		if ((unsigned)*e > 255u)
		{
			if (--eb < (PBYTE)e)
				--e;
		}
		else
			eb = (PBYTE)--e;

		   ++n2;
		
	}
	//如果e没有指向宽字符的首字节
	//就证明不是一个完全的字
	//则退一格
	if ((unsigned)*e > 255u &&
		eb != (PBYTE)e)
	{
		++e;
		--n2;
	}
 
	++b;
	e = b > e ? b  : e;	
	while ((b < e) && (*b = *e))
		++e, ++b;
		 
	 path->len =  b - path->str;
	 //celen不使用了，归0
	 path->celen = 0;

}


VOID ConfirmFormat()
{	
	static CONSOLE_SCREEN_BUFFER_INFO csbi;	
	static const WORD * const nStrMaxEnd = nStrMax + COLUMNS_NUMBER;
	PWORD *lpNCV ;
	WORD nScrBuf;
	LPStrName *sBegin, *sEnd;
	
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	nScrBuf = csbi.dwSize.X;
	
	//累计最长行长度
	for (lpNCV = lpStrMaxs; *lpNCV != NULL; ++lpNCV)
		nWidthLine += **lpNCV + COLUMNS_SPACING;		
		 
	//如果行宽超过缓冲区大小
	//缩短有效列的名字，使得屏幕缓冲区能容纳列宽
	if (nScrBuf && nWidthLine > nScrBuf)
	{
		for (WORD X = 0; lpStrMaxs[X] != NULL; ++X)
		{
			
			PWORD lpNColumns = lpStrMaxs[X];
			WORD cLen = *lpNColumns;

			//只处理大于最短列名和需要短名的字符串
			if (*lpNColumns > COLUMNS_MIN && BIT_GET(bSNs, lpNColumns - nStrMax))
			{
				while (cLen > COLUMNS_MIN && nWidthLine > nScrBuf)
				{
					--cLen;
					--nWidthLine;
				}
				 
				for (WORD i = 0; i < 2; ++i)
				{
					//缩短目录当前列字符串的名字
					sBegin = (LPStrName *)ary_ptr_begin(lpCValids[i][X]);
					sEnd = (LPStrName *)ary_ptr_end(lpCValids[i][X]);

					for (; sBegin != sEnd; ++sBegin)
						ShortName(*sBegin, cLen);
				}
				*lpNColumns = cLen;
				if (nWidthLine <= nScrBuf)
					break;
			}

		}
	}
	//exit(0);
	////printf("nline=%d\n", nWidthLine);
}

VOID ByteToUnitStr(LPStrName strSize, DWORD nSizeHigh, DWORD nSizeLow)
{
	static const DWORD PBH = 268435456;
	static const DWORD EBH = 262144;
	static const DWORD TBH = 256;
	static const DWORD GB = 1073741824;
	static const DWORD MB = 1048576;
	static const DWORD KB = 1024;

	TCHAR strUnit[8] = _T("@B");
	TCHAR strFmt[] = _T("%0.@lf %s");
	double rv = 0;

	//计算PB
	if (nSizeHigh >= PBH)
	{
		rv = (double)nSizeHigh / PBH +
			(double)nSizeLow / ~0u / PBH;

		strUnit[0] = _T('P');
	}//计算EB
	else if (nSizeHigh >= EBH)
	{
		rv = (double)nSizeHigh / EBH  +
			(double)nSizeLow / ~0u / EBH;
		strUnit[0] = _T('E');
	}
	//生成TB字符串
	else if (nSizeHigh >= TBH)
	{
		rv = (double)nSizeHigh / TBH +
			(double)nSizeLow / ~0u / TBH;
		strUnit[0] = _T('T');
	}
	//生成GB字符串
	else if (nSizeHigh > 0 || nSizeLow >= GB)
	{
		rv = (double)nSizeHigh / TBH * 1024 +
			(double)nSizeLow / GB;
		strUnit[0] = _T('G');

	}//生成MB字符串
	else if (nSizeLow >= MB)
	{
		rv = (double)nSizeLow / MB;
		strUnit[0] = _T('M');

	}//生成KB字符串
	else if (nSizeLow >= KB)
	{
		rv = (double)nSizeLow / KB;
		strUnit[0] = _T('K');
	}
	//生成B字符串
	else
	{
		rv = nSizeLow;
		//strUnit[0] = _T('字');
		_myscpy(strUnit,_T("字节"));
	}

	if (rv - 0.1 < (INT)rv)
	{
		strFmt[3] = _T('0');

	}else
	{
		if (rv * 10 - 0.1 < (INT)(rv * 10))
			strFmt[3] = _T('1');
		else 
			strFmt[3] = _T('2');
			
	}

	strSize->len = _stprintf(strSize->str, strFmt, rv, strUnit);
}

BOOL CMDParsing(INT n, LPTSTR params[])
{
	LPTSTR sPram,sPos,sBein,sEnd;
	TCHAR cOld;
	BYTE type = 0;
	BYTE nCValids = 0;
	BYTE bNext;
	BYTE  bLocalValids = 0;
	BYTE bLocalSNS = -1;
	//dwAtbs属性是否被设置过
	BYTE bAtbsSet = false;
	LPCTSTR sTip = _T("参数 %s 格式错误\n请检查格式!");
	
	for (++params; *params; ++params)
	{
		sPram = *params;

		 if (BIT_GET(type, BIT_PRINT))
		 {
			 sBein = sPram;
			 sPos = sPram;
			 while (*sBein)
			 {
//----------------------------------------------	 
				 while (*sPos && *sPos != _T(','))
					 ++sPos;
				 cOld = *sPos;
				 *sPos = 0;

//----------------------------------------------			 
				//不是xx或者xx*
				 if (sPos - sBein < 2 || sPos - sBein > 3)
				 {
					 *sPos = cOld;
					 _tprintf(sTip, sPram);
					 return FALSE;
				 }
				 //避免重复显示 && 符合条件
				 if (!BIT_GET(bLocalValids, FILE_NAME) && _mysupcmp(sBein, _T("fn")))
				 {				
					 lpCValids[ARY_DIR][nCValids]  = columns[ARY_DIR] + FILE_NAME;
					 lpCValids[ARY_FILE][nCValids] = columns[ARY_FILE] + FILE_NAME;
					 lpStrMaxs[nCValids] = nStrMax + FILE_NAME;
					 BIT_SET(bLocalValids, FILE_NAME, TRUE);
					 ++nCValids;

				 }
				 else if (BIT_GET(bLocalSNS, FILE_NAME) && _mysupcmp(sBein, _T("fn*")))
				 {	 
					 //目录加入显示列表
					 lpCValids[ARY_DIR][nCValids] = columns[ARY_DIR] + FILE_NAME;
					 //文件加入显示列表
					 lpCValids[ARY_FILE][nCValids] = columns[ARY_FILE] + FILE_NAME;
					 //有效的列宽
					 lpStrMaxs[nCValids] = nStrMax + FILE_NAME;				 
					 //设置要显示的列
					 BIT_SET(bLocalValids, FILE_NAME, TRUE);
					 //不需要短名
					 BIT_SET(bLocalSNS, FILE_NAME, FALSE);
					 ++nCValids;

				 }
				 else  if (!BIT_GET(bLocalValids, FILE_TYPE) && _mysupcmp(sBein, _T("ty")))
				 {
					 lpCValids[ARY_DIR][nCValids] = columns[ARY_DIR] + FILE_TYPE;
					 lpCValids[ARY_FILE][nCValids] = columns[ARY_FILE] + FILE_TYPE;
					 lpStrMaxs[nCValids] = nStrMax + FILE_TYPE;
					 BIT_SET(bLocalValids, FILE_TYPE, TRUE);					 
					 ++nCValids;
					 
				 }
				 else  if (BIT_GET(bLocalSNS, FILE_TYPE) && _mysupcmp(sBein, _T("ty*")))
				 {
					 lpCValids[ARY_DIR][nCValids] = columns[ARY_DIR] + FILE_TYPE;
					 lpCValids[ARY_FILE][nCValids] = columns[ARY_FILE] + FILE_TYPE;
					 lpStrMaxs[nCValids] = nStrMax + FILE_TYPE;
					 BIT_SET(bLocalValids, FILE_TYPE, TRUE);
					 BIT_SET(bLocalSNS, FILE_TYPE, FALSE);
					 ++nCValids;
				 }
				 else  if (!BIT_GET(bLocalValids, FILE_SIZE) && _mysupcmp(sBein, _T("sz")))
				 {
					 lpCValids[ARY_DIR][nCValids] = columns[ARY_DIR] + FILE_SIZE;
					 lpCValids[ARY_FILE][nCValids] = columns[ARY_FILE] + FILE_SIZE;
					 lpStrMaxs[nCValids] = nStrMax + FILE_SIZE;
					 BIT_SET(bLocalValids, FILE_SIZE, TRUE);
					 ++nCValids;
				 }
				 else  if (BIT_GET(bLocalSNS, FILE_SIZE) && _mysupcmp(sBein, _T("sz*")))
				 {
					 lpCValids[ARY_DIR][nCValids] = columns[ARY_DIR] + FILE_SIZE;
					 lpCValids[ARY_FILE][nCValids] = columns[ARY_FILE] + FILE_SIZE;
					 lpStrMaxs[nCValids] = nStrMax + FILE_SIZE;
					 BIT_SET(bLocalValids, FILE_SIZE, TRUE);
					 BIT_SET(bLocalSNS, FILE_SIZE, FALSE);
					 ++nCValids;
				 }
				 else  if (!BIT_GET(bLocalValids, FILE_ATTRIBUTE) && _mysupcmp(sBein, _T("ab")))
				 {
					 lpCValids[ARY_DIR][nCValids] = columns[ARY_DIR] + FILE_ATTRIBUTE;
					 lpCValids[ARY_FILE][nCValids] = columns[ARY_FILE] + FILE_ATTRIBUTE;
					 lpStrMaxs[nCValids] = nStrMax + FILE_ATTRIBUTE;
					 BIT_SET(bLocalValids, FILE_ATTRIBUTE, TRUE);
					 ++nCValids;
				 }
				 else  if (BIT_GET(bLocalSNS, FILE_ATTRIBUTE) && _mysupcmp(sBein, _T("ab*")))
				 {
					 lpCValids[ARY_DIR][nCValids] = columns[ARY_DIR] + FILE_ATTRIBUTE;
					 lpCValids[ARY_FILE][nCValids] = columns[ARY_FILE] + FILE_ATTRIBUTE;
					 lpStrMaxs[nCValids] = nStrMax + FILE_ATTRIBUTE;
					 BIT_SET(bLocalValids, FILE_ATTRIBUTE, TRUE);
					 BIT_SET(bLocalSNS, FILE_ATTRIBUTE, FALSE);
					 ++nCValids;
				 }
				 else  if (!BIT_GET(bLocalValids, FILE_CREATE_TIME) && _mysupcmp(sBein, _T("ct")))
				 {
					 lpCValids[ARY_DIR][nCValids] = columns[ARY_DIR] + FILE_CREATE_TIME;
					 lpCValids[ARY_FILE][nCValids] = columns[ARY_FILE] + FILE_CREATE_TIME;
					 lpStrMaxs[nCValids] = nStrMax + FILE_CREATE_TIME;
					 BIT_SET(bLocalValids, FILE_CREATE_TIME, TRUE);
					 ++nCValids;
				 }
				 else  if (BIT_GET(bLocalSNS, FILE_CREATE_TIME) && _mysupcmp(sBein, _T("ct*")))
				 {
					 lpCValids[ARY_DIR][nCValids] = columns[ARY_DIR] + FILE_CREATE_TIME;
					 lpCValids[ARY_FILE][nCValids] = columns[ARY_FILE] + FILE_CREATE_TIME;
					 lpStrMaxs[nCValids] = nStrMax + FILE_CREATE_TIME;
					 BIT_SET(bLocalValids, FILE_CREATE_TIME, TRUE);
					 BIT_SET(bLocalSNS, FILE_CREATE_TIME, FALSE);
					 ++nCValids;
				 }
				 else  if (!BIT_GET(bLocalValids, FILE_ACCESS_TIME) && _mysupcmp(sBein, _T("at")))
				 {
					 lpCValids[ARY_DIR][nCValids] = columns[ARY_DIR] + FILE_ACCESS_TIME;
					 lpCValids[ARY_FILE][nCValids] = columns[ARY_FILE] + FILE_ACCESS_TIME;
					 lpStrMaxs[nCValids] = nStrMax + FILE_ACCESS_TIME;
					 BIT_SET(bLocalValids, FILE_ACCESS_TIME, TRUE);
					 ++nCValids;
				 }
				 else  if (BIT_GET(bLocalSNS, FILE_ACCESS_TIME) && _mysupcmp(sBein, _T("at*")))
				 {
					 lpCValids[ARY_DIR][nCValids] = columns[ARY_DIR] + FILE_ACCESS_TIME;
					 lpCValids[ARY_FILE][nCValids] = columns[ARY_FILE] + FILE_ACCESS_TIME;
					 lpStrMaxs[nCValids] = nStrMax + FILE_ACCESS_TIME;
					 BIT_SET(bLocalValids, FILE_ACCESS_TIME, TRUE);
					 BIT_SET(bLocalSNS, FILE_ACCESS_TIME, FALSE);
					 ++nCValids;
				 }
				 else  if (!BIT_GET(bLocalValids, FILE_WRITE_TIME) && _mysupcmp(sBein, _T("wt")))
				 {
					 lpCValids[ARY_DIR][nCValids] = columns[ARY_DIR] + FILE_WRITE_TIME;
					 lpCValids[ARY_FILE][nCValids] = columns[ARY_FILE] + FILE_WRITE_TIME;
					 lpStrMaxs[nCValids] = nStrMax + FILE_WRITE_TIME;
					 BIT_SET(bLocalValids, FILE_WRITE_TIME, TRUE);
					 ++nCValids;
				 }
				 else  if (BIT_GET(bLocalSNS, FILE_WRITE_TIME) && _mysupcmp(sBein, _T("wt*")))
				 {
					 lpCValids[ARY_DIR][nCValids] = columns[ARY_DIR] + FILE_WRITE_TIME;
					 lpCValids[ARY_FILE][nCValids] = columns[ARY_FILE] + FILE_WRITE_TIME;
					 lpStrMaxs[nCValids] = nStrMax + FILE_WRITE_TIME;
					 BIT_SET(bLocalValids, FILE_WRITE_TIME, TRUE);
                     BIT_SET(bLocalSNS, FILE_WRITE_TIME, FALSE);
					 ++nCValids;
				 }
				 else
				 {
				   *sPos = cOld;
				  _tprintf(sTip, sPram);
					return FALSE;
                 }

 //----------------------------------------------
                 sBein = cOld ? sPos + 1 : sPos;
				 //恢复之前的字符
				 *sPos++ = cOld;
//----------------------------------------------
				 //处理完毕归零
				 BIT_SET(type, BIT_PRINT, FALSE);
//----------------------------------------------
		     }

		 }else if(BIT_GET(type, BIT_FILTER))
		 {
			 sPos = sPram;
			 for(;*sPos; sPos += bNext)
			 {
				 TCHAR c = *sPos;
				 bNext = TRUE;

				 if (c == _T('-'))
				 {
					 bNext = FALSE;
					 sBein = sPos;
					 //属性确实被设置过
					 bAtbsSet = TRUE;
					 for (++sBein; *sBein && *sBein != _T(','); ++sBein)
					 {
						 c = _mytolower(*sBein);
						 
						 if (c >= _T('a') && c <= _T('z'))
						 {
							 switch (c)
							 {
							 case _T('d'):
								 dwAtbs |= FILE_ATTRIBUTE_DIRECTORY;
								 break;
							 case _T('r'):
								 dwAtbs |= FILE_ATTRIBUTE_READONLY;
								 break;
							 case _T('h'):
								 dwAtbs |= FILE_ATTRIBUTE_HIDDEN;
								 break;
							 case _T('a'):
								 dwAtbs |= FILE_ATTRIBUTE_ARCHIVE;
								 break;
							 case _T('s'):
								 dwAtbs |= FILE_ATTRIBUTE_SYSTEM;
								 break;
							 case _T('i'):
								 dwAtbs |= FILE_ATTRIBUTE_NOT_CONTENT_INDEXED;
								 break;
							 case _T('l'):
								 dwAtbs |= FILE_ATTRIBUTE_REPARSE_POINT;
								 break;
							 case _T('o'):
								 dwAtbs |= FILE_ATTRIBUTE_OFFLINE;
								 break;
							 case _T('e'):
								 dwAtbs |= FILE_ATTRIBUTE_ENCRYPTED;
								 break;
							 default:
								 //错误,不是有效属性标记
								 dwAtbs = FILE_ATB_NO;
								 _tprintf(sTip, sPram);
								 return FALSE;
							 }

							//非-xx-格式
							if (sBein[-1] != _T('-') || sBein[1] != _T('-'))
							{
								//清空属性
								dwAtbs = FILE_ATB_NO;
								_tprintf(sTip, sPram);
								return FALSE;
							}

						 }
						 //错误不是'-',也不是属性
						 else if (c != _T('-') || 
							 (c == _T('-') && sBein[-1] == _T('-')))
						 {
							 dwAtbs = FILE_ATB_NO;
							 _tprintf(sTip, sPram);
							 return FALSE;
						 }
					} 
//----------------------------------------------					 
						 //去除属性字符串,只保留扩展名字符串
					     sEnd =  sBein + (*sBein ? 1 :  0);
					     sBein = sPos + (*sBein ? 0 : -1) ;
						 sPos = sBein;

						 while (*sBein = *sEnd)
							  ++sBein, ++sEnd;						
						
//----------------------------------------------
				 }
				 //检查格式是否是*.xxxx...
				 else if (sPos[0] == _T('*'))
				 {
					  bNext = FALSE;
					  sBein = sPos;
					  sEnd = NULL;

						//找到最后一个.位置
						for (++sBein; *sBein && *sBein != _T(','); ++sBein)
						{
							if (*sBein == _T('.'))
								sEnd = sBein;
						}

					  if (sEnd != NULL )
					  {
						WORD nLen ;
						//是否已经找到了*.*
						static BYTE bAll = FALSE;

						//只要遇到*.*就认为全部文件匹配
						if(!bAll) 
							bAll = sEnd[1] == _T('*');

						if (bAll)
						{
							nLen = 0;
							sExts = NULL;
							sEnd = sBein + (*sBein ? 1 : 0);
						}
						else
						{
							nLen = sBein - sEnd - (*sBein ? 0 : 1);
							sExts = sPram;
							++sEnd;
							
						}
						sBein = sPos;
						while (*sBein = _mytolower(*sEnd))
							++sBein, ++sEnd;

						sPos += nLen;
					  }
					  else 
					  {
						  _tprintf(sTip, sPram);
						  return FALSE;
					  }

				 }
				 else
				 {
				    _tprintf(sTip, sPram);
				    return FALSE;
					 //错误
				 }
				
			 }
			 
			 BIT_SET(type, BIT_FILTER, FALSE);


		 }else if(_mysupcmp(sPram, _T("/p")) || _mysupcmp(sPram, _T("-p")))
		 {
			 //设置打印属性
			 BIT_SET(type, BIT_PRINT, TRUE);
			
		 }
		 else if (_mysupcmp(sPram, _T("/f")) || _mysupcmp(sPram, _T("-f")))
		 {
			 //设置过滤属性
			 BIT_SET(type, BIT_FILTER, TRUE); 
		 }
		 else if (_mysupcmp(sPram, _T("/?")) || _mysupcmp(sPram, _T("/help")))
		 {
		    _tprintf(sHelp);
			return FALSE;
         }
		 else if(*sPram)
		 {
			 static BYTE nPram = 0;
			 
			 //判断第一次出现的无格式的参数
			 //是否为可遍历的路径
			 if (!nPram)
			 {
				 if (!PathIsDirectory(sPram))
				 {
					 _tprintf(_T("无效的路径 %s\n"), sPram);
					 return FALSE;
				 }
					 
				//指向可遍历的路径
				sPathDir = sPram;
				 
			 }
			 else 
			 {
				 //遇到无效的参数
				 _tprintf(_T("遇到无效参数 %s\n"), sPram);
				 return FALSE;
			 }
			 ++nPram;
			    
		 }
		 
	}
	//如果用户自定义显示
	//则使用用户设置的列
//----------------------------------------------	
	if (nCValids && bLocalValids && sPathDir)
	{
		lpCValids[ARY_DIR][nCValids] = NULL;
		lpCValids[ARY_FILE][nCValids] = NULL;
		lpStrMaxs[nCValids] = NULL;

		bValids = bLocalValids;
		
    }
	if(bLocalSNS < (BYTE)-1)
		bSNs = bLocalSNS;
	//如果dwAtbs确实被设置过，且dwAtbs的值未变
	 //就证明没有成功设置属性,就不需要显示任何属性的文件
	if (bAtbsSet && dwAtbs == FILE_ATB_ALL)
		dwAtbs = FILE_ATB_NO;

	if (sPathDir == NULL )
	{
		_tprintf(_T("路径不能为空！\n"));
		return FALSE;
	}
		
		
//----------------------------------------------
	return TRUE;
	
}


VOID GetTimeStr(LPStrName strTime, PFILETIME timeFile)
{
	FILETIME timeLocal = { 0 };
	SYSTEMTIME sysTime;

	GetLocalTime(&sysTime);//这里得到的是本地时间，也就是标准时间+时区时间

	FileTimeToLocalFileTime(timeFile, &timeLocal);
	FileTimeToSystemTime(&timeLocal, &sysTime);
	
	
	strTime->len = _stprintf(strTime->str,
				_T("%d/%d/%d %d:%d:%d"),
				sysTime.wYear,
				sysTime.wMonth,
				sysTime.wDay,
				sysTime.wHour,
				sysTime.wMinute,
				sysTime.wSecond);


}
VOID Clear(array *columns, DWORD n)
{
	for (DWORD i = 0; i < n; ++i)
	{
		if (ary_length(columns + i))
		{
			LPStrName *b = (LPStrName*)ary_ptr_begin(columns + i);
			LPStrName *e = (LPStrName*)ary_ptr_end(columns + i);

			for (; b != e; ++b)
				 free(*b);

			ary_destroy(columns + i);
	    }

    }
}

VOID PrintFormat()
{
	HANDLE hConsoleOut = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi = { 0 };
	COORD cdSize,cdColor;
	LPTSTR sLineBuf,sLineIter;
	DWORD nWrite;
	WORD nLine[2];
	WORD color[2];
	WORD nBLineCount, nBCCount;
	WORD idxLine = ARY_FILE;
	LPCTSTR sTitles[COLUMNS_NUMBER] =
	{
		_T("文件名"),
		_T("类型"),
		_T("大小"),
		_T("属性"),
		_T("创建时间"),
		_T("访问时间"),
		_T("修改时间")
	};
	WORD nTitles[] ={6,4,4,4,8,8,8};

	//获取控制台缓冲区大小
	GetConsoleScreenBufferInfo(hConsoleOut, &csbi);
	cdSize = csbi.dwSize;
	cdColor = csbi.dwCursorPosition ;

	//保留控制台背景色
	//设置目录为黄色
	color[0] = csbi.wAttributes & 0xf0 | 0x0e;
	//文件为白色
	color[1] = csbi.wAttributes & 0xf0 | 0x0b;
	
	//取最小的长度
	if (cdSize.X && nWidthLine > cdSize.X )
		nWidthLine = cdSize.X;

	//分配一行缓冲区大小的内存
	sLineBuf = (LPTSTR)malloc(sizeof(TCHAR) * nWidthLine + sizeof(TCHAR));

//----------------------------------------------
	nLine[ARY_DIR] = ary_length(*lpCValids[ARY_DIR]);
	nLine[ARY_FILE] = ary_length(*lpCValids[ARY_FILE]);

	if (nLine[ARY_DIR] || nLine[ARY_DIR] && nLine[ARY_FILE])
		idxLine = ARY_DIR;
	else if (nLine[ARY_FILE])
		idxLine = ARY_FILE;
	else
		idxLine = -1;

//----------------------------------------------
	
	sLineIter = sLineBuf;
	nBLineCount = 0;
	for (WORD X = 0; lpStrMaxs[X]; ++X)
	{
		WORD nCol = *lpStrMaxs[X] + COLUMNS_SPACING;
		WORD nMid = (nCol - COLUMNS_SPACING - nTitles[lpStrMaxs[X] - nStrMax]) / 2;
		LPCTSTR sIter = sTitles[lpStrMaxs[X] - nStrMax];
		nBCCount = 0;

		//填充左边的空格
		while (nBCCount < nMid &&
			nBLineCount < nWidthLine)
		{
			nBCCount += 1;
			nBLineCount += 1;
			*sLineIter++ = _T(' ');
		}
			 						  
		  // 复制字符串到sConsolBuf中
		while (nBCCount < nCol  &&
			nBLineCount < nWidthLine &&
			*sIter)
		{
			nBCCount += 2;
			nBLineCount += 2;
			*sLineIter++ = *sIter++;	
		}
				   
		//填充右边的空格
		while (nBCCount < nCol &&
			nBLineCount < nWidthLine )
		{
			nBCCount += 1;
			nBLineCount += 1;
			*sLineIter++ = _T(' ');
		}			  
	
	}
	*sLineIter = _T('\0');
	_tprintf(_T("\n%s\n"), sLineBuf);
	  
	if (GetConsoleScreenBufferInfo(hConsoleOut, &csbi))
	{
		--csbi.dwCursorPosition.Y;
		FillConsoleOutputAttribute(hConsoleOut,
			0xf,
			nWidthLine,
			csbi.dwCursorPosition,
			&nWrite);
    }

//----------------------------------------------
	sLineIter = sLineBuf;
	nBLineCount = 0;
	for (WORD X = 0; lpStrMaxs[X]; ++X)
	{
		WORD nCol = *lpStrMaxs[X];
		nBCCount = 0;

		// 复制字符串到sConsolBuf中
		while (nBCCount < nCol &&
			nBLineCount < nWidthLine)
		{
			nBCCount += 1;
			nBLineCount += 1;
			*sLineIter++ = _T('-');
		}

		//填充右边的空格
		while (nBCCount < nCol + COLUMNS_SPACING &&
			nBLineCount < nWidthLine)
		{
			nBCCount += 1;
			nBLineCount += 1;
			*sLineIter++ = _T(' ');
		}

		//填充右边的空格
		while (nBCCount < nCol &&
			nBLineCount < nWidthLine)
		{
			nBCCount += 1;
			nBLineCount += 1;
			*sLineIter++ = _T(' ');
		}
	}

	*sLineIter = _T('\0');
	_tprintf(_T("%s\n"), sLineBuf);

//----------------------------------------------

	for (WORD i = idxLine; i < 2; ++i)
	{
		for (WORD Y = 0; Y < nLine[i]; ++Y)
		{
			//屏幕缓冲区字节数
			nBLineCount = 2;
			sLineIter = sLineBuf;
			for (WORD X = 0; lpCValids[i][X]; ++X)
			{			
				WORD nCol = *lpStrMaxs[X] + COLUMNS_SPACING;
				LPCTSTR sIter = ((LPStrName *)ary_ptr_begin(lpCValids[i][X]))[Y]->str;
				nBCCount = 0;
				
				//复制字符串到sConsolBuf中
				while (nBCCount < nCol && 
					nBLineCount < nWidthLine && 
					*sIter)
				{				
					 
					BYTE bTmp = *sIter > 255u;
					nBLineCount += bTmp + 1;
					nBCCount += bTmp + 1;
					*sLineIter++ = *sIter++;

				}

				if (lpCValids[i][X + 1])
				{
					//补齐列不足的空格(包括分隔符)
					while (nBCCount < nCol &&
						nBLineCount < nWidthLine)
					{
						++nBLineCount;
						++nBCCount;
						*sLineIter++ = _T(' ');
					}
				}
					   
			}
			*sLineIter = _T('\0');
			_tprintf(_T("%s\n"), sLineBuf);
		 
			if (GetConsoleScreenBufferInfo(hConsoleOut, &csbi))
			{
				--csbi.dwCursorPosition.Y;
				FillConsoleOutputAttribute(hConsoleOut,
					color[i],
					nWidthLine,
					csbi.dwCursorPosition,
					&nWrite);
			}
			 
		}

	}

	free(sLineBuf);
}
 
