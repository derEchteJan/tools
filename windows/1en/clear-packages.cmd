@echo off

:: ----------------------------------------------
set viewdir="C:\Users\unger\Workspace\znx"
set outdir="C:\Users\unger\Desktop\package-export"
set dir1="C:\.conan"
set dir2="%userprofile%\.vna\.conan\data"
set dir3="%userprofile%\.vna\.arti\data"
set dir4="%viewdir%\.venv"
:: ----------------------------------------------

set /p choice=Delete package caches(y/N)?
IF /I %choice% neq y exit /b ::abort

echo deleting %dir1% && del /f /s /q %dir1% > nul && rmdir /s /q %dir1% && mkdir %dir1%
echo deleting %dir2% && del /f /s /q %dir2% > nul && rmdir /s /q %dir2% && mkdir %dir2%
echo deleting %dir3% && del /f /s /q %dir3% > nul && rmdir /s /q %dir3% && mkdir %dir3%
echo deleting %dir4% && del /f /s /q %dir4% > nul && rmdir /s /q %dir4% && mkdir %dir4%