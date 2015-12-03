//insert1: #define section comes here

typedef struct{
//insert2: description of program vars comes here
} program_state_struct;

typedef struct{
	int dummy;
//insert2.5: used parameters from all the properties
} used_params_struct;
//saving state in char: first 6 bits state and last 2 bits type
typedef unsigned char fsm_state;

bool ifStateConclusive(fsm_state state)
{
	if ((state ^ 0x00) == 0 || (state ^ 0x03) == 0)
		return true;
	return false;
}

bool compareNames(char* str1, __constant char* str2)
{
	int i = 0;
	while (str1[i] != '\0' && str2[i] != '\0')
	{
		if (str1[i] != str2[i])
			return false;
		++i;
	}
	if (str1[i] != str2[i])
		return false;
	return true;
}

void copyProgramState(program_state_struct* ps1, const __global program_state_struct* ps2)
{
	if (ps1 == 0 || ps2 == 0)
		return;
	
//insert3: copying the program variables
}

//insert4: user functions and transition functions

//kernel code
__kernel void verification(__global program_state_struct const * restrict input_buffer,
				__global fsm_state *output_buffer,
				__global fsm_state *current_state,
				__global uint *cur_index,
				__global uint *num_of_transitions,
				__global used_params_struct* used_params,
				uint nitems_global) 
{ 

	uint nitems = nitems_global;
	int global_id = get_global_id(0);
	int work_per_thread = nitems / get_global_size(0);

	program_state_struct temp_program_state;
	fsm_state temp_fsm_state;
	unsigned int pred_value = 0;
	
//insert5: kernel code for each property

	return;
}
