# runAs

## runas .NET Core Global Tool [![NuGet](https://buildstats.info/nuget/dotnet-runas?includePreReleases=true)](https://www.nuget.org/packages/dotnet-runas) [![JetBrains incubator project](http://jb.gg/badges/incubator.svg)](https://confluence.jetbrains.com/display/ALL/JetBrains+on+GitHub)

The _runas_ [.NET Core Global Tool](https://docs.microsoft.com/en-us/dotnet/core/tools/global-tools) allows to run a dotnet process under a specified user account. This tool is compatible with any dotnet CLI commands but the most obvious scenario is running dotnet tests:

- to install the _runas_ tool, run the command line `dotnet tool install -g dotnet-runas`

- run tests under a specified user account like `dotnet runas -u:username -p:password test`

- to uninstall the _runas_ tool, run the command line `dotnet tool uninstall -g dotnet-runas`

For Windows operation system it is possible to specify elevated permissions and [other parameters](https://github.com/JetBrains/runAs/wiki/runAs-tool). 

:construction: For Linux and OSX operation systems _username_ and _password_ paramers are available.

## [<img src="https://cdn.worldvectorlogo.com/logos/teamcity.svg" height="20" align="center"/> plugin](https://github.com/JetBrains/teamcity-runas-plugin) [<img src="http://jb.gg/badges/official.svg" />](https://confluence.jetbrains.com/display/ALL/JetBrains+on+GitHub)

When building, testing, and deploying an application with TeamCity, you may need to run a build under a specific user account, different from the one used to run the build agent. The [teamcity-runas plugin](https://github.com/JetBrains/teamcity-runas-plugin) can be used to run TeamCity build steps under a specified user account on Windows, Mac or Linux.

<img src="https://github.com/JetBrains/teamcity-runas-plugin/blob/master/docs/RunAs.png" width="70%" height="70%"/>

## [runAs tool](https://github.com/JetBrains/runAs/wiki/runAs-tool) for windows [![NuGet](https://buildstats.info/nuget/JetBrains.runAs)](https://www.nuget.org/packages/JetBrains.runAs) [![JetBrains incubator project](http://jb.gg/badges/incubator.svg)](https://confluence.jetbrains.com/display/ALL/JetBrains+on+GitHub)

The [runAs tool](https://github.com/JetBrains/runAs/wiki/runAs-tool) provides the ability to run a Windows process under a specified Windows user account.

### Downloads

* [x64 runAs tool](https://teamcity.jetbrains.com/guestAuth/app/rest/builds/buildType:TeamCityPluginsByJetBrains_TeamCityRunAs_CltForWindows,pinned:true,status:SUCCESS,branch:master,tags:release/artifacts/content/bin/x64/JetBrains.runAs.exe)

* [x86 runAs tool](https://teamcity.jetbrains.com/httpAuth/app/rest/builds/buildType:TeamCityPluginsByJetBrains_TeamCityRunAs_CltForWindows,pinned:true,status:SUCCESS,tags:release/artifacts/content/bin/x86/JetBrains.runAs.exe)

[<img src="http://teamcity.jetbrains.com/app/rest/builds/buildType:(id:TeamCityPluginsByJetBrains_TeamCityRunAs_CltForWindows)/statusIcon.svg"/>](http://teamcity.jetbrains.com/viewType.html?buildTypeId=TeamCityPluginsByJetBrains_TeamCityRunAs_CltForWindows&guest=1)
