#include "Line_Fitting.h"
#include <time.h>
#include <stdlib.h>

int main(){

	const int eventNum = 1000;
	const int sampleNum = 1000;
	const double xMean = 0;
	const double yMean = 0;
	const double xStddev = 30;
	const double yStddev = 10;
	const int a = 3;
	const int b = -2;

	srand(time(NULL));

	Marginal* X = makeX(xMean, xStddev, eventNum);
	Marginal* Y = makeY(X, yMean, yStddev, eventNum, a, b);
  Line* line = makeLine(X, Y, sampleNum);

	printFile(line);

	Delete(X, Y, line);
}

