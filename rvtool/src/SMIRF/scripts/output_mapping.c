#include<stdio.h>
#include<stdlib.h>
#include "lookup_struct.h"
#include <string.h>

int DEBUG=0;

lookup_table_t	**table;
char		**mon_vars;
unsigned int table_entries_num=0;
unsigned int table_entry_index = 0;
unsigned int mon_vars_num=0;
unsigned int mon_vars_index=0;

void printHelp()
{
	printf("Incorrect syntax\n");
	printf("Syntax: ./output_mapping path_to_sat_solution_file path_to_lookup_table path_to_monitor_vars_file\n");
}

int main(int argc, char **argv)
{
	FILE *soln_ptr, *lookup_file_ptr, *output_file_ptr, *mon_vars_ptr;
	char	*tokenizer;
	char 	temp_str[MAX_LINE_LENGTH];

	if(argc < 4) {
		printHelp();
		return(EXIT_FAILURE);
	}

	char * soln_file_name = argv[1];
	char * lookup_file_name = argv[2];
	char * mon_vars_name = argv[3];

	if(DEBUG) {
		printf("%s\n", soln_file_name);
		printf("%s\n",lookup_file_name);
	}

	soln_ptr = fopen(soln_file_name, "r");
	if(!soln_ptr) {
		fprintf(stderr, "Cannot open Solution file\n");
		return(EXIT_FAILURE);
	}

	lookup_file_ptr = fopen(lookup_file_name, "r");
	if(!lookup_file_ptr) {
		fprintf(stderr, "Cannot open Lookup table file\n");
		return(EXIT_FAILURE);
	}

	output_file_ptr = fopen(OUTPUT_FILE, "w");
	if(!output_file_ptr) {
		fprintf(stderr,"Cannot open output file for writing\n");
		return(EXIT_FAILURE);
	}

	mon_vars_ptr = fopen(mon_vars_name, "r");
	if(!mon_vars_ptr) {
		fprintf(stderr, "Cannot open mon_vars file\n");
		return(EXIT_FAILURE);
	}

	//This is done to check for the number of entries in the lookup table	
	while(fscanf(lookup_file_ptr,"%s",temp_str)!=EOF) {
		table_entries_num++;
		if(DEBUG)
			printf("Number of Lookup Table Entries: %u\n",table_entries_num);
	}

	table = (lookup_table_t *) malloc(table_entries_num*sizeof(lookup_table_t *));
	if(!table) {
		fprintf(stderr,"Cannot allocate memory for lookup table\n");
		return(EXIT_FAILURE);
	}
	fclose(lookup_file_ptr);

	lookup_file_ptr = fopen(lookup_file_name, "r");
	if(!lookup_file_name) {
		fprintf(stderr, "Cannot open Lookup table file\n");
		return(EXIT_FAILURE);
	}

	while(fscanf(lookup_file_ptr,"%s",temp_str)!=EOF) {
		table[table_entry_index] = (lookup_table_t *) malloc(sizeof(lookup_table_t));
		if(!table[table_entry_index]) {
			fprintf(stderr, "Failed in allocating memory for the table entry\n");
			return(EXIT_FAILURE);
		}
		table_entry_index++;	
		table[table_entry_index-1]->v = strdup(strtok(temp_str,","));
		table[table_entry_index-1]->functionName = strdup(strtok(NULL, ","));
		table[table_entry_index-1]->sequenceNumber = strdup(strtok(NULL, ","));
		table[table_entry_index-1]->nodeName = strdup(strtok(NULL, ","));
		table[table_entry_index-1]->nodeType = strdup(strtok(NULL, ","));
		table[table_entry_index-1]->start_line_num = strdup(strtok(NULL, ","));
		table[table_entry_index-1]->end_line_num = strdup(strtok(NULL, ","));
		table[table_entry_index-1]->weight = strdup(strtok(NULL, ","));
	}

	if(DEBUG) {
		unsigned int i;
		for(i=0;i<table_entry_index;i++) {
			printf("%s,%s,%s,%s,%s,%s,%s,%s\n",table[i]->v, table[i]->functionName, table[i]->sequenceNumber, table[i]->nodeName, table[i]->nodeType, table[i]->start_line_num, table[i]->end_line_num, table[i]->weight);
		}
	}

	//Reading the mon_vars file and counting the number of variables to monitor
	while(fscanf(mon_vars_ptr,"%s",temp_str)!=EOF) {
		mon_vars_num++;
	}
	
	if(DEBUG) {
		printf("Number of monitored variables: %u\n",mon_vars_num);
	}
	fclose(mon_vars_ptr);

	mon_vars = (char *) malloc(mon_vars_num*sizeof(char *));
	if(!mon_vars) {
		fprintf(stderr,"Cannot allocate memory for mon_vars\n");
		return(EXIT_FAILURE);
	}	
	
	mon_vars_ptr = fopen(mon_vars_name, "r");
	if(!mon_vars_ptr) {
		fprintf(stderr, "Cannot open mon_vars file\n");
		return(EXIT_FAILURE);
	}

	while(fscanf(mon_vars_ptr,"%s",temp_str)!=EOF) {
		mon_vars[mon_vars_index++] = strdup(temp_str);
	}

	//Writing the mandatory lines
	fprintf(output_file_ptr, "GOOMF_Header,<#include \"GOOMFInstrumentor.h\">\n");
	fprintf(output_file_ptr, "GOOMF_Init,<initContext();>\n");
	fprintf(output_file_ptr, "GOOMF_Dest,<destroyContext();>\n");

	unsigned temp_counter1,temp_counter2;
	while(fscanf(soln_ptr,"%s",temp_str)!=EOF) {
		for(temp_counter1=0;temp_counter1<table_entry_index;temp_counter1++) {
			//Checking whether the strings are equal
			if(strcmp(temp_str, table[temp_counter1]->sequenceNumber) == 0) {
				for(temp_counter2=0;temp_counter2<mon_vars_num;temp_counter2++) {
					fprintf(output_file_ptr,"%s,%s,%s,<_GOOMF_nextState(context, 0, (void *)(&%s));>\n",table[temp_counter1]->functionName, mon_vars[temp_counter2], table[temp_counter1]->start_line_num, mon_vars[temp_counter2]);
				}		
			}
		}	
	}

	fclose(soln_ptr);
	fclose(lookup_file_ptr);
	fclose(output_file_ptr);
	fclose(mon_vars_ptr);
	
	return(EXIT_SUCCESS);
}
