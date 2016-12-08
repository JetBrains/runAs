# runAs

The runAs tool provides an ability to run the Windows process under the specified Windows user's account.

Download [<img src="https://www.nuget.org/Content/Logos/nugetlogo.png" height="18">](https://www.nuget.org/packages/JetBrains.runAs/)

## Command line arguments

| Argument | Description | Optional | Default value|
| ------------- |:-------------:|:-------------:|:-------------:|
| -u:  | *"user"* or *"domain\user"* or *"user@domain"* |||
| -p:   | user's password | X | empty |
| -w: | working directory | X | empty |
| -b: | base number for exit code | X | -100000 |
| -e: | set an environment variable in the format "name=value" | X | |
| -l: | logging level (*debug*\|*normal*\|*errors*\|*off*) | X | *normal* |
| -il: | [integrity level] (https://msdn.microsoft.com/en-us/library/bb625963.aspx)(*auto*\|*untrusted*\|*low*\|*medium*\|*medium_plus*\|*high*) | X | *auto* |
| -s: | show mode for the created process *hide* or *normal* or *show* | X | *hide* |
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

This tool runs commands under specific Windows account and Windows integrity level. Each account has a set of privileges, like a set of abilities. The Windows integrity level is like a filter for abilities. So your account may have administrative privileges, but a _low_ integrity level. In this case most of the privileges will be filtered. For example: administrator in the _Chrome_ browser. Other account might have standard user privileges, but a _high_ integrity level. In this case the most of these privileges are not filtered. For example: some Windows service working under the standard user account.

Windows API provides 2 ways to create a process:

1. Windows API call _CreateProcessAsUser_ creates process as specified user directly and the integrity level can be elevated.

2. Windows API call _CreateProcessWithLogonW_ creates process as specified user via a dedicated logon service and the integrity level can not be elevated.

To use (1), the caller **should have _SE_ASSIGNPRIMARYTOKEN_NAME_** privilege to replace a filtered (by Windows core) security access token with a primary (not filtered) security access token with the full set of privileges ( the "High" integrity leve). Also the caller **should have _SE_TCB_NAME_** privilege to act as part of the operating system. See this [page](https://msdn.microsoft.com/ru-ru/library/windows/desktop/ms682429(v=vs.85).aspx).

To use (2), the caller **should have a logon SID**. See [this](https://msdn.microsoft.com/en-us/library/windows/desktop/ms682431(v=vs.85).aspx).
You cannot call _CreateProcessWithLogonW_ from a process that is running under the _"LocalSystem"_ account, because the function uses the logon SID in the caller token, and the token for the _"LocalSystem"_ account does not contain this SID. As an alternative, use the (1) or use other accounts.
