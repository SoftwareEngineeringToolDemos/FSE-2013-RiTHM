/*
 ============================================================================
 Name        : MonitorGenerator.c
 Author      : Shay Berkovich
 Version     :
 Copyright   : Real-Time Embedded Software Group, University of Waterloo
 Description : The main file for MonitorGenerator
 ============================================================================
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include "FileParsing.h"
#include "FSMParsing.h"

unsigned int inconclusive_states = 0;

/*
 * Get predicate by name
 */
predicate_struct* getPredicateByName(char* label)
{
	int i = 0;

	for (i=0; i<_GLOBAL_num_of_predicates; ++i)
	{
		if (strcmp(_GLOBAL_predicates_buffer[i].name, label) == 0)
			return &(_GLOBAL_predicates_buffer[i]);
	}

	return NULL;
}


/*
 * Get order of partial predicate
 */
int getPartialPredOrder(predicate_struct* pred)
{
	int i = 0, order = 0;

	for (i=0; i<_GLOBAL_num_of_predicates; ++i)
	{
		if (strcmp(_GLOBAL_predicates_buffer[i].name, pred->name) == 0)
			return order;
		if (_GLOBAL_predicates_buffer[i].isPartial) ++order;
	}

	return -1;
}


/*
 * Return initial state of the property
 */
unsigned char getNumOfPartialPredicates(int prop_number)
{
	int i = 0, count = 0;

	if (prop_number < 0)
	{
		for (i=0; i<_GLOBAL_num_of_predicates; ++i)
		{
			if (_GLOBAL_predicates_buffer[i].isPartial)
				++count;
		}
	}
	else
	{
		for (i=0; i<(_GLOBAL_properties_buffer[prop_number].fsm.num_of_labels); ++i)
		{
			//we are only interested in isolated labels
			if ((strcmp(_GLOBAL_properties_buffer[prop_number].fsm.labels_buffer[i], "<empty>") == 0) ||
				(strchr(_GLOBAL_properties_buffer[prop_number].fsm.labels_buffer[i],'&') != NULL))	continue;

			//take this label from predicates buffer
			predicate_struct* pred = getPredicateByName(_GLOBAL_properties_buffer[prop_number].fsm.labels_buffer[i]);
			if (pred->isPartial)
				++count;
		}
	}
	return count;
}


/*
 * Get condition for the branch from label
 */
void labelsIntoCondition(label_type label, label_type* labels_buffer, int size, char* condition)
{
	int i = 0;

	if (strcmp(label, "<empty>") == 0)
	{
		for (i=0; i<size-1; ++i)
		{
			strcat(condition, "! ");
			strcat(condition, labels_buffer[i]);
			strcat(condition, " && ");
		}

		strcat(condition, "! ");
		strcat(condition, labels_buffer[i]);
		return;
	}

	for (i=0; i<size-1; ++i)
	{
		if (strstr(label, labels_buffer[i]) != NULL)
		{
			strcat(condition, labels_buffer[i]);
			strcat(condition, " && ");
		}
		else
		{
			strcat(condition, "! ");
			strcat(condition, labels_buffer[i]);
			strcat(condition, " && ");
		}
	}

	if (strstr(label, labels_buffer[i]) != NULL)
		strcat(condition, labels_buffer[i]);
	else
	{
		strcat(condition, "! ");
		strcat(condition, labels_buffer[i]);
	}

	return;
}

/*
 * Get line of the predicates as an arguments
 */
void labelsIntoArgumentLine(label_type* labels_buffer, int size, char* arg_line)
{
	int i = 0;
	for (i=0; i<size; ++i)
	{
		//we are only interested in isolated labels
		if ((strcmp(labels_buffer[i], "<empty>") == 0) ||
			(strchr(labels_buffer[i],'&') != NULL))	continue;

		strcat(arg_line, labels_buffer[i]);
		strcat(arg_line, ", ");
	}

	return;
}


/*
 * Generate CPU transition function into fout
 */
int outputCPUTransitionFunction(property_struct* prop, FILE* fout, int prop_number)
{
	char line[MAX_LINE_SIZE];
	int i = 0, j = 0;
	predicate_struct* pred = NULL;

	sprintf(line, "void transition%d(fsm_state* input_state, _GOOMF_device_program_state_ptr input_data, bool* partial_predicates, struct _GOOMF_used_params_struct* used_params)\n{\n", prop_number);
	fprintf(fout, "%s", line);
	fprintf(fout, "\t//property \"%s\"\n", prop->syntax);

	//partial property
	if (getNumOfPartialPredicates(prop_number) > 0)
	{	//partial
		memset(line, 0, sizeof(line));

		fprintf(fout, "\tif (partial_predicates != NULL)\n\t{\n");

		for (i=0; i<(prop->fsm.num_of_labels); ++i)
		{
			//we are only interested in isolated labels
			if ((strcmp(prop->fsm.labels_buffer[i], "<empty>") == 0) ||
				(strchr(prop->fsm.labels_buffer[i],'&') != NULL))	continue;

			//take this label from predicates buffer
			pred = NULL;
			if ((pred = getPredicateByName(prop->fsm.labels_buffer[i])) == NULL)
			{
				fprintf(stderr, "MonitorGenerator: Missing predicate is used in property.\n");
				return EXIT_FAILURE;
			}

			if (pred->isPartial) continue;

			for (j=0; j<_GLOBAL_num_of_program_vars; ++j)
			{
				if (strstr(pred->syntax, _GLOBAL_program_vars_buffer[j].name) != NULL &&
						strstr(line, _GLOBAL_program_vars_buffer[j].name) == NULL)
				{
					strcat(line, "\t");
					strcat(line, _GLOBAL_program_vars_buffer[j].type);
					strcat(line, " ");
					strcat(line, _GLOBAL_program_vars_buffer[j].name);
					strcat(line, ";\n\t");
					strcat(line, _GLOBAL_program_vars_buffer[j].name);
					strcat(line, " = input_data->");
					strcat(line, _GLOBAL_program_vars_buffer[j].name);
					strcat(line, ";\n");
				}
			}
		}
		fprintf(fout, "%s", line);

		//output every relevant predicate
		fprintf(fout, "\n");
		for (i=0; i<(prop->fsm.num_of_labels); ++i)
		{
			//we are only interested in isolated labels
			if ((strcmp(prop->fsm.labels_buffer[i], "<empty>") == 0) ||
				(strchr(prop->fsm.labels_buffer[i],'&') != NULL))	continue;

			pred = getPredicateByName(prop->fsm.labels_buffer[i]);
			if (pred->isPartial) continue;
			sprintf(line, "\tbool %s = (%s);\n", pred->name, pred->syntax);
			fprintf(fout, "%s", line);
		}

		if (_GLOBAL_fsm_format == fsm_as_ifs)
		{	//fsm represented as a sequence of if's
			sprintf(line, "\n\t//transition branches\n");
			fprintf(fout, "%s", line);

			//output branches
			char condition[MAX_LINE_SIZE/2];
			for (i=0; i<(prop->fsm.num_of_fsm_transitions); ++i)
			{
				memset(condition, 0, MAX_LINE_SIZE/2);
				labelsIntoCondition(prop->fsm.fsm_transitions_buffer[i].label, prop->fsm.labels_buffer, prop->fsm.num_of_labels, condition);
				if (i == 0)
					sprintf(line, "\tif( (*input_state == %d) && ( %s ))\n\t\t",
						prop->fsm.fsm_transitions_buffer[i].state_from.id,
						condition);
				else
					sprintf(line, "\telse if( (*input_state == %d) && ( %s ))\n\t\t",
						prop->fsm.fsm_transitions_buffer[i].state_from.id,
						condition);
				fprintf(fout, "%s", line);

				//output action
				if (strlen(prop->fsm.fsm_transitions_buffer[i].action) > 0)
					fprintf(fout, "\t%s;\n", prop->fsm.fsm_transitions_buffer[i].action);

				sprintf(line, "*input_state = %d;\n", prop->fsm.fsm_transitions_buffer[i].state_to.id);
				fprintf(fout, "%s", line);
			}

			fprintf(fout, "\telse return;\n}\n\n");
		}
		else
		{	//fsm represented as a table
			sprintf(line, "\n\t//transition table\n");
			fprintf(fout, "%s", line);
			fprintf(fout, "\t*input_state = getStateOrder(%d, *input_state);\n", prop_number);

			//building pred_value
			char pred_value[MAX_LINE_SIZE/2];
			char temp[MAX_LINE_SIZE/4];
			memset(pred_value, 0, MAX_LINE_SIZE/2);
			for (i=0; i<(prop->fsm.num_of_labels); ++i)
			{
				memset(temp, 0, MAX_LINE_SIZE/4);
				pred = getPredicateByName(prop->fsm.labels_buffer[i]);
				int pred_order = getPartialPredOrder(pred);
				if (pred_order < 0)
				{
					fprintf(stderr, "MonitorGenerator: Partial predicates are messed up.\n");
					return EXIT_FAILURE;
				}

				if (pred->isPartial)
				{
					if (i == prop->fsm.num_of_labels - 1)
						sprintf(temp, "%d*(*(partial_predicates+%d))", (int)pow(2, i), pred_order);
					else
						sprintf(temp, "%d*(*(partial_predicates+%d)) + ", (int)pow(2, i), pred_order);
				}
				else
				{
					if (i == prop->fsm.num_of_labels - 1)
						sprintf(temp, "%d*%s", (int)pow(2, i), prop->fsm.labels_buffer[i]);
					else
						sprintf(temp, "%d*%s + ", (int)pow(2, i), prop->fsm.labels_buffer[i]);
				}
				strcat(pred_value, temp);
			}

			fprintf(fout, "\tunsigned int pred_value = %s;\n", pred_value);
			fprintf(fout, "\t*input_state = getStateID(%d, fsm_table_prop%d[*input_state][pred_value]);\n",
					prop_number, prop_number);
		}

		fprintf(fout, "\t}\n\telse\n\t{\n");
	}

	//not partial part
	bool flags[_GLOBAL_num_of_program_vars];	//required not to output same variable twice
	memset(flags, 0, sizeof(flags));
	memset(line, 0, sizeof(line));
	for (i=0; i<(prop->fsm.num_of_labels); ++i)
	{
		//we are only interested in isolated labels
		if ((strcmp(prop->fsm.labels_buffer[i], "<empty>") == 0) ||
			(strchr(prop->fsm.labels_buffer[i],'&') != NULL))	continue;

		//take this label from predicates buffer
		pred = NULL;
		if ((pred = getPredicateByName(prop->fsm.labels_buffer[i])) == NULL)
		{
			fprintf(stderr, "MonitorGenerator: Missing predicate is used in property.\n");
			return EXIT_FAILURE;
		}

		for (j=0; j<_GLOBAL_num_of_program_vars; ++j)
		{
			if (strstr(pred->syntax, _GLOBAL_program_vars_buffer[j].name) != NULL
					&&	flags[j] != true)
			{
				strcat(line, "\t");
				strcat(line, _GLOBAL_program_vars_buffer[j].type);
				strcat(line, " ");
				strcat(line, _GLOBAL_program_vars_buffer[j].name);
				strcat(line, ";\n\t");
				strcat(line, _GLOBAL_program_vars_buffer[j].name);
				strcat(line, " = input_data->");
				strcat(line, _GLOBAL_program_vars_buffer[j].name);
				strcat(line, ";\n");
				flags[j] = true;
			}
		}
	}
	fprintf(fout, "%s", line);

	//output every relevant predicate
	fprintf(fout, "\n");
	for (i=0; i<(prop->fsm.num_of_labels); ++i)
	{
		//we are only interested in isolated labels
		if ((strcmp(prop->fsm.labels_buffer[i], "<empty>") == 0) ||
			(strchr(prop->fsm.labels_buffer[i],'&') != NULL))	continue;

		pred = getPredicateByName(prop->fsm.labels_buffer[i]);
		sprintf(line, "\tbool %s = (%s);\n", pred->name, pred->syntax);
		fprintf(fout, "%s", line);
	}

	if (_GLOBAL_fsm_format == fsm_as_ifs)
	{	//fsm represented as a sequence of if's
		sprintf(line, "\n\t//transition branches\n");
		fprintf(fout, "%s", line);

		//output branches
		char condition[MAX_LINE_SIZE/2];
		for (i=0; i<(prop->fsm.num_of_fsm_transitions); ++i)
		{
			memset(condition, 0, MAX_LINE_SIZE/2);
			labelsIntoCondition(prop->fsm.fsm_transitions_buffer[i].label, prop->fsm.labels_buffer, prop->fsm.num_of_labels, condition);
			if (i == 0)
				sprintf(line, "\tif( (*input_state == %d) && ( %s ))\n\t",
					prop->fsm.fsm_transitions_buffer[i].state_from.id,
					condition);
			else
				sprintf(line, "\telse if( (*input_state == %d) && ( %s ))\n\t",
					prop->fsm.fsm_transitions_buffer[i].state_from.id,
					condition);
			fprintf(fout, "%s", line);

			//output action
			if (strlen(prop->fsm.fsm_transitions_buffer[i].action) > 0)
			{
				fprintf(fout, "{\n\t\t%s;\n", prop->fsm.fsm_transitions_buffer[i].action);
				fprintf(fout, "\t\t*input_state = %d;\n\t}\n", prop->fsm.fsm_transitions_buffer[i].state_to.id);
			}
			else
			{
				sprintf(line, "\t\t*input_state = %d;\n", prop->fsm.fsm_transitions_buffer[i].state_to.id);
				fprintf(fout, "%s", line);
			}
		}

		fprintf(fout, "\telse return;\n}\n\n");

		if (getNumOfPartialPredicates(prop_number) > 0)
			fprintf(fout, "\t}\n");
	}
	else
	{	//fsm represented as a table
		sprintf(line, "\n\t//transition table\n");
		fprintf(fout, "%s", line);
		fprintf(fout, "\t*input_state = getStateOrder(%d, *input_state);\n", prop_number);

		//building pred_value
		char pred_value[MAX_LINE_SIZE/2];
		char temp[MAX_LINE_SIZE/4];
		memset(pred_value, 0, MAX_LINE_SIZE/2);
		for (i=0; i<(prop->fsm.num_of_labels); ++i)
		{
			memset(temp, 0, MAX_LINE_SIZE/4);
			if (i == prop->fsm.num_of_labels - 1)
				sprintf(temp, "%d*%s", (int)pow(2, i), prop->fsm.labels_buffer[i]);
			else
				sprintf(temp, "%d*%s + ", (int)pow(2, i), prop->fsm.labels_buffer[i]);
			strcat(pred_value, temp);
		}

		fprintf(fout, "\tunsigned int pred_value = %s;\n", pred_value);
		fprintf(fout, "\t*input_state = getStateID(%d, fsm_table_prop%d[*input_state][pred_value]);\n",
				prop_number, prop_number);
		if (getNumOfPartialPredicates(prop_number) > 0)
			fprintf(fout, "\t}\n");

		fprintf(fout, "\treturn;\n}\n\n");
	}

	return EXIT_SUCCESS;
}


/*
 * Generate GPU kernel code for Algorithm Infinite for specific property into fout
 */
int outputKernelCodeForPropertyAlgInfinite(property_struct* prop, FILE* fout, int prop_number)
{
	char line[MAX_LINE_SIZE];
	int i = 0, j = 0;
	predicate_struct* pred = NULL;

	fprintf(fout, "\n\t//property \"%s\"\n", prop->syntax);
	fprintf(fout, "prop%d: \n\tif (global_id + work_per_thread <= cur_index[%d]) \n\t\tgoto prop%d;\n\n",
			prop_number,
			prop_number,
			prop_number+1);

	sprintf(line, "\ttemp_fsm_state = current_state[%d];\n\n", prop_number);
	fprintf(fout, "%s", line);

	fprintf(fout, "\t//translation from work-item units to actual data\n");
	fprintf(fout, "\tfor (int j=(global_id*work_per_thread); j<((global_id+1)*work_per_thread); ++j)\n\t{\n");
	fprintf(fout, "\t\tcopyProgramState(&temp_program_state, &input_buffer[j]);\n\n");

	bool flags[_GLOBAL_num_of_program_vars];	//required not to output same variable twice
	memset(flags, 0, sizeof(flags));
	memset(line, 0, sizeof(line));
	for (i=0; i<(prop->fsm.num_of_labels); ++i)
	{
		//we are only interested in isolated labels
		if ((strcmp(prop->fsm.labels_buffer[i], "<empty>") == 0) ||
			(strchr(prop->fsm.labels_buffer[i],'&') != NULL))	continue;

		pred = NULL;
		pred = getPredicateByName(prop->fsm.labels_buffer[i]);
		for (j=0; j<_GLOBAL_num_of_program_vars; ++j)
		{
			if (strstr(pred->syntax, _GLOBAL_program_vars_buffer[j].name) != NULL
				&&	flags[j] != true)
			{
				strcat(line, "\t\t");
				strcat(line, _GLOBAL_program_vars_buffer[j].name);
				strcat(line, " = temp_program_state.");
				strcat(line, _GLOBAL_program_vars_buffer[j].name);
				strcat(line, ";\n");
			}
		}
	}
	fprintf(fout, "%s\n", line);

	//output every relevant predicate
	for (i=0; i<(prop->fsm.num_of_labels); ++i)
	{
		//we are only interested in isolated labels
		if ((strcmp(prop->fsm.labels_buffer[i], "<empty>") == 0) ||
			(strchr(prop->fsm.labels_buffer[i],'&') != NULL))	continue;

		pred = getPredicateByName(prop->fsm.labels_buffer[i]);
		sprintf(line, "\t\t%s = (%s);\n\n", pred->name, pred->syntax);
		fprintf(fout, "%s", line);
	}

	//output call for transition function for every inconclusive state
	if (_GLOBAL_fsm_format == fsm_as_ifs)
	{
		char arg_line[MAX_LINE_SIZE/2];
		int state_counter = 0;
		for (i=0; i<prop->fsm.num_of_fsm_states; ++i)
		{
			//perform calculation only for the inconclusive states
			if (prop->fsm.fsm_states_buffer[i].state_type == enmST_No || prop->fsm.fsm_states_buffer[i].state_type == enmST_Yes)
				continue;

			memset(arg_line, 0, MAX_LINE_SIZE/2);
			labelsIntoArgumentLine(prop->fsm.labels_buffer, prop->fsm.num_of_labels, arg_line);
			sprintf(line, "\t\ttransition%d(%s%d, &(output_buffer[nitems*%d + %d + j*%d]));\n",
					prop_number,
					arg_line,
					prop->fsm.fsm_states_buffer[i].id,
					inconclusive_states,
					state_counter++,
					getNumOfInconclusiveStates(&(prop->fsm)));
			fprintf(fout, "%s", line);
		}
	}
	else
	{
		//building pred_value
		char pred_value[MAX_LINE_SIZE/2];
		char temp[MAX_LINE_SIZE/4];
		memset(pred_value, 0, MAX_LINE_SIZE/2);
		for (i=0; i<(prop->fsm.num_of_labels); ++i)
		{
			memset(temp, 0, MAX_LINE_SIZE/4);
			if (i == prop->fsm.num_of_labels - 1)
				sprintf(temp, "%d*%s", (int)pow(2, i), prop->fsm.labels_buffer[i]);
			else
				sprintf(temp, "%d*%s + ", (int)pow(2, i), prop->fsm.labels_buffer[i]);
			strcat(pred_value, temp);
		}
		fprintf(fout, "\t\tpred_value = %s;\n", pred_value);

		for (i=0; i<prop->fsm.num_of_fsm_states; ++i)
		{
			//perform calculation only for the inconclusive states
			if (prop->fsm.fsm_states_buffer[i].state_type == enmST_No || prop->fsm.fsm_states_buffer[i].state_type == enmST_Yes)
				continue;

			fprintf(fout, "\t\toutput_buffer[nitems*%d + %d + j*%d] = getStateID(%d, fsm_table_prop%d[%d][pred_value]);\n",
					inconclusive_states,
					i,
					getNumOfInconclusiveStates(&(prop->fsm)),
					prop_number,
					prop_number,
					i);
		}
	}

	//increase the number of the inconclusive states to skip in the next property
	inconclusive_states += getNumOfInconclusiveStates(&(prop->fsm));

	fprintf(fout, "\t}\n");

	return EXIT_SUCCESS;
}


/*
 * Generate GPU kernel code for Algorithm Finite for specific property into fout
 */
int outputKernelCodeForPropertyAlgFinite(property_struct* prop, FILE* fout, int prop_number)
{
	char line[MAX_LINE_SIZE];
	int i = 0, j = 0;
	predicate_struct* pred = NULL;

	fprintf(fout, "\n\n\t//property \"%s\"\n", prop->syntax);
	fprintf(fout, "prop%d: \n\tif (global_id + work_per_thread <= cur_index[%d]) \n\t\tgoto prop%d;\n\n",
			prop_number,
			prop_number,
			prop_number+1);

	sprintf(line, "\ttemp_fsm_state1 = current_state[%d];\n\n", prop_number);
	fprintf(fout, "%s", line);

	fprintf(fout, "\t//translation from work-item units to actual data\n");
	fprintf(fout, "\tfor (int j=(global_id*work_per_thread); j<((global_id+1)*work_per_thread); ++j)\n\t{\n");
	fprintf(fout, "\t\tif (j < cur_index[%d])\n\t\t\tcontinue;\n", prop_number);
	fprintf(fout, "\t\tcopyProgramState(&temp_program_state, &input_buffer[j]);\n\n");

	bool flags[_GLOBAL_num_of_program_vars];	//required not to output same variable twice
	memset(flags, 0, sizeof(flags));
	memset(line, 0, sizeof(line));
	for (i=0; i<(prop->fsm.num_of_labels); ++i)
	{
		//we are only interested in isolated labels
		if ((strcmp(prop->fsm.labels_buffer[i], "<empty>") == 0) ||
			(strchr(prop->fsm.labels_buffer[i],'&') != NULL))	continue;

		pred = NULL;
		pred = getPredicateByName(prop->fsm.labels_buffer[i]);
		for (j=0; j<_GLOBAL_num_of_program_vars; ++j)
		{
			if (strstr(pred->syntax, _GLOBAL_program_vars_buffer[j].name) != NULL
				&&	flags[j] != true)
			{
				strcat(line, "\t\t");
				strcat(line, _GLOBAL_program_vars_buffer[j].name);
				strcat(line, " = temp_program_state.");
				strcat(line, _GLOBAL_program_vars_buffer[j].name);
				strcat(line, ";\n");
			}
		}
	}
	fprintf(fout, "%s\n", line);

	//output every relevant predicate
	for (i=0; i<(prop->fsm.num_of_labels); ++i)
	{
		//we are only interested in isolated labels
		if ((strcmp(prop->fsm.labels_buffer[i], "<empty>") == 0) ||
			(strchr(prop->fsm.labels_buffer[i],'&') != NULL))	continue;

		pred = getPredicateByName(prop->fsm.labels_buffer[i]);
		sprintf(line, "\t\t%s = (%s);\n\n", pred->name, pred->syntax);
		fprintf(fout, "%s", line);
	}

	//output call for transition function for current fsm state
	if (_GLOBAL_fsm_format == fsm_as_ifs)
	{
		char arg_line[MAX_LINE_SIZE/2];
		memset(arg_line, 0, MAX_LINE_SIZE/2);

		labelsIntoArgumentLine(prop->fsm.labels_buffer, prop->fsm.num_of_labels, arg_line);
		sprintf(line, "\t\ttransition%d(%stemp_fsm_state1, &temp_fsm_state2);\n",
			prop_number,
			arg_line);
		fprintf(fout, "%s", line);
	}
	else
	{
		fprintf(fout, "\t\ttemp_fsm_state1 = getStateOrder(%d, temp_fsm_state1);\n", prop_number);

		//building pred_value
		char pred_value[MAX_LINE_SIZE/2];
		char temp[MAX_LINE_SIZE/4];
		memset(pred_value, 0, MAX_LINE_SIZE/2);
		for (i=0; i<(prop->fsm.num_of_labels); ++i)
		{
			memset(temp, 0, MAX_LINE_SIZE/4);
			if (i == prop->fsm.num_of_labels - 1)
				sprintf(temp, "%d*%s", (int)pow(2, i), prop->fsm.labels_buffer[i]);
			else
				sprintf(temp, "%d*%s + ", (int)pow(2, i), prop->fsm.labels_buffer[i]);
			strcat(pred_value, temp);
		}

		fprintf(fout, "\t\tpred_value = %s;\n", pred_value);
		fprintf(fout, "\t\ttemp_fsm_state2 = fsm_table_prop%d[temp_fsm_state1][pred_value];\n\n", prop_number);
	}
	fprintf(fout, "\t\tif( temp_fsm_state1 != temp_fsm_state2 )\n\t\t{\n");
	fprintf(fout, "\t\t\tatomic_min(&(local_min%d), j);\n\t\t\tbreak;\n\t\t}\n", prop_number);
	fprintf(fout, "\t}\n");

	return EXIT_SUCCESS;
}


/*
 * Generate GPU transition function into fout
 */
int outputGPUTransitionFunction(property_struct* prop, FILE* fout, int prop_number, bool isGlobal)
{
	char line[MAX_LINE_SIZE];
	int i = 0;
	char prog_stat_vars[MAX_LINE_SIZE/2];
	memset(prog_stat_vars, 0, sizeof(prog_stat_vars));

	//constructing the labels declaration
	for (i=0; i<(prop->fsm.num_of_labels); ++i)
	{
		//we are only interested in isolated labels
		if ((strcmp(prop->fsm.labels_buffer[i], "<empty>") == 0) ||
			(strchr(prop->fsm.labels_buffer[i],'&') != NULL))	continue;

		if (strlen(prog_stat_vars) == 0)
			strcat(prog_stat_vars, "bool ");
		else
			strcat(prog_stat_vars, ", bool ");
		strcat(prog_stat_vars, prop->fsm.labels_buffer[i]);
	}

	if (isGlobal)
		sprintf(line, "void transition%d(%s, fsm_state input_state, __global fsm_state* output_state)\n{", prop_number, prog_stat_vars);
	else
		sprintf(line, "void transition%d(%s, fsm_state input_state, fsm_state* output_state)\n{", prop_number, prog_stat_vars);
	fprintf(fout, "%s", line);

	fprintf(fout, "\t//property \"%s\"\n", prop->syntax);
	sprintf(line, "\n\t//transition branches\n");
	fprintf(fout, "%s", line);

	//output branches
	char condition[MAX_LINE_SIZE/2];
	for (i=0; i<(prop->fsm.num_of_fsm_transitions); ++i)
	{
		memset(condition, 0, MAX_LINE_SIZE/2);
		labelsIntoCondition(prop->fsm.fsm_transitions_buffer[i].label, prop->fsm.labels_buffer, prop->fsm.num_of_labels, condition);
		sprintf(line, "\tif( input_state == %d && ( %s ))\n\t\t",
				prop->fsm.fsm_transitions_buffer[i].state_from.id,
				condition);
		fprintf(fout, "%s", line);
		sprintf(line, "*output_state = %d;\n", prop->fsm.fsm_transitions_buffer[i].state_to.id);
		fprintf(fout, "%s", line);
	}
	fprintf(fout, "\telse return;\n}\n\n");

	return EXIT_SUCCESS;
}


void getOpenCLType(const char* input_type, char* output_type)
{
	if (!strcmp(input_type, "char"))
		strcpy(output_type, "cl_char");
	else if (!strcmp(input_type, "uchar") || !strcmp(input_type, "unsigned char"))
		strcpy(output_type, "cl_uchar");
	else if (!strcmp(input_type, "short"))
		strcpy(output_type, "cl_short");
	else if (!strcmp(input_type, "ushort") || !strcmp(input_type, "unsigned short"))
		strcpy(output_type, "cl_ushort");
	else if (!strcmp(input_type, "int"))
		strcpy(output_type, "cl_int");
	else if (!strcmp(input_type, "uint") || !strcmp(input_type, "unsigned int"))
		strcpy(output_type, "cl_uint");
	else if (!strcmp(input_type, "long"))
		strcpy(output_type, "cl_long");
	else if (!strcmp(input_type, "ulong") || !strcmp(input_type, "unsigned long"))
		strcpy(output_type, "cl_ulong");
	else if (!strcmp(input_type, "float"))
		strcpy(output_type, "cl_float");
	else if (!strcmp(input_type, "bool"))
		strcpy(output_type, "bool");
	else
		strcpy(output_type, "cl_int");

	return;
}


/*
 * output user functions if defined
 */
void outputUserFunctions(FILE* fout)
{
	int i = 0;

	if (_GLOBAL_functions_buffer == NULL)
		return;

	for (i=0; i<_GLOBAL_num_of_functions; ++i)
	{
		fprintf(fout, "%s\n", _GLOBAL_functions_buffer[i]);
	}
	return;
}


/*
 * output fsm in a table format
 */
void outputFSMTable(FILE* fout, unsigned int prop_number)
{
	unsigned int i = 0, j = 0;
	unsigned int transitions_per_state = 0;

	if (_GLOBAL_properties_buffer[prop_number].fsm.num_of_fsm_states > 0)
		transitions_per_state = _GLOBAL_properties_buffer[prop_number].fsm.num_of_fsm_transitions / _GLOBAL_properties_buffer[prop_number].fsm.num_of_fsm_states;
	else
		return;

	fprintf(fout, "fsm_state fsm_table_prop%d[%d][%d] = ",
			prop_number,
			_GLOBAL_properties_buffer[prop_number].fsm.num_of_fsm_states,
			transitions_per_state);
	fprintf(fout, "{");
	for (i=0; i<_GLOBAL_properties_buffer[prop_number].fsm.num_of_fsm_states; ++i)
	{
		fprintf(fout, "{");
		for (j=0; j<transitions_per_state; ++j)
		{
			unsigned int next_state_order = getNextState(&(_GLOBAL_properties_buffer[prop_number].fsm),	i, j);
			fprintf(fout, "%d", next_state_order);
			if (j < transitions_per_state - 1)
				fprintf(fout, ", ");
		}
		if (i == _GLOBAL_properties_buffer[prop_number].fsm.num_of_fsm_states - 1)
			fprintf(fout, "}");
		else
			fprintf(fout, "},\n\t\t\t\t");
	}
	fprintf(fout, "};\n\n");
}


/*
 * create file for regular CPU monitoring
 */
int outputCPUMonitor()
{
	FILE* fout = NULL, *fout2 = NULL, *ftemplate = NULL;
	int i = 0, j = 0;

	if ((fout = fopen(monitor_file_CPU, "w")) == NULL)
	{
		fprintf(stderr, "MonitorGenerator: Cannot create %s for output.\n", monitor_file_CPU);
		return EXIT_FAILURE;
	}
	if ((fout2 = fopen(program_state_file, "w")) == NULL)
	{
		fprintf(stderr, "MonitorGenerator: Cannot create %s for output.\n", program_state_file);
		return EXIT_FAILURE;
	}

	char filePath[MAX_LINE_SIZE];
	memset(filePath, 0, sizeof(filePath));
	if (_GLOBAL_templates_path == NULL)
	{
		strcat(filePath, "./Templates/");
		strcat(filePath, monitor_file_CPU);
	}
	else
	{
		strcat(filePath, _GLOBAL_templates_path);
		strcat(filePath, monitor_file_CPU);
	}
	if ((ftemplate = fopen(filePath, "r")) == NULL)
	{
		fprintf(stderr, "MonitorGenerator: Cannot open %s template for input.\n", filePath);
		fclose(fout);
		return EXIT_FAILURE;
	}

	char line[MAX_LINE_SIZE];
	//first output program state
	fprintf(fout2, "#ifndef _PROGRAM_STATE_\n");
	fprintf(fout2, "#define _PROGRAM_STATE_\n\n");
	fprintf(fout2, "struct _GOOMF_host_program_state_struct{\n");
	//char output_type[100];
	//memset(output_type, 0, sizeof(output_type));
	for(i=0; i<_GLOBAL_num_of_program_vars; ++i)
	{
		//getOpenCLType(_GLOBAL_program_vars_buffer[i].type, output_type);
		sprintf(line, "\t%s %s;\n", _GLOBAL_program_vars_buffer[i].type, _GLOBAL_program_vars_buffer[i].name);
		fprintf(fout2, "%s", line);
	}
	fprintf(fout2, "};\n\n");
	fprintf(fout2, "#endif");

	//copy up to the second insert section
	while(fgets(line, sizeof(line), ftemplate) != NULL)
	{
		if (strncmp(line, "//insert2:", 10) == 0)
			break;
		fprintf(fout, "%s", line);
	}

	//second insert section
	char output_type[100];
	memset(output_type, 0, sizeof(output_type));
	for(i=0; i<_GLOBAL_num_of_program_vars; ++i)
	{
		getOpenCLType(_GLOBAL_program_vars_buffer[i].type, output_type);
		sprintf(line, "\t%s %s;\n", output_type, _GLOBAL_program_vars_buffer[i].name);
		fprintf(fout, "%s", line);
	}

	//copy up to the second insert section
	while(fgets(line, sizeof(line), ftemplate) != NULL)
	{
		if (strncmp(line, "//insert2.2:", 10) == 0)
			break;
		fprintf(fout, "%s", line);
	}

	//second insert section
	for(i=0; i<_GLOBAL_num_of_program_vars; ++i)
	{
		sprintf(line, "\t%s %s;\n", _GLOBAL_program_vars_buffer[i].type, _GLOBAL_program_vars_buffer[i].name);
		fprintf(fout, "%s", line);
	}

	//copy up to the 2.3 insert section
	while(fgets(line, sizeof(line), ftemplate) != NULL)
	{
		if (strncmp(line, "//insert2.3:", 10) == 0)
			break;
		fprintf(fout, "%s", line);
	}
	//2.3 insert section
	for(i=0; i<_GLOBAL_num_of_program_vars; ++i)
	{
		if (_GLOBAL_program_vars_buffer[i].isParam)
		{
			sprintf(line, "\t%s %s;\n", _GLOBAL_program_vars_buffer[i].type, _GLOBAL_program_vars_buffer[i].name);
			fprintf(fout, "%s", line);
		}
	}

	//copy up to 2.4 insert section
	while(fgets(line, sizeof(line), ftemplate) != NULL)
	{
		if (strncmp(line, "//insert2.4:", 10) == 0)
			break;
		fprintf(fout, "%s", line);
	}
	fprintf(fout, "\ntransition_type _GOOMF_transitions[%d];\n\n", _GLOBAL_num_of_props);
	fprintf(fout, "//const kernel names\n");
	fprintf(fout, "const char* kernel_monitor_alg_finite_gpu = \"%s/GooMF_GPU_monitor_alg_finite.cl\";\n", _GLOBAL_output_path);
	fprintf(fout, "const char* kernel_monitor_alg_infinite_gpu = \"%s/GooMF_GPU_monitor_alg_infinite.cl\";\n", _GLOBAL_output_path);
	fprintf(fout, "const char* kernel_monitor_alg_partial_gpu = \"%s/GooMF_GPU_monitor_alg_partial.cl\";\n\n", _GLOBAL_output_path);
	fprintf(fout, "//array of property gates\n");

	//output fsm tables if needed
	if (_GLOBAL_fsm_format == fsm_as_table)
	{
		for (j=0; j<_GLOBAL_num_of_props; ++j)
			outputFSMTable(fout, j);
	}

	//create the array of booleans
	memset(line, 0, sizeof(line));
	for (j=0; j<_GLOBAL_num_of_props; ++j)
	{
		if (j == _GLOBAL_num_of_props-1)
			strcat(line, "true");
		else
			strcat(line, "true, ");
	}
	fprintf(fout, "bool property_triggers[%d] = {%s};\n\n", _GLOBAL_num_of_props, line);


	//output user functions if defined
	outputUserFunctions(fout);

	//output partial functions
	fprintf(fout, "unsigned int getNumOfPartialPredicates(int property_number)\n{\n");
	for(i=0; i<_GLOBAL_num_of_props; ++i)
	{
		fprintf(fout, "\tif (property_number == %d) return %d;\n", i, getNumOfPartialPredicates(i));
	}
	fprintf(fout, "\treturn %d;\n}\n\n", getNumOfPartialPredicates(-1));

	fprintf(fout, "bool isPropertyPartial(int property_number)\n{\n");
	for(i=0; i<_GLOBAL_num_of_props; ++i)
	{
		fprintf(fout, "\tif (property_number == %d) return %s;\n", i,
				(getNumOfPartialPredicates(i) > 0) ? "true" : "false" );
	}
	fprintf(fout, "\treturn false;\n}\n");

	//copy up to 2.6 insert section
	while(fgets(line, sizeof(line), ftemplate) != NULL)
	{
		if (strncmp(line, "//insert2.6:", 10) == 0)
			break;
		fprintf(fout, "%s", line);
	}
	//copying the vars
	for (i=0; i<_GLOBAL_num_of_program_vars; ++i)
	{
		sprintf(line, "\tps1->%s = ps2->%s;\n", _GLOBAL_program_vars_buffer[i].name, _GLOBAL_program_vars_buffer[i].name);
		fprintf(fout, "%s", line);
	}

	//copy up to the third insert section
	while(fgets(line, sizeof(line), ftemplate) != NULL)
	{
		if (strncmp(line, "//insert3:", 10) == 0)
			break;
		fprintf(fout, "%s", line);
	}
	fprintf(fout, "\treturn %d;\n", _GLOBAL_num_of_props);

	//copy up to the fourth insert section
	while(fgets(line, sizeof(line), ftemplate) != NULL)
	{
		if (strncmp(line, "//insert4:", 10) == 0)
			break;
		fprintf(fout, "%s", line);
	}
	//fourth insert section
	for(i=0; i<_GLOBAL_num_of_props; ++i)
	{
		sprintf(line, "\tif (property_number == %d) return %d;\n", i, getInitialState(i));
		fprintf(fout, "%s", line);
	}

	//copy up to the fifth insert section
	while(fgets(line, sizeof(line), ftemplate) != NULL)
	{
		if (strncmp(line, "//insert5:", 10) == 0)
			break;
		fprintf(fout, "%s", line);
	}
	fprintf(fout, "int getNumOfInconclusiveStates(int property_number)\n{\n");
	unsigned int total_inc_states = 0;
	for(i=0; i<_GLOBAL_num_of_props; ++i)
	{
		fprintf(fout, "\tif (property_number == %d) ", i);
		fprintf(fout, "return %d;\n", getNumOfInconclusiveStates(&(_GLOBAL_properties_buffer[i].fsm)));
		total_inc_states += getNumOfInconclusiveStates(&(_GLOBAL_properties_buffer[i].fsm));
	}
	fprintf(fout, "\n\t//if specified something else - return total number of inconclusive states\n");
	fprintf(fout, "\treturn %d;\n}", total_inc_states);

	fprintf(fout, "\n\nunsigned int getStateOrder(unsigned int num_of_prop, fsm_state s)\n{");
	for(i=0; i<_GLOBAL_num_of_props; ++i)
	{
		for (j=0; j<_GLOBAL_properties_buffer[i].fsm.num_of_fsm_states; ++j)
		{
			fprintf(fout, "\n\tif (num_of_prop == %d && s == %d) return %d;",
					i, _GLOBAL_properties_buffer[i].fsm.fsm_states_buffer[j].id, j);
		}
	}
	fprintf(fout, "\n\n\treturn 0;\n}");

	fprintf(fout, "\n\nfsm_state getStateID(unsigned int num_of_prop, unsigned int o)\n{");
	for(i=0; i<_GLOBAL_num_of_props; ++i)
	{
		for (j=0; j<_GLOBAL_properties_buffer[i].fsm.num_of_fsm_states; ++j)
		{
			fprintf(fout, "\n\tif (num_of_prop == %d && o == %d) return %d;",
					i, j, _GLOBAL_properties_buffer[i].fsm.fsm_states_buffer[j].id);
		}
	}
	fprintf(fout, "\n\n\treturn 0;\n}");

	//building the argument string for _GOOMF_fill_program_state
	fprintf(fout, "\n\nvoid _GOOMF_fill_program_state(_GOOMF_device_program_state_ptr ps,\n");
	memset(line, 0, sizeof(line));
	for(i=0; i<_GLOBAL_num_of_program_vars; ++i)
	{
		strcat(line, "\t");
		strcat(line, _GLOBAL_program_vars_buffer[i].type);
		strcat(line, " ");
		strcat(line, _GLOBAL_program_vars_buffer[i].name);
		if (i < _GLOBAL_num_of_program_vars-1) strcat(line, ",\n");
	}
	fprintf(fout, "%s)\n{\n", line);

	//copying the vars
	for (i=0; i<_GLOBAL_num_of_program_vars; ++i)
	{
		sprintf(line, "\tps->%s = %s;\n", _GLOBAL_program_vars_buffer[i].name, _GLOBAL_program_vars_buffer[i].name);
		fprintf(fout, "%s", line);
	}
	fprintf(fout, "\n\treturn;\n}\n");

	//copy up to the seventh insert section
	while(fgets(line, sizeof(line), ftemplate) != NULL)
	{
		if (strncmp(line, "//insert6:", 10) == 0)
			break;
		fprintf(fout, "%s", line);
	}
	fprintf(fout, "\tswitch(var_order)\n\t{\n");
	for(i=0; i<_GLOBAL_num_of_program_vars; ++i)
	{
		fprintf(fout, "\tcase %d: ps->%s = *((%s*)var_value); break;\n",
				i, _GLOBAL_program_vars_buffer[i].name, _GLOBAL_program_vars_buffer[i].type);
	}
	fprintf(fout, "\tdefault:\n\t\treturn -1;\n\t}\n\n\treturn 0;\n");

	//copy up to the seventh insert section
	while(fgets(line, sizeof(line), ftemplate) != NULL)
	{
		if (strncmp(line, "//insert7:", 10) == 0)
			break;
		fprintf(fout, "%s", line);
	}
	for(i=0; i<_GLOBAL_num_of_props; ++i)
	{
		if (outputCPUTransitionFunction(&(_GLOBAL_properties_buffer[i]), fout, i) == EXIT_FAILURE)
		{
			fclose(fout);
			fclose(ftemplate);
			return EXIT_FAILURE;
		}
	}

	//copy up to the eighth insert section
	while(fgets(line, sizeof(line), ftemplate) != NULL)
	{
		if (strncmp(line, "//insert8:", 10) == 0)
			break;
		fprintf(fout, "%s", line);
	}
	for(i=0; i<_GLOBAL_num_of_props; ++i)
	{
		sprintf(line, "\ttransitions[%d] = transition%d;\n", i, i);
		fprintf(fout, "%s", line);
	}

	//copy up to the ninth insert section
	while(fgets(line, sizeof(line), ftemplate) != NULL)
	{
		if (strncmp(line, "//insert9:", 10) == 0)
			break;
		fprintf(fout, "%s", line);
	}
	sprintf(line, "\t return ");
	for(i=0; i<_GLOBAL_num_of_program_vars; ++i)
	{
		if (i == 0)
			strcat(line, "sizeof(");
		else
			strcat(line, " + sizeof(");
		getOpenCLType(_GLOBAL_program_vars_buffer[i].type, output_type);
		strcat(line, output_type);
		strcat(line, ")\n\t");
	}
	fprintf(fout, "%s;\n", line);

	//copy up to the end
	while(fgets(line, sizeof(line), ftemplate) != NULL)
		fprintf(fout, "%s", line);

	fclose(fout);
	fclose(ftemplate);

	return EXIT_SUCCESS;
}


/*
 * create file for GPU monitoring using algorithm for infinite-history properties
 */
int outputGPUMonitorAlgInfinite()
{
	FILE* fout = NULL, *ftemplate = NULL;
	int i = 0;

	if ((fout = fopen(monitor_file_GPU_alg_infinite, "w")) == NULL)
	{
		fprintf(stderr, "MonitorGenerator: Cannot create %s for output.\n", monitor_file_GPU_alg_infinite);
		return EXIT_FAILURE;
	}

	char filePath[MAX_LINE_SIZE];
	memset(filePath, 0, sizeof(filePath));
	if (_GLOBAL_templates_path == NULL)
	{
		strcat(filePath, "./Templates/");
		strcat(filePath, monitor_file_GPU_alg_infinite);
	}
	else
	{
		strcat(filePath, _GLOBAL_templates_path);
		strcat(filePath, monitor_file_GPU_alg_finite);
	}
	if ((ftemplate = fopen(filePath, "r")) == NULL)
	{
		fprintf(stderr, "MonitorGenerator: Cannot open %s template for input.\n", filePath);
		fclose(fout);
		return EXIT_FAILURE;
	}

	char line[MAX_LINE_SIZE];

	//copy up to the first insert section
	while( fgets(line, sizeof(line), ftemplate) != NULL )
	{
		if (strncmp(line, "//insert1:", 10) == 0)
			break;
		fprintf(fout, "%s", line);
	}

	//first insert section
	sprintf(line, "#define VARS_IN_STATE\t\t(%d)\n", _GLOBAL_num_of_program_vars);
	fprintf(fout, "%s", line);
	//sprintf(line, "#define DELTA\t(%d)\n", _GLOBAL_delta);
	//fprintf(fout, "%s", line);

	//copy up to the second insert section
	while( fgets(line, sizeof(line), ftemplate) != NULL )
	{
		if (strncmp(line, "//insert2:", 10) == 0)
			break;
		fprintf(fout, "%s", line);
	}
	//second insert section
	for(i=0; i<_GLOBAL_num_of_program_vars; ++i)
	{
		sprintf(line, "\t%s %s;\n", _GLOBAL_program_vars_buffer[i].type, _GLOBAL_program_vars_buffer[i].name);
		fprintf(fout, "%s", line);
	}

	//copy up to the 2.5 insert section
	while( fgets(line, sizeof(line), ftemplate) != NULL )
	{
		if (strncmp(line, "//insert2.5:", 10) == 0)
			break;
		fprintf(fout, "%s", line);
	}
	//insert used parameters structure
	for(i=0; i<_GLOBAL_num_of_program_vars; ++i)
	{
		if (_GLOBAL_program_vars_buffer[i].isParam)
		{
			sprintf(line, "\t%s %s;\n", _GLOBAL_program_vars_buffer[i].type, _GLOBAL_program_vars_buffer[i].name);
			fprintf(fout, "%s", line);
		}
	}

	//copy up to the third insert section
	while(fgets(line, sizeof(line), ftemplate) != NULL)
	{
		if (strncmp(line, "//insert3:", 10) == 0)
			break;
		fprintf(fout, "%s", line);
	}

	//third insert section
	for(i=0; i<_GLOBAL_num_of_program_vars; ++i)
	{
		sprintf(line, "\tps1->%s = ps2->%s;\n", _GLOBAL_program_vars_buffer[i].name, _GLOBAL_program_vars_buffer[i].name);
		fprintf(fout, "%s", line);
	}

	//copy up to the forth insert section
	while(fgets(line, sizeof(line), ftemplate) != NULL)
	{
		if (strncmp(line, "//insert4:", 10) == 0)
			break;
		fprintf(fout, "%s", line);
	}

	//output getStateOrder()
	int j = 0;
	fprintf(fout, "\n\nunsigned int getStateOrder(unsigned int num_of_prop, fsm_state s)\n{");
	for(i=0; i<_GLOBAL_num_of_props; ++i)
	{
		for (j=0; j<_GLOBAL_properties_buffer[i].fsm.num_of_fsm_states; ++j)
		{
			fprintf(fout, "\n\tif (num_of_prop == %d && s == %d) return %d;",
					i, _GLOBAL_properties_buffer[i].fsm.fsm_states_buffer[j].id, j);
		}
	}
	fprintf(fout, "\n\n\treturn 0;\n}");

	//output getStateID()
	fprintf(fout, "\n\nfsm_state getStateID(unsigned int num_of_prop, unsigned int o)\n{");
	for(i=0; i<_GLOBAL_num_of_props; ++i)
	{
		for (j=0; j<_GLOBAL_properties_buffer[i].fsm.num_of_fsm_states; ++j)
		{
			fprintf(fout, "\n\tif (num_of_prop == %d && o == %d) return %d;",
					i, j, _GLOBAL_properties_buffer[i].fsm.fsm_states_buffer[j].id);
		}
	}
	fprintf(fout, "\n\n\treturn 0;\n}\n");

	//only output transition functions if ifs format is chosen
	if (_GLOBAL_fsm_format == fsm_as_ifs)
	{
		for(i=0; i<_GLOBAL_num_of_props; ++i)
		{
			if (outputGPUTransitionFunction(&(_GLOBAL_properties_buffer[i]), fout, i, true) == EXIT_FAILURE)
			{
				fclose(fout);
				fclose(ftemplate);
				return EXIT_FAILURE;
			}
		}
	}

	//output user functions if defined
	outputUserFunctions(fout);

	//copy up to the fifth insert section
	while(fgets(line, sizeof(line), ftemplate) != NULL)
	{
		if (strncmp(line, "//insert5:", 10) == 0)
			break;
		fprintf(fout, "%s", line);
	}

	//output all program state variables
	for (i=0; i<_GLOBAL_num_of_program_vars; ++i)
	{
		sprintf(line, "\t%s %s;\n",
				_GLOBAL_program_vars_buffer[i].type,
				_GLOBAL_program_vars_buffer[i].name);
		fprintf(fout, "%s", line);
	}
	fprintf(fout, "\n");

	//output all predicates
	for (i=0; i<_GLOBAL_num_of_predicates; ++i)
	{
		sprintf(line, "\tbool %s;\n", _GLOBAL_predicates_buffer[i].name);
		fprintf(fout, "%s", line);
	}
	fprintf(fout, "\n");

	//output fsm tables if needed
	if (_GLOBAL_fsm_format == fsm_as_table)
	{
		for (i=0; i<_GLOBAL_num_of_props; ++i)
			outputFSMTable(fout, i);
	}

	//output property-specific kernel code
	for(i=0; i<_GLOBAL_num_of_props; ++i)
	{
		if (outputKernelCodeForPropertyAlgInfinite(&(_GLOBAL_properties_buffer[i]), fout, i) == EXIT_FAILURE)
		{
			fclose(fout);
			fclose(ftemplate);
			return EXIT_FAILURE;
		}
	}

	//output final goto label
	fprintf(fout, "\nprop%d:\n\n", _GLOBAL_num_of_props);

	//copy up to the end
	while(fgets(line, sizeof(line), ftemplate) != NULL)
		fprintf(fout, "%s", line);

	fclose(fout);
	fclose(ftemplate);

	return EXIT_SUCCESS;
}


/*
 * create file for GPU monitoring using algorithm for finite-history properties
 */
int outputGPUMonitorAlgFinite()
{
	FILE* fout = NULL, *ftemplate = NULL;
	int i = 0;

	if ((fout = fopen(monitor_file_GPU_alg_finite, "w")) == NULL)
	{
		fprintf(stderr, "MonitorGenerator: Cannot create %s for output.\n", monitor_file_GPU_alg_finite);
		return EXIT_FAILURE;
	}

	char filePath[MAX_LINE_SIZE];
	memset(filePath, 0, sizeof(filePath));
	if (_GLOBAL_templates_path == NULL)
	{
		strcat(filePath, "./Templates/");
		strcat(filePath, monitor_file_GPU_alg_finite);
	}
	else
	{
		strcat(filePath, _GLOBAL_templates_path);
		strcat(filePath, monitor_file_GPU_alg_finite);
	}
	if ((ftemplate = fopen(filePath, "r")) == NULL)
	{
		fprintf(stderr, "MonitorGenerator: Cannot open %s template for input.\n", filePath);
		fclose(fout);
		return EXIT_FAILURE;
	}

	char line[MAX_LINE_SIZE];

	//copy up to the first insert section
	while( fgets(line, sizeof(line), ftemplate) != NULL )
	{
		if (strncmp(line, "//insert1:", 10) == 0)
			break;
		fprintf(fout, "%s", line);
	}

	//first insert section
	sprintf(line, "#define VARS_IN_STATE\t\t(%d)\n", _GLOBAL_num_of_program_vars);
	fprintf(fout, "%s", line);
	//sprintf(line, "#define DELTA\t(%d)\n", _GLOBAL_delta);
	//fprintf(fout, "%s", line);

	//copy up to the second insert section
	while( fgets(line, sizeof(line), ftemplate) != NULL )
	{
		if (strncmp(line, "//insert2:", 10) == 0)
			break;
		fprintf(fout, "%s", line);
	}

	//second insert section
	for(i=0; i<_GLOBAL_num_of_program_vars; ++i)
	{
		sprintf(line, "\t%s %s;\n", _GLOBAL_program_vars_buffer[i].type, _GLOBAL_program_vars_buffer[i].name);
		fprintf(fout, "%s", line);
	}

	//copy up to the 2.5 insert section
	while( fgets(line, sizeof(line), ftemplate) != NULL )
	{
		if (strncmp(line, "//insert2.5:", 10) == 0)
			break;
		fprintf(fout, "%s", line);
	}
	//insert used parameters structure
	for(i=0; i<_GLOBAL_num_of_program_vars; ++i)
	{
		if (_GLOBAL_program_vars_buffer[i].isParam)
		{
			sprintf(line, "\t%s %s;\n", _GLOBAL_program_vars_buffer[i].type, _GLOBAL_program_vars_buffer[i].name);
			fprintf(fout, "%s", line);
		}
	}

	//copy up to the third insert section
	while(fgets(line, sizeof(line), ftemplate) != NULL)
	{
		if (strncmp(line, "//insert3:", 10) == 0)
			break;
		fprintf(fout, "%s", line);
	}

	//third insert section
	for(i=0; i<_GLOBAL_num_of_program_vars; ++i)
	{
		sprintf(line, "\tps1->%s = ps2->%s;\n", _GLOBAL_program_vars_buffer[i].name, _GLOBAL_program_vars_buffer[i].name);
		fprintf(fout, "%s", line);
	}

	//copy up to the forth insert section
	while(fgets(line, sizeof(line), ftemplate) != NULL)
	{
		if (strncmp(line, "//insert4:", 10) == 0)
			break;
		fprintf(fout, "%s", line);
	}

	//output getStateOrder()
	int j = 0;
	fprintf(fout, "\n\nunsigned int getStateOrder(unsigned int num_of_prop, fsm_state s)\n{");
	for(i=0; i<_GLOBAL_num_of_props; ++i)
	{
		for (j=0; j<_GLOBAL_properties_buffer[i].fsm.num_of_fsm_states; ++j)
		{
			fprintf(fout, "\n\tif (num_of_prop == %d && s == %d) return %d;",
					i, _GLOBAL_properties_buffer[i].fsm.fsm_states_buffer[j].id, j);
		}
	}
	fprintf(fout, "\n\n\treturn 0;\n}");

	//output getStateID()
	fprintf(fout, "\n\nfsm_state getStateID(unsigned int num_of_prop, unsigned int o)\n{");
	for(i=0; i<_GLOBAL_num_of_props; ++i)
	{
		for (j=0; j<_GLOBAL_properties_buffer[i].fsm.num_of_fsm_states; ++j)
		{
			fprintf(fout, "\n\tif (num_of_prop == %d && o == %d) return %d;",
					i, j, _GLOBAL_properties_buffer[i].fsm.fsm_states_buffer[j].id);
		}
	}
	fprintf(fout, "\n\n\treturn 0;\n}\n");

	//only output transition functions if ifs format is chosen
	if (_GLOBAL_fsm_format == fsm_as_ifs)
	{
		for(i=0; i<_GLOBAL_num_of_props; ++i)
		{
			if (outputGPUTransitionFunction(&(_GLOBAL_properties_buffer[i]), fout, i, false) == EXIT_FAILURE)
			{
				fclose(fout);
				fclose(ftemplate);
				return EXIT_FAILURE;
			}
		}
	}

	//output user functions if defined
	outputUserFunctions(fout);

	//copy up to the fifth insert section
	while(fgets(line, sizeof(line), ftemplate) != NULL)
	{
		if (strncmp(line, "//insert5:", 10) == 0)
			break;
		fprintf(fout, "%s", line);
	}

	//output fsm tables if needed
	if (_GLOBAL_fsm_format == fsm_as_table)
	{
		for (i=0; i<_GLOBAL_num_of_props; ++i)
			outputFSMTable(fout, i);
	}

	//init local_min's
	for(i=0; i<_GLOBAL_num_of_props; ++i)
		fprintf(fout, "\t__local uint local_min%d;\n", i);

	//set num_of_transitions to nitems
	fprintf(fout, "\n\tif (local_id == 0)\n\t{\n");
	for(i=0; i<_GLOBAL_num_of_props; ++i)
	{
		fprintf(fout, "\t\tlocal_min%d = nitems;\n", i);
		fprintf(fout, "\t\tnum_of_transitions[%d*num_groups + group_id] = nitems;\n", i);
	}
	fprintf(fout, "\t}\n\n");

	//copy up to the sixth insert section
	while(fgets(line, sizeof(line), ftemplate) != NULL)
	{
		if (strncmp(line, "//insert6:", 10) == 0)
			break;
		fprintf(fout, "%s", line);
	}

	//output all program state variables
	for (i=0; i<_GLOBAL_num_of_program_vars; ++i)
	{
		sprintf(line, "\t%s %s;\n",
				_GLOBAL_program_vars_buffer[i].type,
				_GLOBAL_program_vars_buffer[i].name);
		fprintf(fout, "%s", line);
	}
	fprintf(fout, "\n");

	//output all predicates
	for (i=0; i<_GLOBAL_num_of_predicates; ++i)
	{
		sprintf(line, "\tbool %s;\n", _GLOBAL_predicates_buffer[i].name);
		fprintf(fout, "%s", line);
	}

	//output property-specific kernel code
	for(i=0; i<_GLOBAL_num_of_props; ++i)
	{
		if (outputKernelCodeForPropertyAlgFinite(&(_GLOBAL_properties_buffer[i]), fout, i) == EXIT_FAILURE)
		{
			fclose(fout);
			fclose(ftemplate);
			return EXIT_FAILURE;
		}
	}

	//output final barrier
	fprintf(fout, "\nprop%d:\n\n", _GLOBAL_num_of_props);
	fprintf(fout, "barrier(CLK_LOCAL_MEM_FENCE);\n\tif (local_id == 0)\n\t{\n");
	for (i=0; i<_GLOBAL_num_of_props; ++i)
		fprintf(fout, "\t\tnum_of_transitions[%d*num_groups + group_id] = local_min%d;\n", i, i);
	fprintf(fout, "\n\t}");

	//copy up to the end
	while(fgets(line, sizeof(line), ftemplate) != NULL)
		fprintf(fout, "%s", line);

	fclose(fout);
	fclose(ftemplate);

	return EXIT_SUCCESS;
}


/*
 * Generate GPU kernel code for Algorithm Finite for specific property into fout
 */
int outputKernelPartialCode(FILE* fout)
{
	char line[MAX_LINE_SIZE];
	int i = 0;

	//output all program state variables
	for (i=0; i<_GLOBAL_num_of_program_vars; ++i)
	{
		if (!_GLOBAL_program_vars_buffer[i].isPartial) continue;

		sprintf(line, "\t%s %s;\n",
				_GLOBAL_program_vars_buffer[i].type,
				_GLOBAL_program_vars_buffer[i].name);
		fprintf(fout, "%s", line);
	}
	fprintf(fout, "\n");

	//output all predicates
	for (i=0; i<_GLOBAL_num_of_predicates; ++i)
	{
		if (!_GLOBAL_predicates_buffer[i].isPartial) continue;

		sprintf(line, "\tbool %s;\n", _GLOBAL_predicates_buffer[i].name);
		fprintf(fout, "%s", line);
	}

	//output inner loop
	fprintf(fout, "\n\t//translation from work-item units to actual data\n");
	fprintf(fout, "\tfor (int j=(global_id*work_per_thread); j<((global_id+1)*work_per_thread); ++j)\n\t{\n");
	fprintf(fout, "\t\tcopyProgramState(&temp_program_state, &input_buffer[j]);\n\n");

	memset(line, 0, sizeof(line));
	for (i=0; i<_GLOBAL_num_of_program_vars; ++i)
	{
		if (_GLOBAL_program_vars_buffer[i].isPartial)
		{
			strcat(line, "\t\t");
			strcat(line, _GLOBAL_program_vars_buffer[i].name);
			strcat(line, " = temp_program_state.");
			strcat(line, _GLOBAL_program_vars_buffer[i].name);
			strcat(line, ";\n");
		}
	}
	fprintf(fout, "%s\n", line);

	for (i=0; i<_GLOBAL_num_of_predicates; ++i)
	{
		if (_GLOBAL_predicates_buffer[i].isPartial)
			fprintf(fout, "\t\t%s = (%s);\n\n", _GLOBAL_predicates_buffer[i].name, _GLOBAL_predicates_buffer[i].syntax);
	}

	unsigned int count = 0;
	for (i=0; i<_GLOBAL_num_of_predicates; ++i)
	{
		if (_GLOBAL_predicates_buffer[i].isPartial)
		{
			fprintf(fout, "\t\toutput_buffer[j*%d+%d] = %s;\n", getNumOfPartialPredicates(-1), count, _GLOBAL_predicates_buffer[i].name);
			++count;
		}
	}
	fprintf(fout, "\t}\n");

	return EXIT_SUCCESS;
}


/*
 * create file for GPU monitoring using algorithm for partial GPU offload
 */
int outputGPUMonitorAlgPartial()
{
	FILE* fout = NULL, *ftemplate = NULL;
	int i = 0;

	if ((fout = fopen(monitor_file_GPU_alg_partial, "w")) == NULL)
	{
		fprintf(stderr, "MonitorGenerator: Cannot create %s for output.\n", monitor_file_GPU_alg_partial);
		return EXIT_FAILURE;
	}

	char filePath[MAX_LINE_SIZE];
	memset(filePath, 0, sizeof(filePath));
	if (_GLOBAL_templates_path == NULL)
	{
		strcat(filePath, "./Templates/");
		strcat(filePath, monitor_file_GPU_alg_partial);
	}
	else
	{
		strcat(filePath, _GLOBAL_templates_path);
		strcat(filePath, monitor_file_GPU_alg_partial);
	}
	if ((ftemplate = fopen(filePath, "r")) == NULL)
	{
		fprintf(stderr, "MonitorGenerator: Cannot open %s template for input.\n", filePath);
		fclose(fout);
		return EXIT_FAILURE;
	}

	char line[MAX_LINE_SIZE];

	//copy up to the insert1 section
	while( fgets(line, sizeof(line), ftemplate) != NULL )
	{
		if (strncmp(line, "//insert1:", 10) == 0)
			break;
		fprintf(fout, "%s", line);
	}

	//insert1 section
	for(i=0; i<_GLOBAL_num_of_program_vars; ++i)
	{
		sprintf(line, "\t%s %s;\n", _GLOBAL_program_vars_buffer[i].type, _GLOBAL_program_vars_buffer[i].name);
		fprintf(fout, "%s", line);
	}

	//copy up to the insert2 section
	while(fgets(line, sizeof(line), ftemplate) != NULL)
	{
		if (strncmp(line, "//insert2:", 10) == 0)
			break;
		fprintf(fout, "%s", line);
	}
	for(i=0; i<_GLOBAL_num_of_program_vars; ++i)
	{
		sprintf(line, "\tps1->%s = ps2->%s;\n", _GLOBAL_program_vars_buffer[i].name, _GLOBAL_program_vars_buffer[i].name);
		fprintf(fout, "%s", line);
	}

	//copy up to the forth insert3 section
	while(fgets(line, sizeof(line), ftemplate) != NULL)
	{
		if (strncmp(line, "//insert3:", 10) == 0)
			break;
		fprintf(fout, "%s", line);
	}

	//output user functions if defined
	outputUserFunctions(fout);

	//copy up to the insert4 section
	while(fgets(line, sizeof(line), ftemplate) != NULL)
	{
		if (strncmp(line, "//insert4:", 10) == 0)
			break;
		fprintf(fout, "%s", line);
	}

	if (outputKernelPartialCode(fout) == EXIT_FAILURE)
	{
		fclose(fout);
		fclose(ftemplate);
		return EXIT_FAILURE;
	}

	//copy up to the end
	while(fgets(line, sizeof(line), ftemplate) != NULL)
		fprintf(fout, "%s", line);

	fclose(fout);
	fclose(ftemplate);

	return EXIT_SUCCESS;
}


/*
 * Deallocate memory and finalize everything
 */
void finalize()
{
	if (_GLOBAL_predicates_buffer != NULL)
		free(_GLOBAL_predicates_buffer);

	if (_GLOBAL_properties_buffer != NULL)
		free(_GLOBAL_properties_buffer);

	if (_GLOBAL_program_vars_buffer != NULL)
		free(_GLOBAL_program_vars_buffer);
}


/*
 * Main
 */
int main(int argc, char **argv)
{
	//parsing command-line parameters
	if (parseParameters(argc, argv) != EXIT_SUCCESS)
		return EXIT_FAILURE;

	config_t cfg;

	//init configuration instance
	config_init(&cfg);

	//parsing everything from .cfg file
	if (parseFile(&cfg) != EXIT_SUCCESS)
		return EXIT_FAILURE;

	//with properties in hand, build and parse corresponding FSMs
	if (buildFSMs() != EXIT_SUCCESS)
		return EXIT_FAILURE;

	//create file for regular CPU monitoring
	if (outputCPUMonitor() != EXIT_SUCCESS)
		return EXIT_FAILURE;

	//create file for GPU monitoring using algorithm for finite-history properties
	if (outputGPUMonitorAlgFinite() != EXIT_SUCCESS)
		return EXIT_FAILURE;

	//create file for GPU monitoring using algorithm for infinite-history properties
	if (outputGPUMonitorAlgInfinite() != EXIT_SUCCESS)
		return EXIT_FAILURE;

	//create file for GPU monitoring using algorithm for infinite-history properties
	if (outputGPUMonitorAlgPartial() != EXIT_SUCCESS)
		return EXIT_FAILURE;

	//deallocate structures
	config_destroy(&cfg);
	finalize();

	return EXIT_SUCCESS;
}

