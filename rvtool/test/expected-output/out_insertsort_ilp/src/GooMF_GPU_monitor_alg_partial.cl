typedef struct{
	int i;
	int j;
} program_state_struct;

//saving state in char: first 6 bits state and last 2 bits type
typedef unsigned char fsm_state;

void copyProgramState(program_state_struct* ps1, const __global program_state_struct* ps2)
{
	if (ps1 == 0 || ps2 == 0)
		return;
	
	ps1->i = ps2->i;
	ps1->j = ps2->j;
}


//kernel code
__kernel void verification(__global program_state_struct const * restrict input_buffer,
				__global fsm_state *output_buffer,
				uint nitems_global) 
{ 

	uint nitems = nitems_global;
	int global_id = get_global_id(0);
	int work_per_thread = nitems / get_global_size(0);
		
	program_state_struct temp_program_state;
	


	//translation from work-item units to actual data
	for (int j=(global_id*work_per_thread); j<((global_id+1)*work_per_thread); ++j)
	{
		copyProgramState(&temp_program_state, &input_buffer[j]);


	}

	return;
}
