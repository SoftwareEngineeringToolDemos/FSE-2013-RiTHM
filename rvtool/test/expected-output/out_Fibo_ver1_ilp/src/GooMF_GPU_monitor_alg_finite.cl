#define VARS_IN_STATE		(2)

typedef struct{
	int i;
	int n;
} program_state_struct;

typedef struct{
	int dummy;
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
	
	ps1->i = ps2->i;
	ps1->n = ps2->n;
}



unsigned int getStateOrder(unsigned int num_of_prop, fsm_state s)
{
	if (num_of_prop == 0 && s == 5) return 0;
	if (num_of_prop == 0 && s == 3) return 1;

	return 0;
}

fsm_state getStateID(unsigned int num_of_prop, unsigned int o)
{
	if (num_of_prop == 0 && o == 0) return 5;
	if (num_of_prop == 0 && o == 1) return 3;

	return 0;
}
void transition0(bool a, fsm_state input_state, fsm_state* output_state)
{	//property "[] (a)"

	//transition branches
	if( input_state == 5 && ( a ))
		*output_state = 5;
	if( input_state == 5 && ( ! a ))
		*output_state = 3;
	if( input_state == 3 && ( a ))
		*output_state = 3;
	if( input_state == 3 && ( ! a ))
		*output_state = 3;
	else return;
}


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
	int group_id = get_group_id(0);
	int local_id = get_local_id(0);
	uint num_groups = get_num_groups(0);
	unsigned int pred_value = 0;
	
	program_state_struct temp_program_state;
	fsm_state temp_fsm_state1;
	fsm_state temp_fsm_state2;

	__local uint local_min0;

	if (local_id == 0)
	{
		local_min0 = nitems;
		num_of_transitions[0*num_groups + group_id] = nitems;
	}


	int i;
	int n;

	bool a;


	//property "[] (a)"
prop0: 
	if (global_id + work_per_thread <= cur_index[0]) 
		goto prop1;

	temp_fsm_state1 = current_state[0];

	//translation from work-item units to actual data
	for (int j=(global_id*work_per_thread); j<((global_id+1)*work_per_thread); ++j)
	{
		if (j < cur_index[0])
			continue;
		copyProgramState(&temp_program_state, &input_buffer[j]);

		i = temp_program_state.i;
		n = temp_program_state.n;

		a = (i <= n+1);

		transition0(a, temp_fsm_state1, &temp_fsm_state2);
		if( temp_fsm_state1 != temp_fsm_state2 )
		{
			atomic_min(&(local_min0), j);
			break;
		}
	}

prop1:

barrier(CLK_LOCAL_MEM_FENCE);
	if (local_id == 0)
	{
		num_of_transitions[0*num_groups + group_id] = local_min0;

	}
	return;
}
