#include "Joint_Prob.h"
#include <time.h>
#include <stdlib.h>

int main(){

	const int eventNum = 10000;
	const double xMean = 100;
	const double yMean = 0;
	const double xStddev = 7;
	const double yStddev = 40;
	const int a = 3;
	const int b = 10000;

	srand(time(NULL));

	Marginal* X = makeX(xMean, xStddev, eventNum);
	Marginal* Y = makeY(X, yMean, yStddev, eventNum, a, b);
	Joint* joint = makeJoint(X, Y);

	printFile(X, Y, joint);

	Delete(X, Y, joint);

}

