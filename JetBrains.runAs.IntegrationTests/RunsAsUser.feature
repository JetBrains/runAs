Feature: Runs the command under the specific user account

Scenario: User runs the command under the specific user account
	Given I have appended the file command.cmd by the line WhoAmI.exe
	And I've added the argument -u:RunAsTestUser
	And I've added the argument -p:aaa
	And I've added the argument command.cmd
	When I run RunAs tool
	Then the exit code should be 0
	And the output should contain:
	|                |
	| WhoAmI.exe     |
	| .+\\\\RunAsTestUser |

Scenario Outline: User runs the command which contains spaces in the path
	Given I have appended the file <cmdFileName> by the line WhoAmI.exe
	And I've added the argument -u:RunAsTestUser
	And I've added the argument -p:aaa
	And I've added the argument cmd.exe
	And I've added the argument /C
	And I've added the argument <cmdFileName>
	And I've added the argument <args>
	When I run RunAs tool
	Then the exit code should be 0
	And the output should contain:
	|                |
	| WhoAmI.exe     |
	| .+\\\\RunAsTestUser |

Examples:
	| cmdFileName            | args         |
	| command.cmd            |              |
	| command.cmd            | arg          |
	| command.cmd            | arg1 arg2    |
	| command.cmd            | "arg 1" arg2 |
	| "command.cmd"          |              |
	| "command.cmd"          | arg          |
	| "command.cmd"          | arg1 arg2    |
	| "command.cmd"          | "arg 1" arg2 |
	| "command Who Am I.cmd" |              |
	| "command Who Am I.cmd" | arg          |
	| "command Who Am I.cmd" | arg1 arg2    |	

Scenario Outline: User runs the command which contains spaces in the path via config file for args
	Given I have appended the file <cmdFileName> by the line WhoAmI.exe
	And I have appended the file args.txt by the line -u:RunAsTestUser
	And I have appended the file args.txt by the line -p:aaa
	And I have appended the file args.txt by the line cmd.exe
	And I have appended the file args.txt by the line /S
	And I have appended the file args.txt by the line /C
	And I have appended the file args.txt by the line "<cmdFileName>"
	And I've added the argument -c:args.txt	
	When I run RunAs tool
	Then the exit code should be 0
	And the output should contain:
	|                |
	| WhoAmI.exe     |
	| .+\\\\RunAsTestUser |

Examples:
	| cmdFileName            | args         |
	| command.cmd            |              |
	| command.cmd            | arg          |
	| command.cmd            | arg1 arg2    |
	| command.cmd            | "arg 1" arg2 |
	| "command.cmd"          |              |
	| "command.cmd"          | arg          |
	| "command.cmd"          | arg1 arg2    |
	| "command.cmd"          | "arg 1" arg2 |
	| "command Who Am I.cmd" |              |
	| "command Who Am I.cmd" | arg          |
	| "command Who Am I.cmd" | arg1 arg2    |
	| "command Who Am I.cmd" | "arg 1" arg2 |

Scenario Outline: User runs using config file for args
	Given I have appended the file command.cmd by the line WhoAmI.exe
	And I have appended the file <argsFile> by the line -p:aaa
	And I have appended the file <argsFile> by the line command.cmd
	And I've added the argument -u:RunAsTestUser
	And I've added the argument -c:<argsFile>	
	When I run RunAs tool
	Then the exit code should be 0
	And the output should contain:
	|                |
	| WhoAmI.exe     |
	| .+\\\\RunAsTestUser |

Examples:
	| argsFile |
	| args.txt |
	| "my args.txt" |

Scenario: User runs the command with cmd args
	Given I have appended the file command.cmd by the line @echo %1 %2
	And I've added the argument -u:RunAsTestUser
	And I've added the argument -p:aaa
	And I've added the argument command.cmd
	And I've added the argument hello
	And I've added the argument "world !!!"
	When I run RunAs tool
	Then the exit code should be 0
	And the output should contain:
	|                      |
	| hello "world !!!" |

Scenario: User runs the command with cmd args via config file for args
	Given I have appended the file command.cmd by the line @echo %1 %~2
	And I have appended the file args.txt by the line command.cmd
	And I have appended the file args.txt by the line hello
	And I have appended the file args.txt by the line "world !!!"
	And I've added the argument -u:RunAsTestUser
	And I've added the argument -p:aaa
	And I've added the argument -c:args.txt		
	When I run RunAs tool
	Then the exit code should be 0
	And the output should contain:
	|                  |
	| hello world !!! |

Scenario: User runs the command with cmd args via config file with unquoted args
	Given I have appended the file command.cmd by the line @echo %1 %~2
	And I have appended the file args.txt by the line command.cmd
	And I have appended the file args.txt by the line hello
	And I have appended the file args.txt by the line world !!!
	And I've added the argument -u:RunAsTestUser
	And I've added the argument -p:aaa
	And I've added the argument -c:args.txt		
	When I run RunAs tool
	Then the exit code should be 0
	And the output should contain:
	|                 |
	| hello world !!! |

Scenario Outline: RunAs returns the exit code from the target command
	Given I have appended the file command.cmd by the line WhoAmI.exe
	And I have appended the file command.cmd by the line @exit <exitCode> /B
	And I've added the argument -u:RunAsTestUser
	And I've added the argument -p:aaa
	And I've added the argument command.cmd
	When I run RunAs tool
	Then the exit code should be <exitCode>
	And the output should contain:
	|                |
	| WhoAmI.exe     |
	| .+\\\\RunAsTestUser |

Examples:
	| exitCode  |
	| -99999999 |
	| -99       |
	| 0         |
	| 99        |
	| 99999999  |

Scenario: User runs with long args
	Given I have appended the file command.cmd by the line echo %*
	And I have appended the file args.txt by the line -p:aaa
	And I have appended the file args.txt by the line command.cmd
	And I have appended the file args.txt by the line 0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789
	And I have appended the file args.txt by the line 0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789
	And I have appended the file args.txt by the line 0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789
	And I have appended the file args.txt by the line 0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789
	And I have appended the file args.txt by the line 0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789
	And I have appended the file args.txt by the line 0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789
	And I have appended the file args.txt by the line 0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789
	And I have appended the file args.txt by the line 0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789
	And I have appended the file args.txt by the line 0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789
	And I've added the argument -u:RunAsTestUser
	And I've added the argument -c:args.txt	
	When I run RunAs tool
	Then the exit code should be 0