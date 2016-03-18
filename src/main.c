#include <stdio.h>
#include <stdlib.h>
#include "tracing.h"

int main(int argc, char* argv[])
{
	init_tracing();
	add_record(TASK_EVENT_FINISH_JOB);
	dump_tracing_to_screen();
	return 0;
}
