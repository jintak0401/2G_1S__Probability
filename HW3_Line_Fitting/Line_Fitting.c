#include <stdio.h>
#include <time.h>
#include <math.h> 
#include <stdlib.h>
#include <string.h> 
#include "Line_Fitting.h"
#include "Least_Square.h"

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
  X->event = xEvent(X);
	return X;
}

Marginal* makeY(Marginal* X, double mean, double stddev, int eventNum, int a, int b){
	Marginal* Y = (Marginal*)malloc(sizeof(Marginal));
	Y->eventLength = eventNum;
	Y->normalMean = mean;
	Y->normalStddev = stddev;
	Y->a = a; Y->b = b;
	Y->event = yEvent(X, Y);
	return Y;
}

Line* makeLine(Marginal* X, Marginal* Y, int sampleNum){
	Line* line = (Line*)malloc(sizeof(Line));
	line->eventLength = X->eventLength; 
	line->sampleLength = sampleNum;
  /*
	 *line->originalX = X->event;
	 *line->originalY = Y->event;
   */

	line->originalSlope = Y->a;
	line->originalIntercept = Y->b; 
  line->originalVariance = pow(Y->normalStddev, 2);
  /*leastSquare(line->eventLength, line->originalX, line->originalY, &line->fittingSlope, &line->fittingIntercept, &line->correlation);*/

	
  leastSquare(line->eventLength, X->event, Y->event, &line->fittingSlope, &line->fittingIntercept, &line->correlation);
  X->eventLength = sampleNum;
  Y->eventLength = sampleNum;
  line->originalX = xEvent(X);  
  X->event = line->originalX;
  line->originalY = yEvent(X, Y);
  line->fittingY = makeFittingY(line);
  line->estimate = makeEstimate(line);
	return line;
}

int* xEvent(Marginal* X){
	int* event = (int*)malloc(sizeof(int) * X->eventLength);
	memset(event, 0, sizeof(event));
	for (int i = 0; i < X->eventLength; i++){
		int dist = (int)round(normalDist(X->normalMean, X->normalStddev));	
		event[i] = dist;
	}
	return event;
}

int* yEvent(Marginal* X, Marginal* Y){
	int* event = (int*)malloc(sizeof(int) * Y->eventLength);
	memset(event, 0, sizeof(event));
	for (int i = 0; i < Y->eventLength; i++){
		int dist = (int)round(normalDist(Y->normalMean, Y->normalStddev));	
		event[i] = Y->a * X->event[i] + Y->b + dist;
	}
	return event;
}

double* makeFittingY(Line* line) {
  line->fittingY = (double*)malloc(sizeof(double) * line->sampleLength);
  memset(line->fittingY, 0, sizeof(line->fittingY));
  for (int i = 0; i < line->sampleLength; i++) {
    line->fittingY[i] = line->fittingSlope * line->originalX[i] + line->fittingIntercept;
  }
  return line->fittingY;
}

double makeEstimate(Line* line) {
  line->estimate = 0;
  for (int i = 0; i < line->sampleLength; i++) {
    line->estimate += (line->originalY[i] - line->fittingY[i]) * (line->originalY[i] - line->fittingY[i]);
  }
  return line->estimate / line->sampleLength;
}

void printFile(Line* line){

	FILE* fp = fopen("Result.txt", "w");

	fprintf(fp, "X\tY\tY_hat\n");
  for (int i = 0; i < line->sampleLength; i++) {
    fprintf(fp, "%d\t%d\t%lf\n", line->originalX[i], line->originalY[i], line->fittingY[i]);
  }
  
	fclose(fp);

	printf("E[(Y-Y_hat)^2]\t%lf\n", line->estimate);
	printf("Correlation\t%lf\n", line->correlation);
	printf("Y_hat = %lf * X + %lf\n", line->fittingSlope, line->fittingIntercept);

}

void Delete(Marginal* X, Marginal* Y, Line* line){

	free(X->event);
	free(X);

	free(Y->event);
	free(Y);

	free(line->fittingY); 
	free(line);

}

