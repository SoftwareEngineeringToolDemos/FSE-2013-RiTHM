#ifndef _GOOMF_INSTRUMENTOR_ 
#define _GOOMF_INSTRUMENTOR_ 


#include <GooMFOnlineAPI.h> 
#include "/Users/samaneh/Documents/Linux-Monitor/example/lms/ProgramState.h" 


_GOOMF_context context = NULL; 
struct _GOOMF_host_program_state_struct current_program_state; 


void initContext(){ 
_GOOMF_init_context(&context,_GOOMF_enum_buffer_trigger,_GOOMF_enum_alg_infinite,_GOOMF_enum_sync_invocation,4096); 
}; 


void destroyContext(){ 
_GOOMF_destroy_context(&context); 
}; 


#endif 
