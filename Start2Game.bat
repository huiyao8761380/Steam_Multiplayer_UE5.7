cd /d %~dp0
%1 start "" mshta vbscript:createobject("shell.application").shellexecute("""%~0""","::",,"runas",1)(window.close)&exit

net session >nul 2>&1
if not "%errorLevel%" == "0" (
  echo Oops: This tools must run with administrator permissions!
  echo it will popup the UAC dialog, please click [Yes] to continue.
  echo Set UAC = CreateObject^("Shell.Application"^) > "%temp%\getadmin.vbs"
  echo UAC.ShellExecute "%~s0", "%*", "", "runas", 1 >> "%temp%\getadmin.vbs"
  "%temp%\getadmin.vbs"
  exit /b 2
)
rem E:\UE_4.22\Engine\Binaries\Win64\   UE4Editor.exe
SET EDITOR_EXE_DIR=G:\EpicGames\UE_5.7\Engine\Binaries\Win64
@echo off
SET UE_EXE=%EDITOR_EXE_DIR%\UnrealEditor.exe
SET GAME_EXE=D:\UE4_XiangMu\3_Steam_Multiplayer\UnrealProject\PuzzlePlatforms\PuzzlePlatforms.uproject

set var=0
rem %CD%\KBECoursePro.uproject 
rem E:\UE_4.22\Engine\Binaries\Win64\UE4Editor.exe G:\UE_xiangmu\KBEExamProKBECoursePro.uproject -game
rem 循环开始
:continue
set /a var+=1
start /B "Client" "%UE_EXE%" "%CD%\PuzzlePlatforms.uproject" -game -log
if %var% lss 2 goto continue
rem 循环结束 3.上面的2为 两台客户端
echo 循环执行完毕
