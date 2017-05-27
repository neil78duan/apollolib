@set workDir=%CD%
@echo off

SETLOCAL ENABLEDELAYEDEXPANSION 

@set in_filelist=tmpsvnout.txt 
svn info > %in_filelist%


@for /f "delims=" %%i in ('findstr "Revision:" %in_filelist%') do (
	@echo SvnInfo %%i
)

del %in_filelist%
@rem pause