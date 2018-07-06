@echo off

if "%~1" equ "" goto INVALID_ARGS
@echo Important: Administrative privileges are required to run this script

set tempDirectory="%temp%\%random%"
powershell -ExecutionPolicy ByPass -File %~dp0grantAccess.ps1 -username "%1" -tempDirectory "%tempDirectory%"
rd %tempDirectory% /S /Q
exit /b 0

:INVALID_ARGS
@echo Invalid arguments.
@echo Usage: runGrantAccess.cmd username
@echo   where username is the user name of a local Windows user account or the user name of a Windows domain user account in the format: domain\username
@exit /b -1