#define TRACE __TRACE__

extern FILE *logFile;
void logevent(char *file,long line_num, struct _GOOMF_host_program_state_struct event)
{
	if(!logFile)
		logFile=fopen("/tmp/rithm_trace.log","w");
	if(logFile)
		{
		 fprintf(logFile,"\n________________________________________________________________________________\n");	
		 __LOGHEADER__	 	
		 fprintf(logFile,"\n");		
		 __LOGENTRY__	
		 fprintf(logFile,"In %s \n",file);	
	}
}


