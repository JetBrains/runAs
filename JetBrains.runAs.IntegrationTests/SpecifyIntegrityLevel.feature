Feature: User specifies integrity level

Scenario Outline: User specifies integrity level
	Given I've added the argument -p:aaa
	And I've added the argument -il:<intergiryLevel>
	And I've added the argument WhoAmI.exe
	And I've added the argument /all
	When I run RunAs tool	
	And the output should contain:
	|                                                  |
	| <intergiryLevelName> Mandatory Level |

Examples:
	| userName      | intergiryLevel | intergiryLevelName |
	| RunAsTestUser | Low            | Low                |
	|               | Low            | Low                |
	| RunAsTestUser | Medium         | Medium             |
	|               | Medium         | Medium             |	