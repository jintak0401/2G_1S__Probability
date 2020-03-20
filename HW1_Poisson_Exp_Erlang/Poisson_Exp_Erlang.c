#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int* eventInitialize(int eventLength);
int* poissonInitialize(int poissonLength);
int* expInitialize(int expLength);
int* erlangInitialize(int erlangLength);
void Event(int event[], int length, int lambda);
void count(int event[], int poisson[], int exp[], int erlang[], int length, int n, int k);
void poissonPrinting(int poisson[], int poissonLength, int n, int lambda);
void expPrinting(int exp[], int expLength, int lambda);
void erlangPrinting(int erlang[], int erlangLength, int k, int lambda);

double theoriticalPoisson(int x, int n, int lambda);
double theoriticalExp(int x, int lambda);
double theoriticalErlang(int x, int k, int lambda);
double combi(int n, int r);
double factorial(int n);

int main(){
	const int n = 100;
	const int N = 10000;
	const int k = 2;
	const int lambda = 10;
	const int eventLength = n * N;
	const int poissonLength = N;
	const int expLength = 15 * N;
	const int erlangLength = 10 * N;

	int* event = eventInitialize(eventLength);
	
	int* poisson = poissonInitialize(poissonLength);

	int* exp = expInitialize(expLength);

	int* erlang = erlangInitialize(erlangLength);

	Event(event, eventLength, lambda);
	count(event, poisson, exp, erlang, eventLength, n, k);
	
	poissonPrinting(poisson, poissonLength, n, lambda);
	expPrinting(exp, expLength, lambda);
	erlangPrinting(erlang, erlangLength, k, lambda);

	free(event);
	free(poisson);
	free(exp);
	free(erlang);
}

int* eventInitialize(int eventLength){
	int* event = (int*)malloc(sizeof(int) * eventLength);
	for (int i = 0; i < eventLength; i++){
		event[i] = 0;
	}
	return event;
}

int* poissonInitialize(int poissonLength){
	int* poisson = (int*)malloc(sizeof(int) * poissonLength);
	for (int i = 0; i < poissonLength; i++){
		poisson[i] = 0;
	}
	return poisson;
}

int* expInitialize(int expLength){
	int* exp = (int*)malloc(sizeof(int) * expLength);
	for (int i = 0; i < expLength; i++){
		exp[i] = 0;
	}
	return exp;
}

int* erlangInitialize(int erlangLength){
	int* erlang = (int*)malloc(sizeof(int) * erlangLength);
	for (int i = 0; i < erlangLength; i++){
		erlang[i] = 0;
	}
	return erlang;
}

void Event(int event[], int length, int lambda){
	srand(time(NULL));
	for (int i = 0; i < length; i++){
		event[i] = rand() % lambda;	
	}
}

void count(int event[], int poisson[], int exp[], int erlang[], int length, int n, int k){
	int erlangCheck = 0;
	int poissonUnit = 0;
	int expGap = 0;
	int erlangGap = 0;
	int expIndex = 0;
	int erlangIndex = 0;
	for (int i = 0; i < length; i++){
		expGap++;	erlangGap++;
		if (event[i] == 0){
			poisson[i / n]++;	
			exp[expIndex++] = expGap;
			expGap = 0;
			if (erlangCheck == k - 1){
				erlang[erlangIndex++] = erlangGap;
				erlangGap = 0;
				erlangCheck = 0;
			}
			else{
				erlangCheck++;
			}
		}
	}
}

void poissonPrinting(int poisson[], int poissonLength, int n, int lambda){
	double value = 0, theory = 0, diff = 0;
	double* poissonCount = (double*)malloc(sizeof(double) * poissonLength);	//for Probability
	for (int i = 0; i < poissonLength; i++){
		poissonCount[i] = 0;
	}
	for (int i = 0; i < poissonLength; i++){
		poissonCount[poisson[i]]++;																		// poissonCount[i] for P[X = i]
	}
	printf("===============poisson===============\n");
	printf(" X\tEvent\t\tTheory\t\tDiff(Event - Theory)\n");
	for (int i = 0; i < poissonLength; i++){
		if (poissonCount[i] != 0){
			value = poissonCount[i] / poissonLength;
			theory = theoriticalPoisson(i, n, lambda);
			diff = value - theory; 
			printf("%2d\t%lf\t%lf\t%lf\n", i, value, theory, diff);
		}
	}
	printf("\n");
	free(poissonCount);
}

void expPrinting(int exp[], int expLength, int lambda){
	int unit = 0;
	double sum = 0, theory = 0, diff = 0;
	double* expCount = (double*)malloc(sizeof(double) * expLength);		// for Probability
	for (int i = 0; i < expLength; i++){
		expCount[i] = 0;
	}
	for (int i = 1; i < expLength; i++){
		if (exp[i] != 0){
			expCount[exp[i]]++;																		// expCount[i] for P[X <= i] which is CDF
			unit++;
		}
	}
	printf("===============exponential===============\n");
	printf(" X\tEvent\t\tTheory\t\tDiff(Event - Theory)\n");
	for (int i = 0; i < expLength; i++){
		if (expCount[i] > 0){
			sum += expCount[i] / unit;
			theory = theoriticalExp(i, lambda);
			diff = sum - theory;
			printf("%2d\t%lf\t%lf\t%lf\n", i, sum, theory, diff);
		}
	}
	printf("\n");
	free(expCount);
}

void erlangPrinting(int erlang[], int erlangLength, int k, int lambda){
	int unit = 0;
	double sum = 0, diff = 0, theory = 0;
	double* erlangCount = (double*)malloc(sizeof(double) * erlangLength); // for Probability
	for (int i = 0; i < erlangLength; i++){
		erlangCount[i] = 0;
	}
	for (int i = k; i < erlangLength; i++){
		if (erlang[i] != 0){
			erlangCount[erlang[i]]++;                                // erlangCount[i] for P[X <= i] which is CDF
			unit++;
		}
	}
	printf("===============erlang===============\n");
	printf(" X\tEvent\t\tTheory\t\tDiff(Event - Theory)\n");
	for (int i = 0; i < erlangLength; i++){
		if (erlangCount[i] != 0){
			sum += erlangCount[i] / unit;
			theory = theoriticalErlang(i, k, lambda);
			diff = sum - theory;
			printf("%2d\t%lf\t%lf\t%lf\n", i, sum, theory, diff);
		}
	}
	printf("\n");
	free(erlangCount);
}

double theoriticalPoisson(int x, int n, int lambda){
	return combi(n, x) * pow(1.0 / lambda, x) * pow(1 - (1.0 / lambda), n - x);		
}

double theoriticalExp(int x, int lambda){
	return 1 - exp((-1.0) / lambda * x);				
}

double theoriticalErlang(int x, int k, int lambda){
	double sub = 0.0;
	for (int i = 0; i < k; i++){
		sub += pow(1.0 / lambda * x, i) / factorial(i); 
	}
	sub *= exp((-1.0) / lambda * x);
	return 1 - sub; 
}

double combi(int n, int r){
	if (r > n / 2) r = n - r;
	double ans = 1.0;
	for (int i = 0; i < r; i++){
		ans *= n - i;	
	}
	for (int i = 0; i < r; i++){
		ans /= (i + 1);	
	}
	return ans;
}

double factorial(int n){
	double ans = 1.0;
	for (int i = 2; i <= n; i++){
		ans *= i;
	}
	return ans;
}

