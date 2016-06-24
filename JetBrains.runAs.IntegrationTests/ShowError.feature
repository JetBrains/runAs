Feature: Show error when something is wrong

Scenario Outline: User sees header and full error info when unknown user and in the normal/debug log mode
	Given I have appended the file command.cmd by the line echo
	And I've added the argument -u:SomeTestUser_0089DD7B-C9A9-4D38-9CEF-CB71E57AE1F6
	And I've added the argument -p:SomePassword
	And I've added the argument -l:<logLevel>
	And I've added the argument command.cmd
	When I run RunAs tool
	Then the exit code should be -100002
	And the output should contain:
	|                 |
	| JetBrains RunAs |
	| Settings:       |
	And the errors should contain:
	|                                                |
	| Error: The user name or password is incorrect. |
	| CreateProcessWithLogonW returns                |
Examples:
	| logLevel |
	| normal   |
	| debug    |


Scenario Outline: User does not see header and sees brief error info when unknown user and in the errors log mode
	Given I have appended the file command.cmd by the line echo
	And I've added the argument -u:SomeTestUser_0089DD7B-C9A9-4D38-9CEF-CB71E57AE1F6
	And I've added the argument -p:SomePassword
	And I've added the argument -l:<logLevel>
	And I've added the argument command.cmd
	When I run RunAs tool
	Then the exit code should be -100002
	And the output should not contain:
	|                 |
	| JetBrains RunAs |
	| Settings:       |
	And the errors should contain:
	|                                                |
	| Error: The user name or password is incorrect. |	
	And the errors should not contain:
	|                                 |
	| CreateProcessWithLogonW returns |

Examples:
	| logLevel |
	| errors   |


Scenario: User does not see header and any error info when unknown user and in the off log mode
	Given I have appended the file command.cmd by the line echo
	And I've added the argument -u:SomeTestUser_0089DD7B-C9A9-4D38-9CEF-CB71E57AE1F6
	And I've added the argument -p:SomePassword
	And I've added the argument -l:off
	And I've added the argument command.cmd
	When I run RunAs tool
	Then the exit code should be -100002
	And the output should not contain:
	|    |
	| .+ |
	And the errors should not contain:
	|    |
	| .+ |