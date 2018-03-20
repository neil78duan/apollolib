@echo "===========begin build ...===========

@set apo_dir=%CD%


@SET VS_BIN=%VS140COMNTOOLS%
@set VS_BIN=%VS_BIN:Tools=IDE%
@set vsbuild="%VS_BIN%devenv"
@SET builder=%vsbuild%


cd vs
%builder% "vsApo.sln" /rebuild "debug|x64" 
@if %errorlevel% NEQ 0  goto ERROR


@cd %apo_dir%

@rem exit script
@echo " Build project success "
@exit /B 0


:ERROR
@cd "%apo_dir%"
@echo "build ERROR "
@pause