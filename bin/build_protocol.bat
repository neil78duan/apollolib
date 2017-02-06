
protocolBuilderVS.exe -d ../../resource/msg_config -encode gbk
@if %errorlevel% NEQ 0  goto error

copy /y cpp\* ..\..\src\netMessage

@goto :eof

:ERROR
@set build_err=%errorlevel%
@echo "export protocol error "

@date /T  
@time /T
@pause
@exit /B %build_err%