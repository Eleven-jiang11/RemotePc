@echo off
for /r %%i in (debug,releas,ipch) do rd /s /q "%%i"
for /r %%i in (*.sdf *.VC.db) do del /s /f /q "%%i"
pause