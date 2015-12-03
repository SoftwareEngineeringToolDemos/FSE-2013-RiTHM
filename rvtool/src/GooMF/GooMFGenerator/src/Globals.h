#ifndef _GLOBALS_
#define _GLOBALS_

#define MAX_LINE_SIZE			(1000)
#define MAX_STATES_IN_FSM 		(64)
#define MAX_TRANSITIONS_IN_FSM 	(100)
#define MAX_CHARS_IN_LABEL		(50)
#define MAX_ALPHABET_SIZE		(50)
#define MAX_FUNCTION_SIZE		(10000)


//type of the FSM state
typedef enum{enmST_PYes, enmST_PNo, enmST_Yes, enmST_No} state_type_enm;
typedef char label_type[MAX_CHARS_IN_LABEL];
typedef enum{alg_finite, alg_infinite} alg_type_enum;
typedef enum{fsm_as_table, fsm_as_ifs} fsm_format_enum;

//FSM_State declaration
typedef struct{
	int number1;		//for conformance with output of ltl2mon
	int number2;		//for conformance with output of ltl2mon
	unsigned char id;	//unique id
	state_type_enm state_type;
} fsm_state_struct;

typedef struct{
	fsm_state_struct state_from;
	fsm_state_struct state_to;
	char action[MAX_LINE_SIZE];
	label_type label;
} fsm_transition_struct;

typedef struct{
	fsm_state_struct fsm_states_buffer[MAX_STATES_IN_FSM];
	fsm_transition_struct fsm_transitions_buffer[MAX_TRANSITIONS_IN_FSM];
	label_type labels_buffer[MAX_ALPHABET_SIZE];
	int num_of_fsm_states;
	int num_of_fsm_transitions;
	int num_of_labels;
} fsm_struct;

typedef struct{
	const char* name;
	const char* formalism;
	const char* syntax;
	fsm_struct fsm;
} property_struct;

typedef struct{
	const char* name;
	const char* syntax;
	bool isPartial;		//if this predicate is calculated on the GPU side
} predicate_struct;

typedef struct{
	const char* name;
	const char* type;
	bool isPartial;		//if program var participates in partial evaluation on the GPU side
	bool isParam;
} program_var_struct;

//constants
const char* monitor_file_CPU = "GooMF_CPU_monitor.h";
const char* program_state_file = "ProgramState.h";
const char* monitor_file_GPU_alg_finite = "GooMF_GPU_monitor_alg_finite.cl";
const char* monitor_file_GPU_alg_infinite = "GooMF_GPU_monitor_alg_infinite.cl";
const char* monitor_file_GPU_alg_partial = "GooMF_GPU_monitor_alg_partial.cl";

const char* true_state_color = "green";
const char* presumably_true_state_color = "darkseagreen1";
const char* presumably_false_state_color = "pink";
const char* false_state_color = "red";

const char* _GLOBAL_LTL_FORMALISM = "LTL";
const char* _GLOBAL_FSM_FORMALISM = "FSM";
const char* _GLOBAL_config_file_name = NULL;
const char* _GLOBAL_LTL_tools_path = NULL;
const char* _GLOBAL_templates_path = NULL;
const char* _GLOBAL_program_name = NULL;
const char* _GLOBAL_device_type = NULL;
const char* _GLOBAL_output_path = NULL;
const char* _GLOBAL_monitoring_algorithm = NULL;
unsigned int _GLOBAL_num_of_props = 0;
unsigned int _GLOBAL_num_of_predicates = 0;
unsigned int _GLOBAL_num_of_program_vars = 0;
property_struct* _GLOBAL_properties_buffer;
predicate_struct* _GLOBAL_predicates_buffer;
program_var_struct* _GLOBAL_program_vars_buffer;
unsigned int _GLOBAL_max_chars_in_var = 0;
char** _GLOBAL_functions_buffer = NULL;
unsigned int _GLOBAL_num_of_functions = 0;
fsm_format_enum _GLOBAL_fsm_format = fsm_as_table;

#endif
