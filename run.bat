@echo off
cls

chcp 65001

setlocal
call :setESC

if not exist submodules\ (
    git submodule update
)

if "%1"=="re" (
    if exist build\ (
        rmdir /s /q build
    )
    
    mkdir build
) else (
    if not exist build\ (
        mkdir build
    )
)

cd build

echo %ESC%[93m • Generating project...%ESC%[0m

del Release\ModLdr.exe

cmake .. -A Win32 -Thost=x86

echo.
echo %ESC%[92m • Compiling library...%ESC%[0m

msbuild ModLdr.sln /p:Configuration=Release /verbosity:quiet /p:PlatformTarget=x86

if not exist Release\ModLdr.dll (
    rem fuck you
    echo %ESC%[91m • somwin went fuwcy wuwcy.... so sowwy.qwq... wil fix soon promis ^>w^<%ESC%[0m
    goto done
)

xcopy /y ..\resources\DL_mods.png Release\DL_mods.png*
xcopy /y ..\resources\DL_folder.png Release\DL_folder.png*

echo.
echo %ESC%[92m • Compiling runner...%ESC%[0m

clang++ ../runner.cpp -o Release/OneTimeRunner.exe -std=c++20 -m32 -luser32 -lAdvapi32 -lWtsApi32 -DNOMSGBOX

if exist Release\OneTimeRunner.exe (
    echo Compiled
) else (
    rem fuck you
    echo %ESC%[91m • Oh nyo?!?1 i cant compiwe the x3 wunnyew.... so sowwy *huggles tightly* ^>w^<%ESC%[0m
    goto done
)

echo.
echo %ESC%[94m • Running...%ESC%[0m

cd Release

OneTimeRunner.exe

goto done



:setESC
for /F "tokens=1,2 delims=#" %%a in ('"prompt #$H#$E# & echo on & for %%b in (1) do rem"') do (
    set ESC=%%b
    exit /B 0
)
exit /B 0

:done