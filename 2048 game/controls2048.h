#pragma once
#include <Windows.h>
#include <gdiplus.h>
#pragma comment(lib, "GdiPlus.lib")
using namespace Gdiplus;


#define  ARGB(a,r,g,b)   ((((DWORD)(BYTE)a) << 24  ) |   (((DWORD)(BYTE)r) << 16  ) | (((DWORD)(BYTE)g) << 8  ) | (((DWORD)(BYTE)b) << 0  ))
#define  GetAValue(c)  ((c) >> 24)

//#define  ARGB(r,g,b,a)   RGB(r,g,b)  | a
#define CTRL_WINDOW  1
#define CTRL_TRANSPARENTWINDOW  2
//父窗口类
#define CTRL_PARENT  4
//子窗口类
#define CTRL_CHILD   8
#define CTRL_EDIT    16
#define CTRL_BUTTON  32
#define CTRL_LINK    64
//子窗口的布局
#define CTRL_LAYOUT_LEFT     1
#define CTRL_LAYOUT_TOP      2
#define CTRL_LAYOUT_RIGHT    4
#define CTRL_LAYOUT_BOTTOM   8
#define CTRL_LAYOUT_HCENTER  16
#define CTRL_LAYOUT_VCENTER  32
//RECT作为输入
#define CTRL_LAYOUT_INRECT   64
//////////////控件的扩展方向(不可组合使用)//////////////////
/////////////可以与CTRL_LAYOUT_INRECT组合使用////////////
#define CTRL_LAYOUT_EXLEFT   128  
#define CTRL_LAYOUT_EXUP     512  
#define CTRL_LAYOUT_EXRIGHT  1024 
#define CTRL_LAYOUT_EXDOWN 2048
//向左右扩展,平均扩展高的两边 
#define CTRL_LAYOUT_EXLR     4096
//向上下扩展,平均扩展宽的两边 
#define CTRL_LAYOUT_EXTB     8192

struct Control
{    
	//旧的窗口过程
	WNDPROC wpOld; 
    //窗口类型见 CTRL_xxx宏定义
	DWORD type; 


};
VOID CtrlInit();
VOID CtrlExit();
Control* CtrlGetInfo(HWND hCtrl);
HWND CtrlCreate(__in Control *pCtrl,
		__in LPCTSTR lpClassName,
		__in LPCTSTR lpWindowName,
		__in DWORD dwStyle,
		__in DWORD dwExStyle,
		__in int X,
		__in int Y,
		__in int nWidth,
		__in int nHeight,
		__in HWND hWndParent,
	__in DWORD nID);

HWND CtrlCreateWindow(__in LPCTSTR lpClassName,
	__in LPCTSTR lpWindowName,
	__in DWORD dwStyle,
	__in int X,
	__in int Y,
	__in int nWidth,
	__in int nHeight,
	__in HWND hWndParent,
	__in DWORD nID);

 
//重绘父窗口的hChild所在父窗口的区域,不重绘制hChild
VOID CtrlRedrawParent(HWND hChild,HWND hParent,RECT *pRcClient);
void CtrlSetFont(HWND hWnd, INT size, LPCTSTR name, INT weight);
HFONT CtrlGetFont(HWND hWnd);
//把CTRL_LAYOUT转换为DrawText的DT_开始的宏
DWORD  CtrlLayoutToDrawText(DWORD nLayout);
//把CTRL_LAYOUT转换为StringAlignment的的对齐类型
//用于设置StringFormat::SetAlignment()和StringFormat::SetLineAlignment
//返回值的第1字节存储横坐标的对齐方式,第2字节存储纵坐标的对齐方式
DWORD  CtrlLayoutToStringAlignment(DWORD nLayout);
//获取子窗口的布局后的矩形区域,idx参见 CTRL_LAYOUT_开始的宏
//基于父窗口坐标系
//hParent如果为NULL，就使用GetParent(hWnd), 否则使用hParent
VOID CtrlGetPosRect(HWND hWnd,HWND hParent, RECT *pRc, INT idx);
//pRc为原来的矩形(基于父窗口坐标系).用于输入和输出
//当配合CTRL_LAYOUT_INRECT使用的时候,pRc作为hWnd的基于父窗口坐标的RECT
//否则使用hWnd位于父窗口的坐标
//nWidth和nHeight为新的宽度或高度
//idx参见CTRL_LAYOUT_EX开始的宏(不可组合使用)
//可配合CTRL_LAYOUT_INRECT宏使用
VOID CtrlGetExtRect(HWND hWnd, RECT *pRc, INT nWidth,INT nHeight, INT idx);
//调整的拥有缓冲区DC的位图大小的窗口
//pRc为NULL,则使用客户区,否则使用pRc
//offset为dc所在的结构体偏移
VOID CtrlAdjustClientDC(HWND hWnd, RECT *pRc, INT offset);
//设置窗口的圆角矩形的角度宽高(如果有)
//offset为角度所在位置的偏移
VOID CtrlSetRectAngle(HWND hWnd,BYTE w,BYTE h, INT offset);

///////////////////////////////////////////////////////////


struct TspWndInfo : Control
{
	HDC dcClient; 
	//矩形圆角的角度
	BYTE angle[2]; 
	//支持ARGB
	COLORREF color;
};

HWND TspWindowCreate(__in LPCTSTR lpClassName,
	__in LPCTSTR lpWindowName,
	__in DWORD dwStyle,
	__in int X,
	__in int Y,
	__in int nWidth,
	__in int nHeight,
	__in HWND hWndParent,
	__in DWORD nID);
VOID TspWindowDestroy(HWND hTWnd);
//设置透明色,支持ARGB
VOID TspWindowSetBkColor(HWND hTWnd, COLORREF color);
VOID TspWindowOnSize(HWND hTWnd, UINT nType, INT nWidth, INT nHeight);
VOID TspWindowOnPaint(HWND hTWnd,LPPAINTSTRUCT ps);
VOID TspWindowPaintBg(HWND hTWnd);
VOID TspSetRectAngle(HWND hTWnd, BYTE w, BYTE h);

///////////////////////////////////////////////////////////
//水平居中于父窗口
#define SE_HCETER 1
//垂直居中于父窗口
#define SE_VCETER 2
//保持左上角不变
#define SE_FIXED_LT 4
//保持右下角不变
#define SE_FIXED_RB 8
struct SEditInfo : Control
{
	INT  nSel;     //选择的范围(使用低位和高位)
	HDC hDC;
	COLORREF color;
};

HWND SEditCreate(HWND wParent, INT x, INT y, INT w, INT h, LPCTSTR txt, INT nID);
VOID SEditDestroy(HWND hSEidt);
VOID SEditSetText(HWND hSEidt, LPCTSTR txt);
LPTSTR SEditGetText(HWND hSEidt);
VOID SEditSetTextColor(HWND hSEidt, COLORREF color);
COLORREF  SEditGetTextColor(HWND hSEidt);
//获取编辑框调整后的客户区大小(基于父窗口的坐标系),是否考虑换行符,由bNewLine指定
VOID SEditGetAdjustRect(HWND hSEidt,RECT *pRc,BOOL bNewLine);
//根据内容调整编辑框的大小
//idx定义于SE_开头的宏中
VOID SEditAdjustRect(HWND hSEidt,BOOL bNewLine,INT idx);
VOID SEditOnMouseMove(HWND hSEidt, UINT nFlags, POINTS point);
VOID SEditOnKeyAction(HWND hSEidt,UINT nChar);
VOID SEditOnSetText(HWND hSEidt, LPCTSTR txt);
HBRUSH SEditOnCtlColor(HWND hSEidt,HDC hDC);

//设置为只读
VOID SEditSetReadOnly(HWND sEdit, BOOL bRd);
/////////////////////////////////////////////////////////////////

//默认状态
#define BTN_DEFAULT  0  
#define BTN_MS_HOVER 1
#define BTN_MS_LEAVE 2
#define BTN_MS_UP    3
#define BTN_MS_DOWN  4
//设置第1个编辑框
#define BTN_SET_EDIT0 0
//设置第2个编辑框
#define BTN_SET_EDIT1 1
//设置编辑框
#define BTN_SET_EDIT 2
//设置按钮本身的文本
#define BTN_SET_THIS 3


struct BtnInfo : Control
{
	HDC dcClient;
	BYTE angle[2];//矩形圆角的角度
	HWND  hSEdit[2];    //内容
	SolidBrush  *brBg[5];   //背景画刷
	SolidBrush  *brTxt; //按钮本身的文本颜色,非edit的文本颜色
	HCURSOR hCursor;   //光标
	BYTE act;   //鼠标动作(见宏定义)
	BYTE alm;   //对齐方式
	BOOL bDrawTxt;  //是否文本
		
};

HWND BtnCreate(HWND wParent, INT x, INT y, INT w, INT h, INT nID);
void BtnDestroy(HWND hBtn);
void BtnSetText(HWND hBtn, INT idx, LPCTSTR txt);
//需要手动使用free释放内存
LPTSTR BtnGetText(HWND hBtn, INT idx);
void  BtnSetTextColor(HWND hBtn, INT idx, COLORREF color);
COLORREF  BtnGetTextColor(HWND hBtn, INT idx);
void BtnSetBkColor(HWND hBtn, INT idx, COLORREF color);
COLORREF BtnGetBkColor(HWND hBtn, INT idx);
void BtnSetCursor(HWND hBtn, HCURSOR hCursor);
HCURSOR BtnGetCursor(HWND hBtn, HCURSOR hCursor);
//nAlignment参见CTRL_LAYOUT_宏
//此函数只设置BTN_SET_THIS所代表的文本
void BtnSetAlignment(HWND hBtn, INT nAlignment);
//是否绘制文本
void BtnEnableDrawText(HWND hBtn,BOOL bDrawTxt);
void BtnSetEnableAct(HWND hBtn, BOOL b);
//获取一个文字的填充范围
void BtnGetMargins(HWND hBtn, INT idx,RECT *rcFill);
//通过SEdit的与客户区的左边矩和右边据来向左扩展按钮
//idx可以是BTN_SET_EDIT和BTN_SET_THIS
void BtnAdjustSize(HWND hBtn,INT idx,INT nLFill, INT  nTFill,INT nRFill, INT  nBFill);
//设置btn的圆角的角度的宽和高
VOID BtnSetRectAngle(HWND hBtn, BYTE w, BYTE h);
//选择一个SEdit附加到按钮中,返回之前的SEdit,如果为NULL,相当于删除
HWND BtnAttachSEdit(HWND hBtn,INT idx, HWND hSEidt);
void BtnOnDrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct);
void BtnOnMouseMove(HWND hBtn,UINT nFlags, POINTS point);
void BtnOnMouseHover(HWND hBtn, UINT nFlags, POINTS point);
void BtnOnMouseLeave(HWND hBtn, UINT nFlags, POINTS point);
void BtnOnMouseUp(HWND hBtn,UINT nFlags, POINTS point);
void BtnOnMouseDown(HWND hBtn,UINT nFlags, POINTS point);
void BtnOnSize(HWND hBtn, UINT nType, INT nWidth, INT nHeight);
///////////////////////////////////////////////////////////////
//默认状态
#define LINK_DEFAULT  0  
#define LINK_MS_HOVER 1
#define LINK_MS_LEAVE 2
#define LINK_MS_UP    3
#define LINK_MS_DOWN  4
#define LINK_MS_FOCUS 5
struct LinkInfo : public SEditInfo
{
	COLORREF txtColor[6];
	LPTSTR  url;
	//鼠标动作
	BYTE act;
};

HWND LinkCreate(HWND wParent, INT x, INT y, INT w, INT h, LPCTSTR txt, INT nID);
void  LinkDestroy(HWND seLink);
//是否显示删除线和斜体 
void  LinkShowFontOther(HWND seLink, BOOL bUnderline, BOOL bItalic);
void LinkSetTextColor(HWND seLink, INT idx, COLORREF color);
void LinkSetURL(HWND seLink, LPCTSTR url);
void LinkOnMouseMove(HWND seLink, UINT nFlags, POINTS point);
void LinkOnMouseHover(HWND seLink, UINT nFlags, POINTS point);
void LinkOnMouseLeave(HWND seLink, UINT nFlags, POINTS point);
void LinkOnMouseUp(HWND seLink, UINT nFlags, POINTS point);
void LinkOnMouseDown(HWND seLink, UINT nFlags, POINTS point);
HBRUSH LinkOnCtlColor(HWND seLink, HDC hDC);

