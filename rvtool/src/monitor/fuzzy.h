/*
 * fuzzy.h
 *
 *  Created on: 2013-02-11
 *      Author: ramy
 */

#ifndef FUZZY_H_
#define FUZZY_H_

#include "Controller.h"
#include <math.h>

typedef struct LinearEquation {
	float slope;
	float b;
} LinearEquation;

typedef struct point {
	float x;
	float y;
} Point;

typedef struct pair {
	float x1;
	float x2;
} Pair;

typedef struct mem_function {
	Point left;
	Point center;
	Point right;
	LinearEquation leftEquation;
	LinearEquation rightEquation;
	float moment;
	float area;
} MembershipFunction;

typedef struct mem_function2 {
	Point points[7];
	int numberOfPoints;
	LinearEquation equations[6];
	float moment;
	float area;
} MembershipFunction2;

MembershipFunction inputfunctions[5];
MembershipFunction inputfunctions2[5];
MembershipFunction inputfunctions3[5];
MembershipFunction2 inputfunctions4[5];
MembershipFunction outputfunctions[5];

MembershipFunction ebtfunctions[3];
MembershipFunction ebfunctions[3];
MembershipFunction bsfunctions[3];

int mapping[5][5];
int mapping2[3][3];

float k1 = __CV__;
float k2 = 0.3;

timespec last_timestamp;
float sum_periods = 0;
float sum_squares = 0;
int sample_size = 0;

float avg_err = 0;
int sample_count = 0;
int sample_bt = 0;

float getSamplingPeriodForError(float error, timespec delta_time, int bufferTriggered);
float getSamplingPeriodForError2(float error, timespec delta_time, int bufferTriggered);
float getSamplingPeriodForError3(float error, timespec delta_time, int bufferTriggered);

Point PointMake(float x, float y) {
	Point p;
	p.x = x;
	p.y = y;
	return p;
}

LinearEquation LinearEquationMake(float slope, float b) {
	LinearEquation e;
	e.slope = slope;
	e.b = b;
	return e;
}

MembershipFunction MembershipFunctionMake(Point left, Point center, Point right) {
	MembershipFunction f;
	f.left = left;
	f.center = center;
	f.right = right;
	return f;
}

MembershipFunction2 MembershipFunction2Make(Point points[], int count) {
	MembershipFunction2 f;
	int i = 0;
	f.numberOfPoints = count;
	for(i=0;i<count;i++)
		f.points[i] = points[i];
	return f;
}


LinearEquation GetEquationFromPoints(Point p1, Point p2) {
	LinearEquation e;
	e.slope = (p2.y-p1.y)/(p2.x-p1.x);
	if (p2.x == INFINITY)
		e.b = p1.y - e.slope*p1.x;
	else
		e.b = p2.y - e.slope*p2.x;
	return e;
}

float powf(float a, float b) {
	float result = a;
	while(--b) {
		result *= a;
	}
	return result;
}

void SetEquationParameters(MembershipFunction *f) {
	f->leftEquation = GetEquationFromPoints(f->left, f->center);
	f->rightEquation = GetEquationFromPoints(f->center, f->right);

	f->moment =1.0/6.0*(f->right.x*(f->center.x+f->right.x)-f->left.x*(f->center.x+f->left.x));

	f->area = 0.5*(f->right.x - f->left.x);
}

void SetEquationParameters2(MembershipFunction2 *f) {
	MembershipFunction ftemp;
	int i = 0, j=0;
	for(i=0; i+1<f->numberOfPoints;i++) {
		ftemp = MembershipFunctionMake(f->points[i], f->points[i+1], f->points[i]);
		SetEquationParameters(&ftemp);
		f->equations[j++] = ftemp.leftEquation;
	}
}

float EvaluateFunction(MembershipFunction f, float x) {
	if (x == f.center.x)
		return f.center.y;
	if (x < f.center.x && x >= f.left.x)
		return f.leftEquation.slope*x + f.leftEquation.b;
	if (x > f.center.x && x <= f.right.x)
		return f.rightEquation.slope*x + f.rightEquation.b;
	return 0;
}

// Uses the simplified method
float EvaluateFunction2(MembershipFunction f, float x) {
	if (x == f.center.x)
		return f.center.y;
	if (x < f.center.x && x >= f.left.x)
		return (x - f.left.x)/(f.center.x - f.left.x);
	if (x > f.center.x && x <= f.right.x)
		return (f.right.x - x)/(f.right.x - f.center.x);
	return 0;
}

float EvaluateFunction3(MembershipFunction2 f, float x) {
	int i = 0;
	for (i=0; i+1<f.numberOfPoints;i++) {
		if (x<f.points[i].x)
			return 0;
		if (x<=f.points[i+1].x)
			return f.equations[i].slope*x + f.equations[i].b;
	}
	return 0;
}

Pair solveQuadraticEquation(float a, float b, float c) {
	Pair result;
	float delta = b*b - 4*a*c;
	if (delta>= 0) {
		result.x1 = (-b-sqrt(delta))/2/a;
		result.x2 = (-b+sqrt(delta))/2/a;
	}
	else {
		PRINTDEBUG("************COMPLEX**************%i\n",0);
		result.x1 = (-b)/2/a;
		result.x2 = (-b)/2/a;
	}

	return result;
}

Pair getSamplingPeriodRangeForCoefficient(float coeff_variation) {
	float k = coeff_variation;
	float N = sample_size+1;
	float delta = (N+k*k*(N-1))/(N*N*(N-1));
	float mu_p = sum_periods/(N-1);
	float a,b,c;

	a = 1/(N-1)-delta;
	b = -2*sum_periods*delta;
	c = sum_squares/(N-1)-delta*sum_periods*sum_periods;

	return solveQuadraticEquation(a,b,c);
}

Pair getSamplingPeriodRange() {
	Pair p1, p2, result;
	float sp1, sp2;

	p1 = getSamplingPeriodRangeForCoefficient(k1);

	sp1 = p1.x1;
	sp2 = p1.x2;

	result.x1 = (sp1<sp2) ? sp1 : sp2;
	result.x2 = (sp1>sp2) ? sp1 : sp2;

	return result;
}


float getSamplingPeriodForError4(float error, timespec delta_time, int bufferTriggered) {
	int i = 0,j=0;
	float output = 0.0, adjusted_error = 0.0, sp_error, mean;
	Point points[7];
	Pair range;

	adjusted_error = ((float)BUFF_SIZE*safety/100)-(error/(float)(delta_time.tv_sec*1000000.0+delta_time.tv_nsec/1000.0)*sampling_period/1000.0);

	range = getSamplingPeriodRange();
	mean = sum_periods/sample_size;
	if (mean < range.x1 || mean > range.x2) {
		PRINTDEBUG("*****************BAD RANGE********************%i\n",0);
		return getSamplingPeriodForError2(error, delta_time,bufferTriggered);
	}
	else if (isnan(range.x1) || isnan(range.x2)) {
		PRINTDEBUG("*****************NAN RANGE********************%i\n",0);
		return getSamplingPeriodForError(error,delta_time, bufferTriggered);
	}
	else {
		PRINTDEBUG("---->Range between %f and %f, mean is %f\n", range.x1, range.x2, mean);
	}

	points[0] = PointMake(-INFINITY,1);
	points[1] = PointMake(range.x1*2-mean,1);
	points[2] = PointMake(range.x1 - (mean-range.x1)/2,0);
	inputfunctions4[0] = MembershipFunction2Make(points,3);
	points[0] = PointMake(range.x1*2-mean,0);
	points[1] = PointMake(range.x1 - (mean-range.x1)/2,1);
	points[2] = PointMake(range.x1,0);
	points[3] = PointMake(mean,0);
	points[4] = PointMake(range.x2 - (range.x2-mean)/2,1);
	points[5] = PointMake(range.x2,0);
	inputfunctions4[1] = MembershipFunction2Make(points,6);
	points[0] = PointMake(range.x1 - (mean-range.x1)/2,0);
	points[1] = PointMake(range.x1,1);
	points[2] = PointMake(range.x1 + (mean-range.x1)/2,0);
	points[3] = PointMake(mean,1);
	points[4] = PointMake(range.x2 - (range.x2-mean)/2,0);
	points[5] = PointMake(range.x2,1);
	points[6] = PointMake(range.x2 + (range.x2-mean)/2,0);
	inputfunctions4[2] = MembershipFunction2Make(points,7);
	points[0] = PointMake(range.x1,0);
	points[1] = PointMake(range.x1 + (mean-range.x1)/2,1);
	points[2] = PointMake(mean,0);
	points[3] = PointMake(range.x2,0);
	points[4] = PointMake(range.x2 + (range.x2-mean)/2,1);
	points[5] = PointMake(range.x2*2-mean,0);
	inputfunctions4[3] = MembershipFunction2Make(points,6);
	points[0] = PointMake(range.x2 + (range.x2-mean)/2,1);
	points[1] = PointMake(range.x2*2-mean,1);
	points[2] = PointMake(INFINITY,1);
	inputfunctions4[4] = MembershipFunction2Make(points,3);

	for(i = 0; i<5; i++) {
		for (j=0; j<5; j++) {
			if (i==0) SetEquationParameters2(&inputfunctions4[j]);
			output += EvaluateFunction(inputfunctions[i], adjusted_error)*
					EvaluateFunction3(inputfunctions4[j], sampling_period/100000.0)*
					outputfunctions[mapping[i][j]].moment/outputfunctions[mapping[i][j]].area;
		}
	}

	PRINTDEBUG("Firing: %f,%f,%f,%f,%f\n",
				EvaluateFunction3(inputfunctions4[0], sampling_period/100000.0),
				EvaluateFunction3(inputfunctions4[1], sampling_period/100000.0),
				EvaluateFunction3(inputfunctions4[2], sampling_period/100000.0),
				EvaluateFunction3(inputfunctions4[3], sampling_period/100000.0),
				EvaluateFunction3(inputfunctions4[4], sampling_period/100000.0));

	output = output*10000000.0;
	PRINTDEBUG("Sleep Time is %ld:%ld, E: %f, Adjusted R: %f, dist. %f, Controller out is %f\n",
			delta_time.tv_sec, delta_time.tv_nsec, error, adjusted_error, sp_error, output);

	if (sampling_period+(long)output>2000000000 || (sampling_period+(long)output<0 && output>0.0))
		output = 2000000000-sampling_period;

	if (sampling_period+(long)output>0)
		sampling_period += (long)output;
	else
		sampling_period = 0;

	sum_periods += sampling_period/100000;
	sum_squares += pow(sampling_period/100000,2);
	sample_size++;

	return sampling_period;
}

float getSamplingPeriodForError3(float error,timespec delta_time, int bufferTriggered) {
	int i = 0,j=0;
	float output = 0.0, adjusted_error = 0.0, sp_error, mean;
	Pair range;
	float delta_msec = (delta_time.tv_sec*1000.0+delta_time.tv_nsec/1000000.0);

	//adjusted_error = ((float)BUFF_SIZE*safety/100)-(error/(float)(delta_time.tv_sec*1000.0+delta_time.tv_nsec/1000000.0)*sampling_period);
	adjusted_error = ((float)BUFF_SIZE*safety/100)-error;

	sum_periods += delta_msec;
	sum_squares += delta_msec*delta_msec;
	sample_size++;

	range = getSamplingPeriodRange();
	mean = sum_periods/sample_size;
	if (mean < range.x1 || mean > range.x2) {
		PRINTDEBUG("*****************BAD RANGE********************%i\n",0);
		sum_periods -= delta_msec;
		sum_squares -= delta_msec*delta_msec;
		sample_size--;
		return getSamplingPeriodForError2(error,delta_time, bufferTriggered);
	}
	else {
		PRINTDEBUG("---->Range between %f and %f, mean is %f\n", range.x1, range.x2, mean);
	}

	inputfunctions3[0] = MembershipFunctionMake(PointMake(-INFINITY,1),PointMake(range.x1,1),PointMake(range.x1 + (mean-range.x1)/2,0));
	inputfunctions3[1] = MembershipFunctionMake(PointMake(range.x1,0),PointMake(range.x1 + (mean-range.x1)/2,1),PointMake(mean,0));
	inputfunctions3[2] = MembershipFunctionMake(PointMake(range.x1 + (mean-range.x1)/2,0),PointMake(mean,1),PointMake(mean + (range.x2-mean)/2,0));
	inputfunctions3[3] = MembershipFunctionMake(PointMake(mean,0),PointMake(mean + (range.x2-mean)/2,1),PointMake(range.x2,0));
	inputfunctions3[4] = MembershipFunctionMake(PointMake(mean + (range.x2-mean)/2,0),PointMake(range.x2,1),PointMake(INFINITY,1));

	for(i = 0; i<5; i++) {
		for (j=0; j<5; j++) {
			if (i==0) SetEquationParameters(&inputfunctions3[j]);
			output += EvaluateFunction(inputfunctions[i], adjusted_error)*
					EvaluateFunction(inputfunctions3[j], sampling_period)*
					outputfunctions[mapping[i][j]].moment/outputfunctions[mapping[i][j]].area;
		}
	}

	output = output*__SCALE__;
	PRINTDEBUG("Sleep Time is %ld:%ld, E: %f, Adjusted R: %f, dist. %f, Controller out is %f\n",
			delta_time.tv_sec, delta_time.tv_nsec, error, adjusted_error, sp_error, output);


	if (sampling_period+(long)output>MAX_SP)
		output = MAX_SP-sampling_period;

	if (sampling_period+(long)output>1000)
		sampling_period += (long)output;
	else
		sampling_period = 1000;


	return sampling_period;
}

float getSamplingPeriodForError2(float error,timespec delta_time, int bufferTriggered) {
	int i = 0,j=0;
	float output = 0.0, adjusted_error = 0.0, distance_from_mean = 0.0;
	float delta_msec = (delta_time.tv_sec*1000.0+delta_time.tv_nsec/1000000.0);

	sum_periods += delta_msec;
	sum_squares += delta_msec*delta_msec;
	sample_size++;

	//adjusted_error = ((float)BUFF_SIZE*safety/100)-(error/(float)(delta_time.tv_sec*1000.0+delta_time.tv_nsec/1000000.0)*sampling_period);
	adjusted_error = ((float)BUFF_SIZE*safety/100)-error;

	distance_from_mean = 100*(delta_msec - sum_periods/sample_size)/(sum_periods/sample_size);

	for(i = 0; i<5; i++) {
		for (j=0; j<5; j++) {
			output += EvaluateFunction(inputfunctions[i], adjusted_error)*
					EvaluateFunction(inputfunctions2[j], distance_from_mean)*
					outputfunctions[mapping[i][j]].moment/outputfunctions[mapping[i][j]].area;
		}
	}
	PRINTDEBUG("Firing: %f,%f,%f,%f,%f\n",
				EvaluateFunction(inputfunctions2[0], distance_from_mean),
				EvaluateFunction(inputfunctions2[1], distance_from_mean),
				EvaluateFunction(inputfunctions2[2], distance_from_mean),
				EvaluateFunction(inputfunctions2[3], distance_from_mean),
				EvaluateFunction(inputfunctions2[4], distance_from_mean));

	output = output*__SCALE__;
	PRINTDEBUG("Sleep Time is %ld:%ld, E: %f, Adjusted R: %f, dist. %f, Controller out is %f\n",
			delta_time.tv_sec, delta_time.tv_nsec, error, adjusted_error, distance_from_mean, output);

	if (sampling_period+(long)output>MAX_SP)
		output = MAX_SP-sampling_period;

	if (sampling_period+(long)output>1000)
		sampling_period += (long)output;
	else
		sampling_period = 1000;

	return sampling_period;
}

int getBufferSize() {
	int i = 0,j=0;
	float output = 0.0;

	for(i = 0; i<3; i++) {
		for (j=0; j<3; j++) {
			output += EvaluateFunction(ebtfunctions[i], (float)sample_bt/(float)sample_count)*
					EvaluateFunction(ebfunctions[j], avg_err/BUFF_SIZE)*
					outputfunctions[mapping2[i][j]].moment/outputfunctions[mapping2[i][j]].area;
		}
	}
	PRINTDEBUG("Input is %f, %f, Output is %f, %i\n", (float)sample_bt/(float)sample_count, avg_err/BUFF_SIZE, output, (int)output);
	output = output*5;

	return output;
}

float getSamplingPeriodForError(float error,timespec delta_time, int bufferTriggered) {
	int i = 0;
	float output = 0.0, adjusted_error = 0.0;

	//adjusted_error = ((float)BUFF_SIZE*safety/100)-(error/(float)(delta_time.tv_sec*1000.0+delta_time.tv_nsec/1000000.0)*sampling_period);
	adjusted_error = ((float)BUFF_SIZE*safety/100)-error;

	for(i = 0; i<5; i++) {
		output += EvaluateFunction(inputfunctions[i], adjusted_error)*outputfunctions[i].moment/outputfunctions[i].area;
	}

/*	PRINTDEBUG("Firing: %f,%f,%f,%f,%f\n",
			EvaluateFunction(inputfunctions[0], adjusted_error),
			EvaluateFunction(inputfunctions[1], adjusted_error),
			EvaluateFunction(inputfunctions[2], adjusted_error),
			EvaluateFunction(inputfunctions[3], adjusted_error),
			EvaluateFunction(inputfunctions[4], adjusted_error));*/

	output = output*__SCALE__;
	PRINTDEBUG("Sleep Time is %ld:%ld, E: %f, Adjusted R: %f, Controller out is %f\n",
			delta_time.tv_sec, delta_time.tv_nsec, error, adjusted_error, output);

	if (sampling_period+(long)output>MAX_SP)
		output = 2000000-sampling_period;

	if (sampling_period+(long)output>MAX_SP)
		sampling_period += (long)output;
	else
		sampling_period = 1000;

	return sampling_period;
}

void test(float avg, float bt) {
	int i = 0,j=0;
		float output = 0.0;

		for(i = 0; i<3; i++) {
			for (j=0; j<3; j++) {
				output += EvaluateFunction(ebtfunctions[i], bt)*
						EvaluateFunction(ebfunctions[j], avg)*
						outputfunctions[mapping2[i][j]].moment/outputfunctions[mapping2[i][j]].area;
			}
		}
		PRINTDEBUG("Input is %f, %f, Output is %f, %i\n", bt, avg, output, (int)output);

}

void initFuzzy(int remapped) {
	int i = 0, j=0, map = 0;

	clock_gettime(CLOCK_REALTIME, &last_timestamp);

	inputfunctions[0] = MembershipFunctionMake(PointMake(-INFINITY,1),PointMake(-BUFF_SIZE*(100-safety)/100,1),PointMake(-BUFF_SIZE*(100-safety)/100/2,0));
	inputfunctions[1] = MembershipFunctionMake(PointMake(-BUFF_SIZE*(100-safety)/100,0),PointMake(-BUFF_SIZE*(100-safety)/100/2,1),PointMake(0,0));
	inputfunctions[2] = MembershipFunctionMake(PointMake(-BUFF_SIZE*(100-safety)/100/2,0),PointMake(0,1),PointMake(BUFF_SIZE*safety/100/2,0));
	inputfunctions[3] = MembershipFunctionMake(PointMake(0,0),PointMake(BUFF_SIZE*safety/100/2,1),PointMake(BUFF_SIZE*safety/100,0));
	inputfunctions[4] = MembershipFunctionMake(PointMake(BUFF_SIZE*safety/100/2,0),PointMake(BUFF_SIZE*safety/100,1),PointMake(INFINITY,1));

	inputfunctions2[0] = MembershipFunctionMake(PointMake(-INFINITY,1),PointMake(-30,1),PointMake(-15,0));
	inputfunctions2[1] = MembershipFunctionMake(PointMake(-30,0),PointMake(-15,1),PointMake(0,0));
	inputfunctions2[2] = MembershipFunctionMake(PointMake(-15,0),PointMake(0,1),PointMake(15,0));
	inputfunctions2[3] = MembershipFunctionMake(PointMake(0,0),PointMake(15,1),PointMake(30,0));
	inputfunctions2[4] = MembershipFunctionMake(PointMake(15,0),PointMake(30,1),PointMake(INFINITY,1));

	inputfunctions3[0] = MembershipFunctionMake(PointMake(-INFINITY,1),PointMake(-1,1),PointMake(-0.5,0));
	inputfunctions3[1] = MembershipFunctionMake(PointMake(-1.0,0),PointMake(-0.5,1),PointMake(0,0));
	inputfunctions3[2] = MembershipFunctionMake(PointMake(-0.5,0),PointMake(0,1),PointMake(0.5,0));
	inputfunctions3[3] = MembershipFunctionMake(PointMake(0,0),PointMake(0.5,1),PointMake(1,0));
	inputfunctions3[4] = MembershipFunctionMake(PointMake(0.5,0),PointMake(1,1),PointMake(INFINITY,1));

	outputfunctions[0] = MembershipFunctionMake(PointMake(-1.5,1),PointMake(-1,1),PointMake(-0.5,0));
	outputfunctions[1] = MembershipFunctionMake(PointMake(-1.0,0),PointMake(-0.5,1),PointMake(-0.0,0));
	outputfunctions[2] = MembershipFunctionMake(PointMake(-0.5,0),PointMake(-0.0,1),PointMake(0.5,0));
	outputfunctions[3] = MembershipFunctionMake(PointMake(-0.0,0),PointMake(0.5,1),PointMake(1,0));
	outputfunctions[4] = MembershipFunctionMake(PointMake(0.5,0),PointMake(1,1),PointMake(1.5,1));


	ebtfunctions[0] = MembershipFunctionMake(PointMake(-INFINITY,1),PointMake(0,1),PointMake(0.2,0));
	ebtfunctions[1] = MembershipFunctionMake(PointMake(0,0),PointMake(0.2,1),PointMake(1,0));
	ebtfunctions[2] = MembershipFunctionMake(PointMake(0.2,0),PointMake(1,1),PointMake(INFINITY,1));

	ebfunctions[0] = MembershipFunctionMake(PointMake(-INFINITY,1),PointMake(0,1),PointMake(0.5,0));
	ebfunctions[1] = MembershipFunctionMake(PointMake(0,0),PointMake(0.5,1),PointMake(1,0));
	ebfunctions[2] = MembershipFunctionMake(PointMake(0.5,0),PointMake(1,1),PointMake(INFINITY,1));

	mapping2[0][0] = 2;
	mapping2[0][1] = 2;
	mapping2[0][2] = 1;
	mapping2[1][0] = 3;
	mapping2[1][1] = 3;
	mapping2[1][2] = 2;
	mapping2[2][0] = 4;
	mapping2[2][1] = 4;
	mapping2[2][2] = 3;

	for (i = 0; i<5; i++) {
		for (j=0; j<5; j++) {
			map = i+2-j;
			if (map > 4) map = 4;
			if (map < 0) map = 0;
			mapping[i][j] = map;
		}
	}

	if (remapped) {
		mapping[0][0] = 1;
		mapping[1][0] = 2;
		mapping[0][1] = 0;
		mapping[1][1] = 1;
	}

	for(i = 0; i<5; i++) {
		SetEquationParameters(&inputfunctions[i]);
		SetEquationParameters(&inputfunctions2[i]);
		SetEquationParameters(&inputfunctions3[i]);
		SetEquationParameters(&ebtfunctions[i]);
		SetEquationParameters(&ebfunctions[i]);
		SetEquationParameters(&outputfunctions[i]);
		float value = EvaluateFunction(inputfunctions[i], 25);
		value = EvaluateFunction(inputfunctions3[i], -0.0004);
		PRINTDEBUG("Value is %f for function %i\n", value, i);
	}

	test(1.0,0.0);
	test(0.0,1.0);
}

#endif /* CONTROLLER_H_ */
