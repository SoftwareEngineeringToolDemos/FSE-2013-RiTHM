/*
 ============================================================================
 Name        : GooMF_API.h
 Author      : Shay Berkovich
 Version     :
 Copyright   : Real-Time Embedded Software Group, University of Waterloo
 Description : Implementation of worker threads in asynchronous mode
 ============================================================================
 */
#ifndef THREAD_POOL_H
#define THREAD_POOL_H
#include "Utils.h"

extern int pthread_join(pthread_t thread, void **value_ptr);
extern int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine)(void*), void *arg);
extern int flushGPU(_GOOMF_context context, unsigned int current_buffer);
extern int flushCPU(_GOOMF_context context, unsigned int current_buffer);
extern unsigned sleep(unsigned seconds);

#include "ThreadPool.h"

/*
wrap function for asynchronous flush
*/
static void* wrapFunction(void* ptr)
{
	//args_struct* as = (args_struct*)ptr;
	//unsigned int current_buffer = as->current_buffer;
	unsigned int current_buffer = *((int*)ptr);
	int Result = 0;
	struct timespec ts;
	ts.tv_sec =1;
	ts.tv_nsec = 0;

	//while(!(as->context)->kill_workers)
	while(!global_context->kill_workers)
	{
		//wait 1 sec on the semaphore
		Result = sem_timedwait(&(global_context->buffer_is_full[current_buffer]), &ts);
		if (Result == -1) continue;	//timed out

		//synchronizing exclusive access to gpu resources between worker threads
		sem_wait(&(global_context->gpu_resource_mutex));

		//synchronizing exclusive access to buffer between worker thread and main thread
		sem_wait(&(global_context->buffer_mutex[current_buffer]));

		if (_GLOBAL_debug)
		{
			//_GLOBAL_log = fopen("log.txt", "a");
			printf("Thread %d: inside the buffer;\n", current_buffer);
			//fclose(_GLOBAL_log);
		}

		switch (global_context->alg_type)
		{
		case (_GOOMF_enum_alg_seq):
			flushCPU(global_context, current_buffer);
			break;
		case (_GOOMF_enum_alg_partial_offload):
			flushGPU(global_context, current_buffer);
			break;
		default:
			flushGPU(global_context, current_buffer);
		}

		global_context->current_buffer_size[current_buffer] = 0;
		sem_post(&((global_context)->buffer_mutex[current_buffer]));
		sem_post(&(global_context->gpu_resource_mutex));

		//int sem;
		//sem_getvalue(&((global_context)->buffer_is_empty[current_buffer]), &sem);
		//if (sem < 0)
		//	sem_post(&((global_context)->buffer_is_empty[current_buffer]));

		if (_GLOBAL_debug)
		{
			//_GLOBAL_log = fopen("log.txt", "a");
			printf("Thread %d: outside the buffer;\n", current_buffer);
			//fclose(_GLOBAL_log);
		}

	}

	return NULL;
}


/*
 * init worker threads
 */
int createThreadPool(_GOOMF_context context)
{
	//_GLOBAL_args_struct.current_buffer = 0;
	//_GLOBAL_args_struct.context = context;
	int result = pthread_create(&(context->workers[0]), NULL, wrapFunction, (void*)(&global_current_buffer1));
	if (result != 0)
		return _GOOMF_THREAD_ERROR;

	sleep(1);

	//_GLOBAL_args_struct.current_buffer = 1;
	result = pthread_create(&(context->workers[1]), NULL, wrapFunction, (void*)(&global_current_buffer2));
	if (result != 0)
		return _GOOMF_THREAD_ERROR;

	return _GOOMF_SUCCESS;
}


/*
 * Initialize all semaphores and mutexes for asynchronous invocation
 */
int initSyncMechanisms(_GOOMF_context context)
{
	if (context->invocation_type == _GOOMF_enum_async_invocation)
	{
		sem_init(&(context->buffer_mutex[0]), 0, 1);
		sem_init(&(context->buffer_mutex[1]), 0, 1);
		sem_init(&(context->buffer_is_full[0]), 0, 0);
		sem_init(&(context->buffer_is_full[1]), 0, 0);
		//sem_init(&(context->buffer_is_empty[0]), 0, 0);
		//sem_init(&(context->buffer_is_empty[1]), 0, 0);
		sem_init(&(context->gpu_resource_mutex), 0, 1);
		return createThreadPool(context);
	}

	return _GOOMF_SUCCESS;
}

/*
 * Destroy semaphores if needed
 */
int destroySyncMechanisms(_GOOMF_context context)
{
	if (context->invocation_type == _GOOMF_enum_async_invocation)
	{
		sem_destroy(&(context->buffer_mutex[0]));
		sem_destroy(&(context->buffer_mutex[1]));
		sem_destroy(&(context->buffer_is_full[0]));
		sem_destroy(&(context->buffer_is_full[1]));
		//sem_destroy(&(context->buffer_is_empty[0]));
		//sem_destroy(&(context->buffer_is_empty[1]));
		sem_destroy(&(context->gpu_resource_mutex));

		//no need to destroy the workers as at this point they already done
	}

	return _GOOMF_SUCCESS;
}

#endif /* THREAD_POOL_H */
