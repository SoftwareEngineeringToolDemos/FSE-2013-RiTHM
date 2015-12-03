#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "blowfish.h"
#include "../ProgramState.h"
#include <GooMFOnlineAPI.h>
#include <unistd.h>
#include <string.h>

//global declarations
struct timespec ts1, ts2;
_GOOMF_context context = NULL;
_GOOMF_enum_verdict_type v;
bool _GLOBAL_monitoring = false;
int _GLOBAL_iterations = 0;
struct _GOOMF_host_program_state_struct current_program_state;


/*
 * Measures difference between 2 timestamps
 */
struct timespec diff(struct timespec start, struct timespec end)
{
	struct timespec temp;
	if ((end.tv_nsec - start.tv_nsec)<0) {
		temp.tv_sec = end.tv_sec - start.tv_sec - 1;
		temp.tv_nsec = 1000000000 + end.tv_nsec - start.tv_nsec;
	} else {
		temp.tv_sec = end.tv_sec - start.tv_sec;
		temp.tv_nsec = end.tv_nsec - start.tv_nsec;
	}
	return temp;
}

void printResults()
{
	clock_gettime(CLOCK_REALTIME, &ts2);
	struct timespec ts3 = diff(ts1, ts2);

	if (_GLOBAL_monitoring)
	{
		char str[100];
		_GOOMF_get_current_status(context, &v);
		_GOOMF_type_to_string(&v, str);
		printf("%ld sec, %ld microsec, %ld nanosec, property is %s\n",
				ts3.tv_sec,
				ts3.tv_nsec / 1000,
				ts3.tv_nsec,
				str);
	}
	else
		printf("%ld sec, %ld microsec, %ld nanosec\n", ts3.tv_sec, ts3.tv_nsec / 1000,	ts3.tv_nsec);
}


/*
 * Worker function that runs blowfish encode/decode N times
 */
void workerFunctionBlowfish()
{
	unsigned long left = 0, right = 0;
	unsigned long tempLeft, tempRight;
	BLOWFISH_CTX ctx;

	//start timing from here
	clock_gettime(CLOCK_REALTIME, &ts1);

	Blowfish_Init(&ctx, (unsigned char*)"RTESG_UW", 8);

	int i = 0;
	unsigned int seed = 0;
	for(i=0; i<_GLOBAL_iterations; ++i)
	{
		seed = i;
		left = tempLeft = (unsigned long)rand_r(&seed);
		seed = i+1;
		right = tempRight = (unsigned long)rand_r(&seed);

		Blowfish_Encrypt(&ctx, &left, &right);
		Blowfish_Decrypt(&ctx, &left, &right);

		if (_GLOBAL_monitoring)
		{
			current_program_state.left = left;
			current_program_state.right = right;
			current_program_state.tempLeft = tempLeft;
			current_program_state.tempRight = tempRight;
			_GOOMF_next_state(context, (void*)(&current_program_state));
			if (_GOOMF_all_properties_converged(context) == true)
				break;
		}

		usleep(100);
	}

    printResults();

	return;
}

/*
 * Main function
 */
int main(int argc, char** argv)
{
	int c;

	//parsing the arguments
	while ((c = getopt(argc, argv, "m:i:")) != -1)
	{
		switch (c) {
		case 'm':
			if (strcmp(optarg, "true") == 0)
				_GLOBAL_monitoring = true;
			else
				_GLOBAL_monitoring = false;
			break;
		case 'i':
			_GLOBAL_iterations = atoi(optarg);
			break;
		default:
			return EXIT_FAILURE;
		}
	}

	if (_GLOBAL_monitoring)
	{
		_GOOMF_init_context(&context,
				_GOOMF_enum_buffer_trigger,
				_GOOMF_enum_GPU,
				_GOOMF_enum_alg_infinite,
				_GOOMF_enum_sync_invocation,
				4096);
	}

	workerFunctionBlowfish();

	if (_GLOBAL_monitoring)
		_GOOMF_destroy_context(&context);

	return 0;
}
