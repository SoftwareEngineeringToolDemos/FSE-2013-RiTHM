/*
 ============================================================================
 Name        : Parsing.h
 Author      : Shay Berkovich
 Version     :
 Copyright   : Real-Time Embedded Software Group, University of Waterloo
 Description : Aggregates FSM parsing functions
 ============================================================================
 */
#ifndef FSMPARSING_H_
#define FSMPARSING_H_

#include "Globals.h"
#ifdef WINDOWS
    #include <direct.h>
    #define GetCurrentDir _getcwd
#else
    #include <unistd.h>
    #define GetCurrentDir getcwd
 #endif


int addLabelToAlphabet(label_type alphabet[], label_type label, int size)
{
	//we are interested only in single labels
	if ((alphabet == NULL) || (strcmp(label, "<empty>") == 0) || (strstr(label, "&&") != 0))
		return EXIT_FAILURE;

	int i = 0;
	for(i=0; i<size; ++i)
	{
		if (strcmp(alphabet[i], label) == 0)
			return EXIT_FAILURE;	//already in the alphabet
	}

	strcpy(alphabet[i], label);
	return EXIT_SUCCESS;
}


/*
 * Get state ID by numbers
 */
int getStateIDByNumbers(const fsm_state_struct* buffer, int size, int num1, int num2)
{
	int i = 0;
	for (i=0; i<size; ++i)
	{
		if (buffer[i].number1 == num1 && buffer[i].number2 == num2)
			return buffer[i].id;
	}

	return -1;
}

/*
 * Get state order by ID
 */
int getStateOrderByID(const fsm_state_struct* buffer, int size, unsigned char id)
{
	int i = 0;
	for (i=0; i<size; ++i)
	{
		if (buffer[i].id == id)
			return i;
	}

	return -1;
}

/*
 * Generate id of the next state
 */
int generateStateID(int num_of_fsm_states,	state_type_enm state_type)
{
	switch(state_type)
	{
		case enmST_Yes:
			return (num_of_fsm_states << 2) ^ 0x00;
			break;
		case enmST_PYes:
			return (num_of_fsm_states << 2) ^ 0x01;
			break;
		case enmST_PNo:
			return (num_of_fsm_states << 2) ^ 0x02;
			break;
		default:
			return (num_of_fsm_states << 2) ^ 0x03;
	}
}


/*
 * Return initial state of the property
 */
unsigned char getInitialState(int prop_number)
{
	int i = 0;
	for (i = 0; i<_GLOBAL_properties_buffer[prop_number].fsm.num_of_fsm_states; ++i)
	{
		if (_GLOBAL_properties_buffer[prop_number].fsm.fsm_states_buffer[i].number1 == 0 &&
			_GLOBAL_properties_buffer[prop_number].fsm.fsm_states_buffer[i].number1 == 0)
			return _GLOBAL_properties_buffer[prop_number].fsm.fsm_states_buffer[i].id;
	}

	return 0;
}


/*
 * Parsing FSM line to FSM state
 */
int parseLineToState(char* line, fsm_struct* fsm)
{
	char snumber1[10];
	char snumber2[10];
	char scolor[30];
	int inumber1 = 0, inumber2 = 0;

	//parsing the state first number
	char* begin = strchr(line, '(');
	char* end = strchr(line, ',');
	strncpy(snumber1, begin+1, (end-begin));

	//parsing the state second number
	begin = strstr(line, ", ");
	end = strchr(line, ')');
	strncpy(snumber2, begin+2, (end-begin-1));

	inumber1 = atoi(snumber1);
	inumber2 = atoi(snumber2);

	fsm->fsm_states_buffer[fsm->num_of_fsm_states].number1 = inumber1;
	fsm->fsm_states_buffer[fsm->num_of_fsm_states].number2 = inumber2;

	//parsing the state type
	memset(scolor, '\0', sizeof(scolor));
	begin = strstr(line, "color=");
	end = strchr(line, ']');
	strncpy(scolor, begin+6, (end-begin-6));

	if (strcmp(scolor, true_state_color) == 0)
		fsm->fsm_states_buffer[fsm->num_of_fsm_states].state_type = enmST_Yes;
	else if (strcmp(scolor, presumably_true_state_color) == 0)
		fsm->fsm_states_buffer[fsm->num_of_fsm_states].state_type = enmST_PYes;
	else if (strcmp(scolor, presumably_false_state_color) == 0)
		fsm->fsm_states_buffer[fsm->num_of_fsm_states].state_type = enmST_PNo;
	else
		fsm->fsm_states_buffer[fsm->num_of_fsm_states].state_type = enmST_No;

	//id of the state is its sequential number in the FSM
	fsm->fsm_states_buffer[fsm->num_of_fsm_states].id = generateStateID(fsm->num_of_fsm_states,
		fsm->fsm_states_buffer[fsm->num_of_fsm_states].state_type);

	++(fsm->num_of_fsm_states);
	return EXIT_SUCCESS;
}

/*
 * Process action: parse program vars that supposed to be a bounded parameters
 */
void processAction(fsm_struct* fsm, char* action)
{
	int i = 0;

	//go over all progam vars and check if they are used in this action
	for (i=0; i<_GLOBAL_num_of_program_vars; ++i)
	{
		//if program var is used - mark it as a parameter
		if (strstr(action, _GLOBAL_program_vars_buffer[i].name) != NULL)
			_GLOBAL_program_vars_buffer[i].isParam = true;
	}

	return;
}

/*
 * Parsing FSM line to FSM transition
 */
int parseLineToTransition(char* line, fsm_struct* fsm)
{
	char snumber1[10];
	char snumber2[10];
	char snumber3[10];
	char snumber4[10];
	int inumber1 = 0, inumber2 = 0, inumber3 = 0, inumber4 = 0;

	//parsing first state first number
	char* begin1 = strchr(line, '(');
	char* end1 = strchr(line, ',');
	strncpy(snumber1, begin1+1, (end1-begin1));

	//parsing first state second number
	begin1 = strstr(line, ", ");
	end1 = strchr(line, ')');
	strncpy(snumber2, begin1+2, (end1-begin1-1));

	//parsing second state first number
	char* begin2 = strchr(end1, '(');
	char* end2 = strchr(end1, ',');
	strncpy(snumber3, begin2+1, (end2-begin2));

	//parsing second state second number
	begin2 = strstr(end1, ", ");
	end2 = strchr(end1+1, ')');
	strncpy(snumber4, begin2+2, (end2-begin2-1));

	inumber1 = atoi(snumber1);
	inumber2 = atoi(snumber2);
	inumber3 = atoi(snumber3);
	inumber4 = atoi(snumber4);

	//parsing the label
	char* begin3 = strchr(end2, '(');
	char* end3 = strchr(end2+1, ')');
	memset(fsm->fsm_transitions_buffer[fsm->num_of_fsm_transitions].label, '\0',
			sizeof(fsm->fsm_transitions_buffer[fsm->num_of_fsm_transitions].label));
	strncpy(fsm->fsm_transitions_buffer[fsm->num_of_fsm_transitions].label, begin3+1, (end3-begin3-1));

	fsm->fsm_transitions_buffer[fsm->num_of_fsm_transitions].state_from.number1 = inumber1;
	fsm->fsm_transitions_buffer[fsm->num_of_fsm_transitions].state_from.number2 = inumber2;
	fsm->fsm_transitions_buffer[fsm->num_of_fsm_transitions].state_to.number1 = inumber3;
	fsm->fsm_transitions_buffer[fsm->num_of_fsm_transitions].state_to.number2 = inumber4;

	if (addLabelToAlphabet(fsm->labels_buffer, fsm->fsm_transitions_buffer[fsm->num_of_fsm_transitions].label, fsm->num_of_labels) == EXIT_SUCCESS)
		++(fsm->num_of_labels);

	//parsing the action if exists
	memset(fsm->fsm_transitions_buffer[fsm->num_of_fsm_transitions].action, '\0',
		sizeof(fsm->fsm_transitions_buffer[fsm->num_of_fsm_transitions].action));
	if (strstr(end3, "action") != NULL)
	{
		char* begin4 = strchr(end3+3, '\"');
		char* end4 = strchr(end3+15, '\"');
		//parse action to insert to the if's code after that
		strncpy(fsm->fsm_transitions_buffer[fsm->num_of_fsm_transitions].action, begin4+1, (end4-begin4-1));
		processAction(fsm, fsm->fsm_transitions_buffer[fsm->num_of_fsm_transitions].action);
	}

	++(fsm->num_of_fsm_transitions);
	return EXIT_SUCCESS;
}


/*
 * Process FSM after parsing
 */
int fsmProcessing(fsm_struct* fsm)
{
	int i = 0;

	//move conclusive states to the end
	unsigned int count = 0;
	if (fsm->num_of_fsm_states >= 1)
	{
		//first count number of conclusive states
		for (i=0; i<fsm->num_of_fsm_states; ++i)
		{
			if (fsm->fsm_states_buffer[i].state_type == enmST_No ||
				fsm->fsm_states_buffer[i].state_type == enmST_Yes)
				count++;
		}
		//second, swap where needed
		for (i=0; i<fsm->num_of_fsm_states-count; ++i)
		{
			if (fsm->fsm_states_buffer[i].state_type == enmST_Yes)
			{
				fsm_state_struct temp_fsm;
				memcpy(&temp_fsm, &(fsm->fsm_states_buffer[i]), sizeof(fsm_state_struct));
				memcpy(&(fsm->fsm_states_buffer[i]), &(fsm->fsm_states_buffer[fsm->num_of_fsm_states - 2]), sizeof(fsm_state_struct));
				memcpy(&(fsm->fsm_states_buffer[fsm->num_of_fsm_states - 2]), &temp_fsm, sizeof(fsm_state_struct));
			}
			if (fsm->fsm_states_buffer[i].state_type == enmST_No)
			{
				fsm_state_struct temp_fsm;
				memcpy(&temp_fsm, &(fsm->fsm_states_buffer[i]), sizeof(fsm_state_struct));
				memcpy(&(fsm->fsm_states_buffer[i]), &(fsm->fsm_states_buffer[fsm->num_of_fsm_states - 1]), sizeof(fsm_state_struct));
				memcpy(&(fsm->fsm_states_buffer[fsm->num_of_fsm_states - 1]), &temp_fsm, sizeof(fsm_state_struct));
			}
		}
	}

	//now resolve state transitions
	int id = -1;
	int num1 = 0, num2 = 0;
	for (i=0; i<fsm->num_of_fsm_transitions; ++i)
	{
		num1 = fsm->fsm_transitions_buffer[i].state_from.number1;
		num2 = fsm->fsm_transitions_buffer[i].state_from.number2;

		if ((id = getStateIDByNumbers(fsm->fsm_states_buffer, fsm->num_of_fsm_transitions, num1, num2)) == -1)
		{
			fprintf(stderr, "MonitorGenerator: Inconsistency between transitions and states in generated FSM.\n");
			return EXIT_FAILURE;
		}
		fsm->fsm_transitions_buffer[i].state_from.id = id;

		num1 = fsm->fsm_transitions_buffer[i].state_to.number1;
		num2 = fsm->fsm_transitions_buffer[i].state_to.number2;

		if ((id = getStateIDByNumbers(fsm->fsm_states_buffer, fsm->num_of_fsm_transitions, num1, num2)) == -1)
		{
			fprintf(stderr, "MonitorGenerator: Inconsistency between transitions and states in generated FSM.\n");
			return EXIT_FAILURE;
		}
		fsm->fsm_transitions_buffer[i].state_to.id = id;
	}

	return EXIT_SUCCESS;
}


/*
 * parsing the FSM into memory
 */
int parseFSMFromFile(char* fsm_file_name, fsm_struct* fsm)
{
	FILE* fin = NULL;

	if ((fin = fopen(fsm_file_name, "r")) == NULL)
	{
		fprintf(stderr, "MonitorGenerator: Cannot open generated FSM file for reading.\n");
		return EXIT_FAILURE;
	}

	char line[MAX_LINE_SIZE];

	while(fgets(line, sizeof(line), fin) != NULL)
	{
		//ignore the first line
		if (line[0] == 'd') continue;

		//arrived to the last line
		if (line[0] == '}')	break;

		if (strstr(line, "->"))
			parseLineToTransition(line, fsm);
		else
			parseLineToState(line, fsm);
	}

	fclose(fin);
	return fsmProcessing(fsm);
}


/*
 * Get number of inconclusive (presumable_true or presumably_false) states in fsm
 */
unsigned int getNumOfInconclusiveStates(const fsm_struct* fsm)
{
	int i = 0, counter = 0;
	for (i=0; i<fsm->num_of_fsm_states; ++i)
	{
		if (fsm->fsm_states_buffer[i].state_type == enmST_PNo || fsm->fsm_states_buffer[i].state_type == enmST_PYes)
			++counter;
	}

	return counter;
}

/*
 * get order of the next state given current state and transition number
 */
unsigned int getNextState(const fsm_struct* fsm, unsigned int state_order, unsigned int transition)
{
	int i = 0;
	int num1 = 0, num2 = 0;

	num1 = fsm->fsm_states_buffer[state_order].number1;
	num2 = fsm->fsm_states_buffer[state_order].number2;

	//extract all the positive predicates
	char condition[MAX_LINE_SIZE/2];
	memset(condition, 0, MAX_LINE_SIZE/2);
	if (transition == 0)
	{
		strcat(condition, "<empty>");
	}
	else
	{
		while (transition != 0)
		{
			if (transition % 2 != 0)
			{
				if (condition[0] != '\0')
					strcat(condition, "&&");			//no need in && on first predicate
				strcat(condition, fsm->labels_buffer[i]);
			}
			++i;
			transition = transition >> 1;
		}
	}

	//go over transitions and find suitable
	unsigned char state_id = 0;
	for (i=0; i<fsm->num_of_fsm_transitions; ++i)
	{
		if (fsm->fsm_transitions_buffer[i].state_from.number1 == num1 &&
				fsm->fsm_transitions_buffer[i].state_from.number2 == num2 &&
				(strcmp(fsm->fsm_transitions_buffer[i].label, condition) == 0))
		{
			state_id = fsm->fsm_transitions_buffer[i].state_to.id;
			break;
		}
	}

	return getStateOrderByID(fsm->fsm_states_buffer, fsm->num_of_fsm_states, state_id);
}


/*
 * parsing the LTL formula, running the script and converting to the final FSM
 */
int parseLTLFormalismtoFSM(const char* syntax, fsm_struct* fsm)
{
	char command[MAX_LINE_SIZE];
	char currentPath[MAX_LINE_SIZE];

	if (!GetCurrentDir(currentPath, sizeof(currentPath)))
	{
		fprintf(stderr, "MonitorGenerator: Cannot obtain current directory path.\n");
		return EXIT_FAILURE;
	}

	if (_GLOBAL_LTL_tools_path == NULL)
		chdir("./ltl3tools-0.0.7/");
	else
		chdir(_GLOBAL_LTL_tools_path);
	sprintf(command, "./ltl2monLTL4 \"%s\"  > temp.txt", syntax);
	system(command);
	chdir(currentPath);

	memset(command, 0, sizeof(command));
	if (_GLOBAL_LTL_tools_path == NULL)
		strcat(command, "./ltl3tools-0.0.7/");
	else
		strcat(command, _GLOBAL_LTL_tools_path);
	strcat(command, "/temp.txt");

	return parseFSMFromFile("./ltl3tools-0.0.7/temp.txt", fsm);
}


/*
 * parsing the FSM syntax and converting to the final FSM
 */
int parseFSMFormalismtoFSM(char* syntax, fsm_struct* fsm, const char* name)
{
	char* line;
	line = strtok(syntax, "\n");

	while(line != NULL)
	{
		line = strtok(NULL, "\n");

		//ignore the first line
		if (line[0] == 'd') continue;

		//arrived to the last line
		if (line[0] == '}')	break;

		if (strstr(line, "->"))
			parseLineToTransition(line, fsm);
		else
			parseLineToState(line, fsm);
	}

	strcpy(syntax, name);
	return fsmProcessing(fsm);
}


/*
 * Main function for running the monitor construction and then parsing the FSMs
 */
int buildFSMs()
{
	int i = 0;
	int Result = EXIT_SUCCESS;

	//passing over properties and building monitor FSMs for them
	for (i=0; i<_GLOBAL_num_of_props; ++i)
	{
		if ((_GLOBAL_properties_buffer[i].formalism == NULL) || (!strcasecmp(_GLOBAL_properties_buffer[i].formalism, _GLOBAL_LTL_FORMALISM)))
		{
			Result = parseLTLFormalismtoFSM(_GLOBAL_properties_buffer[i].syntax, &(_GLOBAL_properties_buffer[i].fsm));
			if (Result != EXIT_SUCCESS) return Result;
			else continue;
		}
		else if (!strcasecmp(_GLOBAL_properties_buffer[i].formalism, _GLOBAL_FSM_FORMALISM))
		{
			Result = parseFSMFormalismtoFSM(_GLOBAL_properties_buffer[i].syntax,
					&(_GLOBAL_properties_buffer[i].fsm),
					_GLOBAL_properties_buffer[i].name);
			if (Result != EXIT_SUCCESS) return Result;
			else continue;
		}
		else
		{
			fprintf(stderr, "MonitorGenerator: Unrecognized formalism in property %s.\n", _GLOBAL_properties_buffer[i].name);
			return EXIT_FAILURE;
		}
	}

	return EXIT_SUCCESS;
}

#endif /* FSMPARSING_H_ */
