/*
 ============================================================================
 Name        : GooMFOnlineAPI.h
 Author      : Shay Berkovich
 Version     : 1.1
 Date		 : 3/07/2012
 Copyright   : Real-Time Embedded Software Group, University of Waterloo
 Description : Online API for GooMF
 ============================================================================
 */
#ifndef GOOMFONLINEAPI_H_
#define GOOMFONLINEAPI_H_

#include <stdbool.h>
#include <stdio.h>

#define MAIN_FUNCTION_NAME "verification"


/*************************************** errors declarations ***********************************************/
#define _GOOMF_SUCCESS 					(0)
#define _GOOMF_NO_CONTEXT_ERROR 		(-1)
#define _GOOMF_MALLOC_ERROR 			(-2)
#define _GOOMF_ARGUMENT_ERROR 			(-3)
#define _GOOMF_CL_CONTEXT_ERROR			(-4)
#define _GOOMF_CL_DEVICE_ERROR  		(-5)
#define _GOOMF_CL_PROGRAM_ERROR 		(-6)
#define _GOOMF_CL_QUEUE_ERROR   		(-7)
#define _GOOMF_CL_BUILD_ERROR   		(-8)
#define _GOOMF_CL_KERNEL_ERROR  		(-9)
#define _GOOMF_CL_ALLOCATE_ERROR 		(-10)
#define _GOOMF_CL_KERNEL_ARGUMENT_ERROR (-11)
#define _GOOMF_CL_MEMORY_WRITE_ERROR 	(-12)
#define _GOOMF_CL_KERNEL_RUN_ERROR		(-13)
#define _GOOMF_CL_MEMORY_READ_ERROR		(-14)
#define _GOOMF_CALLBACK_ERROR			(-15)
#define _GOOMF_BUFFER_SIZE_ERROR		(-16)
#define _GOOMF_THREAD_ERROR				(-17)
#define _GOOMF_PARTIAL_OFFLOAD_ERROR	(-18)


/******************************************* types declarations ********************************************/
//type for the fsm state
typedef enum{_GOOMF_enum_sat = 0, _GOOMF_enum_psat, _GOOMF_enum_pviol, _GOOMF_enum_viol} _GOOMF_enum_verdict_type;

//type for the computing device
//typedef enum{_GOOMF_enum_CPU = 0, _GOOMF_enum_GPU, _GOOMF_enum_APU} _GOOMF_enum_device_type;

//type for the trigger that triggers the flush of the program trace
typedef enum{_GOOMF_enum_no_trigger = 0, _GOOMF_enum_buffer_trigger, _GOOMF_enum_time_trigger} _GOOMF_enum_trigger_type;

//type for the GooMF_flush() invocation - synchronous (blocking) or asynchronous (non-blocking)
typedef enum{_GOOMF_enum_sync_invocation = 0, _GOOMF_enum_async_invocation} _GOOMF_enum_invocation_type;

//enum for algorithm type
typedef enum{_GOOMF_enum_alg_seq = 0,
				_GOOMF_enum_alg_partial_offload,
				_GOOMF_enum_alg_finite,
				_GOOMF_enum_alg_infinite} _GOOMF_enum_alg_type;

//type for GOOMF monitoring context structure that keeps the current state of the monitor
typedef struct _GOOMF_context_struct* _GOOMF_context;

//type for the callback function to call upon the property convergence
typedef int (*report_handler_type)(int prop_num, _GOOMF_enum_verdict_type verdict_type, const void* program_state);


/******************************************* functions declarations ****************************************/
/*
 * Allocates and initializes the monitoring context structure along with all other initialization stuff
 * - context - the structure to be allocated
 * - trigger_type - triggers the program trace flush
 * - device type - device the monitoring is performed on
 * - buffer_size - if trigger is _GOOMF_enum_buffer_trigger, specify the maximal size of the buffer
 */
extern int _GOOMF_initContext(_GOOMF_context* context,
							_GOOMF_enum_trigger_type trigger_type,
							//_GOOMF_enum_device_type device_type,
							_GOOMF_enum_alg_type alg_type,
							_GOOMF_enum_invocation_type invocation_type,
							int buffer_size);

/*
 * Destroys context structure allocated previously
 * - context - the structure to be destroyed
 */
extern int _GOOMF_destroyContext(_GOOMF_context* context);

/*
 * Copy next program state to the buffer and flush if needed
 * - context - GooMF monitoring context
 */
extern int _GOOMF_nextState(_GOOMF_context context, void* program_state_ptr);

/*
 * Process event, copy next program state to the buffer and flush if needed
 * - context - GooMF monitoring context
 * - var_order - order of the variable in program state that changed
 * - var_value - new value of the variable
 */
extern int _GOOMF_nextEvent(_GOOMF_context context, unsigned int var_order, void* var_value);

/*
 * Register report callback for specific property
 * - prop_num - report callback is called if this property has converged
 * - report_handler - callback function to call; should be concise as it called during the processing
 */
extern int _GOOMF_registerReport(_GOOMF_context context, int prop_num, report_handler_type report_handler);

/*
 * Unregister report callback for specific property
 * - prop_num - report callback is called if this property has converged
 * - report_handler - callback function to call
 */
extern int _GOOMF_unregisterReport(_GOOMF_context context, int prop_num);

/*
 * Enables monitoring of the specific property
 * - context - GooMF monitoring context
 * - prop_num - property to enable
 */
extern int _GOOMF_enableProperty(_GOOMF_context context, int prop_num);

/*
 * Disables monitoring of the specific property
 * - context - GooMF monitoring context
 * - prop_num - property to disable
 */
extern int _GOOMF_disableProperty(_GOOMF_context context, int prop_num);

/*
 * Resets the property to the initial monitoring state
 * - context - GooMF monitoring context
 * - prop_num - property to reset
 */
extern int _GOOMF_resetProperty(_GOOMF_context context, int prop_num);

/*
 * Force flush of the content of the buffer
 * - context - GooMF monitoring context
 */
extern int _GOOMF_flush(_GOOMF_context context);

/*
 * Get current status of all properties
 * - context - GooMF monitoring context
 * verdict - output array of the statuses; it is the responsibility of the caller to deallocate it
 */
extern int _GOOMF_getCurrentStatus(_GOOMF_context context, _GOOMF_enum_verdict_type* verdict);

/*
 * Translate verdict to string
 * - verdict - verdict
 * - str - output string containing the verdict phrase
 */
extern int _GOOMF_typeToString(_GOOMF_enum_verdict_type* verdict, char* str);

/*
 * Check if all properties has converged (either satisfied or violated)
 * - context - GooMF monitoring context
 */
extern bool _GOOMF_allPropertiesConverged(_GOOMF_context context);

/*
 * Set the logger to log the basic events
 * - context - GooMF monitoring context
 * - logger - file descriptor to write into. Assuming the descriptior is open.
 *            It is the responsibility of the caller to open and close the stream.
 */
extern int _GOOMF_setLogger(_GOOMF_context context, FILE* logger);

/*
 * Translate status to the meaningful sentence. It is the responsibility of the caller to allocate
 * a string for the message
 * - result - status
 */
extern int _GOOMF_getErrorDescription(int result, char* message);

#endif /* GOOMFONLINEAPI_H_ */
