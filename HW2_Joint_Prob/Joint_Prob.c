#include <stdio.h>
#include <time.h>
#include <math.h> 
#include <stdlib.h>
#include <string.h> 
#include "Joint_Prob.h"

double normalDist(double mean, double stdev){

	static long double v1, v2, s, tmp;
	long double x;

	do{
		long double u1 = (long double)rand() / RAND_MAX;
		long double u2 = (long double)rand() / RAND_MAX;
		v1 = 2 * u1 - 1;
		v2 = 2 * u2 - 1;
		s = v1 * v1 + v2 * v2;
	} while (s >= 1 || s == 0);

	s = sqrt(-2 * log(s) / s);

	tmp = v1 * s;
	tmp = (stdev * tmp) + mean;

	return tmp;
}

Marginal* makeX(double mean, double stddev, int eventNum){
	Marginal* X = (Marginal*)malloc(sizeof(Marginal));
	X->eventLength = eventNum;
	X->normalMean = mean;
	X->normalStddev = stddev;
	X->a = 0; X->b = 0;
	X->min = 0; X->max = 0;
	X->event = xEvent(X);
	X->sample = makeMarginalSample(X);
	X->prob = makeMarginalProb(X);
	X->variance = makeMarginalVariance(X);
	return X;
}

Marginal* makeY(Marginal* X, double mean, double stddev, int eventNum, int a, int b){
	Marginal* Y = (Marginal*)malloc(sizeof(Marginal));
	Y->eventLength = eventNum;
	Y->normalMean = mean;
	Y->normalStddev = stddev;
	Y->a = a; Y->b = b;
	Y->min = 0; Y->max = 0;
	Y->event = yEvent(X, Y);
	Y->sample = makeMarginalSample(Y);
	Y->prob = makeMarginalProb(Y);
	Y->variance = makeMarginalVariance(Y);
	return Y;
}

Joint* makeJoint(Marginal* X, Marginal* Y){
	Joint* joint = (Joint*)malloc(sizeof(Joint));
	joint->mean = 0;
	joint->firstMean = X->mean;
	joint->secondMean = Y->mean;
	joint->eventLength = X->eventLength; 
	joint->firstLength = X->sampleLength;
	joint->secondLength = Y->sampleLength;
	joint->firstMin = X->min;
	joint->secondMin = Y->min;
	joint->event = jointEvent(X, Y);
	joint->sample = makeJointSample(joint);
	joint->prob = makeJointProb(joint); 
	joint->variance = makeJointVariance(joint);
	joint->correlation = makeCorrelation(X, Y, joint);
	return joint;
}

int* xEvent(Marginal* X){
	int* event = (int*)malloc(sizeof(int) * X->eventLength);
	memset(event, 0, sizeof(event));
	for (int i = 0; i < X->eventLength; i++){
		int dist = (int)round(normalDist(X->normalMean, X->normalStddev));	
		event[i] = dist;
		if (event[i] < X->min){
			X->min = event[i];
		}
		else if (event[i] > X->max){
			X->max = event[i];
		}
	}
	X->sampleLength = X->max - X->min + 1;
	return event;
}

int* yEvent(Marginal* X, Marginal* Y){
	int* event = (int*)malloc(sizeof(int) * Y->eventLength);
	memset(event, 0, sizeof(event));
	for (int i = 0; i < Y->eventLength; i++){
		int dist = (int)round(normalDist(Y->normalMean, Y->normalStddev));	
		event[i] = Y->a * X->event[i] + Y->b + dist;
		if (event[i] < Y->min){
			Y->min = event[i];
		}
		else if (event[i] > Y->max){
			Y->max = event[i];
		}
	}
	Y->sampleLength = Y->max - Y->min + 1;
	return event;
}

int* makeMarginalSample(Marginal* marginal){
	int* sample = (int*)malloc(sizeof(int) * marginal->sampleLength);
	memset(sample, 0, sizeof(sample));

	for (int i = 0; i < marginal->eventLength; i++){
		sample[marginal->event[i] - marginal->min]++;
	}
	return sample;
}

double* makeMarginalProb(Marginal* marginal){
	double* prob = (double*)malloc(sizeof(double) * marginal->sampleLength);
	memset(prob, 0.0, sizeof(prob));

	for (int i = 0; i < marginal->sampleLength; i++){
		prob[i] = (double)marginal->sample[i] / marginal->eventLength; 
	}

	return prob;
}

double makeMarginalVariance(Marginal* marginal){
double squareMean = 0.0;
	marginal->mean = 0.0;

	for (int i = 0; i < marginal->sampleLength; i++){
		if (marginal->prob[i] != 0){
			squareMean += pow(i + marginal->min, 2) * marginal->prob[i];
			marginal->mean += (i + marginal->min) * marginal->prob[i];
		}	
	}
	return squareMean - pow(marginal->mean, 2);
}

int** jointEvent(Marginal* X, Marginal* Y){

	int** event = (int**)malloc(sizeof(int*) * X->eventLength);

	for (int i = 0; i < X->eventLength; i++){
		event[i] = (int*)malloc(sizeof(int) * 2);
	} 

	for (int i = 0; i < X->eventLength; i++){
		for (int j = 0; j < 2; j++){
			event[i][j] = 0;
		}
	}

	for (int i = 0; i < X->eventLength; i++){
		event[i][0]	= X->event[i];
		event[i][1] = Y->event[i];
	}
	return event;
}

int** makeJointSample(Joint* joint){

	int** sample = (int**)malloc(sizeof(int*) * joint->firstLength);
	for (int i = 0; i < joint->firstLength; i++){
		sample[i] = (int*)malloc(sizeof(int) * joint->secondLength);
	}

	for (int i = 0; i < joint->firstLength; i++){
		for (int j = 0; j < joint->secondLength; j++){
			sample[i][j] = 0;
		}
	}

	for (int i = 0; i < joint->eventLength; i++){
		sample[joint->event[i][0] - joint->firstMin][joint->event[i][1] - joint->secondMin]++;
	}

	return sample;
}

double** makeJointProb(Joint* joint){
	
	double** prob = (double**)malloc(sizeof(double*) * joint->firstLength);

	for (int i = 0; i < joint->firstLength; i++){
		prob[i] = (double*)malloc(sizeof(double) * joint->secondLength);
	}

	for (int i = 0; i < joint->firstLength; i++){
		for (int j = 0; j < joint->secondLength; j++){
			prob[i][j] = 0;
		}
	}

	for (int i = 0; i < joint->firstLength; i++){
		for (int j = 0; j < joint->secondLength; j++){
			prob[i][j] = (double)joint->sample[i][j] / joint->eventLength;
		}
	}
	return prob;	
}

double makeJointVariance(Joint* joint){

	for (int i = 0; i < joint->firstLength; i++){
		for (int j = 0; j < joint->secondLength; j++){
			if ((i + joint->firstMin) != 0 && (j + joint->secondMin) != 0 && joint->prob[i][j] != 0){
				joint->mean += (i + joint->firstMin) * (j + joint->secondMin) * joint->prob[i][j];
			}
		}
	}
	return joint->mean - (joint->firstMean * joint->secondMean);
}

double makeCorrelation(Marginal* X, Marginal* Y, Joint* joint){
	return joint->variance / sqrt(X->variance * Y->variance);
}

void printFile(Marginal* X, Marginal* Y, Joint* joint){

	FILE* fp = fopen("X_Marginal_Output.txt", "w");

	fprintf(fp, "X\tunit\tf(x)\n");
	for (int i = 0; i < X->sampleLength; i++){
		if (X->prob[i] != 0){
			fprintf(fp, "%d\t%d\t%lf\n", i + X->min, X->sample[i], X->prob[i]);	
		}
	}

	fclose(fp);

	fp = fopen("Y_Marginal_Output.txt", "w");

	fprintf(fp, "Y\tunit\tf(y)\n");
	for (int i = 0; i < Y->sampleLength; i++){
		if (Y->prob[i] != 0){
			fprintf(fp, "%d\t%d\t%lf\n", i + Y->min, Y->sample[i], Y->prob[i]);	
		}
	}    

	fclose(fp);

	fp = fopen("XY_Joint_Output.txt", "w");

	fprintf(fp, "X\tY\tf(x, y)\n");
	for (int i = 0; i < joint->firstLength; i++){
		for (int j = 0; j < joint->secondLength; j++){
			if (joint->prob[i][j] != 0){
				fprintf(fp, "%d\t%d\t%lf\n", i + joint->firstMin, j + joint->secondMin, joint->prob[i][j]);
			}
		}
	} 

	fclose(fp);

	fp = fopen("Correlation.txt", "w");

	fprintf(fp, "X mean\t%lf\nX variance\t%lf\n\n", X->mean, X->variance);
	fprintf(fp, "Y mean\t%lf\nY variance\t%lf\n\n", Y->mean, Y->variance);
	fprintf(fp, "Joint mean\t%lf\nJoint variance\t%lf\n\n", joint->mean, joint->variance);
	fprintf(fp, "Correlation\t%lf\n", joint->correlation);

	fclose(fp);

	printf("\nX = N(%d, %d ^ 2)\tY = %d * X + %d + N(%d, %d ^ 2)\n\n", X->normalMean, X->normalStddev, Y->a, Y->b, Y->normalMean, Y->normalStddev);
	printf("X mean\t\t%lf\nX variance\t%lf\n\n", X->mean, X->variance);
	printf("Y mean\t\t%lf\nY variance\t%lf\n\n", Y->mean, Y->variance);
	printf("Joint mean\t%lf\nJoint variance\t%lf\n\n", joint->mean, joint->variance);
	printf("Correlation\t%lf\n", joint->correlation);

}

void Delete(Marginal* X, Marginal* Y, Joint* joint){

	free(X->event);
	free(X->sample);
	free(X->prob);
	free(X);

	free(Y->event);
	free(Y->sample);
	free(Y->prob);
	free(Y);

	for (int i = 0; i < joint->eventLength; i++){
		free(joint->event[i]);
	}
	for (int i = 0; i < joint->firstLength; i++){
		free(joint->sample[i]);
		free(joint->prob[i]);
	}
	free(joint->event);
	free(joint->sample);
	free(joint->prob);
	free(joint); 

}
