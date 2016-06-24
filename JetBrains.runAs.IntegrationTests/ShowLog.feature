Feature: Show log info

Scenario: User sees header and settings when in the debug log mode and without error
	Given I have appended the file command.cmd by the line echo
	And I've added the argument -u:RunAsTestUser
	And I've added the argument -p:aaa
	And I've added the argument -l:debug
	And I've added the argument command.cmd
	When I run RunAs tool
	Then the exit code should be 0
	And the output should contain:
	|                 |
	| JetBrains RunAs |
	| Settings:       |

Scenario Outline: User does not see header and settings when is not in the debug log mode and without error
	Given I have appended the file command.cmd by the line echo
	And I've added the argument -u:RunAsTestUser
	And I've added the argument -p:aaa
	And I've added the argument -l:<logLevel>
	And I've added the argument command.cmd
	When I run RunAs tool
	Then the exit code should be 0
	And the output should not contain:
	|                 |
	| JetBrains RunAs |
	| Settings:       |
Examples:
	| logLevel |
	| off      |
	| errors   |
	| normal   |


Scenario Outline: User sees header and settings info when in the debug/normal log mode and some error occured
	Given I have appended the file command.cmd by the line echo
	And I've added the argument -u:SomeAsTestUser
	And I've added the argument -p:SomePassword
	And I've added the argument -l:<logLevel>
	And I've added the argument command.cmd
	When I run RunAs tool
	Then the exit code should be 0
	And the output should contain:
	|                 |
	| JetBrains RunAs |
	| Settings:       |	
Examples:
	| logLevel |
	| debug   |
	| normal   |

