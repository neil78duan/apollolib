This module is an internal net connect to nd-server.
==================�ŵ������б���==========================
��ΰ�Ndnet_Runtime��ӵ�unreal������:

1 �����Ŀ¼������ unreal���� UE4\Source\Runtime Ŀ¼,������ӵ���Ӧ�Ĺ�����.
2 ��Ӻ���
	FWindowsPlatformMisc::MemoryBarrier()
	{
	}
3 �޸�unreal��Ĭ�ϱ������ã�ȥ�� ���Ѿ��浱�ɴ���
 �� vc ���� Programs -> UnrealBuildTool ���� �ֱ��޸� 
		IOS\IOSToolChain.cs   (��165��)
		Mac\MacToolChain.cs   (298)
	ע�͵� 
//Result += " -Wall -Werror";
	�����������ƽ̨�޷�����,�Դ���ͬ
	android : 
		AndroidToolChain.cs  line 900 
		//BaseArguments += " -Werror";

4 ����Ҫ�ĵط����� NDNet_Runtime

5 ����login_apollo.h ͷ�ļ� ,�ڰ���֮ǰѡ���� BUILD_AS_THIRD_PARTY


duan 2016		


=========================�ڹ�����Ŀ�б���========================

run copy_to_target.bat 
and copy  ndcliForUe4 to you project
