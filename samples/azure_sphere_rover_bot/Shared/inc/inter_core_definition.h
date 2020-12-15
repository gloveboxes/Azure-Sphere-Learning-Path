#pragma once

enum LP_INTER_CORE_CMD
{
	LP_IC_UNKNOWN,
	LP_IC_HEARTBEAT,
	LP_IC_REPORT_DISTANCE
};

typedef struct LP_INTER_CORE_BLOCK
{
	enum LP_INTER_CORE_CMD cmd;
	int		distance_left;
	int		distance_right;

} LP_INTER_CORE_BLOCK;