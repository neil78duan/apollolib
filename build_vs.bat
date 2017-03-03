@echo "===========begin build ...===========
@set cur_dir=%CD%
@set work_dir=%CD%

@SET  VS12_BIN=%VS120COMNTOOLS%
@set VS12_BIN=%VS12_BIN:Tools=IDE%
@set vs12build="%VS12_BIN%devenv"

@SET  builder=%vs12build%


cd tool\apolloEditorMfc
%builder% "apolloParseEditor.sln" /build "release|Win32" 
@if %errorlevel% NEQ 0  goto ERROR

@cd %work_dir%
cd tool\protocolBuilder\protocolBuilderVS
%builder% "protocolBuilderVS.sln" /build "release|Win32" 
@if %errorlevel% NEQ 0  goto ERROR

@cd %work_dir%

@rem exit script
@echo " Build project success "
@exit /B 0


:ERROR
@cd "%cur_dir%"
@echo "build ERROR "
@pause