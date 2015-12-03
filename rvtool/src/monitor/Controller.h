/*
 * Controller.h
 *
 *  Created on: 2013-02-19
 *      Author: ramy
 */

#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include <sys/time.h>
#include <time.h>
#include "ProgramState.h"

typedef struct timespec timespec;

#define INITIAL_SP __INITIALPP__
#define MAX_SP __MAXPP__
#define MAX_BS __MAXBS__
#define FREQ __FREQ__

int RTMODE = 1;
int BUFF_SIZE = __BS__;
int BUFF_SIZE_REAL = __BS__;
int safety = __SAFETY__;
int controller_type = __CONTROLLERTYPE__;
int PROPMODE = 0;
struct _GOOMF_host_program_state_struct* events;
timespec last_timestamp;

long int sampling_period = INITIAL_SP;

#define F(X) fixedpt_fromint(X)
/*
#define INTBY10(X) FIXEDBY10(F(X))
#define INTBY100(X) FIXEDBY100(F(X))
#define INTBY1000(X) FIXEDBY1000(F(X))

#define FIXEDBY10(X) fixedpt_div(X,F(10))
#define FIXEDBY100(X) fixedpt_div(X,F(100))
#define FIXEDBY1000(X) fixedpt_div(X,F(1000))
*/

#define PRINTDEBUG(X, ...) if (!RTMODE && !PROPMODE) printf(X, __VA_ARGS__)
#define PRINTPROP(X, ...) if (!RTMODE && PROPMODE) printf(X, __VA_ARGS__)
#define PRINTRT(X, ...) if (RTMODE) {(fp!=0) ? fprintf(fp,X, __VA_ARGS__) : printf(X, __VA_ARGS__);}

void initController() {

}

timespec diff(timespec start, timespec end)
{
	timespec temp;
	if ((end.tv_nsec-start.tv_nsec)<0) {
		temp.tv_sec = end.tv_sec-start.tv_sec-1;
		temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
	} else {
		temp.tv_sec = end.tv_sec-start.tv_sec;
		temp.tv_nsec = end.tv_nsec-start.tv_nsec;
	}
	return temp;
}

#endif /* CONTROLLER_H_ */
