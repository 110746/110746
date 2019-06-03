#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stddef.h>
#pragma   comment(lib,"Msimg32.lib")
#include "controls2048.h"
ULONG_PTR gdiplusToken;


LRESULT CALLBACK CtrlWndProc(HWND, UINT, WPARAM, LPARAM);



LRESULT CALLBACK CtrlWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static Control *ctrl, *ctrlChild;
		
	ctrl = (Control*)GetWindowLong(hWnd, GWL_USERDATA);
 
	switch (message)
	{

	case WM_MOUSEMOVE:
		if (ctrl->type & CTRL_BUTTON)
			BtnOnMouseMove(hWnd, wParam, MAKEPOINTS(lParam));
		//使用了WS_EX_COMPOSITED风格,无须刷新
		//else if (ctrl->type & CTRL_EDIT)
			//SEditOnMouseMove(hWnd, wParam, MAKEPOINTS(lParam));
		if (ctrl->type & CTRL_LINK)
			LinkOnMouseMove(hWnd, wParam, MAKEPOINTS(lParam));
		break;
	case WM_MOUSEHOVER:
		if (ctrl->type & CTRL_BUTTON)
		   BtnOnMouseHover(hWnd, wParam, MAKEPOINTS(lParam));
		if (ctrl->type & CTRL_LINK)
			LinkOnMouseHover(hWnd, wParam, MAKEPOINTS(lParam));
		break;
	case WM_MOUSELEAVE:
		if (ctrl->type & CTRL_BUTTON)
		    BtnOnMouseLeave(hWnd, wParam, MAKEPOINTS(lParam));
		if (ctrl->type & CTRL_LINK)
			LinkOnMouseLeave(hWnd, wParam, MAKEPOINTS(lParam));
		break;
	case WM_LBUTTONUP:
		if (ctrl->type & CTRL_BUTTON)
		    BtnOnMouseUp(hWnd, wParam, MAKEPOINTS(lParam));
		if (ctrl->type & CTRL_LINK)
			LinkOnMouseUp(hWnd, wParam, MAKEPOINTS(lParam));
		//SetFocus(GetTopWindow(hWnd));
		break;
	case WM_LBUTTONDOWN:
		if (ctrl->type & CTRL_BUTTON)
			BtnOnMouseDown(hWnd, wParam, MAKEPOINTS(lParam));
		
		if (ctrl->type & CTRL_LINK)
			LinkOnMouseDown(hWnd, wParam, MAKEPOINTS(lParam));
		
		break;
	case 11111:
		HideCaret(hWnd);
		return 1;
	case WM_SETFOCUS:		
		PostMessage(hWnd, 11111, 0, 0);
		break;
	case WM_KILLFOCUS:
	
		//使用了WS_EX_COMPOSITED风格,无须刷新
	//	if (ctrl->type & CTRL_EDIT)
			//SEditOnSetText(hWnd, NULL);
		break;
	case WM_DRAWITEM:
	{
		ctrlChild = CtrlGetInfo(((LPDRAWITEMSTRUCT)lParam)->hwndItem);
		if (ctrlChild == NULL)
			break;
     
		if (ctrlChild->type & CTRL_BUTTON)
			BtnOnDrawItem((LPDRAWITEMSTRUCT)lParam);
	}
	break;
	case WM_KEYDOWN:
	{
		//extern HWND hWnd;
		//SetFocus(hWnd);
	}
	case WM_KEYUP:
		
		if (ctrl->type & CTRL_EDIT)
			SEditOnKeyAction(hWnd, wParam);
		
		break;
		
	case WM_SETFONT:
		 if(ctrl->type & CTRL_EDIT)
	     {
		   SEditInfo *psei = (SEditInfo *)CtrlGetInfo(hWnd);
		   //更新字体信息,用于测试信息
		   SelectObject(psei->hDC, (HFONT)wParam);
	    }
	break;
	case WM_SETTEXT:
		//使用了WS_EX_COMPOSITED风格,无须刷新
		//if (ctrl->type & CTRL_EDIT)
			//SEditOnSetText(hWnd, (LPTSTR)lParam);		
		break;
	case WM_SETCURSOR:
		if (ctrl->type & CTRL_BUTTON)
		{
			//HCURSOR hCursor;
			//通知父窗口设置按钮要设置光标			
			//SHORT msg[2] = { (SHORT)GetDlgCtrlID(hWnd),(SHORT)WM_SETCURSOR };
			//hCursor = (HCURSOR)SendMessage(GetParent(hWnd), WM_COMMAND,*(WPARAM*)msg, (LPARAM)hWnd);
			//SetCursor(hCursor);
			SetCursor(((BtnInfo*)ctrl)->hCursor);
			return TRUE;

		}else 
			if (ctrl->type & CTRL_LINK)
			{				
				SetCursor(LoadCursor(NULL, IDC_HAND));
				return TRUE;
			}
			
		break;
	 
	case WM_COMMAND:
		break;
	 
	case WM_CTLCOLORBTN:
		return (LRESULT)GetStockObject(NULL_BRUSH);
		
	case WM_SIZE:
		if (ctrl->type & CTRL_TRANSPARENTWINDOW)
			TspWindowOnSize(hWnd, wParam, LOWORD(lParam),HIWORD(lParam));
		if (ctrl->type &  CTRL_BUTTON)
			BtnOnSize(hWnd, wParam, LOWORD(lParam), HIWORD(lParam));
		break;

	case WM_ERASEBKGND:
		//break;
		if (ctrl->type & CTRL_TRANSPARENTWINDOW)
			return 1;
		
		break;

	case WM_PAINT:
		//break;
		if (ctrl->type & CTRL_TRANSPARENTWINDOW)
		{
			static PAINTSTRUCT ps;
			BeginPaint(hWnd, &ps);
			TspWindowOnPaint(hWnd, &ps);
			EndPaint(hWnd, &ps);
					
			 return 0;
		}

		break;
		

	case WM_CTLCOLOREDIT:
	case WM_CTLCOLORSTATIC:
		ctrlChild = CtrlGetInfo((HWND)lParam);
		if (ctrlChild == NULL)
			break;
		if (ctrlChild->type & CTRL_LINK)
			return (LRESULT)LinkOnCtlColor((HWND)lParam, (HDC)wParam);

		if (ctrlChild->type & CTRL_EDIT)
			return (LRESULT)SEditOnCtlColor((HWND)lParam, (HDC)wParam);

		break;

	case WM_DESTROY:
		break;


	}
   	
 	return CallWindowProc(ctrl->wpOld, hWnd, message, wParam, lParam);
}

VOID CtrlInit()
{
	Gdiplus::GdiplusStartupInput StartupInput;
	GdiplusStartup(&gdiplusToken, &StartupInput, NULL);
}

VOID CtrlExit()
{
	Gdiplus::GdiplusShutdown(gdiplusToken);
}

Control* CtrlGetInfo(HWND hCtrl)
{
	return (Control *)GetWindowLong(hCtrl, GWL_USERDATA);
}


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
	__in DWORD nID)
{

 
	// 创建应用程序主窗口
	HWND hWnd = CreateWindowEx(dwExStyle | 0,
		        lpClassName,
		        lpWindowName,
		        dwStyle,
				X,Y, nWidth, nHeight,
		        hWndParent,
		        (HMENU)nID,
		        GetModuleHandle(NULL),
				NULL);

	pCtrl->wpOld = (WNDPROC)SetWindowLong(hWnd, GWL_WNDPROC, (LONG)CtrlWndProc);
	

	//加入父窗口标记
	if (hWndParent != NULL)
	{
		Control *ctrl = (Control *)GetWindowLong(hWndParent, GWL_USERDATA);
		if(ctrl)
		       ctrl->type |= CTRL_PARENT;
	}
	 

	//设置子窗口数据
	pCtrl->type = CTRL_CHILD;
	SetWindowLong(hWnd, GWL_USERDATA, (LONG)pCtrl);
	 

	return hWnd;
 
}

HWND CtrlCreateWindow(__in LPCTSTR lpClassName,
	__in LPCTSTR lpWindowName,
	__in DWORD dwStyle,
	__in int X,
	__in int Y,
	__in int nWidth,
	__in int nHeight,
	__in HWND hWndParent,
	__in DWORD nID)
{
	Control *pCtr = (Control *)calloc(1, sizeof(Control));

	// 创建应用程序主窗口
	HWND hWnd = CtrlCreate(pCtr,lpClassName,
		        lpWindowName,
		        dwStyle,
		WS_EX_COMPOSITED,
				X,Y, nWidth, nHeight,
		        hWndParent,
		        nID);

	pCtr->type |= CTRL_WINDOW;
 
	return hWnd;
}

VOID CtrlRedrawParent(HWND hChild, HWND hParent, RECT *pRcClient)
{
	RECT rcParent = *pRcClient;
	DWORD oldStyle = GetWindowLong(hParent, GWL_STYLE);
	SetWindowLong(hParent, GWL_STYLE, oldStyle & ~WS_CLIPCHILDREN);
	//只更新父窗口,因为需要截取父窗口的背景
	MapWindowPoints(hChild, hParent, (POINT*)&rcParent, 2);
	RedrawWindow(hParent, &rcParent, NULL, RDW_UPDATENOW | RDW_INVALIDATE | RDW_NOCHILDREN);
	SetWindowLong(hParent, GWL_STYLE, oldStyle);
}

void CtrlSetFont(HWND hWnd, INT size, LPCTSTR name, INT weight)
{
	LOGFONT lgFont = { 0 };
	HFONT hFont;
	
	lgFont.lfHeight = size;
	lgFont.lfWeight = weight;
	_tcscpy(lgFont.lfFaceName, name);
	
	hFont = CreateFontIndirect(&lgFont);
	SendMessage(hWnd, WM_SETFONT, (WPARAM)hFont, TRUE);
}
HFONT CtrlGetFont(HWND hWnd)
{
	return (HFONT)SendMessage(hWnd, WM_GETFONT, 0, 0);
}

DWORD  CtrlLayoutToDrawText(DWORD nLayout)
{
	DWORD n = 0;
	
	if (nLayout & CTRL_LAYOUT_LEFT)
		n |= DT_LEFT;

	if (nLayout & CTRL_LAYOUT_TOP)
		n |= DT_TOP;

	if (nLayout & CTRL_LAYOUT_RIGHT)
		n |= DT_RIGHT;

	if (nLayout & CTRL_LAYOUT_BOTTOM)
		n |= DT_BOTTOM;

	if (nLayout & CTRL_LAYOUT_HCENTER)
		n |= DT_CENTER;

	if (nLayout & CTRL_LAYOUT_VCENTER)
		n |= DT_VCENTER;

	return n;
}

DWORD  CtrlLayoutToStringAlignment(DWORD nLayout)
{
	BYTE n[sizeof(DWORD)] = { 0 };

	if (nLayout & CTRL_LAYOUT_LEFT)
		n[0] = StringAlignmentNear;

	if (nLayout & CTRL_LAYOUT_TOP)
		n[1] = StringAlignmentNear;

	if (nLayout & CTRL_LAYOUT_RIGHT)
		n[0] = StringAlignmentFar;

	if (nLayout & CTRL_LAYOUT_BOTTOM)
		n[1] = StringAlignmentFar;

	if (nLayout & CTRL_LAYOUT_HCENTER)
		n[0] = StringAlignmentCenter;

	if (nLayout & CTRL_LAYOUT_VCENTER)

		n[1] = StringAlignmentCenter;

	return *(DWORD*)n;
}


VOID CtrlGetPosRect(HWND hWnd, HWND hParent, RECT *pRc, INT idx)
{
	static RECT  rcParent;	
	INT w, h;

	if(hParent == NULL)
	     hParent  = GetParent(hWnd);


	if (!(idx & CTRL_LAYOUT_INRECT))
	{
		GetClientRect(hWnd, pRc);
		MapWindowPoints(hWnd, hParent, (POINT*)pRc, 2);
	}		

	w = pRc->right - pRc->left;
	h = pRc->bottom - pRc->top;	
    
	if (idx & CTRL_LAYOUT_RIGHT ||
		idx & CTRL_LAYOUT_BOTTOM ||
		idx & CTRL_LAYOUT_HCENTER ||
		idx & CTRL_LAYOUT_VCENTER)
	{
		if (hParent)
			GetClientRect(hParent, &rcParent);
		else
			SystemParametersInfo(SPI_GETWORKAREA, 0, &rcParent, 0);
    }

	
	if (idx & CTRL_LAYOUT_LEFT)
	{
		pRc->left = 0;
		pRc->right = w;
	}

	if (idx & CTRL_LAYOUT_TOP)
	{
		pRc->top = 0;
		pRc->bottom = h;
	}
	

	if (idx & CTRL_LAYOUT_RIGHT)
	{	
		pRc->right = rcParent.right;
		pRc->left = rcParent.right - w;
	}

	if (idx & CTRL_LAYOUT_BOTTOM)
	{

		pRc->bottom = rcParent.bottom;
		pRc->top = rcParent.bottom - h;
	}

	if (idx & CTRL_LAYOUT_HCENTER)
	{
		pRc->left = (rcParent.right - w) / 2;
		pRc->right = pRc->left + w;
	}

	if (idx & CTRL_LAYOUT_VCENTER)
	{

		pRc->top = (rcParent.bottom - h) / 2;
		pRc->bottom = pRc->top + h;
	}
}



VOID CtrlGetExtRect(HWND hWnd, RECT *pRc, INT nWidth, INT nHeight, INT idx)
{

	if (!(idx & CTRL_LAYOUT_INRECT))
	{
		GetClientRect(hWnd, pRc);
		MapWindowPoints(hWnd, GetParent(hWnd), (POINT*)pRc, 2);
	}
	 
	if (idx & CTRL_LAYOUT_EXLEFT)
	{
		//不能和EXLR一起使用
		if (idx & CTRL_LAYOUT_EXLR)
			idx &= ~CTRL_LAYOUT_EXLR;

		pRc->left = pRc->right - nWidth;
	}
	if (idx & CTRL_LAYOUT_EXUP)
	{
		//不能和EXTB一起使用
		if (idx & CTRL_LAYOUT_EXTB)
			idx &= ~CTRL_LAYOUT_EXTB;

		pRc->top = pRc->bottom - nHeight;
	}

	if (idx & CTRL_LAYOUT_EXRIGHT)
	{
		//不能和EXLR一起使用
		if (idx & CTRL_LAYOUT_EXLR)
			idx &= ~CTRL_LAYOUT_EXLR;
		pRc->right = pRc->left + nWidth;
	}

	if (idx & CTRL_LAYOUT_EXDOWN)
	{
		//不能和EXTB一起使用
		if (idx & CTRL_LAYOUT_EXTB)
			idx &= ~CTRL_LAYOUT_EXTB;

		pRc->bottom = pRc->top + nHeight;
	}

	if (idx & CTRL_LAYOUT_EXLR)
	{
		INT nHCenter = pRc->left + (pRc->right - pRc->left) / 2;    
		nWidth /= 2;
		pRc->left = nHCenter - nWidth;
		pRc->right = nHCenter + nWidth;

	}

	if (idx & CTRL_LAYOUT_EXTB)
	{
		INT nVCenter = pRc->top + (pRc->bottom - pRc->top) / 2;
		nHeight /= 2;
		pRc->top = nVCenter - nHeight;
		pRc->bottom = nVCenter + nHeight;
	}

}

VOID CtrlAdjustClientDC(HWND hWnd, RECT *pRc, INT offset)
{
	BYTE *pbCtrl = (BYTE *)CtrlGetInfo(hWnd);
	HDC dcClient = *(HDC*)(pbCtrl + offset);
	HGDIOBJ bmpNew;
	HGDIOBJ bmpoOld;
	HDC hDC = GetDC(hWnd);

	if (pRc == NULL)
	{
		static RECT rc;
		pRc = &rc;
		GetClientRect(hWnd, pRc);
	}
    
	bmpNew = CreateCompatibleBitmap(hDC, pRc->right - pRc->left, pRc->bottom - pRc->top);
	bmpoOld = SelectObject(dcClient, bmpNew);
	DeleteObject(bmpoOld);
	ReleaseDC(hWnd, hDC);
}

VOID CtrlSetRectAngle(HWND hWnd, BYTE w, BYTE h, INT offset)
{
	BYTE *pbCtrl = (BYTE *)CtrlGetInfo(hWnd);
	BYTE *angle = pbCtrl + offset;

	angle[0] = w;
	angle[1] = h;

}

////////////////////////////////////////////////////////////////////////////

HWND TspWindowCreate(__in LPCTSTR lpClassName,
	__in LPCTSTR lpWindowName,
	__in DWORD dwStyle,
	__in int X,
	__in int Y,
	__in int nWidth,
	__in int nHeight,
	__in HWND hWndParent,
	__in DWORD nID)
{

	TspWndInfo *twi = (TspWndInfo*)calloc(1,sizeof(TspWndInfo));

	// 创建应用程序主窗口
	HWND hWnd = CtrlCreate(twi, lpClassName,
		lpWindowName,
		dwStyle,
		0,
		X, Y, nWidth, nHeight,
		hWndParent,
		nID);

	twi->type |= CTRL_TRANSPARENTWINDOW;
     
	//模拟wm_size消息调整大小
	POINTS pts = { (SHORT)nWidth,(SHORT)nHeight };
	SendMessage(hWnd, WM_SIZE, SIZE_MAXIMIZED, *(LPARAM*)&pts);
	 
	return hWnd;

}

VOID TspWindowDestroy(HWND hTWnd)
{
	TspWndInfo *twi = (TspWndInfo *)CtrlGetInfo(hTWnd); 
	DeleteObject(GetCurrentObject(twi->dcClient, OBJ_BITMAP));	
	DeleteDC(twi->dcClient);	
	free(twi);
}


VOID TspWindowSetBkColor(HWND hTWnd, COLORREF color)
{
	TspWndInfo *twi = (TspWndInfo *)CtrlGetInfo(hTWnd);
	BYTE *pc = (BYTE*)&color;
	//转化为gdi使用的颜色
	pc[2] ^= pc[0];
	pc[0] ^= pc[2];
	pc[2] ^= pc[0];

	twi->color = color;
;
	TspWindowPaintBg(hTWnd);
	InvalidateRect(hTWnd, NULL, FALSE);

}

VOID TspWindowOnSize(HWND hTWnd, UINT nType, INT nWidth, INT nHeight)
{
	TspWndInfo *twi = (TspWndInfo *)CtrlGetInfo(hTWnd);

	if (twi->dcClient == NULL)
	{
		HDC hDC = GetDC(hTWnd);			
		twi->dcClient = CreateCompatibleDC(hDC);		
		ReleaseDC(hTWnd,hDC);		
	}
	 
	CtrlAdjustClientDC(hTWnd, NULL, offsetof(TspWndInfo, dcClient));
}
 
VOID TspWindowPaintBg(HWND hTWnd)
{
	TspWndInfo *twi = (TspWndInfo *)CtrlGetInfo(hTWnd);
	RECT rc;
	HGDIOBJ br, brOld;
	HGDIOBJ pen, penOld;

	GetClientRect(hTWnd, &rc);
	br  = CreateSolidBrush(twi->color & 0x00ffffff);
	pen = CreatePen(PS_NULL, 0, 0);

	brOld = SelectObject(twi->dcClient,br);
	penOld = SelectObject(twi->dcClient, pen);
	
	//绘制背景
	RoundRect(twi->dcClient, rc.left, rc.top, rc.right, rc.bottom, twi->angle[0],twi->angle[1]);

	SelectObject(twi->dcClient, brOld);
	SelectObject(twi->dcClient, penOld);
	DeleteObject(br);
	DeleteObject(pen);
}

VOID TspSetRectAngle(HWND hTWnd, BYTE w, BYTE h)
{
	CtrlSetRectAngle(hTWnd, w, h, offsetof(TspWndInfo, angle));
}

VOID TspWindowOnPaint(HWND hTWnd,LPPAINTSTRUCT ps)
{
	TspWndInfo *twi = (TspWndInfo *)CtrlGetInfo(hTWnd);
	static BLENDFUNCTION bf = { AC_SRC_OVER ,0,0,0 };
	RECT *rc = &ps->rcPaint;
	LONG w = rc->right - rc->left;
	LONG h = rc->bottom - rc->top;
	POINT ptParent = { 0,0 };

	if (twi->dcClient != NULL)
	{
		GetClientRect(hTWnd, rc);
		w = rc->right - rc->left;
		h = rc->bottom - rc->top;

		//HGDIOBJ b1 = GetCurrentObject(GetDC(GetParent(hTWnd)), OBJ_BITMAP);
		//HGDIOBJ b2 = GetCurrentObject(ps->hdc, OBJ_BITMAP);
		

		//extern HWND hWndTest;
		//InvalidateRect(hWndTest, NULL, FALSE);
		//UpdateWindow(hWndTest);
		//设置透明度
		bf.SourceConstantAlpha = GetAValue(twi->color);
		
		AlphaBlend(ps->hdc, rc->left, rc->top, w,h,
			twi->dcClient, rc->left, rc->top,w,h,bf);


	
	} 

	
}

////////////////////////////////////////////////////////////////////////////


HWND SEditCreate(HWND wParent, INT x, INT y, INT w, INT h, LPCTSTR txt, INT nID)
{
	SEditInfo* psei = (SEditInfo*)calloc(1, sizeof(SEditInfo));

	HWND sEdit = CtrlCreate(psei, _T("Edit"), txt,
		0 | DS_LOCALEDIT | ES_RIGHT |  ES_MULTILINE |  WS_CHILD |  WS_VISIBLE | ES_CENTER ,
		0, x, y, w, h, wParent, nID);
	
	psei->type |= CTRL_EDIT;
	psei->hDC = GetDC(sEdit);
 
	return sEdit;
}



VOID SEditSetText(HWND hSEidt, LPCTSTR txt)
{
	SendMessage(hSEidt, WM_SETTEXT, 0, (LPARAM)txt);
}
LPTSTR SEditGetText(HWND hSEidt)
{
	HLOCAL hLocal = (HLOCAL)SendMessage(hSEidt, EM_GETHANDLE, 0, 0);
	LPTSTR txt = (LPTSTR)LocalLock(hLocal);
	LocalUnlock(hLocal);
	return txt;
}

VOID SEditSetTextColor(HWND hSEidt, COLORREF color)
{
	((SEditInfo *)CtrlGetInfo(hSEidt))->color = color;
	InvalidateRect(hSEidt, NULL, FALSE);
}



COLORREF  SEditGetTextColor(HWND hSEidt)
{
	SEditInfo *psei = (SEditInfo *)CtrlGetInfo(hSEidt);
	if (psei)
		return psei->color;
	
	return 0;
	
}

VOID SEditGetAdjustRect(HWND hSEidt, RECT *pRc, BOOL bNewLine)
{
	SEditInfo *sei;
	RECT rcClient, rcMargins;
	INT  sLFill = 0, sRFill = 0,sTFill = 0,sBFill = 0;  //填充

	sei = (SEditInfo*)CtrlGetInfo(hSEidt);

	//获取填充范围的矩形
	SendMessage(hSEidt, EM_GETRECT, 0, (LPARAM)&rcMargins);
	//printf("l=%d,t=%d,r=%d,b=%d\n", rcMargins.left, rcMargins.top,rcMargins.right,rcMargins.bottom);


	//获取左边填充和右边填充
	//不准确,弃用
	//LRESULT  nFill = SendMessage(hSEidt, EM_GETMARGINS, 0, 0);
	//sLFill = LOWORD(nFill);//得到左填充
	//sRFill = HIWORD(nFill);//得到右填充

	
	//测量客户区矩形大小所需要的最大能容纳文本的矩形
	GetClientRect(hSEidt, &rcClient);
	*pRc = rcClient;
	 


	DWORD style = 0;
	if (bNewLine)
		style =  DT_WORDBREAK  | DT_CALCRECT;
	else
		style = DT_EXTERNALLEADING | DT_SINGLELINE  | DT_CALCRECT | DT_NOCLIP;
	
	 
	//SIZE size;
	LPCTSTR txt = SEditGetText(hSEidt);
	//GetTextExtentPoint(sei->hDC, txt, _tcslen(txt), &size);
 
	DrawText(sei->hDC, SEditGetText(hSEidt), -1, pRc, style);
	 

	//if (rcMargins.right - rcMargins.left < w ||
		//rcMargins.bottom - rcMargins.top < h)
	{
		//得到左填充
	    if(rcMargins.left)
		  sLFill = rcMargins.left;
		//得到右填充
	   if(rcMargins.right)
		  sRFill = rcClient.right - rcMargins.right;
		//得到顶填充
	   if(rcMargins.top)
		sTFill = rcMargins.top;
	    //得到底填充 
	   if(rcMargins.bottom)
		 sBFill = rcClient.bottom - rcMargins.bottom;
	  
		//计算新的客户区宽度
		pRc->right =  sLFill + pRc->right + sRFill ;
		pRc->bottom =  sTFill + pRc->bottom + sBFill ;
		 
	}
	
	
}

VOID SEditAdjustRect(HWND hSEidt, BOOL bNewLine,INT idx)
{
	RECT rcAdjust;

	//获取调整后的编辑框的客户区
	SEditGetAdjustRect(hSEidt, &rcAdjust, bNewLine);
 

	//转化到父坐标系
	MapWindowPoints(hSEidt, GetParent(hSEidt), (POINT*)&rcAdjust, 2);
	/*
	//SE_FIXED_LT实际上就是MapWindowPoints的结果,所以无需重复调用
	if (idx & SE_FIXED_LT)
		CtrlGetExtRect(hSEidt, &rcAdjust, 
			rcAdjust.right - rcAdjust.left, 
			rcAdjust.bottom - rcAdjust.top,
			CTRL_LAYOUT_EXLEFT | CTRL_LAYOUT_EXUP | CTRL_LAYOUT_INRECT);
			*/
	if (idx & SE_FIXED_RB)
		CtrlGetExtRect(hSEidt, &rcAdjust,
			rcAdjust.right - rcAdjust.left,
			rcAdjust.bottom - rcAdjust.top,
			CTRL_LAYOUT_EXRIGHT | CTRL_LAYOUT_EXDOWN | CTRL_LAYOUT_INRECT);


	if (idx & SE_HCETER)
		CtrlGetPosRect(hSEidt, NULL,&rcAdjust, CTRL_LAYOUT_HCENTER | CTRL_LAYOUT_INRECT );

	if(idx & SE_VCETER)
		CtrlGetPosRect(hSEidt, NULL,&rcAdjust, CTRL_LAYOUT_VCENTER | CTRL_LAYOUT_INRECT);


	MoveWindow(hSEidt, rcAdjust.left, rcAdjust.top,
		rcAdjust.right - rcAdjust.left,
		rcAdjust.bottom - rcAdjust.top, TRUE);

	
}


VOID SEditOnMouseMove(HWND hSEidt, UINT nFlags, POINTS point)
{
 
	   SEditInfo *sei = (SEditInfo *)CtrlGetInfo(hSEidt);

		INT nSel;
		nSel = SendMessage(hSEidt, EM_GETSEL, 0, 0);

		if (sei->nSel != nSel)
		{	 
			RECT rcUpdate;	
			HWND hParent = GetParent(hSEidt);

			GetClientRect(hSEidt, &rcUpdate);
			MapWindowPoints(hSEidt, hParent, (POINT*)&rcUpdate, 2);
			InvalidateRect(hParent, &rcUpdate,FALSE);
			
			//保存下一次选择范围
			sei->nSel = SendMessage(hSEidt, EM_GETSEL, 0, 0);
  
	 
	}
}

VOID SEditOnKeyAction(HWND hSEidt, UINT nChar)
{
	SEditOnSetText(hSEidt, NULL);

}
VOID SEditOnSetText(HWND hSEidt, LPCTSTR txt)
{
	RECT rcUpdate;
	HWND hParent = GetParent(hSEidt);

	GetClientRect(hSEidt, &rcUpdate);
	MapWindowPoints(hSEidt, hParent, (POINT*)&rcUpdate, 2);
	InvalidateRect(hParent, &rcUpdate, FALSE);

}
HBRUSH SEditOnCtlColor(HWND hSEidt, HDC hDC)
{
	//设置字体背景透明
	SetBkMode(hDC, TRANSPARENT);
	//设置字体颜色
	SetTextColor(hDC, SEditGetTextColor(hSEidt));
 
	return (HBRUSH)GetStockObject(NULL_BRUSH);
}

VOID SEditSetReadOnly(HWND sEdit, BOOL bRd)
{
	SendMessage(sEdit, EM_SETREADONLY, bRd, 0);
}

VOID SEditDestroy(HWND hSEidt)
{
	SEditInfo *psei;

	DestroyWindow(hSEidt);

	//设置配置信息
	psei = (SEditInfo *)CtrlGetInfo(hSEidt);
	if (psei)
	{
		free(psei);
		ReleaseDC(hSEidt, psei->hDC);
	}

}

/////////////////////////////////////////////////////////////////////////////////

HWND BtnCreate(HWND wParent, INT x, INT y, INT w, INT h, INT nID)
{
	BtnInfo *bti = (BtnInfo*)calloc(1, sizeof(BtnInfo));

	HWND hBtn = CtrlCreate(bti,_T("Button"), _T(""),
		 WS_CHILD | WS_VISIBLE | BS_OWNERDRAW , WS_EX_TRANSPARENT ,
		x, y, w, h, wParent, nID);

	bti->type |= CTRL_BUTTON;

	BtnSetAlignment(hBtn, CTRL_LAYOUT_HCENTER | CTRL_LAYOUT_VCENTER);
	//设置默认光标为箭头
	BtnSetCursor(hBtn, LoadCursor(NULL, IDC_ARROW));

	POINTS pts = { (SHORT)w,(SHORT)h };
	SendMessage(hBtn, WM_SIZE, SIZE_MAXIMIZED, *(LPARAM*)&pts);
	InvalidateRect(hBtn, NULL, FALSE);

	return hBtn;
}


void BtnDestroy(HWND hBtn)
{

	BtnInfo *bti  = (BtnInfo *)CtrlGetInfo(hBtn);
	
	for (int i = 0; i < sizeof(bti->brBg) / sizeof(*bti->brBg); ++i)
		delete bti->brBg[i];

	delete bti->brTxt;

	DeleteObject(GetCurrentObject(bti->dcClient, OBJ_BITMAP));
	DeleteDC(bti->dcClient);
	
	free(bti);
}


void BtnSetText(HWND hBtn, INT idx, LPCTSTR txt)
{
	BtnInfo *bti = (BtnInfo *)CtrlGetInfo(hBtn);

	switch (idx)
	{
	case BTN_SET_THIS:
		SendMessage(hBtn, WM_SETTEXT, 0, (LPARAM)txt);
		break;
	case BTN_SET_EDIT0:
	case BTN_SET_EDIT1:
		if (bti->hSEdit[idx])
		    SEditSetText(bti->hSEdit[idx], txt);
		break;
	} 

	InvalidateRect(hBtn, 0, FALSE);

}
LPTSTR BtnGetText(HWND hBtn, INT idx)
{
	BtnInfo *bti = (BtnInfo *)CtrlGetInfo(hBtn);
	LPTSTR txt;
	DWORD len;

	switch (idx)
	{
	case BTN_SET_THIS:
		len = SendMessage(hBtn, WM_GETTEXTLENGTH, 0, 0);
		txt = (LPTSTR)calloc(1,len * 2 + sizeof(TCHAR));		
		len = SendMessage(hBtn, WM_GETTEXT, len + 1, (LPARAM)txt);
		break;
	case BTN_SET_EDIT0:
	case BTN_SET_EDIT1:
		len = SendMessage(bti->hSEdit[idx], WM_GETTEXTLENGTH, 0, 0);
		txt = (LPTSTR)malloc(len * 2 + sizeof(TCHAR));
		_tcscpy(txt, SEditGetText(bti->hSEdit[idx]));
		break;
	}
	return txt;
}


void  BtnSetTextColor(HWND hBtn, INT idx, COLORREF color)
{
	BtnInfo *bti = (BtnInfo *)CtrlGetInfo(hBtn);
	
	switch (idx)
	{
	case BTN_SET_THIS:
		if (bti->brTxt)
			bti->brTxt->SetColor(Color(color));
		else 
		   bti->brTxt = new SolidBrush(Color(color));
		break;
	case BTN_SET_EDIT0:
	case BTN_SET_EDIT1:
		if (bti->hSEdit[idx])
			SEditSetTextColor(bti->hSEdit[idx], color);
		break;
	}
	
	InvalidateRect(hBtn, 0, FALSE);

}
COLORREF  BtnGetTextColor(HWND hBtn, INT idx)
{
	BtnInfo *bti = (BtnInfo *)CtrlGetInfo(hBtn);

	switch (idx)
	{
	case BTN_SET_THIS:
	{
		Color  c;
		bti->brTxt->GetColor(&c);
		return c.GetValue();
	}
		
	case BTN_SET_EDIT0:
	case BTN_SET_EDIT1:
		if (bti->hSEdit[idx])
			return SEditGetTextColor(bti->hSEdit[idx]);
		break;
	}
	return 0;
}

void BtnSetBkColor(HWND hBtn, INT idx, COLORREF color)
{
	BtnInfo *bti = (BtnInfo *)CtrlGetInfo(hBtn);
	//DeleteObject(bti->brBg[idx]);
	//bti->brBg[idx] = CreateSolidBrush(color);
	if (bti->brBg[idx])
		bti->brBg[idx]->SetColor(Color(color));
	else 
	   bti->brBg[idx] = new SolidBrush(Color(color));
   if(bti->act == idx)
	  InvalidateRect(hBtn, 0, FALSE);
}
COLORREF BtnGetBkColor(HWND hBtn, INT idx)
{
	BtnInfo *bti = (BtnInfo *)CtrlGetInfo(hBtn);

	//LOGBRUSH lgbr;
	//GetObject(bti->brBg[idx], sizeof(lgbr), &lgbr);
	//return  lgbr.lbColor;

	Color c;
	bti->brBg[idx]->GetColor(&c);
	return c.GetValue();
}

void BtnSetCursor(HWND hBtn, HCURSOR hCursor)
{
	BtnInfo *bti = (BtnInfo *)CtrlGetInfo(hBtn);
	bti->hCursor = hCursor;
}
HCURSOR BtnGetCursor(HWND hBtn, HCURSOR hCursor)
{
	BtnInfo *bti = (BtnInfo *)CtrlGetInfo(hBtn);
	return bti->hCursor;
}

void BtnSetAlignment(HWND hBtn, INT nAlignment)
{
	BtnInfo *bti = (BtnInfo *)CtrlGetInfo(hBtn);
	bti->alm = nAlignment;
	InvalidateRect(hBtn, NULL, FALSE);
}

void BtnEnableDrawText(HWND hBtn,BOOL bDrawTxt)
{
	BtnInfo *bti = (BtnInfo *)CtrlGetInfo(hBtn);
	bti->bDrawTxt = bDrawTxt;
}

void BtnGetMargins(HWND hBtn, INT idx, RECT *rcFill)
{
	BtnInfo *bti = (BtnInfo *)CtrlGetInfo(hBtn);
	RECT rcBtn;
   
	GetClientRect(hBtn, &rcBtn);

	switch (idx)
	{
	case BTN_SET_THIS:
	{
	
		HDC dc = GetDC(hBtn);
		LPTSTR txt = BtnGetText(hBtn, BTN_SET_THIS);
		RECT rc = { 0 };
		SelectObject(dc, CtrlGetFont(hBtn));
		
		DrawText(dc, txt, -1, &rc, CtrlLayoutToDrawText(bti->alm)|DT_CALCRECT);
		CtrlGetPosRect(NULL, hBtn,&rc, bti->alm | CTRL_LAYOUT_INRECT);
		rcFill->left = rc.left - rcBtn.left;
		rcFill->top = rc.top - rcBtn.top;
		rcFill->right = rcBtn.right - rc.right;
		rcFill->bottom = rcBtn.bottom - rc.bottom;				
		free(txt);
		ReleaseDC(hBtn, dc);
	}
		break;
	case BTN_SET_EDIT:
	{
		RECT rc[2];
		RECT *rcHeight, *rcLow, *rcHLenMax;

		GetClientRect(bti->hSEdit[0], rc + 0);
		GetClientRect(bti->hSEdit[1], rc + 1);
		MapWindowPoints(bti->hSEdit[0], hBtn, (POINT*)(rc + 0), 2);
		MapWindowPoints(bti->hSEdit[1], hBtn, (POINT*)(rc + 1), 2);

		if (rc[0].top < rc[1].top)
		{
			rcHeight = rc + 0;
			rcLow = rc + 1;
		}else
		{
			rcHeight = rc + 1;
			rcLow = rc + 0;
		}

		if (rc[0].right - rc[0].left < rc[1].right - rc[1].left)
			rcHLenMax = rc + 1;
		else
			rcHLenMax = rc + 0;
	
		rcFill->left = rcHLenMax->left - rcBtn.left;
		rcFill->top = rcHeight->top - rcBtn.top;
		rcFill->right = rcBtn.right - rcHLenMax->right;
		rcFill->bottom = rcBtn.bottom - rcLow->bottom;
	    
		
	}
		break;
	 }
}

void BtnSetEnableAct(HWND hBtn,BOOL b)
{
	BtnInfo *bti = (BtnInfo *)CtrlGetInfo(hBtn);
	bti->act = b ? BTN_MS_LEAVE : BTN_DEFAULT;
}

void BtnAdjustSize(HWND hBtn, INT idx, INT nLFill, INT  nTFill, INT nRFill, INT  nBFill)
{
	BtnInfo *bti = (BtnInfo *)CtrlGetInfo(hBtn);
	RECT rc1 = { 0 }, rc2 = { 0 };
	RECT rcBtn;
	CONST BOOL isNewLine = FALSE;

	GetClientRect(hBtn, &rcBtn);
	

	switch (idx)
	{
	case BTN_SET_THIS:
	{
		HDC dc = GetDC(hBtn);
		SelectObject(dc, CtrlGetFont(hBtn));
		LPTSTR txt = BtnGetText(hBtn, BTN_SET_THIS);
		rc1 = rcBtn;
		DrawText(dc, txt, -1, &rc1, DT_SINGLELINE | DT_CALCRECT);
		MapWindowPoints(hBtn, GetParent(hBtn), (POINT*)&rcBtn, 2);
		CtrlGetExtRect(hBtn, &rcBtn,
			nLFill + rc1.right + nRFill,
			nTFill + rc1.bottom + nBFill,
			CTRL_LAYOUT_EXLR | CTRL_LAYOUT_EXTB | CTRL_LAYOUT_INRECT);
		free(txt);
		ReleaseDC(hBtn,dc);
	}
	break;
	case BTN_SET_EDIT:
	{
		HWND hSEdit1 = bti->hSEdit[0];
		HWND hSEdit2 = bti->hSEdit[1];
	   

		SEditGetAdjustRect(hSEdit1, &rc1, isNewLine);
		MapWindowPoints(hSEdit1, hBtn, (POINT*)&rc1, 2);				

		SEditGetAdjustRect(hSEdit2, &rc2, isNewLine);
		MapWindowPoints(hSEdit2, hBtn, (POINT*)&rc2, 2);
		
						
		//确定最长的编辑框
		RECT *rcWMax = (rc1.right - rc1.left) < (rc2.right - rc2.left) ? &rc2 : &rc1;		
		INT w = 0;
		//w = abs(rcWMax->left - rcBtn.left);
		w += (rcWMax->right - rcWMax->left);
		//w += abs(rcBtn.right - rcWMax->right);

		//确定高位置和低位置的编辑框
		RECT *rcHeight = rc1.top < rc2.top ? &rc1 : &rc2;
		RECT *rcLow = rc1.top > rc2.top ? &rc1 : &rc2;
		INT h = 0;
		//h = abs(rcHeight->top - rcBtn.top);
		h += rcHeight->bottom - rcHeight->top;
		h += rcLow->top - rcHeight->bottom;
		h += rcLow->bottom - rcLow->top;
		//h += abs(rcBtn.bottom - rcLow->bottom);
		
		MapWindowPoints(hBtn, GetParent(hBtn), (POINT*)&rcBtn, 2);
		CtrlGetExtRect(hBtn, &rcBtn,
			nLFill + w + nRFill,
			nTFill + h + nBFill,
			CTRL_LAYOUT_EXLEFT | CTRL_LAYOUT_EXTB | CTRL_LAYOUT_INRECT);
		 
	}	
		break;
	}

	
	MoveWindow(hBtn, rcBtn.left, rcBtn.top, 
		rcBtn.right - rcBtn.left, 
		rcBtn.bottom - rcBtn.top, TRUE);

	if (idx != BTN_SET_THIS)
	{
		//通过按钮的坐标重新调整编辑框到居中位置
		for (int i = 0; i < 2; ++i)
			SEditAdjustRect(bti->hSEdit[i], isNewLine, SE_HCETER);
     }
	
  
}

VOID BtnSetRectAngle(HWND hBtn, BYTE w, BYTE h)
{
	CtrlSetRectAngle(hBtn, w, h, offsetof(BtnInfo, angle));
}

HWND BtnAttachSEdit(HWND hBtn, INT idx, HWND hSEidt)
{
	BtnInfo *bti = (BtnInfo *)CtrlGetInfo(hBtn);

	HWND hSEditOld = bti->hSEdit[idx];

	if (hSEditOld)
		SEditDestroy(hSEditOld);
	
	bti->hSEdit[idx] = hSEidt;
	return hSEditOld;
}

void BtnOnMouseMove(HWND hBtn,UINT nFlags, POINTS point)
{
	static TRACKMOUSEEVENT tme = { sizeof(TRACKMOUSEEVENT),0,0,4 };
	BtnInfo *bti = (BtnInfo *)CtrlGetInfo(hBtn);

	tme.hwndTrack = hBtn;

	if (bti->act != BTN_MS_HOVER && bti->act != BTN_MS_DOWN && BTN_MS_UP)
	{
		tme.dwFlags = TME_HOVER | TME_LEAVE;
		TrackMouseEvent(&tme);
	}
}


void BtnOnMouseHover(HWND hBtn, UINT nFlags, POINTS point)
{
	BtnInfo *bti = (BtnInfo *)CtrlGetInfo(hBtn);

	if (bti->act == BTN_DEFAULT)
	{
		//InvalidateRect(hBtn, 0, FALSE);
	}
	else
	{
		bti->act = BTN_MS_HOVER;
		InvalidateRect(hBtn, 0, FALSE);
	}

}

void BtnOnMouseLeave(HWND hBtn, UINT nFlags, POINTS point)
{
	BtnInfo *bti = (BtnInfo *)CtrlGetInfo(hBtn);

	if (bti->act == BTN_DEFAULT)
	{
		//InvalidateRect(hBtn, 0, FALSE);
	}
	else
	{
		bti->act = BTN_MS_LEAVE;
		InvalidateRect(hBtn, NULL, FALSE);
	}

}

void BtnOnMouseUp(HWND hBtn,UINT nFlags, POINTS point)
{
	BtnInfo *bti = (BtnInfo *)CtrlGetInfo(hBtn);

	if (bti->act == BTN_DEFAULT)
	{
		//InvalidateRect(hBtn, 0, FALSE);
	}
	else
	{
		bti->act = BTN_MS_UP;
		InvalidateRect(hBtn, NULL, FALSE);
	}

}
void BtnOnMouseDown(HWND hBtn,UINT nFlags, POINTS point)
{
	BtnInfo *bti = (BtnInfo *)CtrlGetInfo(hBtn);

	if (bti->act == BTN_DEFAULT)
	{
		//InvalidateRect(hBtn, 0, FALSE);
	}
	else
	{
	
		 bti->act = BTN_MS_DOWN;
		 InvalidateRect(hBtn, NULL, FALSE);
	}
	 
}

void BtnOnSize(HWND hBtn, UINT nType, INT nWidth, INT nHeight)
{
	BtnInfo *bti = (BtnInfo *)CtrlGetInfo(hBtn);

	if (bti->dcClient == NULL)
	{
		HDC hDC = GetDC(hBtn);
		bti->dcClient = CreateCompatibleDC(hDC);
		ReleaseDC(hBtn, hDC);
	}

	CtrlAdjustClientDC(hBtn, NULL, offsetof(BtnInfo, dcClient));
}
 
void RoundRectPath(GraphicsPath &path,INT x, INT y, INT width, INT height, INT cornerX, INT cornerY)
{
	INT elWid = 2 * cornerX;
	INT elHei = 2 * cornerY;	

	path.AddArc(x, y, elWid, elHei, 180, 90); // 左上角圆弧
	path.AddLine(x + cornerX, y, x + width - cornerX, y); // 上边

	path.AddArc(x + width - elWid, y, elWid, elHei, 270, 90); // 右上角圆弧
	path.AddLine(x + width, y + cornerY, x + width, y + height - cornerY);// 右边

	path.AddArc(x + width - elWid, y + height - elHei, elWid, elHei, 0, 90); // 右下角圆弧
	path.AddLine(x + width - cornerX, y + height, x + cornerX, y + height); // 下边

	path.AddArc(x, y + height - elHei, elWid, elHei, 90, 90);
	path.AddLine(x, y + cornerY, x, y + height - cornerY);

}


void BtnOnDrawItem(LPDRAWITEMSTRUCT pDraw)
{
	BtnInfo *bti = (BtnInfo *)CtrlGetInfo(pDraw->hwndItem);

	if (bti->dcClient == NULL)
		return;
	if (bti->brBg[bti->act] == NULL)
		return;	

	RECT *rc = &pDraw->rcItem;
   
	LONG w = rc->right - rc->left;
	LONG h = rc->bottom - rc->top;

	BitBlt(bti->dcClient, rc->left, rc->top, w, h, pDraw->hDC, rc->left, rc->top, SRCCOPY);
	
	Graphics * g  = Graphics::FromHDC(bti->dcClient);
	GraphicsPath gp;

	g->SetSmoothingMode(SmoothingModeAntiAlias);	
	RoundRectPath(gp, rc->left, rc->top,w,h, bti->angle[0], bti->angle[1]);
	g->FillPath(bti->brBg[bti->act], &gp);

	
	if ( bti->bDrawTxt )
	{	
		LPTSTR txt = BtnGetText(pDraw->hwndItem, BTN_SET_THIS);
        /*
			SetTextCharacterExtra(pDraw->hDC, 2);
			SetBkMode(pDraw->hDC, TRANSPARENT);
			SetTextColor(pDraw->hDC, bti->color);
			SetTextColor(pDraw->hDC, RGB(255,0,0));
			DrawText(pDraw->hDC, txt, -1, &pDraw->rcItem, CtrlLayoutToDrawText(bti->alm) | DT_SINGLELINE);
		 */
		
		Font ft(pDraw->hDC, CtrlGetFont(pDraw->hwndItem));
		RectF rf(rc->left,rc->top,rc->right - rc->left,rc->bottom - rc->top);
		StringFormat fmt; 
		DWORD dwLayout = CtrlLayoutToStringAlignment(bti->alm);	
		fmt.SetAlignment(StringAlignment(dwLayout & 0xff));
		fmt.SetLineAlignment(StringAlignment(dwLayout >> 8 & 0xff));
		g->SetTextRenderingHint(TextRenderingHintAntiAliasGridFit);
		g->DrawString(txt, -1, &ft, rf, &fmt, bti->brTxt);
		free(txt);
	}
	BitBlt(pDraw->hDC, rc->left, rc->top, w, h, bti->dcClient, rc->left, rc->top, SRCCOPY);

	delete g;


}

///////////////////////////////////////////////////////////////////////////
HWND LinkCreate(HWND wParent, INT x, INT y, INT w, INT h, LPCTSTR txt, INT nID)
{
	HWND link = SEditCreate(wParent, x, y, w, h, txt, nID);
	SEditInfo *sei = (SEditInfo*)CtrlGetInfo(link);
	LinkInfo *lki = (LinkInfo *)realloc(sei, sizeof(LinkInfo));
	memset((SEditInfo*)lki + 1, 0, sizeof(LinkInfo) - sizeof(SEditInfo));

	lki->type |= CTRL_LINK;
	lki->act = LINK_MS_LEAVE;

	SetWindowLong(link, GWL_USERDATA, (LONG)lki);
	HideCaret(link);
	DestroyCaret();
	return link;
}

void  LinkDestroy(HWND seLink)
{
	LinkInfo *lki = (LinkInfo *)CtrlGetInfo(seLink);
  if(lki->url)
	free((void*)lki->url);
}


void LinkShowFontOther(HWND seLink, BOOL bUnderline, BOOL bItalic)
{
	 
	LinkInfo *lki = (LinkInfo *)CtrlGetInfo(seLink);
	HFONT hFont = (HFONT)GetCurrentObject(lki->hDC, OBJ_FONT);
	LOGFONT lgFont;
	GetObject(hFont, sizeof(lgFont), &lgFont);


	lgFont.lfUnderline = bUnderline;
	lgFont.lfItalic = bItalic;

	hFont = CreateFontIndirect(&lgFont);
	SendMessage(seLink, WM_SETFONT, (WPARAM)hFont, TRUE);
	RECT r;
	SendMessage(seLink, EM_GETRECT, 0, (LPARAM)&r);
	//重新调整矩形
	SEditAdjustRect(seLink, FALSE, SE_FIXED_LT);
}

void LinkSetTextColor(HWND seLink, INT idx, COLORREF color)
{
	LinkInfo *lki = (LinkInfo *)CtrlGetInfo(seLink);
	lki->txtColor[idx] = color;
}

void LinkSetURL(HWND seLink, LPCTSTR url)
{
	LinkInfo *lki = (LinkInfo *)CtrlGetInfo(seLink);
	size_t len = _tcslen(url);
	lki->url = (LPTSTR)calloc(1, sizeof(TCHAR) * len + sizeof(TCHAR));

	_tcscpy(lki->url, url);
}

void LinkOnMouseMove(HWND seLink, UINT nFlags, POINTS point)
{
	static TRACKMOUSEEVENT tme = { sizeof(TRACKMOUSEEVENT),0,0,10 };
	LinkInfo *lki = (LinkInfo *)CtrlGetInfo(seLink);

	tme.hwndTrack = seLink;
	//进入执行一次hover就行,离开的时候再设置回来
	if (lki->act == LINK_MS_HOVER)
	{
		tme.dwFlags = TME_CANCEL | TME_HOVER;
		TrackMouseEvent(&tme);
	}
	else
	{
		tme.dwFlags = TME_HOVER | TME_LEAVE;
		TrackMouseEvent(&tme);
	}
	 
}


void LinkOnMouseHover(HWND seLink, UINT nFlags, POINTS point)
{
	LinkInfo *lki = (LinkInfo *)CtrlGetInfo(seLink);

	if (lki->act == LINK_DEFAULT)
	{
		//InvalidateRect(seLink, 0, FALSE);
	}
	else
	{
		LinkShowFontOther(seLink, 1, 1);
		lki->act = LINK_MS_HOVER;
		//InvalidateRect(seLink, 0, FALSE);
	}

}

void LinkOnMouseLeave(HWND seLink, UINT nFlags, POINTS point)
{
	LinkInfo *lki = (LinkInfo *)CtrlGetInfo(seLink);

	if (lki->act == LINK_DEFAULT)
	{
		//InvalidateRect(seLink, 0, FALSE);
	}
	else
	{
		LinkShowFontOther(seLink, 0, 0);
		lki->act = LINK_MS_LEAVE;
		//InvalidateRect(seLink, 0, FALSE);
	}

}

void LinkOnMouseUp(HWND seLink, UINT nFlags, POINTS point)
{
	LinkInfo *lki = (LinkInfo *)CtrlGetInfo(seLink);


	if (lki->act == LINK_DEFAULT)
	{
		//InvalidateRect(seLink, 0, FALSE);
	}
	else
	{
		lki->act = LINK_MS_UP;
		//InvalidateRect(seLink, 0, FALSE);
		//打开超链接
		if(lki->url)
		   ShellExecute(seLink, _T("open"), lki->url,_T(""),_T(""), SW_SHOW );
	}

}
void LinkOnMouseDown(HWND seLink, UINT nFlags, POINTS point)
{
	LinkInfo *lki = (LinkInfo *)CtrlGetInfo(seLink);
	 

	if (lki->act == LINK_DEFAULT)
	{
		//InvalidateRect(seLink, 0, FALSE);
	}
	else
	{
		lki->act = LINK_MS_DOWN;
		//InvalidateRect(seLink, 0, FALSE);
	}

}

HBRUSH LinkOnCtlColor(HWND seLink, HDC hDC)
{
	LinkInfo *lki = (LinkInfo *)CtrlGetInfo(seLink);
	//设置字体背景透明
	SetBkMode(hDC, TRANSPARENT);

	//设置字体颜色
	SetTextColor(hDC, lki->txtColor[lki->act]);
	
	return (HBRUSH)GetStockObject(NULL_BRUSH);
}
