Feature: Runs in the self testing mode

Scenario: User gets bittness for tool
	Given I've added the argument -t
	When I run RunAs tool
	Then the exit code should be (32|64)