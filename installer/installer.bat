@echo off
setlocal

echo Sunshine installer
set /p oneshot_path=Enter the Oneshot 2016 installation path:
set /p install_path=Enter the path to install Sunshine:

set "TARGET=%install_path%\oneshot.exe"
set "ICON=%install_path%\icon.ico"

if not exist "%install_path%\" mkdir "%install_path%"

echo Copying the files of the original Oneshot
xcopy "%oneshot_path%\*" "%install_path%\" /E /I /Y
if errorlevel 1 echo Warning: xcopy from oneshot_path returned non-zero code.

echo Patching the original game
xcopy "Sunshine\*" "%install_path%\" /E /I /Y
if errorlevel 1 echo Warning: xcopy from Sunshine returned non-zero code.

echo (create .int files)   :: <-- реализуйте здесь если нужно

powershell -NoProfile -Command ^
  "$s=New-Object -ComObject WScript.Shell; $l=$s.CreateShortcut('%USERPROFILE%\Desktop\Sunshine.lnk'); $l.TargetPath='%TARGET%'; $l.IconLocation='%ICON%'; $l.WorkingDirectory=(Split-Path '%TARGET%'); $l.Save()"

powershell -NoProfile -Command ^
  "$s=New-Object -ComObject WScript.Shell; $l=$s.CreateShortcut('%APPDATA%\Microsoft\Windows\Start Menu\Programs\Sunshine.lnk'); $l.TargetPath='%TARGET%'; $l.IconLocation='%ICON%'; $l.WorkingDirectory=(Split-Path '%TARGET%'); $l.Save()"

echo Done!
endlocal
pause
