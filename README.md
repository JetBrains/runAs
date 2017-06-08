# runAs

[<img src="http://teamcity.jetbrains.com/app/rest/builds/buildType:(id:TeamCityPluginsByJetBrains_TeamCityRunAs_CltForWindows)/statusIcon.svg"/>](http://teamcity.jetbrains.com/viewType.html?buildTypeId=TeamCityPluginsByJetBrains_TeamCityVSTestTestAdapter_Build) [<img src="https://www.nuget.org/Content/Logos/nugetlogo.png" height="18">](https://www.nuget.org/packages/JetBrains.runAs/) 

The _runAs_ CLT provides the ability to run a Windows process under the specified Windows user's account. To use with [TeamCity](https://www.jetbrains.com/teamcity/) see, the [runAS plugin](https://github.com/JetBrains/teamcity-runas-plugin).

Download [x64](https://teamcity.jetbrains.com/httpAuth/app/rest/builds/buildType:TeamCityPluginsByJetBrains_TeamCityRunAs_CltForWindows,pinned:true,status:SUCCESS,tags:release/artifacts/content/bin/x64/JetBrains.runAs.exe) [x86](https://teamcity.jetbrains.com/httpAuth/app/rest/builds/buildType:TeamCityPluginsByJetBrains_TeamCityRunAs_CltForWindows,pinned:true,status:SUCCESS,tags:release/artifacts/content/bin/x86/JetBrains.runAs.exe) latest release of _runAs_ CLT.

## Command line arguments

| Argument | Description | Optional | Default value|
| ------------- |:-------------:|:-------------:|:-------------:|
| -u: | *"user"* or *"domain\user"* or *"user@domain"* |||
| -p: | user's password | X | empty |
| -w: | working directory | X | empty |
| -b: | base number for exit code | X | -100000 |
| -e: | set an environment variable in the format "name=value" | X | |
| -l: | logging level (*debug*\|*normal*\|*errors*\|*off*) | X | *normal* |
| -il: | [integrity level](https://github.com/JetBrains/runAs/wiki#windows-integrity-mechanism) (*auto*\|*untrusted*\|*low*\|*medium*\|*medium_plus*\|*high*) | X | *auto* |
| -s: | show mode for the created process *hide* or *normal* or *show* | X | *hide* |
| -t: | self-testing, returns exit codes: **32** ready to work in the 32-bit mode, **64** ready to work in the 64-bit mode, **1** invoker has no administrative privileges when running under the Windows service, **2** invoker has no SeAssignPrimaryTokenPrivilege privilege (replace a process-level token) when running under the Windows service, **3** invoker has no SeTcbPrivilege privilege (act as part of the operating system) when running under the Windows service | | |
| -i: |- set to **off** when environment variables should not be inherited from the parent process<br/>- set **on** when the environment variables should be inherited from the  parent process<br/>- set to **auto** when some environment variables should be inherited from the parent process| X | *auto* |
| -c: | text file, containing any configuration arguments | | |
| executable | executable file | | |
| command_line_args | command line arguments, the maximum total length of *executable* and *command_line_args* is 1024 characters | X | empty |

For example:

`JetBrains.runAs.exe -p:MyPassword -u:SomeDomain\SomeUserName WhoAmI.exe /ALL`

Or another example that does the same thing:

`JetBrains.runAs.exe -p:MyPassword -c:MyConfig.txt`

where _"MyConfig.txt"_ is a text file containing the following lines:
```
-u:SomeDomain\SomeUserName
WhoAmI.exe
/ALL
```

## How it works

The _runAs_ tool uses one of the three ways to create a process which are provided by Windows API:

1. The Windows API call [_CreateProcessAsUser_](https://msdn.microsoft.com/en-us/library/windows/desktop/ms682429(v=vs.85).aspx) to create process as a specified user directly and the integrity level can be elevated.

2. The Windows API call [_CreateProcessWithLogonW_](https://msdn.microsoft.com/en-us/library/windows/desktop/ms682431(v=vs.85).aspx) to create process as a specified user via a dedicated logon service and the integrity level cannot be elevated.

3. The Windows API call [_CreateProcessWithTokenW_](https://msdn.microsoft.com/en-us/library/windows/desktop/ms682434(v=vs.85).aspx) to create process as a specified user via a dedicated logon service and try changing the integrity level.

To use (1), the caller **needs _SE_ASSIGNPRIMARYTOKEN_NAME_** privilege to replace a filtered (by Windows core) security access token with a primary (not filtered) security access token with the full set of privileges ( the "High" integrity leve). Also the caller **needs _SE_TCB_NAME_** privilege to act as a part of the operating system. See this [page](https://msdn.microsoft.com/ru-ru/library/windows/desktop/ms682429(v=vs.85).aspx). The _runAs_ tool uses it when working under a Windows service.

To use (2), the caller **needs a logon SID**. See [this](https://msdn.microsoft.com/en-us/library/windows/desktop/ms682431(v=vs.85).aspx). _CreateProcessWithLogonW_ cannot be called from a process that is running under the _"LocalSystem"_ account, because the function uses the logon SID in the caller token, and the token for the _"LocalSystem"_ account does not contain this SID. The _runAs_ tool uses this approach when working in the user session.

![Flowchart](https://github.com/JetBrains/runAs/blob/master/docs/runAs.jpg)
