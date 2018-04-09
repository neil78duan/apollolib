@echo "===========begin build ...===========

@set cur_dir=%CD%

@SET VS_BIN=%VS140COMNTOOLS%
@set VS_BIN=%VS_BIN:Tools=IDE%
@set vsbuild="%VS_BIN%devenv"
@SET builder=%vsbuild%



@set buildConfig=release

@if "%3" == "debug" (
	@set buildConfig=debug
	goto :set_type_end 
)
:set_type_end


cd ..\cfg\cpptmpl

@%builder% "cppTemplate.sln" /rebuild "%buildConfig%|x64" 
@if %errorlevel% NEQ 0  goto ERROR
@cd %cur_dir%

copy /y cppTemplate.exe %1\%2.exe

@rem exit script
@echo " Build project success "
@exit /B 0


:ERROR
@cd "%cur_dir%"
@echo "build ERROR %1 %2"
@exit /B 1