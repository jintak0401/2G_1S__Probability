#ifndef __JOINT_PROB_H__
#define __JOINT_PROB_H__

typedef struct Marginal{
	double mean;
	int normalMean;
	double variance; 	
	int normalStddev;
	int* event;
	int eventLength;
	int* sample;
	int sampleLength;
	double* prob;
	int a, b;
	int min, max;
}Marginal;

typedef struct Joint{
	double mean;
	double firstMean;
	double secondMean;
	double variance;
	int** event;
	int eventLength;
	int** sample;
	int firstLength;
	int secondLength;
	double** prob;
	int firstMin;
	int secondMin;
	double correlation;
}Joint;

double normalDist(double mean, double stdev);
Marginal* makeX(double mean, double stddev, int eventNum);
Marginal* makeY(Marginal* X, double mean, double stddev, int eventNum, int a, int b);
Joint* makeJoint(Marginal* X, Marginal* Y);
int* xEvent(Marginal* X);
int* yEvent(Marginal* X, Marginal* Y);
int* makeMarginalSample(Marginal* marginal);
double* makeMarginalProb(Marginal* marginal);
double makeMarginalVariance(Marginal* marginal);
int** jointEvent(Marginal* X, Marginal* Y);
int** makeJointSample(Joint* joint);
double** makeJointProb(Joint* joint);
double makeJointVariance(Joint* joint);
double makeCorrelation(Marginal* X, Marginal* Y, Joint* joint);
void printFile(Marginal* X, Marginal* Y, Joint* joint);
void Delete(Marginal* X, Marginal* Y, Joint* joint);

#endif
