Feature: Environment's inheritence

Scenario Outline: User runs the command using diferent modes of environment's inheritence
	Given I have appended the file command.cmd by the line @echo TestEnvVar="%TestEnvVar%"
	And I've defined the TestEnvVar environment variable by the value TestValue
	And I've added the argument -u:RunAsTestUser
	And I've added the argument -p:aaa
	And I've added the argument -i:<inhetritEnvironment>
	And I've added the argument command.cmd
	When I run RunAs tool
	Then the exit code should be 0
	And the output should contain:
	|          |
	| <output> |

Examples:
	| inhetritEnvironment | output                 |
	| on                  | TestEnvVar="TestValue" |
	| off                 | TestEnvVar=""          |
	| auto                | TestEnvVar="TestValue" |

Scenario Outline: User can set environmet variable via an argument
	Given I have appended the file command.cmd by the line @echo TestEnvVar="%TestEnvVar%"
	And I've defined the TestEnvVar environment variable by the value TestValue
	And I've added the argument -u:RunAsTestUser
	And I've added the argument -p:aaa
	And I've added the argument -e:TestEnvVar=NewTestValue
	And I've added the argument -i:<inhetritEnvironment>
	And I've added the argument command.cmd
	When I run RunAs tool
	Then the exit code should be 0
	And the output should contain:
	|          |
	| <output> |

Examples:
	| inhetritEnvironment | output                    |
	| on                  | TestEnvVar="NewTestValue" |
	| off                 | TestEnvVar="NewTestValue" |
	| auto                | TestEnvVar="NewTestValue" |

Scenario Outline: RunAs tool overrides predefined environment variables in auto mode
	Given I have appended the file command.cmd by the line @echo <OverridedEnvVar>="%<OverridedEnvVar>%"
	And I've defined the <OverridedEnvVar> environment variable by the value TestValue
	And I've added the argument -u:RunAsTestUser
	And I've added the argument -p:aaa
	And I've added the argument -i:auto
	And I've added the argument command.cmd
	When I run RunAs tool
	Then the exit code should be 0
	And the output should contain:
	|                                   |
	| <OverridedEnvVar>=(?!"TestValue") |

Examples:
	| OverridedEnvVar           |
	| USERDOMAIN                |
	| USERNAME                  |
	| USERPROFILE               |

Scenario: RunAs tool combines PATH environment variable in auto mode, first is target PATH, second is initial PATH
	Given I have appended the file command.cmd by the line @echo MyPath="%PATH%"
	And I've defined the Path environment variable by the value Path 1;path2
	And I've added the argument -u:RunAsTestUser
	And I've added the argument -p:aaa
	And I've added the argument -i:auto
	And I've added the argument command.cmd
	When I run RunAs tool
	Then the exit code should be 0
	And the output should contain:
	|                          |
	| MyPath=".+;Path 1;path2" |