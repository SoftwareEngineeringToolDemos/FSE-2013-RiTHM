/*
 ============================================================================
 Name        : MonitorGenerator.c
 Author      : Shay Berkovich
 Version     : 1.1
 Date		 : 3/07/2012
 Copyright   : Real-Time Embedded Software Group, University of Waterloo
 Description : Implementation for ParamRepo ADT
 ============================================================================
 */
#include "ParamRepo.h"

int _initParamRepo(PRepo* context, int size)
{
	*context = NULL;
	*context = (PRepo)malloc(sizeof(struct PRepo_struct));
	if (*context == NULL)
		return _PREPO_ERROR;

	(*context)->properties_array = (PInstance)malloc(size * sizeof(struct PInstance_struct));
	if ((*context)->properties_array == NULL)
	{
		free(*context);
		*context = NULL;
		return _PREPO_ERROR;
	}

	return _PREPO_SUCCESS;
}

int _destroyParamRepo(PRepo* context)
{
	if (*context != NULL)
	{
		free(*context);
		*context = NULL;
	}

	return _PREPO_SUCCESS;
}


