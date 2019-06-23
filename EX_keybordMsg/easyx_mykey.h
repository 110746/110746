#ifndef  EASYX_MYKEYBROD
#define  EASYX_MYKEY
#include "easyx.h"



struct  KeyboardMsg
{
	struct 
	{
		BYTE isDown : 1;  //key键已被按下为1(真)，否则为0(假)
		BYTE isUp   : 1;  //key键已被松开1(真),否则为0(假)
	};

	TCHAR key;  //当前按下的键值
};

/*  获取键盘消息
 *    返回值：返回一个存储键盘消息的KeyboardMsg结构对象
 *
 *	   注意：如果用户按下键不是可打印的字符，KeyboardMsg::key
 *	的值是虚拟键值(例如:VK_LEFT,VK_LSHIFT之类的虚拟键)。如果用
 *	户按下键是可打印的字符，那么KeyboardMsg::key的值是翻译过的
 *	字符(例如1、2、3、a、b、c、<、>、@、#、$)等。
 */
KeyboardMsg WINAPI GetKeyboardMsg();


#endif
