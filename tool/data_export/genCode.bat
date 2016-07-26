
@set workDir=%CD%

SETLOCAL ENABLEDELAYEDEXPANSION 

@set encodeType=gbk
@if "%1" == "" goto :set_encode_end 
@set encodeType=%1
:set_encode_end


@set outputPath=outputData
@set inputPath=game_data
@set src_svn_path="svn://192.168.8.55/design/game_data"

@if  not exist %outputPath% (
    mkdir %outputPath%
)

@rem @set inputPath="../../../scheme/策划案/数值建表/配置文件"

@if  not exist %inputPath% (
     svn co %src_svn_path%    %inputPath%
 	@if %errorlevel% NEQ 0 goto ERROR
)
cd %inputPath%
@rem svn up
@if %errorlevel% NEQ 0 goto ERROR

cd %workDir%
@rem svn up
@if %errorlevel% NEQ 0 goto ERROR

@set outputPath=%workDir%/%outputPath%
@set inputPath=%workDir%/%inputPath%
@set READEXCL=xls2txt.py


@for /f "tokens=1-2" %%i in (filelist.txt) do (
	@echo " begin %%i"
	%READEXCL% %inputPath%/%%i  %outputPath%/%%i.txt %encodeType%
	@if %errorlevel% NEQ 0 goto ERROR
)


@goto :EOF

:ERROR
@echo EXPORT ERROR
@pause
@exit /B 1

