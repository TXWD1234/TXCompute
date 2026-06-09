# Copyright (c) 2026 TXCompute. Licensed under the MIT License.

idf_component_register(
	SRCS
		"include/tx/compute.h"
	INCLUDE_DIRS
		"include"
	REQUIRES
		"TXLib"
		"TXESP_Infrastructure"
		"TerminalEngine"
		"TXCSL"
)