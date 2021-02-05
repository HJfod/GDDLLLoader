@echo off

set INP_NAME=GDDLLLoader/dllmain.cpp cappuccino-sdk\incl\cocos2dx\custom\Sprites\CCMenuItemSpriteExtra\CCMenuItemSpriteExtra.cpp
set DLL_NAME=ModLdr.dll
set EXE_NAME=OneTimeRunner.exe
set COM_PARM=-luser32 -lDbghelp -lmsvcrt -lmsvcmrt -llibMinHook-x86-v141-mt -lnfd -lShlwapi -lWtsApi32 -m32 -lshell32 -lole32 -O3 -lcomdlg32 -lWs2_32 -lcappuccino-sdk/lib/cappuccino -Icappuccino-sdk/incl/cocos2dx -Icappuccino-sdk/incl/cocos2dx/include -Icappuccino-sdk/incl/cocos2dx/kazmath/include -Icappuccino-sdk/incl/cocos2dx/platform/android -Icappuccino-sdk/incl/CocosDenshion/android -Icappuccino-sdk/incl/CocosDenshion/include -Icappuccino-sdk/incl/cocos2dx/platform/win32 -Icappuccino-sdk/incl/cocos2dx/platform/third_party/win32/OGLES
set RUN_ARGS=""
set DLL_COMP=1
set RUN_TRUE=1
set PUB_LISH=0
set SKI_PRUN=0

FOR %%A IN (%*) DO (
    IF "%%A"=="-a" set RUN_ARGS=""
    IF "%%A"=="-r" set DLL_COMP=0
    IF "%%A"=="-c" set RUN_TRUE=0
    IF "%%A"=="-p" set PUB_LISH=1
    IF "%%A"=="-d" set SKI_PRUN=1
    IF "%%A"=="-i" goto comins
    IF "%%A"=="-u" goto comuins
)

if %DLL_COMP%==1 (
    del "%DLL_NAME%"
    echo Compiling DLL...
    clang -std=c++17 -Wno-macro-redefined -Wno-microsoft-enum-forward-reference -shared %COM_PARM% -o %DLL_NAME% %INP_NAME%
    if %errorlevel%==0 (echo Compiled DLL!) else (goto error)
)
if %SKI_PRUN%==1 (goto done)

echo Compiling Runner...
clang++ runner.cpp -o %EXE_NAME% -std=c++17 -m32 -luser32 -lAdvapi32 -lWtsApi32
if %errorlevel%==0 (
    echo Compiled Runner!
    if %PUB_LISH%==1 (
        goto publish
    )
    if %RUN_TRUE%==1 (
        echo.
        %EXE_NAME%
    )
) else (goto error)
goto done

:error
echo Compile error, see output above
goto done

:publish
echo Publishing...
md releases
xcopy /y %EXE_NAME% releases\%EXE_NAME%*
xcopy /y %DLL_NAME% releases\%DLL_NAME%*
xcopy /y MinHook.x86.dll releases\MinHook.x86.dll*
goto done

:comins
echo Compiling Installer...
clang++ installer.cpp -o install.exe -std=c++17 -m32 -luser32 -lWtsApi32 -lAdvapi32
echo Compiled Installer
goto done

:comuins
echo Compiling Uninstaller...
clang++ installer.cpp -o uninstall.exe -std=c++17 -m32 -luser32 -lWtsApi32 -lAdvapi32 -DUNINSTALL
echo Compiled Installer
goto done

:done