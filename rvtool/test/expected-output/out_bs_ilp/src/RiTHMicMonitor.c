#define _GNU_SOURCE
#include <unistd.h>     /* Symbolic Constants */
#include <sys/types.h>  /* Primitive System Data Types */
#include <errno.h>      /* Errors */
#include <stdio.h>      /* Input/Output */
#include <stdlib.h>     /* General Utilities */
#include <pthread.h>    /* POSIX Threads */
#include <string.h>     /* String handling */
#include <sched.h>
#include <sys/time.h>
#include <time.h>
#include <sys/mman.h>
#include "GooMFInstrumentor.h"
#include "ProgramState.h" 
 
#define LSP_SEC 0
#define LSP_NSEC 15

/* Global Variables of Interest of Monitored Prgoram must be here */ 
#include "rithmic_globals.h"
_GOOMF_context context = NULL; 
int initContext(){ 
return _GOOMF_initContext(&context,_GOOMF_enum_no_trigger,_GOOMF_enum_alg_seq,_GOOMF_enum_sync_invocation,100); 
}; 


void report(){ 
	_GOOMF_enum_verdict_type v; 
	char str[100]; 
	_GOOMF_getCurrentStatus(context, &v); 
	_GOOMF_typeToString(&v, str); 
	printf("property 0: %s\n", str); 
}; 


int  destroyContext(){ 
return _GOOMF_destroyContext(&context); 
}; 



#ifndef RETURN_TYPE_INT
#define RETURN_TYPE_INT 1
#endif

#ifndef RETURN_TYPE_VOID
#define RETURN_TYPE_VOID 0
#endif

int kill_monitor = 0;

typedef struct _program_thread_args {
    int argc;
    char **argv;
} program_thread_args;

/* prototype for thread routine */
void monitor_thread ( );
void main_thread ( );



int main(int argc, char** argv)
{
	pthread_t thread1, thread2;  /* thread variables */
	cpu_set_t  mask;
    program_thread_args *args = malloc(sizeof(program_thread_args));
    args->argc = argc;
    args->argv = argv;

	CPU_ZERO(&mask);
    	CPU_SET(0, &mask);
    	int result = sched_setaffinity(0, sizeof(mask), &mask);	
	if (result != 0)
	{
		printf("Cannot set affinity to monitor..\n");
		exit(0);
	}
	
	// This is just a switch to run the program in hi-res timer evaluation mode
	// If checktimers() is called it will output the average of a timer with a given interval
	if (argc >= 2 && !strcmp(argv[1],"-test")) {
		printf("%s\n", argv[1]);
	}
	else {
		/* create monitor and main thread */
		pthread_create(&thread1, NULL, (void *) &monitor_thread, 0);
		pthread_create(&thread2, NULL, (void *) &main_thread, (void *)args);

		/* Main block now waits for both threads to terminate, before it exits
   If main block exits, both threads exit, even if the threads have not
   finished their work */
		pthread_join(thread1, NULL);
		pthread_join(thread2, NULL);
	}

	/* exit */

	report(); 
	destroyContext(); 


    free(args);
	exit(0);
} /* main() */

void monitor_thread ()
{
	struct timeval start, end;
	struct timespec sleep_time;
	struct timespec start_nano, end_nano;
	int i = 0,j=0;

	// super user only -- switch to real-time mode and lock memory pages
	struct sched_param param;

	int policy = SCHED_FIFO;
	/* scheduling parameters of target thread */
	pthread_setschedparam(pthread_self(), policy, &param);
	(mlockall(MCL_CURRENT | MCL_FUTURE));

    // Set monitor's sampling period
	sleep_time.tv_sec = LSP_SEC;
	sleep_time.tv_nsec = LSP_NSEC;

	if (initContext() == _GOOMF_SUCCESS) 
		 printf("Context initialized..\n"); 
	else 
		printf("Problem with context initialization..\n"); 

	while(!kill_monitor) {
		clock_gettime(CLOCK_REALTIME, &start_nano);
		clock_nanosleep(CLOCK_REALTIME, 0, &sleep_time, NULL);

		/*---------------------------------------------------------------------------------------*/ 
		/*----------- The extraction of the variables of interest and calling of GOOMF ----------*/ 
 
		current_program_state.low = binary_search_low; 
		current_program_state.up = binary_search_up; 
		_GOOMF_nextState(context, (void*)(&current_program_state)); 
		_GOOMF_flush(context); 

		/*---------------------------------------------------------------------------------------*/ 
 
		clock_gettime(CLOCK_REALTIME, &end_nano);
	}
	
	pthread_exit(0); 
}

void checktimers() {
	struct timeval start, end;
		struct timespec sleep_time;
		struct timespec start_nano, end_nano;
		int i = 0,j=0;

		struct sched_param param;

		int policy = SCHED_FIFO;
		param.sched_priority = sched_get_priority_max(SCHED_FIFO);
		/* scheduling parameters of target thread */
		sched_setscheduler(0, policy, &param);
		(mlockall(MCL_CURRENT | MCL_FUTURE));

		sleep_time.tv_sec = 0;
		// Configure the number of nanoseconds to delay here
		sleep_time.tv_nsec = 5000;

		double super_average = 0.0;
		for(j=0;j<20;j++) {
			double average = 0.0;
			for(i=0;i<1000;i++) {
				clock_gettime(CLOCK_REALTIME, &start_nano);
				int ret = clock_nanosleep(CLOCK_REALTIME, 0, &sleep_time, NULL);
				clock_gettime(CLOCK_REALTIME, &end_nano);
				average += end_nano.tv_nsec - start_nano.tv_nsec;
			}
			if (average<0) {
				j--;
				continue;
			}
			average/=1000.0;
			printf("Average is %f\n", average);
			super_average += average;
			usleep(200000);
		}

		printf("Super Average is %f\n", super_average/20.0);
}

void main_thread (void *program_main_data)
{
	int i,j,k,y;
	struct sched_param param;
        program_thread_args *args = (program_thread_args *)program_main_data;

	int policy = SCHED_OTHER;
	/* scheduling parameters of target thread */
	pthread_setschedparam(pthread_self(), policy, &param);
    
    int *retVal = malloc(sizeof(int));
    if (!retVal) {
        fprintf(stderr, "main_thread() malloc failed! Aborting...\n");
        pthread_exit(0);
    }
    *retVal = 0;
	// Main thread Body
    program_main();

    kill_monitor = 1;
    pthread_exit((void *)retVal);
	
}
