@echo off

:: ----------------------------------------------
set viewdir="C:\Users\unger\Workspace\znx"
set outdir="C:\Users\unger\Desktop\package-export"
set dir1="C:\.conan"
set dir2="%userprofile%\.vna\.conan\data"
set dir3="%userprofile%\.vna\.arti\data"
set dir4="%viewdir%\.venv\Lib\site-packages"
:: ----------------------------------------------

set /p choice=Export packages(y/N)?
IF /I %choice% neq y exit /b ::abort

mkdir %outdir%

echo compressing %dir1% && cd %dir1%\..\ && tar -czf %outdir%\c-conan.tar .conan
echo compressing %dir2% && cd %dir2%\..\ && tar -czf %outdir%\conan-data.tar data
echo compressing %dir3% && cd %dir3%\..\ && tar -czf %outdir%\arti-data.tar data
echo compressing %dir4% && cd %dir4%\..\ && tar -czf %outdir%\site-packages.tar site-packages

echo bundeling tars
cd %outdir% && tar -czvf packages.tar.gz c-conan.tar conan-data.tar arti-data.tar site-packages.tar && del c-conan.tar conan-data.tar arti-data.tar site-packages.tar