#ifndef _MONITOR_CPU_
#define _MONITOR_CPU_

#include <stdio.h>
#include <stdbool.h>
#include <math.h>

struct _GOOMF_device_program_state_struct{
//insert2: description of program vars comes here
};
typedef struct _GOOMF_device_program_state_struct* _GOOMF_device_program_state_ptr;

struct _GOOMF_host_program_state_struct{
//insert2.2: description of program vars comes here
};
typedef struct _GOOMF_host_program_state_struct* _GOOMF_host_program_state_ptr;

struct _GOOMF_used_params_struct{
	int dummy;
//insert2.3: used parameters from all the properties
};
//saving state in char: first 6 bits state and last 2 bits type
typedef unsigned char fsm_state;

typedef void (*transition_type)(
		fsm_state* input_state,
		_GOOMF_device_program_state_ptr input_data,
		bool* isPartial,
		struct _GOOMF_used_params_struct*);
//insert2.4: global transitions array

void copyProgramState(_GOOMF_device_program_state_ptr ps1, _GOOMF_host_program_state_ptr ps2)
{
//insert2.6: copy program state values
}

int getNumOfProperties()
{
//insert3: number of properties
}

fsm_state getInitialState(int property_number)
{
//insert4: 	
	return 0;
}

//insert5: number of inconclusive states per property, getOrderOfState(), _GOOMF_fill_program_state

int _GOOMF_set_var(_GOOMF_device_program_state_ptr ps, unsigned int var_order, void* var_value)
{
//insert6: set single variable
}

//insert7: transition functions

void initFunctionPointers(transition_type* transitions)
{
//insert8: assigning function pointers
}

int _GOOMF_get_program_state_size()
{
//insert9: return size of the program_state_struct
}


#endif
