#pragma once

typedef int ExitCode;

enum ExitCodes: ExitCode
{
	EXIT_CODE_BASE							= -100000,
	EXIT_CODE_SUCCESS						= 0,
	EXIT_CODE_NO_ADMIN						= 1,
	EXIT_CODE_NO_ASSIGN_PRIMARY_TOKEN_PRIV	= 2,
	EXIT_CODE_NO_TCB_PRIV					= 3,
	EXIT_CODE_READY_FOR_32					= 32,
	EXIT_CODE_READY_FOR_64					= 64
};