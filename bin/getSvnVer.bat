@set workDir=%CD%
@echo off
SETLOCAL ENABLEDELAYEDEXPANSION 

@set dataPath=%1

cd %dataPath%

@set in_filelist=tmpsvnout.txt 
svn info > %in_filelist%


@for /f "delims=" %%i in ('findstr "Revision:" %in_filelist%') do (
	@set SvnInfo=%%i
)
@rem export to lua format
echo id	svn	desc
echo id	svn	desc
echo 1	%SvnInfo%	"Create by %username% at %DATE:~0,4%-%DATE:~5,2%-%DATE:~8,2% %TIME:~0,2%:%TIME:~3,2%:%TIME:~6,2% Svninfo %SvnInfo%"

del %in_filelist%

cd %workDir%

@echo on

@rem pause