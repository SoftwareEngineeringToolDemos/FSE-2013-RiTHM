/*
 ============================================================================
 Name        : Parsing.h
 Author      : Shay Berkovich
 Version     :
 Copyright   : Real-Time Embedded Software Group, University of Waterloo
 Description : Aggregates config file parsing functions, wraps libconfig.h
 ============================================================================
 */
#ifndef FILEPARSING_H_
#define FILEPARSING_H_

#include "Globals.h"
#include <libconfig.h>

/*
 * Parsing the command-line parameters
 */
int parseParameters(int argc, char **argv)
{
	// Command line options for config file
	int c;

	//parsing the arguments
	while ((c = getopt(argc, argv, "o:c:t:l:f:")) != -1)
	{
		switch (c) {
		case 'c':
			_GLOBAL_config_file_name = optarg;
			break;
		case 'o':
			_GLOBAL_output_path = optarg;
			break;
		case 'l':
			_GLOBAL_LTL_tools_path = optarg;
			break;
		case 't':
			_GLOBAL_templates_path = optarg;
			break;
		case 'f':
			if (!strcmp(optarg, "table"))
				_GLOBAL_fsm_format = fsm_as_table;
			else
				_GLOBAL_fsm_format = fsm_as_ifs;
			break;
		case '?':
			if (optopt == 'c')
				fprintf (stderr, "MonitorGenerator: option -%c requires arguments.\n", optopt);
			if (optopt == 'o')
				fprintf (stderr, "MonitorGenerator: option -%o requires arguments.\n", optopt);
			else
				fprintf (stderr, "MonitorGenerator: Unknown option character `\\x%x'.\n", optopt);
			return 1;
		default:
			return EXIT_FAILURE;
		}
	}

	//last arguments check
	if (_GLOBAL_config_file_name == NULL)
	{
		fprintf(stderr, "MonitorGenerator: Wrong command-line parameters.\n");
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}


/*
 * Parsing properties
 */
int parseProperties(config_t* cfg)
{
	config_setting_t *setting;

	//read predicates
	setting = config_lookup(cfg, "properties");
	if(setting == NULL)
	{
		fprintf(stderr, "MonitorGenerator: No 'properties' list in configuration file.\n");
		return EXIT_FAILURE;
	}

	//allocate properties_buffer
	_GLOBAL_num_of_props = config_setting_length(setting);
	if ((_GLOBAL_properties_buffer = (property_struct*)malloc(_GLOBAL_num_of_props * sizeof(property_struct))) == NULL)
		return EXIT_FAILURE;
	memset(_GLOBAL_properties_buffer, 0, _GLOBAL_num_of_props * sizeof(property_struct));

	int i = 0;
	for(i=0; i<_GLOBAL_num_of_props; ++i)
	{
		config_setting_t *property = config_setting_get_elem(setting, i);

		//only lookup the properties with full definition
		if(!config_setting_lookup_string(property, "name", &_GLOBAL_properties_buffer[i].name) ||
				!config_setting_lookup_string(property, "formalism", &_GLOBAL_properties_buffer[i].formalism) ||
				!config_setting_lookup_string(property, "syntax", &_GLOBAL_properties_buffer[i].syntax))
		{
			fprintf(stderr, "MonitorGenerator: Property %d - wrong specification.\n", i);
			return EXIT_FAILURE;
		}
	}

	return EXIT_SUCCESS;
}


/*
 * Parsing predicates
 */
int parsePredicates(config_t* cfg)
{
	config_setting_t *setting;

	//read predicates
	setting = config_lookup(cfg, "predicates");
	if(setting == NULL)
	{
		fprintf(stderr, "MonitorGenerator: No 'predicates' list in configuration file.\n");
		return EXIT_FAILURE;
	}

	//allocate properties_buffer
	_GLOBAL_num_of_predicates = config_setting_length(setting);
	if ((_GLOBAL_predicates_buffer = (predicate_struct*)malloc(_GLOBAL_num_of_predicates * sizeof(predicate_struct)))
			== NULL)
		return EXIT_FAILURE;
	memset(_GLOBAL_predicates_buffer, 0, _GLOBAL_num_of_predicates * sizeof(predicate_struct));

	int i = 0;
	for(i=0; i<_GLOBAL_num_of_predicates; ++i)
	{
		config_setting_t *predicate = config_setting_get_elem(setting, i);
		char isPartialarray[10];
		const char * isPartial = isPartialarray;
		//only lookup the properties with full definition
		if(!config_setting_lookup_string(predicate, "name", &_GLOBAL_predicates_buffer[i].name) ||
			!config_setting_lookup_string(predicate, "syntax", &_GLOBAL_predicates_buffer[i].syntax))
			continue;

		config_setting_lookup_string(predicate, "device",&isPartial);
		if (isPartial != NULL)
			 if(!strcmp(isPartial, "GPU"))
				_GLOBAL_predicates_buffer[i].isPartial = true;
		isPartial = NULL;
	}

	return EXIT_SUCCESS;
}

/*
 * Parsing program state variables
 */
int parseProgramVars(config_t* cfg)
{
	config_setting_t *setting;
	int i = 0, j = 0;

	//read predicates
	setting = config_lookup(cfg, "program_variables");
	if(setting == NULL)
	{
		fprintf(stderr, "MonitorGenerator: No 'program_variables' list in configuration file.\n");
		return EXIT_FAILURE;
	}

	//allocate properties_buffer
	_GLOBAL_num_of_program_vars = config_setting_length(setting);
	if ((_GLOBAL_program_vars_buffer = (program_var_struct*)malloc(_GLOBAL_num_of_program_vars * sizeof(program_var_struct))) == NULL)
		return EXIT_FAILURE;
	memset(_GLOBAL_program_vars_buffer, 0, _GLOBAL_num_of_program_vars * sizeof(program_var_struct));

	for(i=0; i<_GLOBAL_num_of_program_vars; ++i)
	{
		config_setting_t *program_var = config_setting_get_elem(setting, i);

		//only lookup the variables with full definition
		if(!config_setting_lookup_string(program_var, "name", &_GLOBAL_program_vars_buffer[i].name) ||
				!config_setting_lookup_string(program_var, "type", &_GLOBAL_program_vars_buffer[i].type))
			continue;

		_GLOBAL_program_vars_buffer[i].isPartial = false;
		_GLOBAL_max_chars_in_var = (_GLOBAL_max_chars_in_var >= strlen(_GLOBAL_program_vars_buffer[i].name)) ?
			_GLOBAL_max_chars_in_var : strlen(_GLOBAL_program_vars_buffer[i].name);
	}

	for(i=0; i<_GLOBAL_num_of_predicates; ++i)
	{
		if (!_GLOBAL_predicates_buffer[i].isPartial)
			continue;

		for (j=0; j<_GLOBAL_num_of_program_vars; ++j)
		{
			if (strstr(_GLOBAL_predicates_buffer[i].syntax, _GLOBAL_program_vars_buffer[j].name) != NULL)
				_GLOBAL_program_vars_buffer[j].isPartial = true;
		}
	}

	return EXIT_SUCCESS;
}


/*
 * Parsing functions (optional)
 */
int parseFunctions(config_t* cfg)
{
	config_setting_t *setting;

	//read predicates - still fine if no functions present
	setting = config_lookup(cfg, "functions");
	if(setting == NULL)
		return EXIT_SUCCESS;

	//allocate properties_buffer
	_GLOBAL_num_of_functions = config_setting_length(setting);
	if ((_GLOBAL_functions_buffer = (char**)malloc(_GLOBAL_num_of_functions * MAX_FUNCTION_SIZE * sizeof(char))) == NULL)
		return EXIT_FAILURE;
	memset(_GLOBAL_functions_buffer, '\0', _GLOBAL_num_of_functions * MAX_FUNCTION_SIZE);

	int i = 0;
	for(i=0; i<_GLOBAL_num_of_functions; ++i)
	{
		_GLOBAL_functions_buffer[i] = config_setting_get_string_elem(setting, i);
	}

	return EXIT_SUCCESS;
}


/*
 * Parsing the .cfg file
 */
int parseFile(config_t* cfg)
{
	//read the file; if there is an error, report it and exit
	if(!config_read_file(cfg, _GLOBAL_config_file_name))
	{
		fprintf(stderr, "MonitorGenerator: %s:%d - %s\n", config_error_file(cfg), config_error_line(cfg), config_error_text(cfg));
		config_destroy(cfg);
		return EXIT_FAILURE;
	}

	//read the program name
	if(!config_lookup_string(cfg, "program_name", &_GLOBAL_program_name))
	{
		fprintf(stderr, "MonitorGenerator: No 'program_name' setting in configuration file.\n");
		config_destroy(cfg);
		return EXIT_FAILURE;
	}

	if (parseProperties(cfg) != EXIT_SUCCESS)
		return EXIT_FAILURE;

	if (parsePredicates(cfg) != EXIT_SUCCESS)
		return EXIT_FAILURE;

	if (parseProgramVars(cfg) != EXIT_SUCCESS)
		return EXIT_FAILURE;

	if (parseFunctions(cfg) != EXIT_SUCCESS)
		return EXIT_FAILURE;

	return EXIT_SUCCESS;
}

#endif /* PARSING_H_ */
