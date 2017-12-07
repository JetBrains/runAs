[<img src="http://jb.gg/badges/official.svg" />](https://confluence.jetbrains.com/display/ALL/JetBrains+on+GitHub) [<img src="http://teamcity.jetbrains.com/app/rest/builds/buildType:(id:TeamCityPluginsByJetBrains_TeamCityRunAs_CltForWindows)/statusIcon.svg"/>](http://teamcity.jetbrains.com/viewType.html?buildTypeId=TeamCityPluginsByJetBrains_TeamCityRunAs_CltForWindows&guest=1) [![NuGet Version and Downloads count](https://buildstats.info/nuget/JetBrains.runAs)](https://www.nuget.org/packages/JetBrains.runAs)

# runAs

The _runAs_ tool provides the ability to run a Windows process under a specified Windows user account. 

## Download

* [x64 runAs tool](https://teamcity.jetbrains.com/guestAuth/app/rest/builds/buildType:TeamCityPluginsByJetBrains_TeamCityRunAs_CltForWindows,pinned:true,status:SUCCESS,branch:master,tags:release/artifacts/content/bin/x64/JetBrains.runAs.exe)

* [x86 runAs tool](https://teamcity.jetbrains.com/httpAuth/app/rest/builds/buildType:TeamCityPluginsByJetBrains_TeamCityRunAs_CltForWindows,pinned:true,status:SUCCESS,tags:release/artifacts/content/bin/x86/JetBrains.runAs.exe)

* [<img src="https://cdn.worldvectorlogo.com/logos/teamcity.svg" height="20" align="center"/> plugin](https://github.com/JetBrains/teamcity-runas-plugin)

## Command line arguments

| Argument | Description | Mandatory | Default value|
| :------------- |:-------------|:-------------:|:-------------:|
| -u: | *"user"* or *"domain\user"* or *"user@domain"* | :black_medium_small_square: ||
| -p: | user's password | | empty |
| -w: | working directory | | empty |
| -b: | base number for exit code | | -100000 |
| -e: | set an environment variable in the format "name=value" | | |
| -l: | logging level (*debug*\|*normal*\|*errors*\|*off*) | | *normal* |
| -il: | [integrity level](https://github.com/JetBrains/runAs/wiki#windows-integrity-mechanism) (*auto*\|*untrusted*\|*low*\|*medium*\|*medium_plus*\|*high*) | | *auto* |
| -s: | show mode for the created process *hide* or *normal* or *show* | | *hide* |
| -t: | self-testing, returns exit codes:<br/><br/>  **32** - ready to work in the 32-bit mode<br/><br/>  **64** - ready to work in the 64-bit mode<br/><br/>  **1** - invoker has no administrative privileges when running under the Windows service<br/><br/>  **2** - invoker has no SeAssignPrimaryTokenPrivilege privilege (replace a process-level token) when running under the Windows service<br/><br/>  **3** - invoker has no SeTcbPrivilege privilege (act as part of the operating system) when running under the Windows service | | |
| -i: |  **off** - when environment variables should not be inherited from the parent process<br/><br/>  **on** - when the environment variables should be inherited from the  parent process<br/><br/>  **auto** - when some environment variables should be inherited from the parent process| | *auto* |
| -c: | text file, containing any configuration arguments | | |
| executable | executable file |  :black_medium_small_square: | |
| command_line_args | command line arguments, the maximum total length of *executable* and *command_line_args* is 1024 characters | | empty |

For example:

```JetBrains.runAs.exe -p:MyPassword -u:SomeDomain\SomeUserName WhoAmI.exe /ALL```

Or another example that does the same thing:

```JetBrains.runAs.exe -p:MyPassword -c:MyConfig.txt```

where _MyConfig.txt_ is a text file containing the following lines:
```
-u:SomeDomain\SomeUserName
WhoAmI.exe
/ALL
```

## How it works

The _runAs_ tool uses one of the three ways provided by the Windows API to create a process:

1. The Windows API call [_CreateProcessAsUser_](https://msdn.microsoft.com/en-us/library/windows/desktop/ms682429(v=vs.85).aspx) to create a process as a specified user directly; the integrity level can be elevated.

2. The Windows API call [_CreateProcessWithLogonW_](https://msdn.microsoft.com/en-us/library/windows/desktop/ms682431(v=vs.85).aspx) to create a process as a specified user via a dedicated logon service; the integrity level cannot be elevated.

3. The Windows API call [_CreateProcessWithTokenW_](https://msdn.microsoft.com/en-us/library/windows/desktop/ms682434(v=vs.85).aspx) to create process as a specified user via a dedicated logon service and try changing the integrity level.

To use (1), the caller **needs _SE_ASSIGNPRIMARYTOKEN_NAME_** privilege to replace a filtered (by Windows core) security access token with a primary (not filtered) security access token with the full set of privileges ( the "High" integrity leve). Also the caller **needs the _SE_TCB_NAME_** privilege to act as a part of the operating system. See this [page](https://msdn.microsoft.com/ru-ru/library/windows/desktop/ms682429(v=vs.85).aspx). The _runAs_ tool uses it when working under a Windows service.

To use (2), the caller **needs a logon SID**. See [this page](https://msdn.microsoft.com/en-us/library/windows/desktop/ms682431(v=vs.85).aspx). _CreateProcessWithLogonW_ cannot be called from a process that is running under the _"LocalSystem"_ account, because the function uses the logon SID in the caller token, and the token for the _"LocalSystem"_ account does not contain this SID. The _runAs_ tool uses this approach when working in the user session.

![Flowchart](https://github.com/JetBrains/runAs/blob/master/docs/runAs.jpg)
