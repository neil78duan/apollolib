This module is an internal net connect to nd-server.
==================放到引擎中编译==========================
如何把Ndnet_Runtime添加到unreal代码中:

1 把这个目录拷贝到 unreal工程 UE4\Source\Runtime 目录,并且添加到对应的工程中.
2 添加函数
	FWindowsPlatformMisc::MemoryBarrier()
	{
	}
3 修改unreal的默认编译设置，去掉 “把警告当成错误”
 打开 vc 工程 Programs -> UnrealBuildTool 工程 分别修改 
		IOS\IOSToolChain.cs   (在165行)
		Mac\MacToolChain.cs   (298)
	注释掉 
//Result += " -Wall -Werror";
	如果还有其他平台无法编译,以此雷同
	android : 
		AndroidToolChain.cs  line 900 
		//BaseArguments += " -Werror";

4 在需要的地方引入 NDNet_Runtime

5 包含login_apollo.h 头文件 ,在包含之前选定义 BUILD_AS_THIRD_PARTY


duan 2016		


=========================在工程项目中编译========================

run copy_to_target.bat 
and copy  ndcliForUe4 to you project
