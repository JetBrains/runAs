@ECHO OFF

IF "%~1" EQU "" GOTO INVALID_ARGS
IF "%~2" EQU "" GOTO INVALID_ARGS
IF "%~3" EQU "" GOTO INVALID_ARGS
IF "%~4" EQU "" GOTO INVALID_ARGS
IF "%~5" NEQ "" GOTO INVALID_ARGS

REM Define OS bitness
"%~dp0x86\JetBrains.runAs.exe" -t -l:errors
SET "EXIT_CODE=%ERRORLEVEL%"

IF %EXIT_CODE% EQU 64 (
    GOTO RUN_AS
)

IF %EXIT_CODE% EQU 32 (
    GOTO RUN_AS
)

ECHO.
IF %EXIT_CODE% EQU 1 ECHO [91mInvoker has no administrative privileges, when running under the Windows service.
IF %EXIT_CODE% EQU 2 ECHO [91mInvoker has no SeAssignPrimaryTokenPrivilege (Replace a process-level token), when running under the Windows service.
IF %EXIT_CODE% EQU 3 ECHO [91mInvoker has no SeTcbPrivilege (Act as part of the operating system), when running under the Windows service.
EXIT /B %EXIT_CODE%

:RUN_AS
IF %EXIT_CODE% EQU 64 (
    SET RUN_AS_PATH="%~dp0x64\JetBrains.runAs.exe"
    IF %3 EQU 64 (
        SET CMD_PATH="cmd.exe"		
    ) ELSE (
        SET CMD_PATH="%windir%\SysWOW64\cmd.exe"
    )
) ELSE (
    SET RUN_AS_PATH="%~dp0x86\JetBrains.runAs.exe"
    SET CMD_PATH="cmd.exe"
)

REM Run as user

%RUN_AS_PATH% -i:auto -l:errors "-p:%~4" "-c:%~1" "-c:%~2"

SET "EXIT_CODE=%ERRORLEVEL%"
ECHO.
IF %EXIT_CODE% EQU -10000 ECHO [91mUnknown error occurred.[0m
IF %EXIT_CODE% EQU -10001 ECHO [91mInvalid usage of the tool.[0m
IF %EXIT_CODE% EQU -10002 ECHO [91mSecurity error occurred.[0m
IF %EXIT_CODE% EQU -10003 ECHO [91mWIN32 API error occurred.[0m

EXIT /B %EXIT_CODE%

:INVALID_ARGS
ECHO [91mInvalid arguments.[0m
ECHO [91mUsage: runAs.cmd settings_file_name command_file_name bitness password[0m
EXIT -1