@echo off
setlocal
cd /d "%~dp0"

set "APP="

if exist "build\Debug\CacheSimulatorLab.exe" set "APP=build\Debug\CacheSimulatorLab.exe"
if not defined APP if exist "build\Release\CacheSimulatorLab.exe" set "APP=build\Release\CacheSimulatorLab.exe"
if not defined APP if exist "build\CacheSimulatorLab.exe" set "APP=build\CacheSimulatorLab.exe"

if defined APP (
    start "" "%APP%"
    exit /b 0
)

echo CacheSimulatorLab.exe was not found.
echo.
echo Build the project once in Qt Creator or install CMake and Qt 6.
echo After the first build, this file will open the executable directly.
pause

endlocal
