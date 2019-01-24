@set my_dir=%CD%

@SET myTarget=.\ndcliForUe4
@set NDHOME=..\..\ndsdk
@set APOLLOLIB=..\..\apollolib

del /Q /S .\ndcliForUe4

mkdir %myTarget%
cd %myTarget% 
mkdir include 
mkdir include\ndapplib
mkdir src 
mkdir src\login
mkdir src\nd_common
mkdir src\nd_net
mkdir src\login
mkdir src\ndapplib
cd ..

copy NdNetClient.Build.cs %myTarget%\

@rem copy include 
xcopy /s %NDHOME%\include\nd_common 	%myTarget%\include\nd_common\
xcopy /s %NDHOME%\include\nd_crypt	%myTarget%\include\nd_crypt\
xcopy /s %NDHOME%\include\nd_net	%myTarget%\include\nd_net\
xcopy /s %NDHOME%\include\ndcli		%myTarget%\include\ndcli\
copy /y %NDHOME%\include\ndapplib\nd_msgpacket.h	%myTarget%\include\ndapplib\
copy /y %NDHOME%\include\ndapplib\nd_iBaseObj.h	%myTarget%\include\ndapplib\
copy /y %NDHOME%\include\ndapplib\nd_datatransfer.h	%myTarget%\include\ndapplib\
copy /y %NDHOME%\include\ndapplib\ndsingleton.h	%myTarget%\include\ndapplib\
copy /y %NDHOME%\include\ndapplib\readcfg.h	%myTarget%\include\ndapplib\
copy /y %NDHOME%\include\ndlib.h	%myTarget%\include\
copy /y %NDHOME%\include\nd_msg.h	%myTarget%\include\

copy /y %APOLLOLIB%\include\apollo_client.h		%myTarget%\include\
copy /y %APOLLOLIB%\include\apollo_data.h		%myTarget%\include\
copy /y %APOLLOLIB%\include\apollo_errors.h		%myTarget%\include\
copy /y %APOLLOLIB%\include\msg_def.h			%myTarget%\include\
copy /y %APOLLOLIB%\include\_error_def.h		%myTarget%\include\
copy /y %APOLLOLIB%\cli_common\login_apollo.h	%myTarget%\include\
copy /y %APOLLOLIB%\cli_common\apoClientU3d.h	%myTarget%\include\

@rem copy src 
xcopy /s %NDHOME%\src\nd_crypt		%myTarget%\src\nd_crypt\
xcopy /s %NDHOME%\src\ndclient		%myTarget%\src\ndclient\
copy /y %NDHOME%\src\nd_common\src\*.c 	%myTarget%\src\nd_common\
copy /y %NDHOME%\src\nd_net\src\*.c 	%myTarget%\src\nd_net\

copy /y %NDHOME%\src\ndapplib\nd_msgpack.cpp %myTarget%\src\ndapplib\
copy /y %NDHOME%\src\ndapplib\nd_datatransfer.cpp	%myTarget%\src\ndapplib\

copy /y %APOLLOLIB%\cli_common\login_apollo.cpp		%myTarget%\src\login\
copy /y %APOLLOLIB%\cli_common\apoClientU3d.cpp		%myTarget%\src\login\

pause
exit /B 0 

:ERROR
@echo "export protocol error"
@pause 

zu