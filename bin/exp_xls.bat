@set workDir=%CD%

SETLOCAL ENABLEDELAYEDEXPANSION 

@if "%1" == "" (
	echo "usage: exp_xls.bat file-list.txt input-excel-path output-text-path [encode-type(gbk|utf8)] [is-auto-svn-up 0|1]"
	goto :EOF 
)

@if "%2" == "" (
	echo "usage: exp_xls.bat file-list.txt input-excel-path output-text-path [encode-type(gbk|utf8)] [is-auto-svn-up 0|1]"
	goto :EOF 
)

@if "%3" == "" (
	echo "usage: exp_xls.bat file-list.txt input-excel-path output-text-path [encode-type(gbk|utf8)] [is-auto-svn-up 0|1]"
	goto :EOF 
)


@set encodeType=gbk
@if "%4" == "" goto :set_encode_end 
@set encodeType=%4
:set_encode_end

@set in_filelist=%1
@set inputPath=%2
@set outputPath=%3

@if  not exist %outputPath% (
    mkdir %outputPath%
)

@if  not exist %inputPath% (
     echo "input path not exist "
	 goto :EOF
)

@if "%5" == "1" (
	cd %inputPath%
	@svn up
	@if %errorlevel% NEQ 0 goto ERROR	
)

cd %workDir%

@rem exoprt lua version file , if you don't like to export remark id 
call getSvnVer.bat  %inputPath% > %outputPath%\version.txt
@rem --end

@set READEXCL=xls2txt.py


@for /f "tokens=1-2" %%i in (%in_filelist%) do (
	@echo " begin %%i"
	%READEXCL% %inputPath%/%%i  %outputPath%/%%i.txt %encodeType%
	@if %errorlevel% NEQ 0 goto ERROR
)

@goto :EOF

:ERROR
@echo EXPORT ERROR
@pause
@exit /B 1

