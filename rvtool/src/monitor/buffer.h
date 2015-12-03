
#include "Controller.h"
#include "GooMFInstrumentor.h" 
#include <stdio.h>

#define TRACE __TRACE__

int _GLOBAL_monitoring = 1;
int _GLOBAL_iterations = 0;
int event_count = 0;

int queue_event(struct _GOOMF_host_program_state_struct event, long line_num) {
	if (_GLOBAL_monitoring)
	{
		if(event_count >= BUFF_SIZE_REAL)
		{
			event_count++;
			return -1;
		}
		else {
			event_count++;
			_GOOMF_nextState(context, (void*)(&event)); 
			if (TRACE && logFile) {
				__LOGENTRY__
				fprintf(logFile,"\n");
			}
			if (event_count >= BUFF_SIZE_REAL)
				return -1;
			return 0;
		}
	}
	return 0;
}

void buffer_init() {
	if (_GLOBAL_monitoring)
	{
		initContext(); 
		if (TRACE) {		
			logFile = fopen("/tmp/rithm_trace.log","w");
			if(logFile)
				{
					__LOGHEADER__
					fprintf(logFile,"\n");
			}
		}
	}
}
void buffer_end() {
	if(logFile)
		fclose(logFile);
}
void flush_buffer() {
	event_count = 0;
	_GOOMF_flush(context);
}

