/*
 ============================================================================
 Name        : GooMFOfflineAPI.c
 Author      : Shay Berkovich
 Version     : 1.1
 Date		 : 3/07/2012
 Copyright   : Real-Time Embedded Software Group, University of Waterloo
 Description : Offline API for GooMF
 ============================================================================
 */
#ifndef GOOMFOFFLINEAPI_H_
#define GOOMFOFFLINEAPI_H_
#include "GooMFOnlineAPI.h"


/**************************************** function callbacks types *****************************************/
/*
 * This function is called to open input data stream.
 * - Should return negative number in case of failure or non-negative in case of success
 */
typedef int (*open_handler)();

/*
 * This function is called to parse next program state from input data stream.
 * - The next program state should be placed into next_program_state.
 * - Should return negative number in case of failure or non-negative in case of success.
 */
typedef int (*get_next_state_handler)(void* next_program_state_ptr);

/*
 * This function is called to close the input data stream.
 * - Should return negative number in case of failure or non-negative in case of success
 */
typedef int (*close_handler)();

/*
 * This function is called upon any property convergence; should be concise as it called during the processing
 * - prop_num - number of converged property
 * - verdict_type - violation/satisfaction
 * - program_state - program state that caused the convergence
 * - Should return negative number in case of failure or non-negative in case of success.
 */
typedef int (*report_handler_type)(int prop_num, _GOOMF_enum_verdict_type verdict_type, const void* program_state);



/*************************************** functions ***********************************************************/
/*
 * Analyzer function - uses provided callback functions to analyze the input data stream
 * - trigger_type - triggers the program trace flush
 * - device type - device the monitoring is performed on
 * - buffer_size - if trigger is _GOOMF_enum_buffer_trigger, specify the maximal size of the buffer
 */
extern int _GOOMF_analyze(open_handler oh_callback,
					get_next_state_handler gnsh_callback,
					close_handler ch_callback,
					report_handler_type rh,
					_GOOMF_enum_trigger_type trigger_type,
					//_GOOMF_enum_device_type device_type,
					_GOOMF_enum_alg_type alg_type,
					_GOOMF_enum_invocation_type invocation_type,
					FILE* logger,
					unsigned int buffer_size);

#endif /* GOOMFOFFLINEAPI_H_ */
