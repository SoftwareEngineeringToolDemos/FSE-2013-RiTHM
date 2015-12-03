/*
 ============================================================================
 Name        : GooMF.c
 Author      : Shay Berkovich
 Version     :
 Copyright   : Real-Time Embedded Software Group, University of Waterloo
 Description : The main file for GooMF implementation
 ============================================================================
 */
//#include "Utils.h"

#include "GooMFOnlineAPI.h"
#include "GooMFOfflineAPI.h"
#include "ThreadPool.h"

extern int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine)(void*), void *arg);
extern int pthread_join(pthread_t thread, void **value_ptr);

/*
 * Reset FSM states for specific property or for all properties if prop_num is negative
 */
static void resetFSMStates(fsm_state* cur_states_host_ptr, int prop_num)
{
	int i = 0;

	if (prop_num > getNumOfProperties())
		return;

	if (prop_num < 0)
	{
		for (i=0; i<getNumOfProperties(); ++i)
			cur_states_host_ptr[i] = getInitialState(i);
	}
	else
		cur_states_host_ptr[prop_num] = getInitialState(prop_num);

	return;
}


/*
 * OpenCL initialization
 */
static int openclInit(_GOOMF_context context)
{
	//now comes GPU-specific stuff
	cl_device_id device;
	cl_uint num_devices = 0;
	cl_int Result = CL_SUCCESS, ErrorCode = CL_SUCCESS;

	//Getting a platform
	cl_platform_id platform;
	cl_uint num = 0;
	clGetPlatformIDs(1, &platform, &num);

	//Find the required device.
	Result = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, &num_devices);
	if (Result != CL_SUCCESS)
		return _GOOMF_CL_DEVICE_ERROR;

	//TODO: in future extend the algorithm to several cards
	if (num_devices != 1)
		return _GOOMF_CL_DEVICE_ERROR;

	//Create OpenCL context
	context->clcontext = clCreateContext(NULL, 1, &device, NULL, NULL, &ErrorCode);
	if (ErrorCode != CL_SUCCESS || context->clcontext  == NULL)
		return _GOOMF_CL_CONTEXT_ERROR;

	//Create command queue
	context->clqueue = clCreateCommandQueue(context->clcontext, device, 0, &ErrorCode);
	if (context->clqueue == NULL)
		return _GOOMF_CL_QUEUE_ERROR;

	//choosing the kernel filename
	const char* filename = NULL;
	switch (context->alg_type)
	{
	case (_GOOMF_enum_alg_finite):
		filename = kernel_monitor_alg_finite_gpu;
		break;
	case (_GOOMF_enum_alg_infinite):
		filename = kernel_monitor_alg_infinite_gpu;
		break;
	default:
		filename = kernel_monitor_alg_partial_gpu;
	}

	context->clprogram = get_program_from_file(context->clcontext, filename);
	if (context->clprogram == NULL)
		return _GOOMF_CL_PROGRAM_ERROR;

	//Building
	Result = clBuildProgram(context->clprogram, 1, &device, NULL, NULL, NULL);
	if (Result != CL_SUCCESS)
	{
		char buf[0x10000];
		clGetProgramBuildInfo(context->clprogram,
								device,
								CL_PROGRAM_BUILD_LOG,
								0x10000,
								buf,
								NULL);
		printf("\n%s\n", buf);
		return _GOOMF_CL_BUILD_ERROR;
	}

	//create kernel
	context->clkernel = clCreateKernel(context->clprogram, MAIN_FUNCTION_NAME, &ErrorCode);
	if (ErrorCode != CL_SUCCESS || context->clkernel  == NULL)
		return _GOOMF_CL_KERNEL_ERROR;


	//Buffers initialization
	//Create output buffer.
	//property history is infinite
	switch (context->alg_type)
	{
	case _GOOMF_enum_alg_infinite:
		context->output_states_buffer = clCreateBuffer(context->clcontext,
									CL_MEM_WRITE_ONLY,
									getNumOfInconclusiveStates(-1) * context->max_buffer_size * sizeof(fsm_state),
									NULL,
									&ErrorCode);
		break;
	case _GOOMF_enum_alg_finite:
		context->output_states_buffer = clCreateBuffer(context->clcontext,
									CL_MEM_WRITE_ONLY,
									context->num_of_props * sizeof(fsm_state),
									NULL,
									&ErrorCode);
		break;
	default:	//partial offload algorithm
		context->output_states_buffer = clCreateBuffer(context->clcontext,
									CL_MEM_WRITE_ONLY,
									context->max_buffer_size * _GLOBAL_num_of_partial_predicates * sizeof(fsm_state),
									NULL,
									&ErrorCode);
		break;
	}
	if (ErrorCode != CL_SUCCESS || context->output_states_buffer == NULL)
		return _GOOMF_CL_ALLOCATE_ERROR;

	//Create cur_state buffer; initial states are already loaded by _GOOMF_common_init()
	if (context->alg_type != _GOOMF_enum_alg_partial_offload)
	{	//partial algorithm does not use this
		context->current_fsm_states_buffer = clCreateBuffer(context->clcontext,
									CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
									context->num_of_props * sizeof(fsm_state),
									context->current_fsm_states_ptr,
									&ErrorCode);
		if (ErrorCode != CL_SUCCESS || context->current_fsm_states_buffer == NULL)
			return _GOOMF_CL_ALLOCATE_ERROR;
	}

	//Create cur_index buffer
	if (context->alg_type != _GOOMF_enum_alg_partial_offload)
	{	//partial algorithm does not use this
		context->current_indices_output_ptr = (cl_uint*)malloc(context->num_of_props  * sizeof(cl_uint));
		memset(context->current_indices_output_ptr, 0, context->num_of_props * sizeof(cl_uint));
		context->current_indices_output_buffer = clCreateBuffer(context->clcontext,
										CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
										context->num_of_props * sizeof(cl_uint),
										context->current_indices_output_ptr,
										&ErrorCode);
		if (ErrorCode != CL_SUCCESS || context->current_indices_output_buffer == NULL)
			return _GOOMF_CL_ALLOCATE_ERROR;
	}

	//Create num_of_transitions buffer
	if (context->alg_type != _GOOMF_enum_alg_partial_offload)
	{	//partial algorithm does not use this
		context->num_of_transitions_output_ptr = (cl_uint*)malloc(context->num_of_props  *
												(context->max_buffer_size / _GLOBAL_local_group_size) *
												sizeof(cl_uint));
		context->num_of_transitions_output_buffer = clCreateBuffer(context->clcontext,
									CL_MEM_WRITE_ONLY,
									context->num_of_props * (context->max_buffer_size / _GLOBAL_local_group_size) * sizeof(cl_uint),
									NULL,
									&ErrorCode);
		if (ErrorCode != CL_SUCCESS || context->num_of_transitions_output_buffer == NULL)
			return _GOOMF_CL_ALLOCATE_ERROR;
	}

	//Create used_params_struct buffer
	if (context->alg_type != _GOOMF_enum_alg_partial_offload)
	{	//partial algorithm does not use this
		context->parameters_struct_ptr = NULL;
		context->parameters_struct_buffer = clCreateBuffer(context->clcontext,
										CL_MEM_READ_ONLY | CL_MEM_ALLOC_HOST_PTR, //CL_MEM_USE_HOST_PTR
										sizeof(struct _GOOMF_used_params_struct),
										context->parameters_struct_ptr,
										&ErrorCode);
		if (ErrorCode != CL_SUCCESS || context->parameters_struct_buffer == NULL)
			return _GOOMF_CL_ALLOCATE_ERROR;

		//And now map the buffer to the user space
		context->parameters_struct_ptr = clEnqueueMapBuffer(context->clqueue,
				context->parameters_struct_buffer,
				true,
				CL_MAP_WRITE,
				0,
				sizeof(struct _GOOMF_used_params_struct),
				0, NULL, NULL, NULL);
		memset(context->parameters_struct_ptr, 0, sizeof(struct _GOOMF_used_params_struct));	//APU
	}

	//Create an input data buffer.
	//Do not allocate buffer for program states - GPU will do it
	context->buffer[0] = NULL;
	context->opencl_input_buffer[0] = clCreateBuffer(context->clcontext,
									CL_MEM_READ_ONLY | CL_MEM_ALLOC_HOST_PTR, //CL_MEM_USE_HOST_PTR
									context->max_buffer_size * _GOOMF_get_program_state_size(),
									context->buffer[0],
									&ErrorCode);
	if (ErrorCode != CL_SUCCESS || context->opencl_input_buffer[0] == NULL)
		return _GOOMF_CL_ALLOCATE_ERROR;

	//And now map the buffer to the user space
	context->buffer[0] = clEnqueueMapBuffer(context->clqueue,
			context->opencl_input_buffer[0],
			true,
			CL_MAP_WRITE,
			0,
			context->max_buffer_size * _GOOMF_get_program_state_size(),
			0, NULL, NULL, NULL);
	memset(context->buffer[0], 0, context->max_buffer_size * _GOOMF_get_program_state_size());	//APU

	//if asynchronous invocation then double-buffering is present
	if (context->invocation_type == _GOOMF_enum_async_invocation)
	{
		context->buffer[1] = NULL;
		context->opencl_input_buffer[1] = clCreateBuffer(context->clcontext,
									CL_MEM_READ_ONLY | CL_MEM_ALLOC_HOST_PTR, //CL_MEM_USE_HOST_PTR
									context->max_buffer_size * _GOOMF_get_program_state_size(),
									context->buffer[1],
									&ErrorCode);
		if (ErrorCode != CL_SUCCESS || context->opencl_input_buffer[1] == NULL)
			return _GOOMF_CL_ALLOCATE_ERROR;

		context->buffer[1] = clEnqueueMapBuffer(context->clqueue,
			context->opencl_input_buffer[1],
			true,
			CL_MAP_WRITE,
			0,
			context->max_buffer_size * _GOOMF_get_program_state_size(),
			0, NULL, NULL, NULL);
		memset(context->buffer[1], 0, context->max_buffer_size * _GOOMF_get_program_state_size());	//APU
	}

	//Set kernel arguments
	if (context->alg_type != _GOOMF_enum_alg_partial_offload)
	{	//partial algorithm does not use this
		Result = clSetKernelArg(context->clkernel,	KERNEL_ARG_ORDER_INPUT, sizeof(void *), (void*) &(context->opencl_input_buffer[0]));
		if (Result != CL_SUCCESS)
			return _GOOMF_CL_KERNEL_ARGUMENT_ERROR;

		Result = clSetKernelArg(context->clkernel, KERNEL_ARG_ORDER_OUT_STATES, sizeof(void*), (void*) &(context->output_states_buffer));
		if (Result != CL_SUCCESS)
			return _GOOMF_CL_KERNEL_ARGUMENT_ERROR;

		Result = clSetKernelArg(context->clkernel, KERNEL_ARG_ORDER_IN_STATES, sizeof(void *), (void*) &(context->current_fsm_states_buffer));
		if (Result != CL_SUCCESS)
			return _GOOMF_CL_KERNEL_ARGUMENT_ERROR;

		Result = clSetKernelArg(context->clkernel, KERNEL_ARG_ORDER_INDICES, sizeof(void *), (void*) &(context->current_indices_output_buffer));
		if (Result != CL_SUCCESS)
			return _GOOMF_CL_KERNEL_ARGUMENT_ERROR;

		Result = clSetKernelArg(context->clkernel, KERNEL_ARG_ORDER_TRANSITIONS, sizeof(void *), (void*) &(context->num_of_transitions_output_buffer));
		if (Result != CL_SUCCESS)
			return _GOOMF_CL_KERNEL_ARGUMENT_ERROR;

		Result = clSetKernelArg(context->clkernel, KERNEL_ARG_ORDER_PARAMS, sizeof(void *), (void*) &(context->parameters_struct_buffer));
		if (Result != CL_SUCCESS)
			return _GOOMF_CL_KERNEL_ARGUMENT_ERROR;

		cl_uint num_src_items = context->max_buffer_size;
		Result = clSetKernelArg(context->clkernel, KERNEL_ARG_ORDER_ITEMS, sizeof(num_src_items), (void*) &num_src_items);
		if (Result != CL_SUCCESS)
			return _GOOMF_CL_KERNEL_ARGUMENT_ERROR;
	}
	else
	{	//we need different kernel arguments for partial algorithm
		Result = clSetKernelArg(context->clkernel,	KERNEL_ARG_ORDER_INPUT, sizeof(void *), (void*) &(context->opencl_input_buffer[0]));
		if (Result != CL_SUCCESS)
			return _GOOMF_CL_KERNEL_ARGUMENT_ERROR;

		Result = clSetKernelArg(context->clkernel, KERNEL_ARG_ORDER_OUT_STATES, sizeof(void*), (void*) &(context->output_states_buffer));
		if (Result != CL_SUCCESS)
			return _GOOMF_CL_KERNEL_ARGUMENT_ERROR;

		cl_uint num_src_items = context->max_buffer_size;
		Result = clSetKernelArg(context->clkernel, 2, sizeof(num_src_items), (void*) &num_src_items);
		if (Result != CL_SUCCESS)
			return _GOOMF_CL_KERNEL_ARGUMENT_ERROR;
	}

	return _GOOMF_SUCCESS;
}


/*
 * Init functions common to all devices
 */
static int commonInit(_GOOMF_context context)
{
	if (context == NULL)
		return _GOOMF_NO_CONTEXT_ERROR;

	//set number of monitored properties
	context->num_of_props = getNumOfProperties();

	//partial predicates
	_GLOBAL_num_of_partial_predicates = getNumOfPartialPredicates(-1);

	//allocate buffer for report handlers
	context->report_handlers_array = NULL;
	context->report_handlers_array = (report_handler_type*)malloc(context->num_of_props * sizeof(report_handler_type));
	if (context->report_handlers_array == NULL)
		return _GOOMF_MALLOC_ERROR;

	//allocate buffer for current verdicts
	context->current_fsm_states_ptr = NULL;
	context->current_fsm_states_ptr = (fsm_state*)malloc(context->num_of_props * sizeof(fsm_state));
	if (context->current_fsm_states_ptr == NULL)
		return _GOOMF_MALLOC_ERROR;

	//initializing the states
	resetFSMStates(context->current_fsm_states_ptr, -1);

	//allocate array of function pointers
	context->transitions = NULL;
	context->transitions = (transition_type*)malloc(context->num_of_props * sizeof(transition_type));
	if (context->transitions == NULL)
		return _GOOMF_MALLOC_ERROR;

	initFunctionPointers(context->transitions);

	return _GOOMF_SUCCESS;
}


/*
 * Context structure initialization for GPU-based monitoring
 */
static int initContextGPU(_GOOMF_context context)
{
	int Result = commonInit(context);
	if (Result != _GOOMF_SUCCESS)
		return Result;

	Result = openclInit(context);
	if (Result != _GOOMF_SUCCESS)
		return Result;

	return _GOOMF_SUCCESS;
}


/*
 * Context structure initialization for CPU-based monitoring
 */
static int initContextCPU(_GOOMF_context context)
{
	int Result = commonInit(context);
	if (Result != _GOOMF_SUCCESS)
		return Result;

	//allocate buffer for program states
	context->current_buffer = 0;
	context->buffer[0] = NULL;
	context->buffer[0] = (_GOOMF_device_program_state_ptr)malloc((context->max_buffer_size) *_GOOMF_get_program_state_size());
	if (context->buffer[0] == NULL)
		return _GOOMF_MALLOC_ERROR;

	//we need double-buffering for asynchronous invocation
	if (context->invocation_type == _GOOMF_enum_async_invocation)
	{
		context->buffer[1] = NULL;
		context->buffer[1] = (_GOOMF_device_program_state_ptr)malloc((context->max_buffer_size) *_GOOMF_get_program_state_size());
		if (context->buffer[1] == NULL)
			return _GOOMF_MALLOC_ERROR;
	}

	//allocate buffer for used_params struct
	context->parameters_struct_ptr = NULL;
	context->parameters_struct_ptr = (struct _GOOMF_used_params_struct*)malloc(sizeof(struct _GOOMF_used_params_struct));
	if (context->parameters_struct_ptr == NULL)
		return _GOOMF_MALLOC_ERROR;

	return _GOOMF_SUCCESS;
}


/*
 * Main context structure initialization
 */
int _GOOMF_initContext(_GOOMF_context* context,
							_GOOMF_enum_trigger_type trigger_type,
							//_GOOMF_enum_device_type device_type,
							_GOOMF_enum_alg_type alg_type,
							_GOOMF_enum_invocation_type invocation_type,
							int buffer_size)
{
	if (buffer_size <= 0 ||
			//device_type < _GOOMF_enum_CPU ||
			//device_type > _GOOMF_enum_APU ||
			trigger_type < _GOOMF_enum_no_trigger ||
			trigger_type > _GOOMF_enum_time_trigger ||
			invocation_type < _GOOMF_enum_sync_invocation ||
			invocation_type > _GOOMF_enum_async_invocation ||
			buffer_size <= 0)
		return _GOOMF_ARGUMENT_ERROR;

	if (_GLOBAL_num_of_partial_predicates <= 0 && alg_type == _GOOMF_enum_alg_partial_offload)
		return _GOOMF_PARTIAL_OFFLOAD_ERROR;

	if (_GLOBAL_debug)
	{
	//	_GLOBAL_log = fopen("log.txt", "w");
	//	fclose(_GLOBAL_log);
	}

	//GPU buffer should be multiple of 256
	//if ((alg_type != _GOOMF_enum_alg_seq) && (buffer_size % 256 != 0))
	//	return _GOOMF_BUFFER_SIZE_ERROR;

	*context = NULL;
	*context = (_GOOMF_context)malloc(sizeof(struct _GOOMF_context_struct));
	if (*context == NULL)
		return _GOOMF_MALLOC_ERROR;

	(*context)->buffer[0] = NULL;
	(*context)->buffer[1] = NULL;
	//(*context)->device_type = device_type;
	(*context)->trigger_type = trigger_type;
	(*context)->invocation_type = invocation_type;
	(*context)->alg_type = alg_type;
	(*context)->current_buffer_size[0] = 0;
	(*context)->current_buffer_size[1] = 0;
	(*context)->max_buffer_size = buffer_size;
	(*context)->num_of_props = 0;
	(*context)->current_indices_output_ptr = NULL;
	(*context)->parameters_struct_ptr = NULL;
	(*context)->current_fsm_states_ptr = NULL;
	//(*context)->live_buffer_processing[0] = false;
	//(*context)->live_buffer_processing[1] = false;
	(*context)->logger = false;
	(*context)->kill_workers = false;

	_GLOBAL_groups_number = (*context)->max_buffer_size / _GLOBAL_local_group_size;

	global_context = *context;

	int Result = _GOOMF_SUCCESS;
	switch(alg_type)
	{
	case(_GOOMF_enum_alg_seq):
		Result = initContextCPU(*context);
	break;
	case(_GOOMF_enum_alg_partial_offload):
		Result = initContextGPU(*context);
	break;
	default:
		Result = initContextGPU(*context);
	}
	if (Result != _GOOMF_SUCCESS)
		return Result;

	return initSyncMechanisms(*context);
}


/*
 * Main context structure deallocation
 */
int _GOOMF_destroyContext(_GOOMF_context* context)
{
	if (*context == NULL)
		return _GOOMF_NO_CONTEXT_ERROR;

	//wait while parallel processing is over
	(*context)->kill_workers = true;
	if ((*context)->invocation_type == _GOOMF_enum_async_invocation)
	{
		pthread_join((*context)->workers[0], NULL);
		pthread_join((*context)->workers[1], NULL);
	}

	destroySyncMechanisms(*context);

	//printing out the final status
	if ((*context)->logger != NULL)
	{
		char str[100];
		_GOOMF_enum_verdict_type v[3];
		_GOOMF_getCurrentStatus(*context, v);
		fprintf((*context)->logger, "Properties status:\n");
		int i = 0;
		for (i=0; i<(*context)->num_of_props; ++i)
		{
			_GOOMF_typeToString(&(v[i]), str);
			fprintf((*context)->logger, "\t%s;\n", str);
		}
	}

	if ((*context)->alg_type == _GOOMF_enum_alg_seq)
	{
		if ((*context)->buffer[0] != NULL)
		{
			free((*context)->buffer[0]);
			(*context)->buffer[0] = NULL;
		}
		if ((*context)->buffer[1] != NULL)
		{
			free((*context)->buffer[1]);
			(*context)->buffer[1] = NULL;
		}
	}
	else
	{
		clEnqueueUnmapMemObject((*context)->clqueue,
				(*context)->opencl_input_buffer[0],
				(*context)->buffer[0],
				0, NULL, NULL);
		clReleaseMemObject((*context)->opencl_input_buffer[0]);

		if ((*context)->invocation_type == _GOOMF_enum_async_invocation)
		{
			clEnqueueUnmapMemObject((*context)->clqueue,
					(*context)->opencl_input_buffer[1],
					(*context)->buffer[1],
					0, NULL, NULL);
			clReleaseMemObject((*context)->opencl_input_buffer[1]);
		}
	}

	clReleaseProgram((*context)->clprogram);
	clReleaseKernel((*context)->clkernel);
	clReleaseCommandQueue((*context)->clqueue);
	clReleaseContext((*context)->clcontext);

	if ((*context)->transitions != NULL)
	{
		free((*context)->transitions);
		(*context)->transitions = NULL;
	}

	if ((*context)->report_handlers_array != NULL)
	{
		free((*context)->report_handlers_array);
		(*context)->report_handlers_array = NULL;
	}

	if ((*context)->current_indices_output_ptr != NULL)
	{
		free((*context)->current_indices_output_ptr);
		(*context)->current_indices_output_ptr = NULL;
	}
	clReleaseMemObject((*context)->current_indices_output_buffer);

	if ((*context)->current_fsm_states_ptr != NULL)
	{
		free((*context)->current_fsm_states_ptr);
		(*context)->current_fsm_states_ptr = NULL;
	}
	clReleaseMemObject((*context)->current_fsm_states_buffer);

	if ((*context)->alg_type == _GOOMF_enum_alg_seq && (*context)->parameters_struct_ptr != NULL)
	{
		free((*context)->parameters_struct_ptr);
		(*context)->parameters_struct_ptr = NULL;
	}
	else
		clReleaseMemObject((*context)->parameters_struct_buffer);

	if ((*context)->num_of_transitions_output_ptr != NULL)
	{
		free((*context)->num_of_transitions_output_ptr);
		(*context)->num_of_transitions_output_ptr = NULL;
	}
	clReleaseMemObject((*context)->num_of_transitions_output_buffer);

	free(*context);
	*context = NULL;

	if (_GLOBAL_debug)
	{
	//	_GLOBAL_log = fopen("log.txt", "a");
		printf("Main thread: all done.");
	//	fclose(_GLOBAL_log);
	}

	return _GOOMF_SUCCESS;
}


/*
 * Flush and process the program states that are in the buffer
 */
int flushCPU(_GOOMF_context context, unsigned int current_buffer)
{
	if (context == NULL)
		return _GOOMF_NO_CONTEXT_ERROR;

	int i = 0, j = 0;
	for (i=0; i<context->current_buffer_size[current_buffer]; ++i)
	{
		for (j=0; j<context->num_of_props; ++j)
		{
			//perform monitoring only if property is enabled and has not converged yet
			if (property_triggers[j] && !isStateConclusive(context->current_fsm_states_ptr[j]))
			{
				((*(context->transitions)[j]))
						(&(context->current_fsm_states_ptr[j]),
						&(context->buffer[current_buffer][i]),
						NULL,
						context->parameters_struct_ptr);

				//report if state is conclusive
				if (isStateConclusive(context->current_fsm_states_ptr[j]))
				{
					if (context->report_handlers_array[j] != NULL)
						(context->report_handlers_array[j])(j, context->current_fsm_states_ptr[j] ^ 0x00, &(context->buffer[current_buffer][i]));
				}
			}
		}
	}
	context->current_buffer_size[current_buffer] = 0;
	if (context->invocation_type == _GOOMF_enum_async_invocation)
	{
		sem_post(&(context->gpu_resource_mutex));
		//context->live_buffer_processing[current_buffer] = false;
	}

	return _GOOMF_SUCCESS;
}


/*
 * Second part of the Alg1 that we have to do sequentially because of lack of global synchronization
 */
static void findNextState(_GOOMF_context context, fsm_state* output_ptr, int prop_number, unsigned int current_buffer)
{
	if (output_ptr == NULL || context == NULL)
		return;

	int i = 0, order = 0, index = 0;
	fsm_state temp1, temp2;

	for (i=0; i<prop_number; ++i)
		index += (context->current_buffer_size[current_buffer] * getNumOfInconclusiveStates(i));

	for (i=0; i<context->current_buffer_size[current_buffer]; ++i)
	{
		order = getStateOrder(prop_number, context->current_fsm_states_ptr[prop_number]);

		temp1 = output_ptr[index + getNumOfInconclusiveStates(prop_number)*i + order];
		temp2 = context->current_fsm_states_ptr[prop_number];
		if (temp1 != temp2)
		{
			//if state transition happened - update current fsm states
			context->current_fsm_states_ptr[prop_number] = temp1;
			if (isStateConclusive(context->current_fsm_states_ptr[prop_number]))
			{
				if (context->report_handlers_array[prop_number] != NULL)
				{
					(context->report_handlers_array[prop_number])(prop_number,
							context->current_fsm_states_ptr[prop_number] ^ 0x00,
							&(context->buffer[current_buffer][i]));
					break;
				}
			}
		}
	}
}


/*
 * Check if all properties came to decision
 */
static bool isConverged(_GOOMF_context context, unsigned int current_buffer)
{
	int i = 0;

	//check if all properties came to decision
	for (i=0; i<getNumOfProperties(); ++i)
	{
		int j = 0;
		for (j=0; j<_GLOBAL_groups_number; ++j)
		{
			if (context->num_of_transitions_output_ptr[i*_GLOBAL_groups_number + j] < context->current_buffer_size[current_buffer])
				break;
		}

		//finding property that just converged
		if ((isStateConclusive(context->current_fsm_states_ptr[i])) && j<_GLOBAL_groups_number)
		{
			//if property converged - report
			if (context->report_handlers_array[i] != NULL)
			{
				(context->report_handlers_array[i])(i, context->current_fsm_states_ptr[i] ^ 0x00,
					&(context->buffer[current_buffer][context->num_of_transitions_output_ptr[i*_GLOBAL_groups_number + j]]));
			}
		}

		//finding property that is not done
		if ((!isStateConclusive(context->current_fsm_states_ptr[i])) && j<_GLOBAL_groups_number)
			return false;
	}

	//all properties either came to decision or finished all input
	if (i >= getNumOfProperties())
		return true;
	else
		return false;
}


/*
 * Process current chunk of program states
 */
static int processChunk(_GOOMF_context context, bool* transition_happened, unsigned int current_buffer)
{
	cl_int Result = CL_SUCCESS, ErrorCode = CL_SUCCESS;
	int i = 0;
	*transition_happened = false;

	memset(context->num_of_transitions_output_ptr, 0,
			context->num_of_props * (context->max_buffer_size / _GLOBAL_local_group_size) * sizeof(cl_uint));

	//update kernel argument only if asynchronous invocation is present
	if (context->invocation_type == _GOOMF_enum_async_invocation)
	{
		Result = clSetKernelArg(context->clkernel,	KERNEL_ARG_ORDER_INPUT, sizeof(void *), (void*) &(context->opencl_input_buffer[current_buffer]));
		if (Result != CL_SUCCESS)
			return _GOOMF_CL_KERNEL_RUN_ERROR;
	}

	//before the usage - map the buffer back to device space
	clEnqueueUnmapMemObject(context->clqueue,
			context->opencl_input_buffer[current_buffer],
			context->buffer[current_buffer],
			0, NULL, NULL);
	//before the usage - map the used parameters struct back to device space
	clEnqueueUnmapMemObject(context->clqueue,
			context->parameters_struct_buffer,
			context->parameters_struct_ptr,
			0, NULL, NULL);
	//kernel invocation
	Result = clEnqueueNDRangeKernel(context->clqueue,
							context->clkernel,
							1,
							NULL,
							&(context->current_buffer_size[current_buffer]),
							NULL,
							0,
							NULL,
							NULL);
	//after the usage - map the buffer back to the user space
	context->buffer[current_buffer] = clEnqueueMapBuffer(context->clqueue,
			context->opencl_input_buffer[current_buffer],
			true,
			CL_MAP_WRITE,
			0,
			context->max_buffer_size * _GOOMF_get_program_state_size(),
			0, NULL, NULL, NULL);
	//after the usage - map the used parameters struct back to the user space
	context->parameters_struct_ptr = clEnqueueMapBuffer(context->clqueue,
			context->parameters_struct_buffer,
			true,
			CL_MAP_WRITE,
			0,
			sizeof(struct _GOOMF_used_params_struct),
			0, NULL, NULL, NULL);
	if (Result != CL_SUCCESS)
		return _GOOMF_CL_KERNEL_RUN_ERROR;

	//waiting for completion (blocking call)
	Result = clFinish(context->clqueue);
	if (Result != CL_SUCCESS)
		return _GOOMF_CL_KERNEL_RUN_ERROR;

	//for finite properties check if some transitions happened
	if (context->alg_type == _GOOMF_enum_alg_finite)
	{
		//reading number of transitions happened
		Result = clEnqueueReadBuffer(context->clqueue,
										context->num_of_transitions_output_buffer,
										CL_TRUE,
										0,
										getNumOfProperties() * (context->max_buffer_size / _GLOBAL_local_group_size) * sizeof(cl_uint),
										context->num_of_transitions_output_ptr,
										0, NULL, NULL);
		if (Result != CL_SUCCESS)
			return _GOOMF_CL_MEMORY_READ_ERROR;

		int j = 0;
		for (i=0; i<getNumOfProperties(); ++i)
		{
			//now go over the number of groups
			for (j=0; j<_GLOBAL_groups_number; ++j)
			{
				if (context->num_of_transitions_output_ptr[i*_GLOBAL_groups_number + j] < context->current_buffer_size[current_buffer])
					break;
			}
			if (j < _GLOBAL_groups_number)	//transition happened
				break;
		}

		if (i >= getNumOfProperties())
		{
			*transition_happened = false;
			return _GOOMF_SUCCESS;	// no transition happened
		}
	}

	fsm_state* output_ptr;
	if (context->alg_type == _GOOMF_enum_alg_infinite)
	{
		int size = getNumOfInconclusiveStates(-1) * context->max_buffer_size * sizeof(fsm_state);
		output_ptr = (fsm_state*)clEnqueueMapBuffer(context->clqueue,
											context->output_states_buffer,
											CL_TRUE,
											CL_MAP_READ,
											0,
											size,
											0, NULL, NULL, &ErrorCode);
		if (output_ptr == NULL || ErrorCode != CL_SUCCESS)
		{
			*transition_happened = true;
			return _GOOMF_CL_MEMORY_READ_ERROR;
		}

		//pass over output states, calculate new state and report if conclusive state reached
		for (i=0; i<getNumOfProperties(); ++i)
		{
			//calculate next state only if property is enabled
			if (property_triggers[i])
				findNextState(context, output_ptr, i, current_buffer);
		}

		//by default transition happens in infinite algorithm
		*transition_happened = true;
		return _GOOMF_SUCCESS;
	}
	else
	{
		int j = 0;
		for (i=0; i<getNumOfProperties(); ++i)
		{
			//now go over the number of groups
			for (j=0; j<_GLOBAL_groups_number; ++j)
			{
				if (context->num_of_transitions_output_ptr[i*_GLOBAL_groups_number + j] < context->current_buffer_size[current_buffer])	//take only those props where transition happened
				{
					((*(context->transitions)[i]))
							(context->current_fsm_states_ptr + i,
							context->buffer[current_buffer] + context->num_of_transitions_output_ptr[i*_GLOBAL_groups_number + j],
							NULL,
							context->parameters_struct_ptr);
					context->current_indices_output_ptr[i] = context->num_of_transitions_output_ptr[i*_GLOBAL_groups_number + j] + 1;
					break;
				}
			}
		}

		//check if all properties converged/reached last index and report if needed
		if (isConverged(context, current_buffer) == true)
		{
			*transition_happened = true;
			return _GOOMF_SUCCESS;
		}

		//writing current index back to GPU memory
		Result = clEnqueueWriteBuffer(context->clqueue,
										context->current_indices_output_buffer,
										CL_TRUE,
										0,
										getNumOfProperties() * sizeof(cl_uint),
										context->current_indices_output_ptr,
										0, NULL, NULL);
		if (Result != CL_SUCCESS)
		{
			*transition_happened = true;
			return _GOOMF_CL_MEMORY_WRITE_ERROR;
		}

		//writing current states back to GPU memory
		Result = clEnqueueWriteBuffer(context->clqueue,
										context->current_fsm_states_buffer,
										CL_TRUE,
										0,
										getNumOfProperties() * sizeof(fsm_state),
										context->current_fsm_states_ptr,
										0, NULL, NULL);
		if (Result != CL_SUCCESS)
		{
			*transition_happened = true;
			return _GOOMF_CL_MEMORY_WRITE_ERROR;
		}

		//if algorithm 2 and chunk not processed completely
		return processChunk(context, transition_happened, current_buffer);
	}
}


/*
 * Process current chunk of program states with partial algorithm
 */
static int processPartialChunk(_GOOMF_context context, unsigned int current_buffer, fsm_state** output_ptr)
{
	cl_int Result = CL_SUCCESS, ErrorCode = CL_SUCCESS;

	//update kernel argument only if asynchronous invocation is present
	if (context->invocation_type == _GOOMF_enum_async_invocation)
	{
		Result = clSetKernelArg(context->clkernel, KERNEL_ARG_ORDER_INPUT, sizeof(void *), (void*)(&(context->opencl_input_buffer[current_buffer])));
		if (Result != CL_SUCCESS)
			return _GOOMF_CL_KERNEL_RUN_ERROR;
	}

	if (context->current_buffer_size[current_buffer] != context->max_buffer_size)
	{
		return _GOOMF_SUCCESS;
	}

	//before the usage - map the buffer back to device space
	clEnqueueUnmapMemObject(context->clqueue,
			context->opencl_input_buffer[current_buffer],
			context->buffer[current_buffer],
			0, NULL, NULL);
	//kernel invocation
	Result = clEnqueueNDRangeKernel(context->clqueue,
							context->clkernel,
							1,
							NULL,
							&(context->current_buffer_size[current_buffer]),
							NULL,
							0,
							NULL,
							NULL);
	//after the usage - map the buffer back to the user space
	context->buffer[current_buffer] = clEnqueueMapBuffer(context->clqueue,
			context->opencl_input_buffer[current_buffer],
			true,
			CL_MAP_WRITE,
			0,
			context->max_buffer_size * _GOOMF_get_program_state_size(),
			0, NULL, NULL, NULL);
	if (Result != CL_SUCCESS)
		return _GOOMF_CL_KERNEL_RUN_ERROR;

	//waiting for completion (blocking call)
	Result = clFinish(context->clqueue);
	if (Result != CL_SUCCESS)
		return _GOOMF_CL_KERNEL_RUN_ERROR;

	*output_ptr = (fsm_state*)clEnqueueMapBuffer(context->clqueue,
										context->output_states_buffer,
										CL_TRUE,
										CL_MAP_READ,
										0,
										context->max_buffer_size * _GLOBAL_num_of_partial_predicates * sizeof(fsm_state),
										0, NULL, NULL, &ErrorCode);
	if ((*output_ptr) == NULL || ErrorCode != CL_SUCCESS)
		return _GOOMF_CL_MEMORY_READ_ERROR;

	return _GOOMF_SUCCESS;
}

/*
 * Process current chunk of program states with partial algorithm
 */
static int sequentialPartialPass(_GOOMF_context context, fsm_state* output_ptr, unsigned int current_buffer)
{
	if (context == NULL)
		return _GOOMF_NO_CONTEXT_ERROR;

	if (output_ptr == NULL || current_buffer < 0 || current_buffer > 1)
		return _GOOMF_ARGUMENT_ERROR;

	int i = 0, j = 0;
	unsigned int partial_index = 0;
	//pass over output states, calculate new state and report if conclusive state reached
	for (i=0; i<context->current_buffer_size[current_buffer]; ++i)
	{
		for (j=0; j<context->num_of_props; ++j)
		{
			//perform monitoring only if property is enabled and has not converged yet
			if (property_triggers[j] && !isStateConclusive(context->current_fsm_states_ptr[j]))
			{
				if (isPropertyPartial(j))
					((*(context->transitions)[j]))
						(&(context->current_fsm_states_ptr[j]),
						&(context->buffer[current_buffer][i]),
						(bool*)(output_ptr + partial_index),
						context->parameters_struct_ptr);
				else
					((*(context->transitions)[j]))
						(&(context->current_fsm_states_ptr[j]),
						&(context->buffer[current_buffer][i]),
						NULL,
						context->parameters_struct_ptr);

				//report if state is conclusive
				if (isStateConclusive(context->current_fsm_states_ptr[j]))
				{
					if (context->report_handlers_array[j] != NULL)
						(context->report_handlers_array[j])
							(j, context->current_fsm_states_ptr[j] ^ 0x00,
							&(context->buffer[current_buffer][i]));
				}
			}
		}
		partial_index += _GLOBAL_num_of_partial_predicates;
	}
	context->current_buffer_size[current_buffer] = 0;
	//if (context->invocation_type == _GOOMF_enum_async_invocation)
	//	context->live_buffer_processing[current_buffer] = false;

	return _GOOMF_SUCCESS;
}

/*
 * Flush and process the program states that are in the buffer
 */
int flushGPU(_GOOMF_context context, unsigned int current_buffer)
{
	bool transition_happened = false;
	cl_int Result = CL_SUCCESS;
	int i = 0;

	if (context == NULL)
		return _GOOMF_NO_CONTEXT_ERROR;

	if (context->current_buffer_size[current_buffer] == 0)
		return _GOOMF_SUCCESS;

	if (_GLOBAL_debug)
	{
		//_GLOBAL_log = fopen("log.txt", "a");
		printf("Thread %d: inside GPU resource;\n", current_buffer);
		//fclose(_GLOBAL_log);
	}

	//update number of items for processing
	if (context->current_buffer_size[current_buffer] != context->max_buffer_size)
	{
		unsigned int arg_number = KERNEL_ARG_ORDER_ITEMS;
		if (context->alg_type == _GOOMF_enum_alg_partial_offload)
			arg_number = 2;
		Result = clSetKernelArg(context->clkernel,
				arg_number,
				sizeof(context->current_buffer_size[current_buffer]),
				(void*)(&context->current_buffer_size[current_buffer]));
		if (Result != CL_SUCCESS)
			return _GOOMF_CL_KERNEL_ARGUMENT_ERROR;
	}

	//if property is disabled - set its current index to the last number, otherwise to 0
	if (context->alg_type != _GOOMF_enum_alg_partial_offload)
	{
		bool indicesChanged = false;
		for (i=0; i<context->num_of_props; ++i)
		{
			if (property_triggers[i] == false)
				context->current_indices_output_ptr[i] = context->current_buffer_size[current_buffer];
			else
				context->current_indices_output_ptr[i] = 0;
		}

		if (indicesChanged)
		{
			Result = clEnqueueWriteBuffer(context->clqueue,
											context->current_indices_output_buffer,
											CL_TRUE,
											0,
											getNumOfProperties() * sizeof(cl_uint),
											context->current_indices_output_ptr,
											0, NULL, NULL);
			if (Result != CL_SUCCESS)
				return _GOOMF_CL_MEMORY_WRITE_ERROR;
		}
	}

	/*
	//writing current index back to GPU memory
	Result = clEnqueueWriteBuffer(context->clqueue,
									context->current_indices_output_buffer,
									CL_TRUE,
									0,
									getNumOfProperties() * sizeof(cl_uint),
									context->current_indices_output_ptr,
									0, NULL, NULL);*/

	fsm_state* output_ptr;
	if (context->alg_type != _GOOMF_enum_alg_partial_offload)
	{
		Result = processChunk(context, &transition_happened, current_buffer);
		context->current_buffer_size[current_buffer] = 0;
	}
	else
		Result = processPartialChunk(context, current_buffer, &output_ptr);
	if (Result != CL_SUCCESS)
		return Result;

	if (context->alg_type != _GOOMF_enum_alg_partial_offload && transition_happened)
	{//update indices and fsm states for the next iteration
		//only for non-cyclic properties: if transition happened - update indexes
		if (context->alg_type == _GOOMF_enum_alg_finite)
		{
			//writing current index back to GPU memory
			memset(context->current_indices_output_ptr, 0, getNumOfProperties() * sizeof(cl_uint));
			Result = clEnqueueWriteBuffer(context->clqueue,
											context->current_indices_output_buffer,
											CL_TRUE,
											0,
											getNumOfProperties() * sizeof(cl_uint),
											context->current_indices_output_ptr,
											0, NULL, NULL);
			if (Result != CL_SUCCESS)
				return _GOOMF_CL_MEMORY_WRITE_ERROR;
		}

		//writing current state back to GPU memory
		Result = clEnqueueWriteBuffer(context->clqueue,
										context->current_fsm_states_buffer,
										CL_TRUE,
										0,
										getNumOfProperties() * sizeof(fsm_state),
										context->current_fsm_states_ptr,
										0, NULL, NULL);
		if (Result != CL_SUCCESS)
			return _GOOMF_CL_MEMORY_WRITE_ERROR;
	}
	else if (context->alg_type == _GOOMF_enum_alg_partial_offload)
	{
		Result = sequentialPartialPass(context, output_ptr, current_buffer);
		if (Result != CL_SUCCESS)
			return _GOOMF_CL_MEMORY_WRITE_ERROR;
	}

	if (_GLOBAL_debug)
	{
		//_GLOBAL_log = fopen("log.txt", "a");
		printf("Thread %d: outside GPU resource;\n", current_buffer);
		//fclose(_GLOBAL_log);
	}
	return _GOOMF_SUCCESS;
}


/*
 * Perform verification on the buffer of program states
 */
int _GOOMF_flush(_GOOMF_context context)
{
	if (context == NULL)
		return _GOOMF_NO_CONTEXT_ERROR;

	if (context->invocation_type == _GOOMF_enum_sync_invocation)
	{	//synchronous invocation
		switch (context->alg_type)
		{
		case (_GOOMF_enum_alg_seq):
			flushCPU(context, context->current_buffer);
			break;
		case (_GOOMF_enum_alg_partial_offload):
			flushGPU(context, context->current_buffer);
			break;
		default:
			flushGPU(context, context->current_buffer);
		}
	}
	else
	{	//asynchronous invocation
		context->current_buffer = (context->current_buffer + 1) % 2;
		sem_post(&(context->buffer_is_full[context->current_buffer]));

		if (_GLOBAL_debug)
		{
			//_GLOBAL_log = fopen("log.txt", "a");
			printf("Main thread: buffer flushed;\n");
			//fclose(_GLOBAL_log);
		}

		/*
		//sync_is_alive indicates that parallel trace processing is taking place
		//context->live_buffer_processing[context->current_buffer] = true;

		_GLOBAL_args_struct.context = context;
		_GLOBAL_args_struct.current_buffer = context->current_buffer;

		pthread_t p;
		if (pthread_create(&p, NULL, wrapFunction, (void*)(&_GLOBAL_args_struct)) != 0)
			return _GOOMF_THREAD_ERROR;
		context->workers[context->current_buffer] = p;
		*/
	}

	return _GOOMF_SUCCESS;
}


int _GOOMF_nextState(_GOOMF_context context, void* program_state_ptr)
{
	if (context == NULL)
		return _GOOMF_NO_CONTEXT_ERROR;

	if (program_state_ptr == NULL)
		return _GOOMF_ARGUMENT_ERROR;

	int bindex = 0;
	if (context->invocation_type == _GOOMF_enum_async_invocation)
		bindex  = (context->current_buffer +1) % 2;

	if (_GLOBAL_debug)
	{
		//_GLOBAL_log = fopen("log.txt", "a");
		printf("Main thread: going to add element no. %d to buffer no. %d;\n",
				context->current_buffer_size[bindex], bindex);
		//fclose(_GLOBAL_log);
	}

	//copy the structure limited by the size of the program state
	if (context->invocation_type == _GOOMF_enum_async_invocation)
	{
		while (context->current_buffer_size[bindex] >= context->max_buffer_size);
		sem_wait(&(context->buffer_mutex[bindex]));
	}
	copyProgramState(&(context->buffer[bindex][context->current_buffer_size[bindex]]), program_state_ptr);
	//memcpy(&(context->last_program_state), &(context->buffer[bindex][context->current_buffer_size[bindex]]),
	//		sizeof(struct _GOOMF_device_program_state_struct));
	++(context->current_buffer_size[bindex]);
	if (context->invocation_type == _GOOMF_enum_async_invocation)
		sem_post(&(context->buffer_mutex[bindex]));

	switch(context->trigger_type)
	{
	case (_GOOMF_enum_no_trigger):
			break;	//user invokes _GOOMF_flush()
	case (_GOOMF_enum_time_trigger):
			break;
	default: 	//buffer trigger
		if (context->current_buffer_size[bindex] >= context->max_buffer_size)
			return _GOOMF_flush(context);
	}

	return _GOOMF_SUCCESS;
}

int _GOOMF_nextEvent(_GOOMF_context context, unsigned int var_order, void* var_value)
{
	if (context == NULL)
		return _GOOMF_NO_CONTEXT_ERROR;

	if (var_value == NULL)
		return _GOOMF_ARGUMENT_ERROR;

	int bindex = 0;
	if (context->invocation_type == _GOOMF_enum_async_invocation)
		bindex  = (context->current_buffer +1) % 2;

	//copy the structure limited by the size of the program state
	_GOOMF_set_var(&(context->last_program_state), var_order, var_value);

	if (context->invocation_type == _GOOMF_enum_async_invocation)
		sem_wait(&(context->buffer_mutex[bindex]));
	memcpy(&(context->buffer[bindex][context->current_buffer_size[bindex]]), &(context->last_program_state),
		sizeof(struct _GOOMF_device_program_state_struct));
	if (context->invocation_type == _GOOMF_enum_async_invocation)
		sem_post(&(context->buffer_mutex[bindex]));

	++(context->current_buffer_size[bindex]);

	switch(context->trigger_type)
	{
	case (_GOOMF_enum_no_trigger):
			break;	//user invokes _GOOMF_flush()
	case (_GOOMF_enum_time_trigger):
			break;
	default: 	//buffer trigger
		if (context->current_buffer_size[bindex] >= context->max_buffer_size)
			return _GOOMF_flush(context);
	}

	return _GOOMF_SUCCESS;
}

/*
 * Get current monitoring status from context
 */
int _GOOMF_getCurrentStatus(_GOOMF_context context, _GOOMF_enum_verdict_type* verdict)
{
	if (context == NULL)
		return _GOOMF_NO_CONTEXT_ERROR;

	int i = 0;
	for (i=0; i<context->num_of_props; ++i)
		verdict[i] = context->current_fsm_states_ptr[i] % 4;

	return _GOOMF_SUCCESS;
}


/*
 * Translate a verdict type to a string
 */
int _GOOMF_typeToString(_GOOMF_enum_verdict_type* verdict, char* str)
{
	if (verdict == NULL || str == NULL)
		return _GOOMF_ARGUMENT_ERROR;

	if (*verdict == _GOOMF_enum_sat)
		strcpy(str, "satisfied");
	else if (*verdict == _GOOMF_enum_psat)
		strcpy(str, "probably satisfied");
	else if(*verdict == _GOOMF_enum_pviol)
		strcpy(str, "probably violated");
	else
		strcpy(str, "violated");

	return _GOOMF_SUCCESS;
}


/*
 * Return true only if all the properties came to conclusion
 */
bool _GOOMF_allPropertiesConverged(_GOOMF_context context)
{
	if (context == NULL)
		return false;

	int i = 0;
	for (i=0; i<context->num_of_props; ++i)
	{
		if (!isStateConclusive(context->current_fsm_states_ptr[i]))
			return false;
	}

	return true;
}


int _GOOMF_registerReport(_GOOMF_context context, int prop_num, report_handler_type rh)
{
	if (context == NULL)
		return _GOOMF_NO_CONTEXT_ERROR;

	if (prop_num < 0 || prop_num > getNumOfProperties()-1 || rh == NULL)
		return _GOOMF_ARGUMENT_ERROR;

	context->report_handlers_array[prop_num] = rh;

	return _GOOMF_SUCCESS;
}


int _GOOMF_unregisterReport(_GOOMF_context context, int prop_num)
{
	if (context == NULL)
		return _GOOMF_NO_CONTEXT_ERROR;

	if (prop_num < 0 || prop_num > getNumOfProperties()-1)
		return _GOOMF_ARGUMENT_ERROR;

	context->report_handlers_array[prop_num] = NULL;

	return _GOOMF_SUCCESS;
}


int _GOOMF_enableProperty(_GOOMF_context context, int prop_num)
{
	if (context == NULL)
		return _GOOMF_NO_CONTEXT_ERROR;

	if (prop_num < 0 || prop_num > getNumOfProperties()-1)
		return _GOOMF_ARGUMENT_ERROR;

	property_triggers[prop_num] = true;

	return _GOOMF_SUCCESS;
}


int _GOOMF_disableProperty(_GOOMF_context context, int prop_num)
{
	if (context == NULL)
		return _GOOMF_NO_CONTEXT_ERROR;

	if (prop_num < 0 || prop_num > getNumOfProperties()-1)
		return _GOOMF_ARGUMENT_ERROR;

	property_triggers[prop_num] = false;

	return _GOOMF_SUCCESS;
}


int _GOOMF_resetProperty(_GOOMF_context context, int prop_num)
{
	if (context == NULL)
		return _GOOMF_NO_CONTEXT_ERROR;

	if (prop_num < 0 || prop_num > getNumOfProperties()-1)
		return _GOOMF_ARGUMENT_ERROR;

	context->current_fsm_states_ptr[prop_num] = getInitialState(prop_num);

	return _GOOMF_SUCCESS;
}


int _GOOMF_setLogger(_GOOMF_context context, FILE* logger)
{
	if (context == NULL)
		return _GOOMF_NO_CONTEXT_ERROR;

	if (logger == NULL)
		return _GOOMF_ARGUMENT_ERROR;

	context->logger = logger;
	return _GOOMF_SUCCESS;
}


int _GOOMF_analyze(open_handler oh_callback,
					get_next_state_handler gnsh_callback,
					close_handler ch_callback,
					report_handler_type rh_callback,
					_GOOMF_enum_trigger_type trigger_type,
					//_GOOMF_enum_device_type device_type,
					_GOOMF_enum_alg_type alg_type,
					_GOOMF_enum_invocation_type invocation_type,
					FILE* logger,
					unsigned int buffer_size)
{
	_GOOMF_context 	goomf_context;
	int Result = 	_GOOMF_SUCCESS, i = 0;

	//allocation phase
	if (oh_callback == NULL || gnsh_callback == NULL || ch_callback == NULL || rh_callback == NULL)
		return _GOOMF_CALLBACK_ERROR;

	//create context with synchronous invocation
	Result = _GOOMF_initContext(&goomf_context,
			trigger_type,
			//device_type,
			alg_type,
			invocation_type,
			buffer_size);
	if (Result != _GOOMF_SUCCESS)
		return Result;

	//set the logger if needed
	if (logger != NULL)
	{
		Result = _GOOMF_setLogger(goomf_context, logger);
		if (Result != _GOOMF_SUCCESS)
			return Result;
	}

	if (oh_callback() < 0)
		return _GOOMF_CALLBACK_ERROR;

	//register all report callbacks as the same report
	for (i=0; i<goomf_context->num_of_props; ++i)
		goomf_context->report_handlers_array[i] = rh_callback;

	//processing phase
	struct _GOOMF_host_program_state_struct next_host_program_state;
	memset(&next_host_program_state, 0, sizeof(next_host_program_state));

	while (gnsh_callback((void*)(&next_host_program_state)) >= 0)
		_GOOMF_nextState(goomf_context, (void*)(&next_host_program_state));

	//deallocation phase
	Result = _GOOMF_destroyContext(&goomf_context);
	if (Result != _GOOMF_SUCCESS)
		return Result;

	if (ch_callback() < 0)
		return _GOOMF_CALLBACK_ERROR;
	return _GOOMF_SUCCESS;
}

int _GOOMF_getErrorDescription(int result, char* message)
{
	switch (result)
	{
	case _GOOMF_NO_CONTEXT_ERROR:
		strcpy(message, "GooMF no context error.");
		break;
	case _GOOMF_MALLOC_ERROR:
		strcpy(message, "GooMF cannot allocate/free memory error.");
		break;
	case _GOOMF_ARGUMENT_ERROR:
		strcpy(message, "GooMF wrong argument to the function error.");
		break;
	case _GOOMF_CL_CONTEXT_ERROR:
		strcpy(message, "OpenCL context error.");
		break;
	case _GOOMF_CL_DEVICE_ERROR:
		strcpy(message, "OpenCL device error.");
		break;
	case _GOOMF_CL_PROGRAM_ERROR:
		strcpy(message, "OpenCL program error.");
		break;
	case _GOOMF_CL_BUILD_ERROR:
		strcpy(message, "OpenCL build error.");
		break;
	case _GOOMF_CL_QUEUE_ERROR:
		strcpy(message, "OpenCL queue error.");
		break;
	case _GOOMF_CL_KERNEL_ERROR:
		strcpy(message, "OpenCL kernel error.");
		break;
	case _GOOMF_CL_ALLOCATE_ERROR:
		strcpy(message, "OpenCL buffer allocation error.");
		break;
	case _GOOMF_CL_KERNEL_ARGUMENT_ERROR:
		strcpy(message, "OpenCL kernel argument error.");
		break;
	case _GOOMF_CL_MEMORY_WRITE_ERROR:
		strcpy(message, "OpenCL memory write error.");
		break;
	case _GOOMF_CL_KERNEL_RUN_ERROR:
		strcpy(message, "OpenCL kernel run error.");
		break;
	case _GOOMF_CL_MEMORY_READ_ERROR:
		strcpy(message, "OpenCL memory read error.");
		break;
	case _GOOMF_CALLBACK_ERROR:
		strcpy(message, "GooMF callback error.");
		break;
	case _GOOMF_BUFFER_SIZE_ERROR:
		strcpy(message, "GooMF wrong verification buffer size error.");
		break;
	case _GOOMF_THREAD_ERROR:
		strcpy(message, "GooMF multithreading error.");
		break;
	case _GOOMF_PARTIAL_OFFLOAD_ERROR:
		strcpy(message, "Partial offload algorithms chosen, but there is no partial-marked predicates in properties file.");
		break;
	default:
		strcpy(message, "GooMF unknown error.");
		break;
	}

	return _GOOMF_SUCCESS;
}
