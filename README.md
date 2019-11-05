# ProcessHook
	通过内存写入实现模块隐藏，实现IAT Hook(对应于MessageBox)和Inline Hook(对应于sum函数)

	该项目在vs2019以及x86选项下编译通过


目录说明:

	ProcessHook.sln: vs2019工程文件，可直接双击打开项目
	ProcessHook/ProcessHook.vcxproj: vs2019工程文件
	ProcessHook/ProcessHook.vcxproj.filters: vs2019工程文件

	ProcessHook/ProcessHook.cpp: 入口函数WinMain，实现简易窗口
	ProcessHook/PETools.cpp: PE相关函数
	ProcessHook/InjectModule.cpp: 模块注入(需修改被注入程序的地址)
	ProcessHook/IATHook.cpp: IAT Hook
	ProcessHook/InlineHook.cpp: Inline Hook

	ProcessHook/Test.cpp: 被注入程序的源码，包含MessageBox和sum函数
						  (仅在Debug模式下编译通过才能被正确Hook，重新编译后需要修改InlineHook.cpp文件里sum函数的rva)
	ProcessHook/Test.exe: 被注入程序

	