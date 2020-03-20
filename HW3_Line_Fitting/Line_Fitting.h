#ifndef __LINE_FITTING_H__
#define __LINE_FITTING_H__

typedef struct Marginal{
	int normalMean;
	int normalStddev;
	int* event;
	int eventLength;
	int a, b;
}Marginal;

typedef struct Line{
	int eventLength;
	int sampleLength;
	int* originalX;
	int* originalY;
	double* fittingY;
	double originalSlope;
	double fittingSlope;
	double originalIntercept;
	double fittingIntercept;
	double correlation;
	double estimate;
	double originalVariance;

	int fittingLength;
  	
} Line;


double normalDist(double mean, double stdev);
Marginal* makeX(double mean, double stddev, int eventNum);
Marginal* makeY(Marginal* X, double mean, double stddev, int eventNum, int a, int b);
Line* makeLine(Marginal* X, Marginal* Y, int sampleLength);
int* xEvent(Marginal* X);
int* yEvent(Marginal* X, Marginal* Y);
double* makeFittingY(Line* line);
double makeEstimate(Line* line);
void printFile(Line* line);
void Delete(Marginal* X, Marginal* Y, Line* line);

#endif
