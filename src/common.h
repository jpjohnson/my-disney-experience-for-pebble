#pragma once

typedef struct {
	int index;
	char name[128];
	char time[128];
	int fastPass;
} Plan;

enum {
	PLAN_INDEX = 0x0,
	PLAN_NAME = 0x1,
	PLAN_TIME = 0x2,
	PLAN_FP = 0x3,
	PLAN_ERROR = 0x4
};
