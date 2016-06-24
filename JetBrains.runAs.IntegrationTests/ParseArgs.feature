Feature: RunAs parses command line argument

Scenario: RunAs returns -201 exit code and shows error message when user runs without user name arg
	Given I have appended the file command.cmd by the line WhoAmI.exe
	And I've added the argument -p:aaa
	And I've added the argument command.cmd
	When I run RunAs tool
	Then the exit code should be -100001
	And the errors should contain:
	|                                         |
	| Error:.*"user_name" should not be empty |	

Scenario: RunAs returns -201 exit code and shows error message when user runs without execurable arg
	Given I have appended the file command.cmd by the line WhoAmI.exe
	And I've added the argument -u:RunAsTestUser
	And I've added the argument -p:aaa
	When I run RunAs tool
	Then the exit code should be -100001
	And the errors should contain:
	|                                          |
	| Error:.*"executable" should not be empty |

Scenario: RunAs returns -201 exit code and shows error message when user add cmd args not in the end via config file for args
	Given I have appended the file command.cmd by the line @echo %1 %2
	And I have appended the file args.txt by the line -p:aaa
	And I have appended the file args.txt by the line command.cmd
	And I have appended the file args.txt by the line hello
	And I have appended the file args.txt by the line "world !!!"
	And I've added the argument -c:args.txt	
	And I've added the argument -u:RunAsTestUser	
	When I run RunAs tool
	Then the exit code should be -100001

Scenario: RunAs returns -201 exit code and shows error message when user add cmd args not in the
	Given I have appended the file command.cmd by the line @echo %1 %2
	And I've added the argument -u:RunAsTestUser	
	And I've added the argument -c:args.txt	
	And I've added the argument hello
	And I've added the argument command.cmd
	And I've added the argument "world !!!"
	And I've added the argument -p:aaa
	When I run RunAs tool
	Then the exit code should be -100001

Scenario Outline: RunAs returns error exit code with specified exit code base
	Given I have appended the file command.cmd by the line WhoAmI.exe
	And I've added the argument -p:aaa
	And I've added the argument -b:<exitCodeBase>
	And I've added the argument command.cmd
	When I run RunAs tool
	Then the exit code should be <exitCode>
		
Examples:
	| exitCodeBase | exitCode |
	| -200         | -201     |
	| -0           | -1       |
	| 200          | 201      |

Scenario Outline: User runs the command whith invalid arg for the inherited environment
	Given I have appended the file command.cmd by the line @echo TestEnvVar=%TestEnvVar%
	And I've defined the TestEnvVar environment variable by the value TestValue
	And I've added the argument -u:RunAsTestUser
	And I've added the argument -p:aaa
	And I've added the argument -i:<inhetritEnvironment>
	And I've added the argument command.cmd
	When I run RunAs tool
	Then the exit code should be -100001
	And the errors should contain:
	|                             |
	| Error:.*Invalid argument "i" |

Examples:
	| inhetritEnvironment |
	| t                   |
	| f                   |
	| trueFalse           |