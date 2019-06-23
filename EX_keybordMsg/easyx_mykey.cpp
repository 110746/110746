#include "easyx_mykey.h"

static WNDPROC wndProc;
static KeyboardMsg kMsg ;

static LRESULT CALLBACK ExWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParm)
{
	 switch(message)
	 {
	 case WM_KEYDOWN:
		  *(char*)&kMsg = 5;
		  kMsg.key = (TCHAR)wParam;		 
		 break;
	 case WM_KEYUP:
		  *(char*)&kMsg = 2;
		 break;
	 case WM_CHAR:
		  kMsg.key = (TCHAR)wParam;
		  break;
	 }
	return wndProc(hWnd, message,wParam,lParm);
}

KeyboardMsg WINAPI GetKeyboardMsg()
{
	static bool bOne = true;
	static KeyboardMsg kMsgEmpty;
	KeyboardMsg kRet = kMsg;

	if(bOne)
	{
		HWND hWnd = GetHWnd();
		LONG lProc,lProcOld;
		PVOID p1 = &lProc;
		PVOID p2 = &wndProc;

		wndProc = ExWndProc;
		*(PLONG)p1 = *(PLONG)p2;
		lProcOld = SetWindowLong(hWnd, GWL_WNDPROC, lProc);	    
		*(PLONG)p2 = lProcOld;

		bOne = false;
	}
	///////保证循环事件唯一///////

	if(kMsg.isDown)
		kMsg.isDown = 0;

	if(kMsg.isUp)
	{
		kMsg.isUp = 0;
		kMsg.key = 0;	
		
	}
  
 return kRet;

}
