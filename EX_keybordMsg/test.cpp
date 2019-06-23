// vs2010_easyx.cpp : 定义控制台应用程序的入口点。
//

#include <stdio.h>
#include "easyx.h"
#include "easyx_mykey.h"

 
int _tmain(int argc, _TCHAR* argv[])
{
	initgraph(640, 480,SHOWCONSOLE );   // 创建绘图窗口，大小为 640x480 像素
  
	while(true)
	{ 	 
		KeyboardMsg km = GetKeyboardMsg();

		switch(km.key)
		{
		case  VK_LEFT:
			    if(km.isDown )
				  _tprintf(_T("LEFT down\n"));
				if(km.isUp)
					_tprintf(_T("LEFT up\n"));
			   break;
		case  VK_UP:
			if(km.isDown )
				_tprintf(_T("UP down\n"));
			if(km.isUp)
				_tprintf(_T("UP up\n"));
			break;
		case  VK_RIGHT:
			if(km.isDown )
				_tprintf(_T("RIGHT down\n"));
			if(km.isUp)
				_tprintf(_T("RIGHT up\n"));
			break;
		case  VK_DOWN:
			if(km.isDown )
				_tprintf(_T("DOWN down\n"));
			if(km.isUp)
				_tprintf(_T("DOWN up\n"));
			break;

		default:			
			if(km.isDown)
				_tprintf(_T("%c down\n"),km.key);
			if(km.isUp)
				_tprintf(_T("%c up\n"),km.key);
		}
		 
		
	 Sleep(10);
	}
	 
	   
	  
	_getch();              // 按任意键继续
	closegraph(); 
	return 0;

}



 


