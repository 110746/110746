#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_DEPRECATE

#include <windows.h>
#include <tchar.h>
#include <conio.h>
#include <stdio.h>
#include <time.h>
#include "controls2048.h"
#include "resource1.h"


//两个方块合并时发送分数
//WPARAM保存分数
//LPARAM未使用
#define WM2048_SCORE     WM_USER + 1
//开始处理游戏
#define WM2048_MAINGAME  WM_USER + 2 

/////////////////////////////////////////////////
#define WND_W (640 + 200)
#define WND_H (480 + 450)
//未使用的方块标记
#define INVALID_VALUES -1

//style的数目
#define BMP_N 11 
//列数
#define MAP_XN 4
//行数
#define MAP_YN 4
#define RECT_W 110
#define RECT_H 110
//矩形的圆角角度
#define RECT_ANGLE 10
//背景矩形的圆角角度
#define RECT_ANGLE_BG 20
//方块间距
#define MAP_SPACIN 18
//动画速度(以微妙为单位)
#define ANIMA_SPEED 3000 
#define ANIMA_ZOOM_SPEDD 3500
#define COLOR_BG  RGB(22,22,22)


//新游戏 按钮ID
#define IDB_NEWGANME   1001
//分数编辑框ID
#define IDE_SCORE     1002
//最高分数编辑框ID
#define IDE_MAXSCORE  1003
//显示游戏结束或胜利的编辑框
#define IDE_MSG       1004 
//用来遮挡整个窗口背景的窗口
///////////////遮挡窗口ID////////////////
#define IDW_KEEPOUT    1004
//遮挡地图的那小块窗口,属于IDW_KEEPOUT的子窗口
#define IDW_KEEPOUTMAP 1005
//提示信息编辑框
#define IDE_TIPMSG     1006
//重玩按钮
#define IDB_TRYAGIN    1007
//菜单面板
#define IDW_MP         1008
//返回主菜单按钮
#define IDB_RMM       1009
//菜单项1
#define IDMI_0         1010
//菜单项2
#define IDMI_1         1011
//菜单项3
#define IDMI_2         1012
//菜单项4
#define IDMI_3         1013
//菜单项5
#define IDMI_4         1014
 

typedef struct _tag_rect_anime RectAnime;
typedef struct _tag_rect
{
	LONG left;
	LONG top;
	LONG right;
	LONG bottom;

	POINT rcMove[2];//需要移动的2个方块的X/Y坐标
	int  validCount;   //rcMove的有效计数
	int  mergeCount;  //两个方块合并计数

	int ps;  //当前方块使用的风格下标
	char use; //此区域是否被占用了

} Rectx;


typedef struct _tag_bmps_anime
{
	HBITMAP hBmp;
	HBITMAP hbMask;
	int value; //值

}BmpInfo;

typedef struct _tag_rect_anime
{
	LONG left;
	LONG top;
	LONG right;
	LONG bottom;

	LONG leftOld;
	LONG topOld;
	LONG rightOld;
	LONG bottomOld;

	LONG mX, mY;  //指向要移动到的位置上

				  //4段距离的百分比
#define SG0 0.0f
#define SG1 0.1f
#define SG2 0.4f
#define SG3 1.0f
	static BYTE sps[3];
	BYTE *psps;
	SHORT sgs[4];   //距离
	SHORT *psgs;

	//数字N的贴图索引
	int ps;
	//移动和缩放动画共享的贴图索引
	int psAnime;
	//当前使用的动画数组
	//bmpZooms或bmpMoves
	BmpInfo** bmpAnime;
	//当前方块所执行的动作
	typedef void(*Action)(RectAnime *);
	//如何执行动画
	Action actAnime;
	//如何擦除背景
	Action actDraw;


}RectAnime;
//每段距离的加速度
BYTE RectAnime::sps[3] = { 1,8,16 };


//微秒延时函数
void UsSleep(ULONG ulMicroSeconds);
//获取两个矩形相交的区域rc1为当前要绘制的矩形,rc2为擦除矩形
void GetRectIntersect(RECT *rc1, RECT *re2, RECT *rc, int dir);

 

//////////////////////////////////////////////
//保存所有空的方块
void CopyMapEmptys();
//复制多个rcMap到rcAnime,返回值确定方块是否已满
void CopyMapAnimes();
//检查下一次位置上的方块是否可以移动
bool CheckIsMove();
//初始化
void Initialize();
//在随机空白位置上分配一个方块
RectAnime* AllocRectRdm();
//开始新的游戏
void NewGame();
void DrawRect();
void DrawMove(RectAnime *rca);
void DrawZoom(RectAnime *rca);
void DrawZoomAnime(RectAnime *rca);
bool MoveLeft();
bool MoveUp();
bool MoveRight();
bool MoveDown();


//动作:方块左移
void ActionLeft(RectAnime *rca);
//动作:方块上移
void ActionUp(RectAnime *rca);
//动作:方块右移
void ActionRight(RectAnime *rca);
//动作:方块下移
void ActionDown(RectAnime *rca);
//动作:方块缩放
void ActionZoom(RectAnime *rca);
/////////////////////////////////
bool fcp1(int v1, int v2) { return v1 <= v2; }
bool fcp2(int v1, int v2) { return v1 >= v2; }
/////////////////////////////////////////////


//游戏主入口
void MainGame(int key);

//退出并释放资源
void  ExitCleanUp();


//地图索引
static Rectx rcMap[MAP_YN][MAP_XN];

//缩放动画贴图池
static BmpInfo* bmpPool[BMP_N];


//动画帧列表,从纹理池中选取
static BmpInfo* bmpZooms[BMP_N];

//移动方块贴图列表
static BmpInfo* bmpMoves[BMP_N];
//主窗口的dc
//HDC hDC; 
//绘制整个客户区的dc和bmp，避免WM_PAINT消息闪烁
static HDC dcMClient;
static HBITMAP bmpClient;

//内存dc,用来绘制的

//用于复制bmp的dc
static HDC    dcMem;
//来使用
static HBITMAP bmpDraw;

//主窗口
HWND      hWnd;
HWND hWndTest;
//分数编辑框
HWND hScore;
//最高分数编辑框
HWND hMaxScore; 

//背景
static RECT rcBg;
static SIZE szBg;
static HBITMAP bmpBgMask;
static HBITMAP bmpBg;

//需要绘制的方块(动画)
static RectAnime rcAnime[MAP_YN][MAP_XN];

typedef struct _tag_rect_bpos
{
	char x : 4; //低4位为x坐标
	char y : 4; //高4位为y坐标
}BPos;
//空的方块位置列表
static BPos rcIdxEmpty[MAP_YN * MAP_XN];
//rcIdxEmpty的数目
static char rcIECount;

//Zoom动画最大包围核的大小
static int boxMaxW, boxMaxH;
static char keyCode = -1;
//是否胜利
bool isWin;  



//用默认的值初始化一个控件
void  CtrlInit(HWND hParent);
void  CtrlSetScore(DWORD nScore);
#define SW_WIN 1
#define SW_GV  2
void  CtrlShow(INT idx);
/////////////////////////////// 


//////////////////////////控件定义结束////////////////////////////////// 


LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nShowCmd)
{
	
	static TCHAR lpszAppName[] = TEXT("Wnd_2048");
	MSG       msg;
	WNDCLASS  wc;

	wc.style = CS_HREDRAW | CS_VREDRAW  ;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = lpszAppName;

	// 注册窗口类
	if (!RegisterClass(&wc))
	{
		MessageBox(NULL, TEXT("This program requires Windows NT!"),
			lpszAppName, MB_ICONERROR);
		return 0;
	}

	//通过所需要的客户端大小调整整个窗口
	RECT rcWnd = { 0,0,WND_W,WND_H };
	//没有最大化放大，禁止调整大小的风格
	DWORD wndStyle = WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_SIZEBOX   ;

	AdjustWindowRect(&rcWnd, wndStyle, NULL);
		 
	RECT rcDesktop = { 0 };
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rcDesktop, 0);

    //设置为屏幕居中位置
	rcWnd.right = rcWnd.right - rcWnd.left;
	rcWnd.bottom = rcWnd.bottom - rcWnd.top;
	rcWnd.left = (rcDesktop.right - rcWnd.right) / 2;
	rcWnd.top = (rcDesktop.bottom - rcWnd.bottom) / 2;

	 
	// 创建应用程序主窗口
	hWnd = CtrlCreateWindow(lpszAppName,
			TEXT("2048"), 
			wndStyle | WS_VISIBLE    ,
			rcWnd.left,
			rcWnd.top,
			rcWnd.right ,
			rcWnd.bottom ,
			NULL,
			0);


/*
	hWndTest = CtrlCreateWindow(lpszAppName,
			TEXT("2048"),
			wndStyle | WS_VISIBLE,
			rcWnd.left,
			rcWnd.top,
			rcWnd.right,
			rcWnd.bottom,
			NULL,
			0);
			*/

	Initialize();
	CtrlInit();

	//打开控制台
	AllocConsole();
	freopen("CONOUT$", "w", stdout);

	
	//所有内容都复制到dcMClient中	
	//SelectObject(dcMem, bmpBg);
	//BitBlt(dcMClient, 0, 0, szBg.cx, szBg.cy, dcMem, rcBg.left, rcBg.top, SRCCOPY);

	SetViewportOrgEx(dcMClient, rcBg.left, rcBg.top, NULL);
	SelectObject(dcMem, bmpBgMask);
	BitBlt(dcMClient, 0, 0, szBg.cx, szBg.cy, dcMem, 0, 0, SRCPAINT);
	SelectObject(dcMem, bmpBg);
	BitBlt(dcMClient, 0, 0, szBg.cx, szBg.cy, dcMem, 0, 0, SRCAND);
	SetViewportOrgEx(dcMClient, 0, 0, NULL);

	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);
	CtrlInit(hWnd);
	
	
   //分配2个方块
	for (int i = 0; i <2; ++i)
		DrawZoomAnime(AllocRectRdm());

	 				// 消息循环 	
	while (GetMessage(&msg, 0, 0, 0))
	{

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	
	}
	ExitCleanUp();
	CtrlExit();
	return msg.wParam;
}

 
//
// 窗口过程函数
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static PAINTSTRUCT ps;
	static RECT        *rcPUpdate;
    	
 
	//处理子窗口
	if (GetParent(hWnd))
	{
		switch (message)
		{
		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
			case IDB_TRYAGIN:
				CtrlShow(SW_HIDE);
				NewGame();
				break;
			case IDB_RMM:
				ShowWindow(GetDlgItem(::hWnd, IDW_MP), SW_SHOW);
				ShowWindow(GetDlgItem(::hWnd, IDW_KEEPOUT), SW_HIDE);
				break;
			case IDMI_0:
				puts("菜单0");
				ShowWindow(GetDlgItem(::hWnd, IDW_MP), SW_HIDE);
				SetFocus(::hWnd);
				break;
			case IDMI_1:
				puts("菜单1");
				break;
			case IDMI_2:
				puts("菜单2");
				break;
			case IDMI_3:
				puts("菜单3");
				break;
			case IDMI_4:
				puts("菜单4");
				PostMessage(::hWnd, WM_CLOSE, 0, 0);
				break;
			
			}
		break;

		case WM_NCHITTEST:
			if (GetDlgCtrlID(hWnd) == IDW_MP)
			{
				POINT pt = { LOWORD(lParam),HIWORD(lParam) };
				RECT rc;
				GetClientRect(hWnd, &rc);
				MapWindowPoints(hWnd, HWND_DESKTOP, (POINT*)&rc, 2);
				if (PtInRect(&rc, pt))
					return HTCAPTION;
				
			}
			break;
		case WM_ERASEBKGND:
			return 1;
		}
		 
	
			 
	}else 
	//处理主窗口
	{
		switch (message)
		{
		case WM2048_SCORE:
			CtrlSetScore(wParam);
			break;

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
			case IDB_NEWGANME:
				switch (HIWORD(wParam))
				{
				case WM_SETCURSOR:
					return (LRESULT)LoadCursor(NULL, IDC_HAND);
				case BN_CLICKED:
					NewGame();
					break;

				}
				break;
			case  666:
			{
				static int ary[4] = { SW_GV,SW_WIN,SW_HIDE,SW_SHOW };
				static int *pa = ary;
			 
				CtrlShow(*pa++);
				if (pa == ary + 4)
					pa = ary;
				 
			}
				break;
			}
			break;
		case WM_KEYDOWN:
			switch (wParam)
			{
			case VK_ESCAPE:
				//显示菜单菜单
				ShowWindow(GetDlgItem(hWnd, IDW_MP), SW_SHOW);
				SetFocus(GetDlgItem(hWnd, IDW_MP));
				return 0;

			}

			MainGame(wParam);
			break;
		case WM_ERASEBKGND:
			if (hWndTest == hWnd)
				break;
			return 1;
		case WM_PAINT:
			BeginPaint(hWnd, &ps);
			rcPUpdate = &ps.rcPaint;

			if (hWndTest == hWnd)
			{
				HDC dc = GetDC(::hWnd);
				BitBlt(ps.hdc, 0, 0, WND_H, WND_H,
					dc, 0, 0, SRCCOPY);
			}
			else 
			{
				HBITMAP bmp = CreateCompatibleBitmap(ps.hdc, rcPUpdate->right - rcPUpdate->left, rcPUpdate->bottom - rcPUpdate->top);
				HGDIOBJ o = SelectObject(ps.hdc, bmp);
				//复制部分跟新区域
				BitBlt(ps.hdc, rcPUpdate->left, rcPUpdate->top,
					rcPUpdate->right - rcPUpdate->left,
					rcPUpdate->bottom - rcPUpdate->top,
					dcMClient, rcPUpdate->left, rcPUpdate->top, SRCCOPY);

				DeleteObject(bmp);
				
			}
			
	  
	
			EndPaint(hWnd, &ps);
			
			
			printf("主窗口: l=%d,t=%d,r=%d,b=%d,w=%d,h=%d\n", rcPUpdate->left, rcPUpdate->top, 
			rcPUpdate->right, rcPUpdate->bottom, rcPUpdate->right - rcPUpdate->left,
			  rcPUpdate->bottom - rcPUpdate->top);

			return 0;

		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;

		}
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
	
}


///////////////////////////////////////////////////////////////////////



void MainGame(int key)
{
	static  bool (*actAnime[4])() = { MoveLeft,MoveUp, MoveRight, MoveDown };
	static bool isMove;

	if (isWin) return;

	keyCode = key - VK_LEFT;
	if (keyCode < 0 || keyCode > 3)
		return;
	

	//检查当前是否可以移动,并进行位置设置
	isMove = actAnime[keyCode]();
	//保存所有空闲位置
	CopyMapEmptys();
	if (isMove)
	{		
		//绘制界面
		DrawRect();
		
		if (isWin == true)
		{
			CtrlShow(SW_WIN);
			return;
		}

		//绘制随机新生成的方块的动画
		DrawZoomAnime(AllocRectRdm());
		//恢复动画操作
		CopyMapAnimes();
		
		
	}
	//检查下一次是否可移动
	isMove = CheckIsMove();
	 
	//方块已被填满,且没有可移动的地方
	//证明游戏已经结束
	if (!isMove && !rcIECount)
	{
		CtrlShow(SW_GV);
	}
	 
	//清除所有残留的键盘消息
	static MSG msg;
	while (PeekMessage(&msg, NULL, WM_KEYFIRST, WM_KEYLAST, PM_REMOVE));
	
		 
}
///////////////////////////////////////////////////////////////////////

void UsSleep(ULONG ulMicroSeconds)
{
	static LARGE_INTEGER  timeStop;
	static LARGE_INTEGER  timeStart;
	static LARGE_INTEGER  Freq;
	static LONGLONG  ulTimeToWait;

	if (!QueryPerformanceFrequency(&Freq))
		return;

	QueryPerformanceCounter(&timeStart);

	ulTimeToWait = Freq.QuadPart * ulMicroSeconds / 1000000;

	timeStop = timeStart;

	while ((timeStop.QuadPart - timeStart.QuadPart) < ulTimeToWait)
		QueryPerformanceCounter(&timeStop);

}

void GetRectIntersect(RECT *rc1, RECT *rc2, RECT *rc, int dir)
{

	//0向左移动
	//1向上移动
	//2向右移动
	//3向下移动
	switch (dir)
	{
	case 0:
		//right和bottom其实是长和宽
		rc->right = rc2->right - rc1->right;
		rc->bottom = rc1->bottom - rc1->top;
		rc->left = rc1->right;
		rc->top = rc1->top;
		break;
	case 1:
		rc->right = rc1->right - rc1->left;
		rc->bottom = rc2->bottom - rc1->bottom;
		rc->left = rc1->left;
		rc->top = rc1->bottom;
		break;
	case 2:
		rc->right = rc1->left - rc2->left;
		rc->bottom = rc1->bottom - rc1->top;
		rc->left = rc2->left;
		rc->top = rc1->top;
		break;
	case 3:
		rc->right = rc1->right - rc1->left;
		rc->bottom = rc1->top - rc2->top;
		rc->left = rc1->left;
		rc->top = rc2->top;
		break;

	}
}



///////////////////////////////////////////////////////////////////////

void CopyMapEmptys()
{
	rcIECount = 0;  //空的位置计数
	for (int y = 0; y < MAP_YN; ++y)
	{
		for (int x = 0; x < MAP_XN; ++x)
		{
			if (rcMap[y][x].use == INVALID_VALUES)
			{
				//记录空的位置
				rcIdxEmpty[rcIECount].x = x;
				rcIdxEmpty[rcIECount++].y = y;
			}
		}		

	}
}


void CopyMapAnimes()
{
	static Rectx* rcm;
	static RectAnime *rca;
	

	for (int y = 0; y < MAP_YN; ++y)
	{
		for (int x = 0; x < MAP_XN; ++x)
		{
			rcm = rcMap[y] + x;

			if (rcm->use != INVALID_VALUES)
			{
				rcm->rcMove[0].x = x;
				rcm->rcMove[0].y = y;
				rcm->rcMove[1].x = INVALID_VALUES;
				rcm->validCount = 1;
				rcm->mergeCount = 0;

				rca = rcAnime[y] + x;
				rca->mX = x;
				rca->mY = y;
				*(RECT*)rca = *(RECT*)rcm;
				*(RECT*)&rca->leftOld = *(RECT*)rcm;
				rca->ps = rcm->ps;
				//设置动画从第0帧开始
				rca->psAnime = 0;
				//默认贴图设置为移动动画
				rca->bmpAnime = bmpMoves;
				rca->actDraw = DrawMove;

			}

		}
	}
}

 
bool CheckIsMove()
{	

	 Rectx *rcm;
	//"十字"检查
	for (int y = 0; y < MAP_YN ; ++y)
	{
		for (int x = 0; x < MAP_XN ; ++x)
		{
		
			rcm = rcMap[y] + x;

			if (rcm->use == INVALID_VALUES)
				continue;

			if (x > 0)
			{
				//检查左边是否有空闲的位置
				if (rcm[-1].use == INVALID_VALUES)
					return true;
				//检查左边的值是否相等
				if (bmpMoves[rcm[0].ps]->value == bmpMoves[rcm[-1].ps]->value)
					return true;
		     }
		
			if (y > 0)
			{
				//检查上边是否有空闲的位置
				if (rcm[-MAP_XN].use == INVALID_VALUES)
					return true;
				//检查左边的值是否相等
				if (bmpMoves[rcm[0].ps]->value == bmpMoves[rcm[-MAP_XN].ps]->value)
					return true;
			}
	
			if (x < MAP_XN - 1)
			{
				//检查右边是否有空闲的位置
				if (rcm[1].use == INVALID_VALUES)
					return true;
				//检查右边的值是否相等
				if (bmpMoves[rcm[0].ps]->value == bmpMoves[rcm[1].ps]->value)
					return true;
			}
	

			if (y < MAP_YN - 1)
			{
				//检查下边是否有空闲的位置
				if (rcm[MAP_XN].use == INVALID_VALUES)
					return true;
				//检查下边的值是否相等
				if (bmpMoves[rcm[0].ps]->value == bmpMoves[rcm[MAP_XN].ps]->value)
					return true;
		    }


		}
	}
		
		  
	return false;
}
///////////////////////////////////////////////////////////////////////


void ActionLeft(RectAnime *rca)
{

	 Rectx *rc = rcMap[rca->mY] + rca->mX;

	if (rca->left == rc->left)
	{
		//动画完成
		rca->actAnime = NULL;
		//合并计数+1
		++rc->mergeCount;
		return;
	}

	if (rca->left <= *rca->psgs)
	{
		if (rca->psgs > rca->sgs)
			--rca->psgs;

		--rca->psps;

	}

	rca->left -= *rca->psps;
	rca->right -= *rca->psps;

}
void ActionUp(RectAnime *rca)
{
	 Rectx *rc = rcMap[rca->mY] + rca->mX;

	if (rca->top == rc->top)
	{
		//动画完成
		rca->actAnime = NULL;
		//合并计数+1
		++rc->mergeCount;
		return;
	}

	if (rca->top <= *rca->psgs)
	{
		if (rca->psgs > rca->sgs)
			--rca->psgs;

		--rca->psps;

	}

	rca->top -= *rca->psps;
	rca->bottom -= *rca->psps;
}
void ActionRight(RectAnime *rca)
{
	 Rectx *rc = rcMap[rca->mY] + rca->mX;

	if (rca->left == rc->left)
	{
		//动画完成
		rca->actAnime = NULL;
		//合并计数+1
		++rc->mergeCount;
		return;
	}

	if (rca->left >= *rca->psgs)
	{
		if (rca->psgs > rca->sgs)
			--rca->psgs;

		--rca->psps;

	}

	rca->left += *rca->psps;
	rca->right += *rca->psps;

}
void ActionDown(RectAnime *rca)
{
	 Rectx *rc = rcMap[rca->mY] + rca->mX;

	if (rca->top == rc->top)
	{
		//动画完成
		rca->actAnime = NULL;
		//合并计数+1
		++rc->mergeCount;
		return;
	}

	if (rca->top >= *rca->psgs)
	{
		if (rca->psgs > rca->sgs)
			--rca->psgs;

		--rca->psps;

	}

	rca->top += *rca->psps;
	rca->bottom += *rca->psps;
}

void ActionZoom(RectAnime *rca)
{
	//(rca->bmpAnime[rca->ps][rca->psAnime].hbMask == NULL)
	if (rca->bmpAnime[rca->ps][rca->psAnime + 1].hbMask == NULL)
	{
		rca->actAnime = NULL;
		rca->psAnime = 0;
		rca->bmpAnime = bmpMoves;
		return;
	}

	++rca->psAnime;

}
////////////////////////////////////////////////////////////////////////
void Initialize()
{
	RECT rcClient;
	GetClientRect(hWnd, &rcClient);
	const int bgW = RECT_W * MAP_XN + MAP_SPACIN * (MAP_XN + 1);
	const int bgH = RECT_H * MAP_YN + MAP_SPACIN * (MAP_YN + 1);

	rcBg.left = WND_W / 2 - bgW / 2;
	rcBg.top = WND_H / 2 - bgH / 2;
	rcBg.right = rcBg.left + bgW;
	rcBg.bottom = rcBg.top + bgH;
	szBg.cx = bgW;
	szBg.cy = bgH;

	 Rectx *rcm = *rcMap;
	RectAnime *rca = *rcAnime;
	int x, y = 0 + MAP_SPACIN;

	for (int i = 0; i < MAP_YN; ++i)
	{
		x = 0 + MAP_SPACIN;
		for (int j = 0; j < MAP_XN; ++j, ++rcm, ++rca)
		{
			rcm->left = x;
			rcm->top = y;
			rcm->right = rcm->left + RECT_W;
			rcm->bottom = rcm->top + RECT_H;
			rcm->use = INVALID_VALUES;   //标记为未使用

			rca->mX = INVALID_VALUES;
			rca->mY = INVALID_VALUES;

			x += RECT_W + MAP_SPACIN;

			rcIdxEmpty[rcIECount].x = i;
			rcIdxEmpty[rcIECount++].y = j;

		}
		y += RECT_H + MAP_SPACIN;
	}


	///////////////创建整个客户区的DC////////////////
	HDC hDC = GetDC(hWnd);
	bmpClient = CreateCompatibleBitmap(hDC, WND_W, WND_H);
	dcMClient = CreateCompatibleDC(hDC);	
	SelectObject(dcMClient, bmpClient);


	HBITMAP bmp = (HBITMAP)LoadImage(GetModuleHandle(NULL), 
		MAKEINTRESOURCE(IDB_BITMAP1), IMAGE_BITMAP,
		0, 0, LR_DEFAULTSIZE);

	HBRUSH br = CreatePatternBrush(bmp);
	FillRect(dcMClient, &rcClient, br);
	DeleteObject(bmp);
	
	//////////////生成背景纹理////////////////////////////////////

	//创建公用内存dc
	dcMem = CreateCompatibleDC(hDC);
	bmpBgMask = CreateCompatibleBitmap(hDC, bgW, bgH);
	bmpBg = CreateCompatibleBitmap(hDC, bgW, bgH);

	//边框
	HPEN  pBg = CreatePen(PS_NULL, 1, RGB(187, 173, 160));

	//背景色
	HBRUSH bBg1 = CreateSolidBrush(RGB(187, 173, 160));
	//方块色
	HBRUSH bBg2 = CreateSolidBrush(RGB(205, 193, 180));

	//画背景掩码
	SelectObject(dcMem, bmpBgMask);
	SelectObject(dcMem, GetStockObject(WHITE_BRUSH));
	RoundRect(dcMem, 0, 0, bgW, bgH, 20, 20);

	//画背景
	SelectObject(dcMem, bmpBg);
	SelectObject(dcMem, pBg);
	SelectObject(dcMem, bBg1);
	RECT rcDrawBg = { 0,0,bgW,bgH };
	FillRect(dcMem, &rcDrawBg, (HBRUSH)GetStockObject(WHITE_BRUSH));
	RoundRect(dcMem, 0, 0, bgW, bgH, RECT_ANGLE_BG, RECT_ANGLE_BG);

	SelectObject(dcMem, bBg2);
	rcm = *rcMap;
	for (int i = 0; i < MAP_YN * MAP_XN; ++i, ++rcm)
	{
		RoundRect(dcMem, rcm->left, rcm->top,
			rcm->right, rcm->bottom,
			RECT_ANGLE, RECT_ANGLE);
	}
	DeleteObject(pBg);
	DeleteObject(bBg1);
	DeleteObject(bBg2);

 
///////////////////////////////////////////////////////////////////
	COLORREF bgColors[BMP_N];
	COLORREF txtColors[BMP_N];
	bgColors[0] = RGB(238, 228, 218);
	txtColors[0] = RGB(157, 133, 113);

	bgColors[1] = RGB(238, 224, 198);
	txtColors[1] = RGB(116, 107, 100);

	bgColors[2] = RGB(242, 176, 116);
	txtColors[2] = RGB(244, 244, 255);

	//16
	bgColors[3] = RGB(253, 145, 92);
	txtColors[3] = RGB(244, 244, 255);

	//32
	bgColors[4] = RGB(247, 124, 90);
	txtColors[4] = RGB(244, 244, 255);

	//64
	bgColors[5] = RGB(248, 93, 49);
	txtColors[5] = RGB(244, 244, 255);

	//128
	bgColors[6] = RGB(236, 206, 107);
	txtColors[6] = RGB(244, 244, 255);

	//256
	bgColors[7] = RGB(236, 203, 87);
	txtColors[7] = RGB(244, 244, 255);

	//512
	bgColors[8] = RGB(237, 200, 67);
	txtColors[8] = RGB(244, 244, 255);

	//1024
	bgColors[9] = RGB(236, 197, 46);
	txtColors[9] = RGB(244, 244, 255);

	//2048
	bgColors[10] = RGB(244, 186, 1);
	txtColors[10] = RGB(244, 244, 255);

	LONG w, h;

	//中间矩形
	RECT rect = { 0,0,RECT_W ,RECT_H };
	RECT *rcMiddle = &rect;

	RECT boxs[4];
	//初始矩形
	w = (LONG)(RECT_W * 0.2f);
	h = (LONG)(RECT_H * 0.2f);
	boxs[0].left = rcMiddle->left + (RECT_W - w) / 2;
	boxs[0].top = rcMiddle->top + (RECT_H - h) / 2;
	boxs[0].right = boxs[0].left + w;
	boxs[0].bottom = boxs[0].top + h;

	//外面的矩形
	w = (LONG)(RECT_W * 1.18f);
	h = (LONG)(RECT_H * 1.18f);
	boxs[1].left = rcMiddle->left + (RECT_W - w) / 2;
	boxs[1].top = rcMiddle->top + (RECT_H - h) / 2;
	boxs[1].right = boxs[1].left + w;
	boxs[1].bottom = boxs[1].top + h;

	//里面矩形
	w = (LONG)(RECT_W * 0.6f);
	h = (LONG)(RECT_H * 0.6f);
	boxs[2].left = rcMiddle->left + (RECT_W - w) / 2;
	boxs[2].top = rcMiddle->top + (RECT_H - h) / 2;
	boxs[2].right = boxs[2].left + w;
	boxs[2].bottom = boxs[2].top + h;

	//原矩形
	boxs[3] = *rcMiddle;


	LONG len;
	RECT rcZoom = boxs[0];
	int boxMinW = boxs[0].right - boxs[0].left;
	int boxMinH = boxs[0].bottom - boxs[0].top;
	boxMaxW = boxs[1].right - boxs[1].left;
	boxMaxH = boxs[1].bottom - boxs[1].top;
	int idx = 1;


	len = boxs[0].left - boxs[1].left;
	//指向缩放动画贴图池
	BmpInfo *pbzp;

	int txtSize, txtActSize;
	HFONT hFont;
	LOGFONT lgFont = { 0 };

	//hFont = (HFONT)GetCurrentObject(hDCX, OBJ_FONT);
	//GetObject(hFont, sizeof(lgFont), &lgFont);
	lgFont.lfWeight = FW_EXTRABOLD;
	_tcscpy(lgFont.lfFaceName, _T("微软雅黑"));
	lgFont.lfQuality = ANTIALIASED_QUALITY;


	//2~2048 BMP_N种方块风格的字体缩放比例
	const float txtZP[BMP_N] = { 1.0f,1.0f,1.0f, 0.8f,0.8f,0.8f,0.6f,0.6f,0.6f,0.5f,0.5f };
	const TCHAR *sNumber[BMP_N] = { _T("2"),_T("4"),_T("8"),_T("16"),_T("32"),_T("64"),_T("128"),_T("256"),_T("512"),_T("1024"),_T("2048") };

	HBRUSH bBlack = CreateSolidBrush(RGB(0, 0, 0));
	HBRUSH bWhite = CreateSolidBrush(RGB(255, 255, 255));
	HBRUSH bDraw;
	HPEN   pDraw = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));

	////////////////////////////生成纹理池//////////////////////////////////////////////////
	//字体最大大小
	txtSize = 90;

	SetBkMode(dcMem, TRANSPARENT);
	//去除黑色边框
	SelectObject(dcMem, pDraw);


	for (int i = 0; i < BMP_N; ++i)
	{
		//多出来的一个为空
		bmpPool[i] = (BmpInfo*)calloc(1, (1 + len) * sizeof(BmpInfo));
		pbzp = bmpPool[i];

		w = boxMinW;
		h = boxMinH;

		rcZoom = boxs[0];
		rcZoom.left = (boxMaxW - boxMinW) / 2;
		rcZoom.top = (boxMaxH - boxMinH) / 2;
		rcZoom.right = rcZoom.left + w;
		rcZoom.bottom = rcZoom.top + h;

		txtActSize = (LONG)(txtSize * txtZP[i]);
		
		bDraw = CreateSolidBrush(bgColors[i]);
		SetTextColor(dcMem, txtColors[i]);

	 
		while (rcZoom.left > 0)
		{
			
			//创建字体
			lgFont.lfHeight = (LONG)(h / 100.0f * txtActSize);
			hFont = CreateFontIndirect(&lgFont);
			//Font font(dcMem, &lgFont);
	       
			
			//生成掩码图(白色边框，黑色形状)
			pbzp->hbMask = CreateCompatibleBitmap(hDC, boxMaxW, boxMaxH);
			SelectObject(dcMem, pbzp->hbMask);
			static RECT r = { 0,0,boxMaxW, boxMaxH };
			FillRect(dcMem, &r, bBlack);
			SelectObject(dcMem, bWhite);
			RoundRect(dcMem, rcZoom.left, rcZoom.top, rcZoom.right, rcZoom.bottom, RECT_ANGLE, RECT_ANGLE);
			
			//生成纹理图
			pbzp->hBmp = CreateCompatibleBitmap(hDC, boxMaxW, boxMaxH);
			SelectObject(dcMem, pbzp->hBmp);
			SelectObject(dcMem, hFont);
			FillRect(dcMem, &r, bWhite);
			SelectObject(dcMem, bDraw);
			RoundRect(dcMem, rcZoom.left, rcZoom.top, rcZoom.right, rcZoom.bottom, RECT_ANGLE, RECT_ANGLE);
			DrawText(dcMem, sNumber[i], -1, &rcZoom, DT_VCENTER | DT_CENTER | DT_SINGLELINE);


			--rcZoom.left;
			++rcZoom.right;
			--rcZoom.top;
			++rcZoom.bottom;
			w += 2;
			h += 2;
			++pbzp;

			DeleteObject(hFont);

		}
		DeleteObject(bDraw);
	}

	DeleteObject(bBlack);
	DeleteObject(bWhite);
	DeleteObject(pDraw);

	///////////////////////////////////////////////////////////////////
	//动画box的圈数
	const int ZOOM_N = 3;
	//动画的加速度的段数
	const int   ZOOM_SG_N = 3;
	//每段的距离百分比
	const float ZOOM_SG0 = 0.3f;
	const float ZOOM_SG1 = 0.8f;
	const float ZOOM_SG2 = 1.0f;
	static char sps[ZOOM_N][ZOOM_SG_N] = { { 2,4,2 },{ -2,-4,-2 },{ 2,4,2 } };

	char(*pspss)[ZOOM_SG_N] = sps, *psps = *sps;
	int   sgs[ZOOM_N][ZOOM_SG_N];
	int(*psgss)[ZOOM_SG_N] = sgs, *psgs = *psgss;
	bool(*cmp[ZOOM_N])(int, int) = { fcp1,fcp2,fcp1 };


	for (int i = 0; i < ZOOM_N; ++i)
	{
		int len = boxs[i].left - boxs[i + 1].left;
		sgs[i][0] = (LONG)(boxs[i].left - len * ZOOM_SG0);
		sgs[i][1] = (LONG)(boxs[i].left - len * ZOOM_SG1);
		sgs[i][2] = (LONG)(boxs[i].left - len * ZOOM_SG2);
	}

	for (int i = 0; i < BMP_N; ++i)
	{

		rcZoom = boxs[0];
		//默认最外圈停止
		idx = 1;

		pspss = sps;
		psgss = sgs;
		psps = *sps;
		psgs = *psgss;

		int bzIdx = 0;
		int v = 0;

		//预先分配最大动画贴图数
		bmpZooms[i] = (BmpInfo*)calloc(ZOOM_N, len * sizeof(BmpInfo));

		while (true)
		{
			//把不连续的每一帧按顺序存储到赋值符号左边的空间中
			//(设置缩放动画帧列表)
			bmpZooms[i][bzIdx++] = bmpPool[i][v];

			if (cmp[idx - 1](rcZoom.left, boxs[idx].left))
			{
				if (idx == ZOOM_N)
				{
					//(设置移动动画帧列表)
					//因为只有一个动画,所以无需分配新空间
					//直接指向贴图池特定贴图地址就行
					bmpMoves[i] = bmpPool[i] + v;
					bmpMoves[i]->value = _wtoi(sNumber[i]);
				}

				if (++idx == 4)
					break;

				psps = *++pspss;
				psgs = *++psgss;
			}

			if (cmp[idx - 1](rcZoom.left, *psgs))
			{
				++psgs;
				++psps;
			}

			v = *psps / 2;
			rcZoom.left -= v;
			rcZoom.right += v;


			v = abs(rcZoom.left - boxs[0].left);
			v = v ? v - 1 : 0;

		}

		//调整到合适的大小,+1空出来一个空的(所有成员均为0)
		bmpZooms[i] = (BmpInfo*)realloc(bmpZooms[i], (bzIdx + 1) * sizeof(BmpInfo));
	}
	//////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////
	srand((unsigned)time(NULL));
	ReleaseDC(hWnd, hDC);
}

void test_read_map()
{
	FILE* pf = fopen("map", "rb");
	fread(rcMap, sizeof(rcMap), 1, pf);

	 Rectx *rcX;
	RectAnime *rca;
	POINT *prca;

	for (int y = 0; y < MAP_YN; ++y)
	{
		for (int x = 0; x < MAP_XN; ++x)
		{
			rcX = rcMap[y] + x;

			if (rcX->use != INVALID_VALUES)
			{
				prca = rcX->rcMove;
				for (int i = 0; i < rcX->validCount; ++i)
				{
					rca = rcAnime[prca[i].y] + prca[i].x;

					fread(&rca->left, sizeof(RECT), 1, pf);
					fread(&rca->mX, sizeof(POINT), 1, pf);
					fread(rca->sgs, sizeof(SHORT) * sizeof(rca->sgs) / sizeof(*rca->sgs), 1, pf);
					fread(&rca->ps, sizeof(rca->ps), 1, pf);
					fread(&rca->psAnime, sizeof(rca->psAnime), 1, pf);

					char c;
					fread(&c, 1, 1, pf);
					switch (c)
					{
					case 0:
						rca->bmpAnime = bmpMoves;
						break;
					case 1:
						rca->bmpAnime = bmpZooms;
						break;
					}

					fread(&c, 1, 1, pf);
					switch (c)
					{
					case 0:
						rca->actAnime = NULL;
						break;
					case 1:
						rca->actAnime = ActionLeft;
						break;
					case 2:
						rca->actAnime = ActionUp;
						break;
					case 3:
						rca->actAnime = ActionRight;
						break;
					case 4:
						rca->actAnime = ActionDown;
						break;
					case 5:
						rca->actAnime = ActionZoom;
						break;
					}
				}
			}
		}
	}


	//fread(rcAnime, sizeof(rcAnime), 1, pf);

	fclose(pf);
}



void test_save_map()
{
	FILE* pf = fopen("map", "wb");
	fwrite(rcMap, sizeof(rcMap), 1, pf);


	 Rectx *rcX;
	RectAnime *rca;
	POINT *prca;


	for (int y = 0; y < MAP_YN; ++y)
	{
		for (int x = 0; x < MAP_XN; ++x)
		{
			rcX = rcMap[y] + x;

			if (rcX->use != INVALID_VALUES)
			{
				prca = rcX->rcMove;
				for (int i = 0; i < rcX->validCount; ++i)
				{
					rca = rcAnime[prca[i].y] + prca[i].x;

					fwrite(&rca->left, sizeof(RECT), 1, pf);
					fwrite(&rca->mX, sizeof(POINT), 1, pf);
					fwrite(rca->sgs, sizeof(SHORT) * (sizeof(rca->sgs) / sizeof(*rca->sgs)), 1, pf);
					fwrite(&rca->ps, sizeof(rca->ps), 1, pf);
					fwrite(&rca->psAnime, sizeof(rca->psAnime), 1, pf);

					char c;
					if (rca->bmpAnime == bmpMoves)
						c = 0;
					else if (rca->bmpAnime == bmpZooms)
						c = 1;
					fwrite(&c, 1, 1, pf);

					if (rca->actAnime == NULL)
						c = 0;
					else if (rca->actAnime == ActionLeft)
						c = 1;
					else if (rca->actAnime == ActionUp)
						c = 2;
					else if (rca->actAnime == ActionRight)
						c = 3;
					else if (rca->actAnime == ActionDown)
						c = 4;
					else if (rca->actAnime == ActionZoom)
						c = 5;
					fwrite(&c, 1, 1, pf);

				}
			}
		}
	}

	//fwrite(rcAnime, sizeof(rcAnime), 1, pf);


	fclose(pf);
}

RectAnime* AllocRectRdm()
{

	//test_read_map();
	//return NULL;
	BPos *bps;
	 Rectx *rcm;
	RectAnime *rca;
	//非全局isMove
	bool isMove = 0;


		bps = rcIdxEmpty + rand() % rcIECount--;

		rcm = rcMap[bps->y] + bps->x;
		rca = rcAnime[bps->y] + bps->x;
		rcm->rcMove[0].x = bps->x;
		rcm->rcMove[0].y = bps->y;
		rcm->rcMove[1].x = INVALID_VALUES;
		rcm->rcMove[1].y = INVALID_VALUES;
		rcm->validCount = 1;
		rcm->mergeCount = 0;
		rcm->use = 1;
		rcm->ps = rand() % 2;

		rca->mX = bps->x;
		rca->mY = bps->y;
		*(RECT*)rca = *(RECT*)rcm;
		*(RECT*)&rca->leftOld = *(RECT*)rcm;

		rca->ps = rcm->ps;
		//设置动画从第0帧开始
		rca->psAnime = 0;
		//默认贴图设置为移动动画
		rca->bmpAnime = bmpZooms;
		rca->actAnime = ActionZoom;
		rca->actDraw = DrawZoom;


	return rca;
}

void NewGame()
{
	//清除背景
	SetViewportOrgEx(dcMClient, rcBg.left, rcBg.top, NULL);
	SelectObject(dcMem, bmpBgMask);
	BitBlt(dcMClient, 0, 0, szBg.cx, szBg.cy, dcMem, 0, 0, SRCPAINT);
	SelectObject(dcMem, bmpBg);
	BitBlt(dcMClient, 0, 0, szBg.cx, szBg.cy, dcMem, 0, 0, SRCAND);
	SetViewportOrgEx(dcMClient, 0, 0, NULL);

	Rectx *rcm;
	RectAnime *rca;

	rcIECount = 0;  //空的位置计数
	for (int y = 0; y < MAP_YN; ++y)
	{
		for (int x = 0; x < MAP_XN; ++x)
		{
			rcm = rcMap[y] + x;
			rca = rcAnime[y] + x;

			rcm->rcMove[0].x = x;
			rcm->rcMove[0].y = y;
			rcm->rcMove[1].x = INVALID_VALUES;
			rcm->validCount = 1;
			rcm->mergeCount = 0;
			rcm->ps = 0;
			rcm->use = INVALID_VALUES;

			rca->mX = x;
			rca->mY = y;
			*(RECT*)rca = *(RECT*)rcm;
			*(RECT*)&rca->leftOld = *(RECT*)rcm;
			rca->ps = rcm->ps;
			//设置动画从第0帧开始
			rca->psAnime = 0;
			//默认贴图设置为移动动画
			rca->bmpAnime = bmpMoves;
			rca->actDraw = DrawMove;

			//记录空的位置
			rcIdxEmpty[rcIECount].x = x;
			rcIdxEmpty[rcIECount++].y = y;
		}

	}

	 
	RedrawWindow(hWnd, &rcBg, NULL,
		RDW_UPDATENOW | RDW_INVALIDATE | RDW_NOCHILDREN);

	for(int i = 0 ; i < 2 ; ++i)
	   DrawZoomAnime(AllocRectRdm());

	isWin = false;

}

void DrawRect()
{
	 Rectx *rcX;
	RectAnime *rca;
	POINT *prca;
	bool bOk; //所有方块绘制是否完成
	static RECT rcUpdate;
	static const int v1 = (boxMaxW - RECT_W) / 2;
	static const int v2 = (boxMaxH - RECT_H) / 2;
	 

	do
	{
		bOk = false;
		rcUpdate.left = 0;

		for (int y = 0; y < MAP_YN; ++y)
		{
			for (int x = 0; x < MAP_XN; ++x)
			{
				rcX = rcMap[y] + x;

				if (rcX->use != INVALID_VALUES)
				{
					prca = rcX->rcMove;
					for (int i = 0; i < rcX->validCount; ++i)
					{
						rca = rcAnime[prca[i].y] + prca[i].x;

						if (rca->actAnime)
						{
							rca->actAnime(rca);
							if (rcX->mergeCount == rcX->validCount)
							{
								if (rcX->mergeCount == 2)
								{
									rca = rcAnime[prca[0].y] + prca[0].x;
									rca->actAnime = ActionZoom;
									rca->actDraw = DrawZoom;
									rca->bmpAnime = bmpZooms; //切记，一定要指向动画
									rca->ps = rcX->ps;
									rcX->validCount = 1;

									//获胜了
									if (rcX->ps == BMP_N - 1)
										isWin = true;
									
									//加分
									PostMessage(hWnd, WM2048_SCORE, bmpMoves[rca->ps]->value, 0);
								}
								
								//到达终点合并计数归零
								rcX->mergeCount = 0;

							}
							bOk = true;
						}

					
						if (rcUpdate.left == 0)
						{
							rcUpdate.left = rca->left;
							rcUpdate.right = rca->right;
							rcUpdate.top = rca->top;
							rcUpdate.bottom = rca->bottom;
						}
						
						switch (keyCode)
						{
						case 0:
							if (rcUpdate.left > rca->left)
								rcUpdate.left = rca->left;
							if (rcUpdate.right < rca->rightOld)
								rcUpdate.right = rca->rightOld;
							if (rcUpdate.top > rca->top)
								rcUpdate.top = rca->top;
							if (rcUpdate.bottom < rca->bottom)
								rcUpdate.bottom = rca->bottom;
							break;

						case 1:
							if (rcUpdate.top > rca->top)
								rcUpdate.top = rca->top;
							if (rcUpdate.bottom < rca->bottomOld)
								rcUpdate.bottom = rca->bottomOld;
							if (rcUpdate.left > rca->left)
								rcUpdate.left = rca->left;
							if (rcUpdate.right < rca->right)
								rcUpdate.right = rca->right;
							break;

						case 2:
							if (rcUpdate.left > rca->leftOld)
								rcUpdate.left = rca->leftOld;
							if (rcUpdate.right < rca->right)
								rcUpdate.right = rca->right;
							if (rcUpdate.top > rca->top)
								rcUpdate.top = rca->top;
							if (rcUpdate.bottom < rca->bottom)
								rcUpdate.bottom = rca->bottom;
							break;

						case 3:
							if (rcUpdate.top > rca->topOld)
								rcUpdate.top = rca->topOld;
							if (rcUpdate.bottom < rca->bottom)
								rcUpdate.bottom = rca->bottom;
							if (rcUpdate.left > rca->left)
								rcUpdate.left = rca->left;
							if (rcUpdate.right < rca->right)
								rcUpdate.right = rca->right;
							break;
						}
						

						//开始绘制
						rca->actDraw(rca);				
					}

				}

			}
		}

	 
		rcUpdate.left += rcBg.left;
		rcUpdate.right += rcBg.left;
		rcUpdate.top += rcBg.top;
		rcUpdate.bottom += rcBg.top;
		 		
	   RedrawWindow(hWnd, &rcUpdate, NULL, 
		   RDW_UPDATENOW | RDW_INVALIDATE | RDW_NOCHILDREN);
		
		 
	/*
		BitBlt(hDC, rcUpdate.left, rcUpdate.top,
			rcUpdate.right - rcUpdate.left,
			rcUpdate.bottom - rcUpdate.top,
			dcMClient, rcUpdate.left, rcUpdate.top,
			SRCCOPY);
	*/
	 
		 UsSleep(ANIMA_SPEED);
	} while (bOk);


}


void DrawMove(RectAnime *rca)
{
	static const int v1 = (boxMaxW - RECT_W) / 2;
	static const int v2 = (boxMaxH - RECT_H) / 2;
	static const int v3 = RECT_ANGLE / 2;
	static const int v4 = RECT_ANGLE / 2 - 1;
	static RECT rcAdjust;
	RECT *rcCur = (RECT*)&rca->left;
	RECT *rcOld = (RECT*)&rca->leftOld;
	HBITMAP bmpMask = rca->bmpAnime[rca->ps][rca->psAnime].hbMask;
	HBITMAP bmpDraw = rca->bmpAnime[rca->ps][rca->psAnime].hBmp;

	//把坐标映射到游戏背景大小
	SetViewportOrgEx(dcMClient, rcBg.left, rcBg.top, NULL);

	//取得矩形贴图的形状部分绘制掩码和图形
	SelectObject(dcMem, bmpMask);
	BitBlt(dcMClient, rca->left, rca->top, RECT_W, RECT_H,
		dcMem, v1, v2, SRCPAINT);


	//贴纹理
	SelectObject(dcMem, bmpDraw);
	BitBlt(dcMClient, rca->left, rca->top, RECT_W, RECT_H,
		dcMem, v1, v2, SRCAND);

	
	//0向左移动
	//1向上移动
	//2向右移动
	//3向下移动
	//用贴图填充两个矩形相交的部分
	switch (keyCode)
	{
	case 0:
		//right和bottom其实是长和宽
		rcAdjust.right = rcOld->right - rcCur->right;
		rcAdjust.bottom = rcCur->bottom - rcCur->top;
		rcAdjust.left = rcCur->right;
		rcAdjust.top = rcCur->top;

		SelectObject(dcMem, bmpBg);
		BitBlt(dcMClient, rcAdjust.left - v4, rcAdjust.top,
			rcAdjust.right + v3, rcAdjust.bottom, dcMem,
			rcAdjust.left - v4, rcAdjust.top, SRCCOPY);

		SelectObject(dcMem, bmpMask);
		BitBlt(dcMClient, rca->right - v4, rca->top, v3, RECT_H,
			dcMem, boxMaxW - v1 - v3, v2, SRCPAINT);

		SelectObject(dcMem, bmpDraw);
		BitBlt(dcMClient, rca->right - v4, rca->top, v3, RECT_H,
			dcMem, boxMaxW - v1 - v3, v2, SRCAND);
		break;
	case 1:
		rcAdjust.right = rcCur->right - rcCur->left;
		rcAdjust.bottom = rcOld->bottom - rcCur->bottom;
		rcAdjust.left = rcCur->left;
		rcAdjust.top = rcCur->bottom;

		SelectObject(dcMem, bmpBg);
		BitBlt(dcMClient, rcAdjust.left, rcAdjust.top - v4,
			rcAdjust.right, rcAdjust.bottom + v3, dcMem,
			rcAdjust.left, rcAdjust.top - v4, SRCCOPY);

		SelectObject(dcMem, bmpMask);
		BitBlt(dcMClient, rca->left - 0, rca->bottom - v4, RECT_W, v3,
			dcMem, v1, boxMaxH - v2 - v3, SRCPAINT);

		SelectObject(dcMem, bmpDraw);
		BitBlt(dcMClient, rca->left - 0, rca->bottom - v4, RECT_W, v3,
			dcMem, v1, boxMaxH - v2 - v3, SRCAND);

		break;
	case 2:
		rcAdjust.right = rcCur->left - rcOld->left;
		rcAdjust.bottom = rcCur->bottom - rcCur->top;
		rcAdjust.left = rcOld->left;
		rcAdjust.top = rcCur->top;

		SelectObject(dcMem, bmpBg);
		BitBlt(dcMClient, rcAdjust.left, rcAdjust.top,
			rcAdjust.right + v4, rcAdjust.bottom, dcMem,
			rcAdjust.left, rcAdjust.top, SRCCOPY);

		SelectObject(dcMem, bmpMask);
		BitBlt(dcMClient, rca->left, rca->top, v3, RECT_H,
			dcMem, v1, v2, SRCPAINT);

		SelectObject(dcMem, bmpDraw);
		BitBlt(dcMClient, rca->left, rca->top, v3, RECT_H,
			dcMem, v1, v2, SRCAND);
		break;
	case 3:
		rcAdjust.right = rcCur->right - rcCur->left;
		rcAdjust.bottom = rcCur->top - rcOld->top;
		rcAdjust.left = rcCur->left;
		rcAdjust.top = rcOld->top;

		SelectObject(dcMem, bmpBg);
		BitBlt(dcMClient, rcAdjust.left, rcAdjust.top,
			rcAdjust.right, rcAdjust.bottom + v4, dcMem,
			rcAdjust.left, rcAdjust.top, SRCCOPY);

		SelectObject(dcMem, bmpMask);
		BitBlt(dcMClient, rca->left, rca->top, RECT_W, v3,
			dcMem, v1, v2, SRCPAINT);

		SelectObject(dcMem, bmpDraw);
		BitBlt(dcMClient, rca->left, rca->top, RECT_W, v3,
			dcMem, v1, v2, SRCAND);
		break;

	}
	
	//保存需要擦除的痕迹
	*rcOld = *rcCur;
	SetViewportOrgEx(dcMClient, 0, 0, NULL);

}


void DrawZoom(RectAnime *rca)
{
	static int v1 = (boxMaxW - RECT_W) / 2;
	static int v2 = (boxMaxH - RECT_H) / 2;
	int left = rca->left - v1;
	int top = rca->top - v2;

	SetViewportOrgEx(dcMClient, rcBg.left, rcBg.top, NULL);

	//取得背景图上的相应位置来作为图形缩放位置的背景
	SelectObject(dcMem, bmpBg);
	BitBlt(dcMClient, left, top, boxMaxW, boxMaxH, dcMem, left, top, SRCCOPY);


	//贴掩码
	SelectObject(dcMem, rca->bmpAnime[rca->ps][rca->psAnime].hbMask);
	BitBlt(dcMClient, left, top, boxMaxW, boxMaxH, dcMem, 0, 0, SRCPAINT);


	//贴纹理
	SelectObject(dcMem, rca->bmpAnime[rca->ps][rca->psAnime].hBmp);
	BitBlt(dcMClient, left, top, boxMaxW, boxMaxH, dcMem, 0, 0, SRCAND);

	SetViewportOrgEx(dcMClient, 0, 0, NULL);

}
void DrawZoomAnime(RectAnime *rca)
{
	static int v1 = (boxMaxW - RECT_W) / 2;
	static int v2 = (boxMaxH - RECT_H) / 2;
	static RECT rcUpdate;


	while (rca->actAnime)
	{
		rca->actAnime(rca);
		DrawZoom(rca);
 
		rcUpdate.left = rcBg.left + rca->left - v1;
		rcUpdate.top = rcBg.top + rca->top - v2;
		rcUpdate.right = rcUpdate.left + boxMaxW;
		rcUpdate.bottom = rcUpdate.top + boxMaxH;

		 RedrawWindow(hWnd, &rcUpdate, NULL, RDW_UPDATENOW | RDW_INVALIDATE | RDW_NOCHILDREN);

		UsSleep(ANIMA_ZOOM_SPEDD);

	}

	rca->actDraw = DrawMove;
}


bool MoveLeft()
{
	RectAnime *rca;
	int preX = 0;
	static int pts[MAP_XN], *ptsb, *ptse;
	 Rectx *rcX;
	POINT *ppt;
	bool isMove = false;

	for (int y = 0; y < MAP_YN; ++y)
	{
		rcX = rcMap[y];
		ptsb = ptse = pts;
		preX = INVALID_VALUES;

		for (int x = 0; x < MAP_XN; ++x)
		{
			if (rcX[x].use != INVALID_VALUES)
			{
				if (preX != INVALID_VALUES &&
					bmpMoves[rcX[preX].ps]->value == bmpMoves[rcX[x].ps]->value)
				{
					//当前帧存在可移动的方块
					isMove = true;

					rca = rcAnime[y] + preX;
					rcAnime[y][x].mX = rca->mX;

					++rcX[rca->mX].ps;
					//此时有2个方块需要合并
					rcX[rca->mX].validCount = 2;

					//指向两个目标动画
					ppt = rcX[rca->mX].rcMove;
					ppt[0].y = y;
					ppt[1].y = y;
					ppt[0].x = preX;
					ppt[1].x = x;


					rcX[x].use = INVALID_VALUES;
					*ptse++ = x;
					preX = INVALID_VALUES;

				}
				else
				{
					if (ptsb < ptse)
					{
						//当前帧存在可移动的方块
						isMove = true;

						rcAnime[y][x].mX = *ptsb;
						rcX[*ptsb].use = 1;
						//设置风格
						rcX[*ptsb].ps = rcX[x].ps;
						//此时有1个方块需要移动到目的地
						rcX[*ptsb].validCount = 1;

						//指向两个目标动画
						ppt = rcX[*ptsb].rcMove;
						ppt[0].y = y;
						ppt[0].x = x;
						ppt[1].x = INVALID_VALUES; //任意设置X/Y都能作为结束标记
						++ptsb;

						rcX[x].use = INVALID_VALUES;
						*ptse++ = x;

					}

					preX = x;
				}

				//设置动画速率	 
				rca = rcAnime[y] + x;
				rca->actAnime = ActionLeft;
				LONG len = rca->left - rcX[rca->mX].left;

				rca->psps = RectAnime::sps + 3;
				rca->sgs[0] = (SHORT)rcX[rca->mX].left; //段起始位置
				rca->sgs[1] = (SHORT)(rcX[rca->mX].left + len * SG1);
				rca->sgs[2] = (SHORT)(rcX[rca->mX].left + len * SG2);
				rca->sgs[3] = (SHORT)rca->left;//段结束位置
				rca->psgs = rca->sgs + 3;

			}
			else
			{
				*ptse++ = x;
			}


		}
	}

	return isMove;
}

bool MoveUp()
{
	RectAnime *rca;
	int preY = 0;
	static int pts[MAP_XN], *ptsb, *ptse;
	 Rectx *rcY;
	POINT *ppt;
	bool isMove = false;


	for (int x = 0; x < MAP_XN; ++x)
	{
		ptsb = ptse = pts;
		preY = INVALID_VALUES;

		for (int y = 0; y < MAP_YN; ++y)
		{

			if (rcMap[y][x].use != INVALID_VALUES)
			{
				if (preY != INVALID_VALUES &&
					bmpMoves[rcMap[preY][x].ps]->value == bmpMoves[rcMap[y][x].ps]->value)
				{
					//当前帧存在可移动的方块
					isMove = true;

					rca = rcAnime[preY] + x;
					rcY = rcMap[rca->mY] + x;

					rcAnime[y][x].mY = rca->mY;

					//设置风格
					++rcY->ps;
					//此时有2个方块需要合并
					rcY->validCount = 2;


					//指向两个目标动画
					ppt = rcY->rcMove;
					ppt[0].x = x;
					ppt[1].x = x;
					ppt[0].y = preY;
					ppt[1].y = y;

					rcMap[y][x].use = INVALID_VALUES;
					*ptse++ = y;
					preY = INVALID_VALUES;

				}
				else
				{
					if (ptsb < ptse)
					{
						//当前帧存在可移动的方块
						isMove = true;

						rcAnime[y][x].mY = *ptsb;
						rcY = rcMap[*ptsb] + x;

						rcY->use = 1;

						//设置风格
						rcY->ps = rcMap[y][x].ps;
						//此时有1个方块需要移动到目的地
						rcY->validCount = 1;

						//指向相同位置的动画
						ppt = rcY->rcMove;
						ppt[0].x = x;
						ppt[0].y = y;
						ppt[1].x = INVALID_VALUES;
						++ptsb;

						rcMap[y][x].use = INVALID_VALUES;
						*ptse++ = y;

					}

					preY = y;
				}

				//设置动画速率
				rca = rcAnime[y] + x;
				rcY = rcMap[rca->mY] + x;
				rca->actAnime = ActionUp;
				LONG len = rca->top - rcMap[rca->mY][x].top;

				rca->psps = RectAnime::sps + 3;
				rca->sgs[0] = (SHORT)rcY->top; //段起始位置
				rca->sgs[1] = (SHORT)(rcY->top + len * SG1);
				rca->sgs[2] = (SHORT)(rcY->top + len * SG2);
				rca->sgs[3] = (SHORT)rca->top;//段结束位置
				rca->psgs = rca->sgs + 3;

			}
			else
			{
				*ptse++ = y;
			}



		}

	}
	return isMove;
}


bool MoveRight()
{
	RectAnime *rca;
	int preX = 0;
	static int pts[MAP_XN], *ptsb, *ptse;
	 Rectx *rcX;
	POINT *ppt;
	bool isMove = false;

	for (int y = 0; y < MAP_YN; ++y)
	{
		rcX = rcMap[y];
		ptsb = ptse = pts;
		preX = INVALID_VALUES;

		for (int x = MAP_XN - 1; x >= 0; --x)
		{
			if (rcX[x].use != INVALID_VALUES)
			{
				if (preX != INVALID_VALUES &&
					bmpMoves[rcX[preX].ps]->value == bmpMoves[rcX[x].ps]->value)
				{
					//当前帧存在可移动的方块
					isMove = true;

					rca = rcAnime[y] + preX;
					rcAnime[y][x].mX = rca->mX;

					//设置风格
					++rcX[rca->mX].ps;
					//此时有2个方块需要合并
					rcX[rca->mX].validCount = 2;

					//指向两个目标动画
					ppt = rcX[rca->mX].rcMove;
					ppt[0].y = y;
					ppt[1].y = y;
					ppt[0].x = preX;
					ppt[1].x = x;

					rcX[x].use = INVALID_VALUES;
					*ptse++ = x;
					preX = INVALID_VALUES;

				}
				else
				{
					if (ptsb < ptse)
					{
						//当前帧存在可移动的方块
						isMove = true;

						rcAnime[y][x].mX = *ptsb;
						rcX[*ptsb].use = 1;

						//设置风格
						rcX[*ptsb].ps = rcX[x].ps;
						//此时有1个方块需要移动到目的地
						rcX[*ptsb].validCount = 1;

						//指向两个目标动画
						ppt = rcX[*ptsb].rcMove;
						ppt[0].y = y;
						ppt[0].x = x;
						ppt[1].x = INVALID_VALUES;
						++ptsb;

						rcX[x].use = INVALID_VALUES;
						*ptse++ = x;

					}

					preX = x;
				}

				//设置动画速率
				rca = rcAnime[y] + x;
				rca->actAnime = ActionRight;
				LONG len = rca->left - rcX[rca->mX].left;

				rca->psps = RectAnime::sps + 3;
				rca->sgs[0] = (SHORT)rcX[rca->mX].left; //段起始位置
				rca->sgs[1] = (SHORT)(rcX[rca->mX].left + len * SG1);
				rca->sgs[2] = (SHORT)(rcX[rca->mX].left + len * SG2);
				rca->sgs[3] = (SHORT)rca->left;//段结束位置
				rca->psgs = rca->sgs + 3;

			}
			else
			{
				*ptse++ = x;
			}


		}

	}

	return isMove;
}


bool MoveDown()
{
	RectAnime *rca;
	int preY = 0;
	static int pts[MAP_XN], *ptsb, *ptse;
	 Rectx *rcY;
	POINT *ppt;
	bool isMove = false;

	for (int x = 0; x < MAP_XN; ++x)
	{
		//rcX = rcMap[y];
		ptsb = ptse = pts;
		preY = INVALID_VALUES;

		for (int y = MAP_YN - 1; y >= 0; --y)
		{
			if (rcMap[y][x].use != INVALID_VALUES)
			{
				if (preY != INVALID_VALUES &&
					bmpMoves[rcMap[preY][x].ps]->value == bmpMoves[rcMap[y][x].ps]->value)
				{
					//当前帧存在可移动的方块
					isMove = true;

					rca = rcAnime[preY] + x;
					rcY = rcMap[rca->mY] + x;

					rcAnime[y][x].mY = rca->mY;

					//设置风格
					++rcY->ps;
					//此时有2个方块需要合并
					rcY->validCount = 2;

					//指向两个目标动画
					ppt = rcY->rcMove;
					ppt[0].x = x;
					ppt[1].x = x;
					ppt[0].y = preY;
					ppt[1].y = y;

					rcMap[y][x].use = INVALID_VALUES;
					*ptse++ = y;
					preY = INVALID_VALUES;

				}
				else
				{
					if (ptsb < ptse)
					{
						//当前帧存在可移动的方块
						isMove = true;

						rcAnime[y][x].mY = *ptsb;
						rcY = rcMap[*ptsb] + x;

						rcY->use = 1;

						rcY->ps = rcMap[y][x].ps;
						//此时有1个方块需要移动到目的地
						rcY->validCount = 1;

						//指向相同位置的动画
						ppt = rcY->rcMove;
						ppt[0].x = x;
						ppt[0].y = y;
						ppt[1].x = INVALID_VALUES;
						++ptsb;

						rcMap[y][x].use = INVALID_VALUES;
						*ptse++ = y;

					}

					preY = y;
				}

				//设置动画速率
				rca = rcAnime[y] + x;
				rcY = rcMap[rca->mY] + x;
				rca->actAnime = ActionDown;
				LONG len = rca->top - rcMap[rca->mY][x].top;

				rca->psps = RectAnime::sps + 3;
				rca->sgs[0] = (SHORT)rcY->top; //段起始位置
				rca->sgs[1] = (SHORT)(rcY->top + len * SG1);
				rca->sgs[2] = (SHORT)(rcY->top + len * SG2);
				rca->sgs[3] = (SHORT)rca->top;//段结束位置
				rca->psgs = rca->sgs + 3;

			}
			else
			{
				*ptse++ = y;
			}


		}

	}

	return isMove;
}

void  ExitCleanUp()
{
	BmpInfo *pBI;
	for (int i = 0; i < BMP_N; ++i)
	{
		pBI = bmpPool[i];
		for (; pBI->hbMask; ++pBI)
		{
			DeleteObject(pBI->hbMask);
			DeleteObject(pBI->hBmp);
		}

		pBI = bmpZooms[i];
		for (; pBI->hbMask; ++pBI)
		{
			DeleteObject(pBI->hbMask);
			DeleteObject(pBI->hBmp);
		}

		free(bmpPool[i]);
		free(bmpZooms[i]);
		//bmpMove无需释放
	}

	DeleteDC(dcMem);
	DeleteObject(bmpBg);

	DeleteObject(bmpDraw);

	DeleteObject(bmpClient);
	ReleaseDC(hWnd, dcMClient);
	 

}


///////////////////////////////////////////////////////////////
void  CtrlInit(HWND hParent)
{
 
	 

	int id = 0;
	LPCTSTR str;
	RECT rect;

	str = _T("2048");
	HWND seTxt0 = SEditCreate(hParent, rcBg.left, rcBg.top - 170, 1, 1, str, id++);
	CtrlSetFont(seTxt0, 110, _T("微软雅黑"), FW_BOLD);
	SEditSetTextColor(seTxt0, RGB(119, 110, 101));
	SEditSetReadOnly(seTxt0, TRUE);
	SEditAdjustRect(seTxt0, FALSE, SE_FIXED_LT);
	SEditAdjustRect(seTxt0, FALSE, SE_FIXED_LT);
	GetClientRect(seTxt0, &rect);
	MapWindowPoints(seTxt0, hParent, (POINT*)&rect, 2);


	///////////////////////////////////////////////////////////////
	str = _T("2048 单机小游戏");
	HWND seTxt1 = SEditCreate(hParent, rcBg.left, rect.bottom, 1, 1, str, id++);
	CtrlSetFont(seTxt1, 22, _T("微软雅黑"), FW_SEMIBOLD);
	SEditSetTextColor(seTxt1, RGB(119, 110, 101));
	SEditSetReadOnly(seTxt1, TRUE);
	SEditAdjustRect(seTxt1, FALSE, SE_FIXED_LT);
	SEditAdjustRect(seTxt1, FALSE, SE_FIXED_LT);
	SEditGetAdjustRect(seTxt1, &rect, 0);
	MapWindowPoints(seTxt1, hParent, (POINT*)&rect, 2);

	///////////////////////////////////////////////////////////////

	str = _T("你的目标是把相同数字的方块相加,得到2048!");
	HWND seTxt2 = SEditCreate(hParent, rcBg.left, rect.bottom + 2, 1, 1, str, id++);
	CtrlSetFont(seTxt2, 22, _T("微软雅黑"), FW_NORMAL);
	SEditSetTextColor(seTxt2, RGB(119, 110, 101));
	SEditSetReadOnly(seTxt2, TRUE);
	SEditAdjustRect(seTxt2, FALSE, SE_FIXED_LT);
	SEditAdjustRect(seTxt2, FALSE, SE_FIXED_LT);
	SEditGetAdjustRect(seTxt2, &rect, 0);
	MapWindowPoints(seTxt2, hParent, (POINT*)&rect, 2);

	///////////////////////////////////////////////////////////////
		 
	HWND bNewGame = BtnCreate(hParent, rcBg.right - 100, rect.bottom - 40, 100,40, IDB_NEWGANME);
	
	BtnSetBkColor(bNewGame, BTN_DEFAULT, ARGB(255, 242, 58, 58));
	BtnSetBkColor(bNewGame, BTN_MS_LEAVE, ARGB(255, 242, 58, 58));
	BtnSetBkColor(bNewGame, BTN_MS_HOVER, ARGB(255, 237, 137, 137));
	BtnSetBkColor(bNewGame, BTN_MS_DOWN, ARGB(100, 149, 33, 33));
	BtnSetBkColor(bNewGame, BTN_MS_UP, ARGB(250, 237, 137, 137));
	BtnSetEnableAct(bNewGame, TRUE);
	BtnSetCursor(bNewGame, LoadCursor(NULL, IDC_HAND));
	CtrlSetFont(bNewGame, 22, _T("微软雅黑"), FW_SEMIBOLD);
	BtnSetText(bNewGame, BTN_SET_THIS,_T("新游戏"));
	BtnEnableDrawText(bNewGame, TRUE);
	BtnSetTextColor(bNewGame, BTN_SET_THIS, ARGB(255,255, 255, 255));	
	BtnSetRectAngle(bNewGame, 20, 20);
	GetClientRect(bNewGame, &rect);
	MapWindowPoints(bNewGame, hParent, (POINT*)&rect, 2);
	
 
 
///////////////////////////////////////////////////////////////

	//w = 55,h = 55
	HWND bBest = BtnCreate(hParent, rect.right - 44, rect.top - 40 - 55, 55, 55, id++);
	BtnSetBkColor(bBest, BTN_DEFAULT, ARGB(187, 173, 160,254));
	BtnSetEnableAct(bBest, FALSE);
	BtnSetRectAngle(bBest, 5, 5);

	str = _T("最高分");
	HWND bBTxt1 = SEditCreate(bBest, 0, 5, 1, 1, str, id++);
	CtrlSetFont(bBTxt1, 18, _T("微软雅黑"), FW_SEMIBOLD);
	SEditSetTextColor(bBTxt1, RGB(238, 228, 218));
	SEditSetReadOnly(bBTxt1, TRUE);
	SEditAdjustRect(bBTxt1, FALSE, SE_HCETER);
	SEditAdjustRect(bBTxt1, FALSE, SE_HCETER);
	BtnAttachSEdit(bBest, 0, bBTxt1);
	GetClientRect(bBTxt1, &rect);
	MapWindowPoints(bBTxt1, bBest, (POINT*)&rect, 2);

	
	str = _T("0");
	hMaxScore = SEditCreate(bBest, 0, rect.bottom , 1, 1, str, IDE_MAXSCORE);
	CtrlSetFont(hMaxScore, 30, _T("微软雅黑"), FW_ULTRABOLD);
	SEditSetTextColor(hMaxScore, RGB(255, 255, 255));
	SEditSetReadOnly(hMaxScore, TRUE);
	SEditAdjustRect(hMaxScore, FALSE, SE_HCETER);
	SEditAdjustRect(hMaxScore, FALSE, SE_HCETER);
	BtnAttachSEdit(bBest, 1, hMaxScore);

	GetClientRect(bBest, &rect);
	MapWindowPoints(bBest, hParent, (POINT*)&rect, 2);


///////////////////////////////////////////////////////////////
	
//w = 55,h = 55
	HWND bScore = BtnCreate(hParent, rect.left - 55 - 10, rect.top, 55, 55, id++);
	BtnSetBkColor(bScore, BTN_DEFAULT, ARGB(187, 173, 160,255));
	BtnSetEnableAct(bScore, FALSE);
	BtnSetRectAngle(bScore, 5, 5);

	str = _T("分数");
	HWND bSTxt1 = SEditCreate(bScore, 0, 5, 1, 1, str, id++);
	CtrlSetFont(bSTxt1, 18, _T("微软雅黑"), FW_SEMIBOLD);
	SEditSetTextColor(bSTxt1, RGB(238, 228, 218));
	SEditSetReadOnly(bSTxt1, TRUE);
	SEditAdjustRect(bSTxt1, FALSE, SE_HCETER);
	SEditAdjustRect(bSTxt1, FALSE, SE_HCETER);
	BtnAttachSEdit(bScore, 0, bSTxt1);
	GetClientRect(bSTxt1, &rect);
	MapWindowPoints(bSTxt1, bScore, (POINT*)&rect, 2);

	 
	str = _T("0");
	hScore = SEditCreate(bScore, 0, rect.bottom, 1, 1, str, IDE_SCORE);
	CtrlSetFont(hScore, 30, _T("微软雅黑"), FW_ULTRABOLD);
	SEditSetTextColor(hScore, RGB(255, 255, 255));
	SEditSetReadOnly(hScore, TRUE);
	SEditAdjustRect(hScore, FALSE, SE_HCETER);
	SEditAdjustRect(hScore, FALSE, SE_HCETER);
	BtnAttachSEdit(bScore, 1, hScore);

	InvalidateRect(bScore, NULL, 0);
 
	///////////////////////////////////////////////////////////////

	str = _T("如何玩:使用箭头键移动方块.")
		  _T("当在不同的方向上两个值相同且相邻的方块接触时,它们会合成一个方块,")
		  _T("你的目标是让合成的方块的值为2048.")
		  _T("加油!祝你好运!");
	HWND seTxt3 = SEditCreate(hParent, rcBg.right - 210, rcBg.bottom + 10,204, 300, str, id++);
	CtrlSetFont(seTxt3, 20, _T("微软雅黑"), FW_NORMAL);
	SEditSetTextColor(seTxt3, RGB(75, 72, 67));
	SEditSetReadOnly(seTxt3, TRUE);
	SEditAdjustRect(seTxt3, TRUE, SE_FIXED_LT);

	GetClientRect(seTxt3, &rect);
	MapWindowPoints(seTxt3, hWnd, (POINT*)&rect, 2);

	///////////////////////////////////////////////////////////////

	str = _T("作者:群主隔壁.");
	HWND seAuthor = SEditCreate(hParent, rcBg.left, rect.bottom + 20, 200, 60, str, id++);
	CtrlSetFont(seAuthor, 16, _T("宋体"), FW_BOLD);
	SEditSetTextColor(seAuthor, RGB(255, 0, 0));
	SEditSetReadOnly(seAuthor, TRUE);
	SEditAdjustRect(seAuthor, TRUE, SE_FIXED_LT);
	GetClientRect(seAuthor, &rect);
	MapWindowPoints(seAuthor, hWnd, (POINT*)&rect, 2);


	HWND seQQ = LinkCreate(hParent, rect.left, rect.bottom + 2, 200, 160,
		_T("QQ:438165261"), id++);
	SEditSetReadOnly(seQQ, TRUE);
	CtrlSetFont(seQQ, 14, _T("宋体"), FW_SEMIBOLD);
	SEditAdjustRect(seQQ, FALSE, SE_FIXED_LT);
	LinkSetTextColor(seQQ, LINK_MS_LEAVE, RGB(31, 124, 229));
	LinkSetTextColor(seQQ, LINK_MS_HOVER, RGB(255, 0, 0));
	LinkSetTextColor(seQQ, LINK_MS_DOWN, RGB(255, 0, 0));
	LinkSetTextColor(seQQ, LINK_MS_UP, RGB(134, 118, 106));
	LinkSetTextColor(seQQ, LINK_MS_FOCUS, RGB(144, 139, 139));
	LinkSetURL(seQQ, _T("tencent://message/?uin=438165261"));
	GetClientRect(seQQ, &rect);
	MapWindowPoints(seQQ, hWnd, (POINT*)&rect, 2);

	HWND seEmail = LinkCreate(hParent, rect.left, rect.bottom + 2, 200, 160,
		_T("邮箱:www.438165261@qq.com"), id++);
	SEditSetReadOnly(seEmail, TRUE);
	CtrlSetFont(seEmail, 14, _T("宋体"), FW_SEMIBOLD);
	SEditAdjustRect(seEmail, FALSE, SE_FIXED_LT);
	LinkSetTextColor(seEmail, LINK_MS_LEAVE, RGB(31, 124, 229));
	LinkSetTextColor(seEmail, LINK_MS_HOVER, RGB(255, 0, 0));
	LinkSetTextColor(seEmail, LINK_MS_DOWN, RGB(255, 0, 0));
	LinkSetTextColor(seEmail, LINK_MS_UP, RGB(134, 118, 106));
	LinkSetTextColor(seEmail, LINK_MS_FOCUS, RGB(144, 139, 139));
	LinkSetURL(seEmail, _T("mailto:xxxx"));
	GetClientRect(seEmail, &rect);
	MapWindowPoints(seEmail, hWnd, (POINT*)&rect, 2);

/////////////////////////////////////////////////////////////////////////////////
	
	HWND  wKeepOut = TspWindowCreate(_T("Wnd_2048"), _T(""),
		    WS_CHILD  | WS_CLIPCHILDREN ,
		 0, 0,WND_W, WND_H - 00, hParent, IDW_KEEPOUT); 
	SetWindowPos(wKeepOut, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	
	
	 HWND wKeepOutMap = TspWindowCreate(_T("Wnd_2048"), _T(""),
		0 | WS_CHILD | WS_VISIBLE  ,
		 rcBg.left - 0 , rcBg.top - 0 ,
		 szBg.cx + 0 , szBg.cy + 0 , wKeepOut, IDW_KEEPOUTMAP);
	 //设置窗口的形状和游戏地图背景的相等
	 HRGN rgn = CreateRoundRectRgn(0, 0,szBg.cx + 0 , szBg.cy + 0 , RECT_ANGLE_BG, RECT_ANGLE_BG);
	 SetWindowRgn(wKeepOutMap, rgn, TRUE);
	 
//////////////////////////
	 HWND seWin = SEditCreate(wKeepOutMap, 0, 0, 1, 1, _T("你胜利了!"), IDE_MSG);
	 CtrlSetFont(seWin, 110, _T("微软雅黑"), 800);
	 SEditSetTextColor(seWin, RGB(247, 245, 234));
	 SEditSetReadOnly(seWin, TRUE);
	 SEditAdjustRect(seWin, FALSE, SE_HCETER | SE_VCETER);

	
//////////////////////////////////////////////////
	 HWND bTryAgin = BtnCreate(wKeepOutMap, 150, 100, 1, 1, IDB_TRYAGIN);
	 BtnSetText(bTryAgin, BTN_SET_THIS, _T("再玩一次"));
	 CtrlSetFont(bTryAgin, 20, _T("微软雅黑"), 600);
	 BtnSetCursor(bTryAgin, LoadCursor(NULL, IDC_HAND));	 
	 BtnSetBkColor(bTryAgin, BTN_DEFAULT, BtnGetBkColor(bNewGame, BTN_DEFAULT) & 0x00ffffff | 0x90000000 );
	 BtnSetBkColor(bTryAgin, BTN_MS_LEAVE, BtnGetBkColor(bNewGame, BTN_MS_LEAVE) & 0x00ffffff | 0x90000000);
	 BtnSetBkColor(bTryAgin, BTN_MS_HOVER, BtnGetBkColor(bNewGame, BTN_MS_HOVER) & 0x00ffffff | 0x90000000);
	 BtnSetBkColor(bTryAgin, BTN_MS_DOWN, BtnGetBkColor(bNewGame, BTN_MS_DOWN) & 0x00ffffff | 0x90000000);
	 BtnSetBkColor(bTryAgin, BTN_MS_UP, BtnGetBkColor(bNewGame, BTN_MS_UP) & 0x00ffffff | 0x90000000);
	 BtnSetTextColor(bTryAgin, BTN_SET_THIS, BtnGetTextColor(bNewGame, BTN_SET_THIS) & 0x00ffffff | 0x90000000);
	 BtnSetEnableAct(bTryAgin, TRUE);
	 BtnEnableDrawText(bTryAgin, TRUE);
	 BtnAdjustSize(bTryAgin, BTN_SET_THIS, 18, 5, 18, 5);
	 BtnSetRectAngle(bTryAgin, 10, 10);

	 HWND bRetMeun = BtnCreate(wKeepOutMap, 150, 100, 1, 1, IDB_RMM);
	 BtnSetText(bRetMeun, BTN_SET_THIS, _T("返回主菜单"));
	 CtrlSetFont(bRetMeun, 20, _T("微软雅黑"), 600);
	 BtnSetCursor(bRetMeun, LoadCursor(NULL, IDC_HAND));
	 BtnSetBkColor(bRetMeun, BTN_DEFAULT, BtnGetBkColor(bNewGame, BTN_DEFAULT) & 0x00ffffff | 0x90000000);
	 BtnSetBkColor(bRetMeun, BTN_MS_LEAVE, BtnGetBkColor(bNewGame, BTN_MS_LEAVE) & 0x00ffffff | 0x90000000);
	 BtnSetBkColor(bRetMeun, BTN_MS_HOVER, BtnGetBkColor(bNewGame, BTN_MS_HOVER) & 0x00ffffff | 0x90000000);
	 BtnSetBkColor(bRetMeun, BTN_MS_DOWN, BtnGetBkColor(bNewGame, BTN_MS_DOWN) & 0x00ffffff | 0x90000000);
	 BtnSetBkColor(bRetMeun, BTN_MS_UP, BtnGetBkColor(bNewGame, BTN_MS_UP) & 0x00ffffff | 0x90000000);
	 BtnSetTextColor(bRetMeun, BTN_SET_THIS, BtnGetTextColor(bNewGame, BTN_SET_THIS) & 0x00ffffff | 0x90000000);
	 BtnSetEnableAct(bRetMeun, TRUE);
	 BtnEnableDrawText(bRetMeun, TRUE);
	 BtnAdjustSize(bRetMeun, BTN_SET_THIS, 18, 5, 18, 5);
	 BtnSetRectAngle(bRetMeun, 10, 10);
	 
	 //使得两个按钮居中对齐
	 RECT rc,rcTryAgin,rcExitGame;
	 INT left,w1,w2,h1,h2,sp = 20;
	 GetClientRect(wKeepOutMap, &rc);
	 GetClientRect(bTryAgin, &rcTryAgin);
	 GetClientRect(bRetMeun, &rcExitGame);
	 MapWindowPoints(bTryAgin, NULL, (POINT*)&rcTryAgin, 2);
	 MapWindowPoints(bRetMeun, NULL, (POINT*)&rcExitGame, 2);	 
 	 w1 = rcTryAgin.right - rcTryAgin.left;
	 h1 = rcTryAgin.bottom - rcTryAgin.top;
	 w2 = rcExitGame.right - rcExitGame.left;
	 h2 = rcExitGame.bottom - rcExitGame.top;
	 left = (rc.right - (w1 + sp + w2)) / 2;	 	
	 rcTryAgin.left = left;
	 rcTryAgin.right = rcTryAgin.left  + w1;
	 rcTryAgin.top += h1 * 2;
	 rcTryAgin.bottom += h1 * 2;
	 rcExitGame.left = left + w1 + sp;
	 rcExitGame.right = rcExitGame.left + w2;
	 rcExitGame.top += h2 * 2;
	 rcExitGame.bottom += h2 * 2; 
	 MoveWindow(bTryAgin, rcTryAgin.left, rcTryAgin.top, w1, h1,FALSE);
	 MoveWindow(bRetMeun, rcExitGame.left, rcExitGame.top, w2, h2, FALSE);
	
//////////////////////////////////////////////////////////////////	
	
	 
	 HWND  wPanelMenu = TspWindowCreate(_T("Wnd_2048"), _T(""),
		  WS_VISIBLE | WS_CHILD  |  WS_CLIPCHILDREN ,
		 0, 0, WND_W, WND_H - 0, hParent, IDW_MP);
	 TspWindowSetBkColor(wPanelMenu, ARGB(200, 1, 1, 1));
	SetWindowPos(wPanelMenu, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	 TspSetRectAngle(wPanelMenu, 250,250);
     
	 const INT II = 5;
	 const INT SP = 10;
	 DWORD miIDS[II] = { IDMI_0,IDMI_1,IDMI_2,IDMI_3,IDMI_4 };
	 LPCTSTR txts[II] = { _T("继续游戏"),_T("设置游戏"),_T("存档游戏"),_T("读取存档"),_T("退出游戏") };

	 rect.right = szBg.cx - 100;  //宽
	 rect.bottom = 80;  //高
	 rect.left = rcBg.left + (szBg.cx - rect.right) / 2;
	 //垂直居中
	 rect.top = rcBg.top +  (szBg.cy  - (rect.bottom * II + SP * 4)) / 2;
	 
	 
	 
	 for (int i = 0; i < II; ++i)
	 {
		 HWND bMItem = BtnCreate(wPanelMenu, rect.left, rect.top, rect.right, rect.bottom,miIDS[i]);
		 BtnSetBkColor(bMItem, BTN_DEFAULT, ARGB(120, 255, 255, 255));
		 BtnSetBkColor(bMItem, BTN_MS_LEAVE, ARGB(120, 255, 0, 0));
		 BtnSetBkColor(bMItem, BTN_MS_HOVER, ARGB(200, 255, 0, 0));
		 BtnSetBkColor(bMItem, BTN_MS_DOWN, ARGB(100, 255, 0, 0));
		 BtnSetBkColor(bMItem, BTN_MS_UP, ARGB(200, 255, 0, 0));
		// BtnSetCursor(bMItem, LoadCursor(NULL, IDC_HAND));
		 BtnEnableDrawText(bMItem, TRUE);
		 CtrlSetFont(bMItem, 50, _T("微软雅黑"), FW_SEMIBOLD);
		 BtnSetText(bMItem, BTN_SET_THIS, txts[i]);
		 BtnSetRectAngle(bMItem, 40, 40);
		 BtnSetTextColor(bMItem, BTN_SET_THIS, ARGB(150, 255, 255, 255));
		 BtnSetEnableAct(bMItem, TRUE);
		HRGN rgn = CreateRoundRectRgn(0,0, rect.right, rect.bottom,40, 40);
		SetWindowRgn(bMItem, rgn, TRUE);
		
		 rect.top += rect.bottom + SP;
	 }
	 
	
	 
}

void  CtrlSetScore(DWORD nScore)
{
	static TCHAR sScore[32];
	static RECT rcScore, rcMaxScore, rcBScore, rcBMaxScore;
	static INT nSpacing; 
	//static INT nScoreLS, nScoreRS,nMaxScoreLS, nMaxScoreRS;
	static RECT rcScoreFill, rcMaxScoreFill;
	static HWND bScore = GetParent(hScore);
	static HWND bMaxScore = GetParent(hMaxScore);
	static DWORD nAllScore = 0;
	static DWORD nMaxAllScore = _tcstol(SEditGetText(hMaxScore), NULL, 10);

    

	//获取父窗口的区域
	GetClientRect(bScore, &rcBScore);
	GetClientRect(bMaxScore, &rcBMaxScore);
	
	if (nSpacing == 0)
	{
		//计算两个按钮间的间距
		RECT rcTmp1 = rcBScore;
		RECT rcTmp2 = rcBMaxScore;		 
		MapWindowPoints(bScore,hWnd, (POINT*)&rcTmp1,2);
		MapWindowPoints(bMaxScore,hWnd, (POINT*)&rcTmp2, 2);
		//计算两个按钮间的间隔
		nSpacing = rcTmp2.left - rcTmp1.right;	
	}

	if (rcScoreFill.left == 0)
	{
		//获取特定按钮的编辑框组成的矩形范围
		//以最大分数按钮的填充为准
		BtnGetMargins(bMaxScore, BTN_SET_EDIT, &rcScoreFill);
		BtnGetMargins(bMaxScore, BTN_SET_EDIT, &rcMaxScoreFill);
	}

	//设置当前分数
	//nAllScore += 10000000;
	//nAllScore += 1532;
	nAllScore += nScore;
	_stprintf(sScore, _T("%ld"), nAllScore);
	

	SEditSetText(hScore, sScore);
	//水平居中调整分数编辑框的位置和大小
	SEditAdjustRect(hScore, FALSE, SE_HCETER);
	//获取分数编辑框需要的区域
	SEditGetAdjustRect(hScore, &rcScore, FALSE);
	 
	//如果分数编辑框超出了父窗口就调整父窗口和编辑框的大小
	if (rcScore.right + rcScoreFill.left + rcScoreFill.right > rcBScore.right)
	{
		BtnAdjustSize(bScore, BTN_SET_EDIT, 
						rcScoreFill.left, 
						rcScoreFill.top,
						rcScoreFill.right,
						rcScoreFill.bottom);
		//重新获取分数按钮的大小 
		GetClientRect(bScore, &rcBScore);
	}
		
	if (nMaxAllScore < nAllScore)
	{
		SEditSetText(hMaxScore, sScore);
		nMaxAllScore = _tcstol(SEditGetText(hMaxScore), NULL, 10);
		
		//水平居中调整最高分数编辑框的位置和大小
		SEditAdjustRect(hMaxScore, FALSE, SE_HCETER);
		//获取最高分编辑框需要的区域
		SEditGetAdjustRect(hMaxScore, &rcMaxScore, FALSE);

		//如果分数编辑框超出了父窗口就调整父窗口和编辑框的大小
		if (rcMaxScore.right + rcMaxScoreFill.left + rcMaxScoreFill.right > rcBMaxScore.right)
		{
			BtnAdjustSize(bMaxScore, BTN_SET_EDIT, 
							rcMaxScoreFill.left,
							rcMaxScoreFill.top,
							rcMaxScoreFill.right,
							rcMaxScoreFill.bottom);
			//重新获取最高分按钮的大小 
			GetClientRect(bMaxScore, &rcBMaxScore);

			MapWindowPoints(bScore, hWnd, (POINT*)&rcBScore, 2);
			MapWindowPoints(bMaxScore, hWnd, (POINT*)&rcBMaxScore, 2);
			//if (rcBMaxScore.left <= rcBScore.right)
			{
				//通过间隔,重新调整分数按钮的位置				
				INT  w = rcBScore.right - rcBScore.left;
				rcBScore.right = rcBMaxScore.left - nSpacing;
				rcBScore.left = rcBScore.right - w;
				MoveWindow(bScore, rcBScore.left, rcBScore.top,
					rcBScore.right - rcBScore.left,
					rcBScore.bottom - rcBScore.top, FALSE);
					

			}
		
		}
			
	}
		

}
 
void  CtrlShow(INT idx)
{
	static HWND wKeepOut = GetDlgItem(hWnd, IDW_KEEPOUT);
	static HWND wKeepOutMap = GetDlgItem(wKeepOut, IDW_KEEPOUTMAP);
	static HWND eMsg= GetDlgItem(wKeepOutMap, IDE_MSG);
	static HWND bTryAgin = GetDlgItem(wKeepOutMap, IDB_TRYAGIN);
	static HWND bExitGame = GetDlgItem(wKeepOutMap, IDB_RMM);

	switch (idx)
	{
	case SW_GV:
		SEditSetText(eMsg, _T("再接再厉!"));
		TspWindowSetBkColor(wKeepOut, ARGB(200,1,1,1));
		TspWindowSetBkColor(wKeepOutMap, ARGB(180,1, 1, 1));
		ShowWindow(wKeepOut, SW_SHOW);
		break;
	case SW_WIN:
		SEditSetText(eMsg, _T("您成功了!"));	
		TspWindowSetBkColor(wKeepOut, ARGB(200,206, 193, 165));
		TspWindowSetBkColor(wKeepOutMap, ARGB(180,216, 183, 102));
		ShowWindow(wKeepOut, SW_SHOW);
		break;
	case SW_HIDE:
		ShowWindow(wKeepOut, SW_HIDE);
		break;
	case SW_SHOW:
		ShowWindow(wKeepOut, SW_SHOW);
		break;
	 }
 
}
