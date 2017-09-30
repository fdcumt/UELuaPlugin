set CurDir=%~dp0
rmdir /s/q %CurDir%Plugins\LuaPlugin\Binaries
rmdir /s/q %CurDir%Plugins\LuaPlugin\Intermediate
set UE4PATH=E:\GitHub\MaYunAthena\Athena\UnrealEngine

"%UE4PATH%\Engine\Build\BatchFiles\GenerateProjectFiles.bat" %CurDir%MyProject.uproject
Pause