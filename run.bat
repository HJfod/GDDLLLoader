@echo off
cls

chcp 65001

setlocal
call :setESC

if not exist submodules\ (
    git submodule update
)

if not exist build\ (
    mkdir build
)

cd build

echo %ESC%[92m • Generating...%ESC%[0m

del Release\ModLdr.exe

cmake ..

echo.
echo %ESC%[93m • Compiling...%ESC%[0m

msbuild ModLdr.sln /property:Configuration=Release /verbosity:quiet

echo.
echo %ESC%[94m • Running...%ESC%[0m

Release\ModLdr.exe

:setESC
for /F "tokens=1,2 delims=#" %%a in ('"prompt #$H#$E# & echo on & for %%b in (1) do rem"') do (
  set ESC=%%b
  exit /B 0
)
exit /B 0