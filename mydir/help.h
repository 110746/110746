#pragma once
#include <Windows.h>
#include <tchar.h>

LPCTSTR sHelp = _T("\n\n显示目录中文件的信息。\n")
				_T("    作者:群主隔壁\n\n")
					_T("MYDIR [[盘符:][路径]] ")
					_T("[[[/P][-P]] [要显示的列名[*]]] ")
					_T("[[[/F][-F]][扩展名通配符][文件属性]] \n\n")

					_T("   [盘符:][路径]\n")
					_T("        指定要列出文件信息的盘符或目录。\n\n")

					_T("   /P    指定要显示的列名(不区分大小写,列名之间用逗号分隔)。\n")
					_T("         FN 文件名       TY 类型\n")
					_T("         SZ 大小         AB 属性\n")
					_T("         CT 创建时间     AT 访问时间\n")
					_T("         WT 修改时间                \n\n")

					_T("   /F    过滤属性或扩展名(不区分大小写,属性之间用逗号分隔)。\n")
					_T(" 属性    D 目录          R 只读文件 \n")
                    _T("         H 隐藏文件      A 准备存档的文件\n")
					_T("         S 系统文件      I 无内容索引文件\n")
					_T("         L 重新分析点    O 脱机文件\n")
					_T("         E 加密的文件            \n\n\n")

                    _T("   /P 选项在默认情况下会根据屏幕缓冲区的大小来自动调整每列的长度，\n")
                    _T("可以在其属性后面加上*号，从而避免自动根据屏幕缓冲区大小来自动调整。\n")
                    _T("例如: /P fn*,ty,wt*   fn和wt将显示完整的列，而ty则会自动调整列长。\n\n")

                    _T("   假如你需要查看具有特定属性的文件，可以如下组合使用:\n")
                    _T("/F -s-r-h-  将会列出包含：系统、只读、隐藏三种属性的文件。\n")
                    _T("  也许有时候你会对特定扩展名的文件感兴趣，那么你可以如下使用:\n")
                    _T("/F *.*    列出所有文件。  \n")
                    _T("/F *.abc  列出所有具有abc扩展名的文件。\n\n")

                    _T("   为了更精确的找出你需要的文件，你也可以组合使用/P和/F选项，例如:\n")
                    _T("/F ab,fn*,ct,wt /F -r-,*.c,*.cpp,-a-h- \n")
                    _T("以上组合会将会列出所有隐藏和只读的c和cpp文件，并显示它的属性、完整的名字、创建日期和修改日期。 \n\n")
                    
					_T("   当然了，也许以上基本的过滤功能，并不能得到你想要的文件列表，\n")
                    _T("或许你应该通过管道命令(|)将列表传递给其他命令来帮助你。\n\n")
	;
