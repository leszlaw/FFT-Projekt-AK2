#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <iostream>

using namespace std;

#define MODULO_AMOUNT 3
#define MAX_INPUT_LEN   80
#define MAX_FLT_LEN     63
#define BUFFER_LEN      (MAX_FLT_LEN - 1 + MAX_INPUT_LEN)
#define n 50
int modulo[MODULO_AMOUNT] = { 2 * n + 1, 2 * n, 2 * n - 1 }; //Jednoznaczna reprezentacja liczby od -(M-1)/2 do (M-1)/2 dla nieparzystych i od -M/2 do (M/2 -1) dla przarzystych, gdzie M=m1*m2*m3

#define FILTER_LEN  63
double coefficients[FILTER_LEN] =
{
  -0.0448093,  0.0322875,   0.0181163,   0.0087615,   0.0056797,
   0.0086685,  0.0148049,   0.0187190,   0.0151019,   0.0027594,
  -0.0132676, -0.0232561,  -0.0187804,   0.0006382,   0.0250536,
   0.0387214,  0.0299817,   0.0002609,  -0.0345546,  -0.0525282,
  -0.0395620,  0.0000246,   0.0440998,   0.0651867,   0.0479110,
   0.0000135, -0.0508558,  -0.0736313,  -0.0529380,  -0.0000709,
   0.0540186,  0.0766746,   0.0540186,  -0.0000709,  -0.0529380,
  -0.0736313, -0.0508558,   0.0000135,   0.0479110,   0.0651867,
   0.0440998,  0.0000246,  -0.0395620,  -0.0525282,  -0.0345546,
   0.0002609,  0.0299817,   0.0387214,   0.0250536,   0.0006382,
  -0.0187804, -0.0232561,  -0.0132676,   0.0027594,   0.0151019,
   0.0187190,  0.0148049,   0.0086685,   0.0056797,   0.0087615,
   0.0181163,  0.0322875,  -0.0448093
};
#define SAMPLES   80
int samples[SAMPLES] =
{
	1,3,4,5,2,7,22,43,22,5,1,7,23,2,6,9,10,33,38,222,
	1,1,4,5,2,7,22,33,22,5,1,7,23,2,6,67,10,33,38,52,
	6,3,4,5,2,3,5,43,22,5,1,68,23,2,6,9,13,33,38,42,
	4,2,4,5,2,7,22,43,22,5,1,7,23,2,6,9,10,33,38,56,
};
//int** insamp = new int*[BUFFER_LEN];
//
//
//void firFloatInit(void)
//{
//    memset(insamp, 0, sizeof(insamp));
//}

void FIR(int* coeffs, int* input, int* output, int length, int filterLength);
int countM();
int* INTToRNS(int INTnumber);
int RNSToINT(int* RNSnumber);
int main(void)
{

	//printf("%d %d", sizeof(double), sizeof(int));
	int Input[SAMPLES];
	int Output[SAMPLES];
	int *coeffs = new int [FILTER_LEN];
	for (int i = 0; i < FILTER_LEN; i++)
	{
		coeffs[i] = coefficients[i] * 10000000;
		printf("%d " , coeffs[i]);
		printf("\n");
	}
	printf("////////////////////////////////////////////////////////");
	FIR(coeffs, samples, Output, SAMPLES,FILTER_LEN);
		
	double* c = new double[FILTER_LEN];
	for (int i = 0; i < FILTER_LEN; i++)
	{
		c[i] = static_cast <double>(Output[i])/ 1000000.0;
		printf("%.7f ", c[i]);
		printf("\n");
	}
	delete[]coeffs;
	delete[]c;
	return 0;
}


void FIR(int* coeffs, int* input, int* output, int length, int filterLength)
{
	int** bufferRNS = new int* [BUFFER_LEN];
	int** RNS = new int* [length];
	int M = countM();
	int V[3] = { 0,0,0 };
 	for (int i = 0; i < BUFFER_LEN; i++) bufferRNS[i] = V;
	for (int i = 0; i < length; i++)
	{
		RNS[i] = INTToRNS(input[i]);
		//printf(" %d %d %d  \n", RNS[i][0], RNS[i][1], RNS[i][2]);
	}

    int* coeffp; 
    int** inputp; 

    memcpy(&bufferRNS[filterLength - 1], RNS,
        3*length * sizeof(int));

	int** outputRNS = new int*[length];
	for (int i = 0; i < length; i++)outputRNS[i] = new int[3];
	for (int l = 0; l < length; l++)
	{
		//printf("/////////////////////////////////////////////////\n");
		for (int i = 0; i < 3; i++)
		{
			coeffp = coeffs;
			inputp = &bufferRNS[filterLength - 1 + l];
			int sum = 0;
			//printf("/////////////////////////////////////////////////\n");
			for (int k = 0; k < filterLength; k++)
			{
				int buf = ((*coeffp++) % modulo[i]);
				if (buf < 0) buf += modulo[i];
				//buf *= (*bufferRNS--)[i];
				buf *= bufferRNS[filterLength - 1 + l-k][i];
				buf %= modulo[i];
				if (buf < 0) buf += modulo[i];
				sum += buf;
				//printf("buf[%d] = %d \n", k, buf);
			}
			//printf("/////////////////////////////////////////////////\n");
			sum %= modulo[i];
			if (sum < 0) sum += modulo[i];
			outputRNS[l][i] = sum;
			
			//printf("output[%d][%d] = %d \n", l, i, outputRNS[l][i]);
		}
		//printf("/////////////////////////////////////////////////\n");
	}


	//printf("/////////////////////////////////////////////////\n");
	for (int i = 0; i < length; i++) {
		output[i] = RNSToINT(outputRNS[i]);
	}
	//printf("/////////////////////////////////////////////////\n");
	
}


int countM()
{
	int M = 1;
	for (int i = 0; i < MODULO_AMOUNT; i++)M *= modulo[i];
	return M;
}

int* INTToRNS(int INTnumber)
{
	int* RNSnumber = new int[MODULO_AMOUNT + 1];
	for (int i = 0; i < MODULO_AMOUNT; i++) {
		RNSnumber[i] = INTnumber % modulo[i];
		if (RNSnumber[i] < 0)
		{
			RNSnumber[i] += modulo[i];
			RNSnumber[MODULO_AMOUNT] = -1;
		}
		else RNSnumber[MODULO_AMOUNT] = 1;
	}
	return RNSnumber;
}

int RNSToINT(int* RNSnumber)
{

	int INTnumber = 0;
	int M = countM();
	if ((RNSnumber[0] + RNSnumber[2]) % 2)
	{//x1 + x3 nieparzyste
		//INTnumber = modulo[1] * (RNSnumber[1] - RNSnumber[0]) + RNSnumber[1] + modulo[0] * modulo[1] * (RNSnumber[2]/2 +( RNSnumber[0] + RNSnumber[2]) / 2 - RNSnumber[1]) % modulo[2];
		INTnumber = (M / 2 + (modulo[1] * modulo[2] * RNSnumber[0]) / 2 - modulo[0] * modulo[2] * RNSnumber[1] + (modulo[0] * modulo[1] * RNSnumber[2]) / 2) % M;
		if (INTnumber < 0)INTnumber += M;
		if (RNSnumber[MODULO_AMOUNT] == -1)INTnumber -= M;
	}
	else
	{//x1 + x3 parzyste
		//INTnumber = modulo[1] * (RNSnumber[1] - RNSnumber[0]) + RNSnumber[1] + modulo[0] * modulo[1] * ((RNSnumber[0] + RNSnumber[2]) / 2 - RNSnumber[1]) % modulo[2];
		INTnumber = ((modulo[1] * modulo[2] * RNSnumber[0]) / 2 - modulo[0] * modulo[2] * RNSnumber[1] + (modulo[0] * modulo[1] * RNSnumber[2]) / 2) % M;
		if (INTnumber < 0)INTnumber += M;
		if (RNSnumber[MODULO_AMOUNT] == -1)INTnumber -= M;
	}

	return INTnumber;
}