
#include "Controller.h"

#define HISTORY_MAX_SIZE 10

#define KP __PID_P__
#define KI __PID_I__
#define KD __PID_D__


float _error_history[HISTORY_MAX_SIZE];
int _history_size = -1;

float sum_errors = 0;
int previous_error;

void add_error(float error) {
	_history_size++;
	if (_history_size == HISTORY_MAX_SIZE) {
		_history_size = 0;
	}
	sum_errors = sum_errors - _error_history[_history_size] + error;
	_error_history[_history_size] = error;
}

long PID(int error, timespec delta_time, int isBufferTriggered) {
	int i = 0;
	char str1[20];
	char str2[20];
	char str3[20];
	char str4[20];
	float differential_error, adjusted_error;
	float current_error = 0;

	if (error>0)
		error = (int)(((float)error)/(BUFF_SIZE*safety/100)*(BUFF_SIZE*(1-safety/100)));

	if (!isBufferTriggered) {
		delta_time.tv_sec = sampling_period/1000000;
		delta_time.tv_nsec = (sampling_period%1000000)*1000;
		adjusted_error =(float)(error);
	}
	else if ((last_timestamp.tv_sec == 0 && last_timestamp.tv_nsec == 0) || sampling_period == 0) {
		delta_time.tv_nsec = 0;
		delta_time.tv_sec = 0;
		differential_error = 0;
		adjusted_error = (float)(error);
	}
	else {
		differential_error = (float)(error-previous_error)/(float)(delta_time.tv_sec*1000000+delta_time.tv_nsec/1000);

		adjusted_error = (float)(error)/(float)(delta_time.tv_sec*1000000+delta_time.tv_nsec/1000)*(float)(sampling_period);
		
		if (!RTMODE) printf("Orig. e: %i, diff ms: %li, sp ms: %li, adjusted e: %f\n",
				error,
				(delta_time.tv_sec*1000000+delta_time.tv_nsec/1000),
				sampling_period,
			    adjusted_error);
	}

	current_error = (float)(error)*(float)(delta_time.tv_sec*1000000+delta_time.tv_nsec/1000);

	add_error(current_error);
	
	previous_error = error;
	float delta = (float)(error)*(KP) +(float)sum_errors*KI + differential_error*KD;
	if (!RTMODE) printf("Error: %i, Current Error: %f, I:%f, D:%f, Delta=%f\n",error, current_error, sum_errors,
		differential_error, delta);
	if (!RTMODE) printf("PID-->P:%f, I:%f, D=%f\n",(float)(error)*(KP),(float)sum_errors*KI , differential_error*KD);
	//if (delta<0) printf("Decreasing Delta\n");
	
/*	if (sampling_period+(long)delta>100000 || (sampling_period+(long)delta<0 && delta>0.0))
		delta = 100000-sampling_period;

	if (sampling_period+(long)delta>0)
		sampling_period += (long)delta;
	else
		sampling_period = 0;*/

	if (sampling_period+(long)delta<=MAX_SP)
		sampling_period += (long)delta;
		
		if (sampling_period < 100)
			sampling_period = 100;

	return sampling_period;
}
