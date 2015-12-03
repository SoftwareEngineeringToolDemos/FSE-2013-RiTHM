#ifndef UTILS_H_
#define UTILS_H_
#include <stdio.h>
#include <CL/cl.h>
#include <string.h>
#include <stdlib.h>
#include <semaphore.h>
#include <time.h>
#include "GooMF_CPU_monitor.h"

const unsigned int KERNEL_ARG_ORDER_INPUT = 0;
const unsigned int KERNEL_ARG_ORDER_OUT_STATES = 1;
const unsigned int KERNEL_ARG_ORDER_IN_STATES = 2;
const unsigned int KERNEL_ARG_ORDER_INDICES = 3;
const unsigned int KERNEL_ARG_ORDER_TRANSITIONS = 4;
const unsigned int KERNEL_ARG_ORDER_PARAMS = 5;
const unsigned int KERNEL_ARG_ORDER_ITEMS = 6;

unsigned int _GLOBAL_local_group_size = 256;
unsigned int _GLOBAL_groups_number = 0;
unsigned int _GLOBAL_num_of_partial_predicates = 0;
bool _GLOBAL_debug = false;
FILE* _GLOBAL_debug_log = NULL;

//main context structure
struct _GOOMF_context_struct
{
	_GOOMF_device_program_state_ptr buffer[2];	//2 buffers for double-buffering
	unsigned int current_buffer_size[2];	//as number of buffers
	struct _GOOMF_device_program_state_struct last_program_state;
	fsm_state* current_fsm_states_ptr;
	transition_type* transitions;
	report_handler_type* report_handlers_array;
	unsigned int current_buffer;

	int max_buffer_size;
	int num_of_props;

	//bool live_buffer_processing[2];	//shared variable to indicate that parallel processing is taking place
	FILE* logger;

	//_GOOMF_enum_device_type device_type;
	_GOOMF_enum_trigger_type trigger_type;
	_GOOMF_enum_alg_type alg_type;
	_GOOMF_enum_invocation_type invocation_type;

	//opencl-related stuff
	cl_program clprogram;
	cl_kernel clkernel;
	cl_command_queue clqueue;
	cl_context clcontext;

	//opencl host and gpu buffers
	cl_mem output_states_buffer;
	cl_mem current_fsm_states_buffer;
	cl_mem current_indices_output_buffer;
	cl_uint* current_indices_output_ptr;
	cl_mem num_of_transitions_output_buffer;
	cl_uint* num_of_transitions_output_ptr;
	cl_mem parameters_struct_buffer;
	cl_mem opencl_input_buffer[2];	//2 buffers for double-buffering
	struct _GOOMF_used_params_struct* parameters_struct_ptr;
	sem_t buffer_mutex[2];		//2 mutex semaphores for 2 buffers
	sem_t buffer_is_full[2];	//semaphore to mark the buffer is ready
	sem_t buffer_is_empty[2];
	sem_t gpu_resource_mutex;	//mutex for guarding the gpu queue, kernel etc.
	pthread_t workers[2];
	bool kill_workers;			//to indicate that input is exhausted
};
typedef struct _GOOMF_context_struct* _GOOMF_context;

_GOOMF_context global_context;
int global_current_buffer1 = 0;
int global_current_buffer2 = 1;

//struct for passing to the worker thread
/*typedef struct
{
	_GOOMF_context context;
	unsigned int current_buffer;
} args_struct;*/

//args_struct _GLOBAL_args_struct;

/*
 * Check if monitor state is a conclusive state
 */
bool isStateConclusive(fsm_state state)
{
	if ((state ^ 0x00) == 0 || (state ^ 0x03) == 0)
		return true;
	return false;
}


/*
 * Read content of the file into the given string
*/
char* file_open_and_read(const char* file_name, char* preamble, unsigned int* final_length)
{
    FILE* file_stream = NULL;
    size_t source_length;

    // open the OpenCL source code file
    #ifdef _WIN32   // Windows version
        if(fopen_s(&file_stream, file_name, "rb") != 0)
        {
            return NULL;
        }
    #else           // Linux version
        file_stream = fopen(file_name, "rb");
        if(file_stream == 0)
        {
            return NULL;
        }
    #endif

    size_t preamble_length = 0; //strlen(cPreamble);

    // get the length of the source code
    fseek(file_stream, 0, SEEK_END);
    source_length = ftell(file_stream);
    fseek(file_stream, 0, SEEK_SET);

    // allocate a buffer for the source code string and read it in
    char* source_string = (char *)malloc(source_length + preamble_length + 1);
    memcpy(source_string, preamble, preamble_length);
    if (fread((source_string) + preamble_length, source_length, 1, file_stream) != 1)
    {
        fclose(file_stream);
        free(source_string);
        return 0;
    }

    // close the file and return the total length of the combined (preamble + source) string
    fclose(file_stream);
    if(final_length != 0)
    {
        *final_length = source_length + preamble_length;
    }
    source_string[source_length + preamble_length] = '\0';

    return source_string;
}


/*
Common to both devices code: get program and kernel, compile and run while benchmarking time
*/
cl_program get_program_from_file(cl_context context, const char* file_name)
{
	cl_int ErrorCode = CL_SUCCESS;
	size_t string_size = 0;

	char* source = file_open_and_read(file_name, "", &string_size);
    if (source == NULL) {
         printf("Error: cannot open file with CL source code\n");
         return NULL;
    }

	//Perform runtime source compilation, and obtain kernel entry point.
    const char* csource = source;
    cl_program program = clCreateProgramWithSource(context,	1, &csource, NULL, &ErrorCode);
	if (ErrorCode != CL_SUCCESS || program == NULL)
	{
		free(source);
		return NULL;
	}

	free(source);
	return program;
}


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

#endif /* UTILS_H_ */
