typedef struct{
//insert1: description of program vars comes here
} program_state_struct;

//saving state in char: first 6 bits state and last 2 bits type
typedef unsigned char fsm_state;

void copyProgramState(program_state_struct* ps1, const __global program_state_struct* ps2)
{
	if (ps1 == 0 || ps2 == 0)
		return;
	
//insert2: copying the program variables
}

//insert3: user functions

//kernel code
__kernel void verification(__global program_state_struct const * restrict input_buffer,
				__global fsm_state *output_buffer,
				uint nitems_global) 
{ 

	uint nitems = nitems_global;
	int global_id = get_global_id(0);
	int work_per_thread = nitems / get_global_size(0);
		
	program_state_struct temp_program_state;
	
//insert4: kernel code for each property

	return;
}
