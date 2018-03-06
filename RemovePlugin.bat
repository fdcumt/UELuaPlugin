set CurDir=%~dp0
rmdir /s/q %CurDir%Plugins\LuaPlugin\Binaries
rmdir /s/q %CurDir%Plugins\LuaPlugin\Intermediate
ping -n 2 127.0.0.1>nul
set UE4PATH=E:\UESourceCode\UE4.18.3

"%UE4PATH%\Engine\Build\BatchFiles\GenerateProjectFiles.bat" %CurDir%MyProject.uproject
Pause