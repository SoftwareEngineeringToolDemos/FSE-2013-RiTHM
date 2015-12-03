#ifndef _GOOMF_INSTRUMENTOR_ 
#define _GOOMF_INSTRUMENTOR_ 


#include "GooMFOnlineAPI.h" 
#include "ProgramState.h" 


extern _GOOMF_context context; 
struct _GOOMF_host_program_state_struct current_program_state; 


extern int initContext(); 

extern void report(); 

extern int destroyContext(); 

#endif 
