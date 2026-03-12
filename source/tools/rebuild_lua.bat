@echo off
REM rebuild_lua.bat - Quick rebuild script for Lua components
REM Usage: rebuild_lua.bat [force]
REM   force - Delete all .prop files before regenerating

echo ==================================
echo Lua Component Rebuild Workflow
echo ==================================
echo.

REM Check for force flag
if "%1"=="force" (
    echo [0/3] Force regeneration - deleting .prop files...
    del /Q "data\ComponentSystem\Lua_*.prop" 2>nul
    del /Q "data\ComponentSystem\Lua_*.prop.meta" 2>nul
    echo [OK] Old property files deleted
    echo.
)

REM Step 1: Generate C++ wrappers
echo [1/3] Generating C++ component wrappers...
cd /d "%~dp0bin"
LuaIntegrationProject_x64d.exe -video_app null -sound_app null -console_command quit
if %ERRORLEVEL% NEQ 0 (
    echo Generation failed!
    pause
    exit /b 1
)
echo [OK] Generation complete
echo.

REM Step 2: Build solution
echo [2/3] Building solution...
cd /d "%~dp0"

REM Find MSBuild
for /f "usebackq tokens=*" %%i in (`"%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" -latest -requires Microsoft.Component.MSBuild -find MSBuild\**\Bin\MSBuild.exe`) do (
    set MSBUILD=%%i
)

if not defined MSBUILD (
    echo MSBuild not found!
    pause
    exit /b 1
)

"%MSBUILD%" LuaIntegrationProject.sln /p:Configuration=Debug /p:Platform=x64 /m /v:minimal
if %ERRORLEVEL% NEQ 0 (
    echo Build failed!
    pause
    exit /b 1
)
echo [OK] Build complete
echo.

echo ==================================
echo Workflow complete!
echo ==================================
echo.
echo Run bin\LuaIntegrationProject_x64d.exe to test
echo.
echo TIP: Use "rebuild_lua.bat force" to delete all .prop files first
pause