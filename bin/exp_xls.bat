@set workDir=%CD%

SETLOCAL ENABLEDELAYEDEXPANSION 

@if "%1" == "" (
	echo "usage: exp_xls.bat file-list.txt input-excel-path output-text-path [encode-type(gbk|utf8)] "
	goto :EOF 
)

@if "%2" == "" (
	echo "usage: exp_xls.bat file-list.txt input-excel-path output-text-path [encode-type(gbk|utf8)] "
	goto :EOF 
)

@if "%3" == "" (
	echo "usage: exp_xls.bat file-list.txt input-excel-path output-text-path [encode-type(gbk|utf8)] "
	goto :EOF 
)


@set encodeType=gbk
@if "%4" == "" goto :set_encode_end 
@set encodeType=%4
:set_encode_end

@set out_filelist=%1
@set inputPath=%2
@set outputPath=%3

@if  not exist %outputPath% (
    mkdir %outputPath%
)

@if  not exist %inputPath% (
     echo "input path not exist "
	 goto :EOF
)


cd %workDir%

@rem exoprt lua version file , if you don't like to export remark id 
call getSvnVer.bat  %inputPath% > %outputPath%\version.txt
@rem --end

@rem set READEXCL=python XLStoTxt2.py
@set READEXCL=python XlsToTxt_python3.py


@for /f "delims=" %%i in ('dir /b/a "%inputPath%\*.xlsx"') do (
	@echo " begin %%i"
	%READEXCL% %inputPath% %%i %outputPath% %encodeType%
	@if %errorlevel% NEQ 0 goto ERROR
	@echo " end %%i"
)

@echo "excel to txt successs!!!!!!!!!!"
del %out_filelist%
@for /f "delims=" %%i in ('dir /b/a "%outputPath%\*.txt"') do (
	@echo %%i >> %out_filelist%
)

@rem @for /f "tokens=1-2" %%i in (%in_filelist%) do (
@rem 	@echo " begin %%i"
@rem 	%READEXCL% %inputPath%/%%i  %outputPath%/%%i.txt %encodeType%
@rem 	@if %errorlevel% NEQ 0 goto ERROR
@rem  )

@goto :EOF

:ERROR
@echo EXPORT ERROR
@pause
@exit /B 1

