/* based_task.c -- A basic real-time task skeleton. 
 *
 * This (by itself useless) task demos how to setup a 
 * single-threaded LITMUS^RT real-time task.
 */

/* First, we include standard headers.
 * Generally speaking, a LITMUS^RT real-time task can perform any
 * system call, etc., but no real-time guarantees can be made if a
 * system call blocks. To be on the safe side, only use I/O for debugging
 * purposes and from non-real-time sections.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

/* Second, we include the LITMUS^RT user space library header.
 * This header, part of liblitmus, provides the user space API of
 * LITMUS^RT.
 */
#include "litmus.h"

/* Next, we define period and execution cost to be constant. 
 * These are only constants for convenience in this example, they can be
 * determined at run time, e.g., from command line parameters.
 *
 * These are in milliseconds.
 */
#define PERIOD            100
#define RELATIVE_DEADLINE 100
#define EXEC_COST         10


#define __DEBUG_PRINT__
#define WORKLOAD1MS_SQRT_TIMES (161000)

static inline void workload_for_1ms(void)
{
    volatile double temp = 0;
    long long i;

    for ( i = 0; i < WORKLOAD1MS_SQRT_TIMES; ++i )
	{
        temp = sqrt((double)i*i);
		temp = temp;
	}
}


/* ================= Ctrl-C Exit Signal: SIGINT ================ */
int32_t gs32exitflag = 0;
void set_exitflag(int32_t s32flag)
{
    gs32exitflag = s32flag;
    return;
}

static void my_sighnl(int32_t signo)
{
    if(SIGINT == signo)
    {
        printf("\nCatch SIGINT, Exit!\n");
        set_exitflag(1);
    }
    return;
}

/* Catch errors.
 */
#define CALL( exp ) do { \
		int ret; \
		ret = exp; \
		if (ret != 0) \
			fprintf(stderr, "%s failed: %m\n", #exp);\
		else \
			fprintf(stderr, "%s ok.\n", #exp); \
	} while (0)


/* Declare the periodically invoked job. 
 * Returns 1 -> task should exit.
 *         0 -> task should continue.
 */
int job(void);

/* ================ Global Var for this task ====================*/
int gs32wcet_ms     = 0;	/* WCET in Milisecond */
int gs32period_ms   = 0; /* Period in Milisecond */
int gs32duration_ms = 0; /* Duration in Milisecond */
int gs32count	    = 0; /* How many periods do we have? */
unsigned int gu32priority = 0;
int gs32IsForever   = 0; /* CANNOT RUN FOREVER */
int gs32RefID       = -1;
/* ================ Gloabl Var End ==============================*/

/* typically, main() does a couple of things: 
 * 	1) parse command line parameters, etc.
 *	2) Setup work environment.
 *	3) Setup real-time parameters.
 *	4) Transition to real-time mode.
 *	5) Invoke periodic or sporadic jobs.
 *	6) Transition to background mode.
 *	7) Clean up and exit.
 *
 * The following main() function provides the basic skeleton of a single-threaded
 * LITMUS^RT real-time task. In a real program, all the return values should be 
 * checked for errors.
 */
int main(int argc, char** argv)
{
	int do_exit;
	int s32duration_s;
	struct rt_task param;

	/* Parse the argument */
	if(6 != argc)
	{
		printf("Argument Error!\n");
		exit(-1);	
	}
	gs32wcet_ms     = atoi(argv[1]);
	gs32period_ms   = atoi(argv[2]);
	s32duration_s   = atoi(argv[3]);
	gu32priority    = atoi(argv[4]);
	gs32RefID		= atoi(argv[5]);

	gs32duration_ms = 1000 * s32duration_s;
	gs32count       = (gs32duration_ms / gs32period_ms) + 1;
	if(0 == s32duration_s)
	{
		gs32IsForever = 1;
	}

#ifdef __DEBUG_PRINT__
	printf("[BTK] ATT: WCET = %dms, Period =%dms, Duration=%dms\n",
			gs32wcet_ms,
			gs32period_ms,
			gs32duration_ms);
#endif
	/* Setup Signal Handler */
    if(SIG_ERR == signal(SIGINT, my_sighnl))
    {
        fprintf(stderr, "Cannot catch SIGINT!\n");
        exit(EXIT_FAILURE);
    }

	/* Setup task parameters */
	init_rt_task_param(&param);
	param.exec_cost = ms2ns(gs32wcet_ms);
	param.period = ms2ns(gs32period_ms);
	param.relative_deadline = ms2ns(gs32period_ms);
	param.priority = gu32priority;
	/* ATTENTION: Deadline = Period */

	/* What to do in the case of budget overruns? */
	param.budget_policy = NO_ENFORCEMENT;

	/* The task class parameter is ignored by most plugins. */
	param.cls = RT_CLASS_SOFT;

	/* The priority parameter is only used by fixed-priority plugins. */
	param.priority = LITMUS_LOWEST_PRIORITY;

	/* The task is in background mode upon startup. */


	/*****
	 * 1) Command line paramter parsing would be done here.
	 */



	/*****
	 * 2) Work environment (e.g., global data structures, file data, etc.) would
	 *    be setup here.
	 */



	/*****
	 * 3) Setup real-time parameters. 
	 *    In this example, we create a sporadic task that does not specify a 
	 *    target partition (and thus is intended to run under global scheduling). 
	 *    If this were to execute under a partitioned scheduler, it would be assigned
	 *    to the first partition (since partitioning is performed offline).
	 */
	CALL( init_litmus() );

	/* To specify a partition, do
	 *
	 * param.cpu = CPU;
	 * be_migrate_to(CPU);
	 *
	 * where CPU ranges from 0 to "Number of CPUs" - 1 before calling
	 * set_rt_task_param().
	 */
	CALL( set_rt_task_param(gettid(), &param) );


	/*****
	 * 4) Transition to real-time mode.
	 */
	CALL( task_mode(LITMUS_RT_TASK) );

	/* The task is now executing as a real-time task if the call didn't fail. 
	 */

	/* STOP itself */
	// kill(getpid(), SIGSTOP);
	raise(SIGSTOP);


	/*****
	 * 5) Invoke real-time jobs.
	 */
	do {
		/* Wait until the next job is released. */
		sleep_next_period();
		/* Invoke job. */
		do_exit = job();		
	} while (!do_exit && !gs32exitflag);


	
	/*****
	 * 6) Transition to background mode.
	 */
	CALL( task_mode(BACKGROUND_TASK) );



	/***** 
	 * 7) Clean up, maybe print results and stats, and exit.
	 */
	return 0;
}


int job(void) 
{
	int i;
	/* Do real-time calculation. */
	for(i = 0; i < gs32wcet_ms; i++)
	{
		workload_for_1ms();
	}
	gs32count--;
	
	if(0 == gs32IsForever && 0 >= gs32count)
	{
		/* exit */
		return 1;
	}
	else
	{
		/* do not exit */
		return 0;
	}
}
