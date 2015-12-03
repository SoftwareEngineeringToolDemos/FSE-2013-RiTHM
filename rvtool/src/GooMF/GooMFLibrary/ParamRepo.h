/*
 ============================================================================
 Name        : ParamRepo.h
 Author      : Shay Berkovich
 Version     : 1.1
 Date		 : 13/08/2012
 Copyright   : Real-Time Embedded Software Group, University of Waterloo
 Description : Header for ParamRepo ADT
 ============================================================================
 */
#ifndef PARAMREPO_H_
#define PARAMREPO_H_
#include <stdio.h>
#include <stdlib.h>

#define NUM_OF_PARAMS 1
/*************************************** errors declarations ***********************************************/
#define _PREPO_SUCCESS 					(0)
#define _PREPO_ERROR			 		(-1)
/***********************************************************************************************************/

struct PInstance_struct{
	unsigned char current_state;
	void* param_list[NUM_OF_PARAMS];
};
typedef struct PInstance_struct* PInstance;

//main repository structure
struct PRepo_struct
{
	struct PInstance_struct* properties_array;
	unsigned int size;
};
typedef struct PRepo_struct* PRepo;

extern int _initParamRepo(PRepo* context, int size);

extern int _destroyParamRepo(PRepo* context);

#endif /* PARAMREPO_H_ */
