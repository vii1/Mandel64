@echo off
copy /y mandel.lbl mon
echo break .main >> mon
"%userprofile%\Aplicaciones\VICE\x64.exe" -autostart-warp +autostart-handle-tde -moncommands mon -keepmonopen mandel.prg