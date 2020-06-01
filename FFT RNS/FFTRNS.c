#include <stdio.h>
#include <stdlib.h>
//#include <conio.h>
#include <curses.h>
#define MODULO_AMOUNT 3
#define n 50
int modulo[MODULO_AMOUNT] = { 2 * n + 1, 2 * n, 2 * n - 1 };//Jednoznaczna reprezentacja liczby od -(M-1)/2 do (M-1)/2 dla nieparzystych i od -M/2 do (M/2 -1) dla przarzystych, gdzie M=m1*m2*m3

int lc1[32]={0,3227,6518,9940,13573,17515,21895,26892,32768,39928,49041,61305,79109,108022,164736,332699,0,-332699,-164736,-108022,-79109,-61305,-49041,-39928,-32768,-26892,-21895,-17515,-13573,-9940,-6518,-3227};
int lc2[32]={0,-6393,-12540,-18205,-23170,-27246,-30274,-32138,-32768,-32138,-30274,-27246,-23170,-18205,-12540,-6393,0,6393,12540,18205,23170,27246,30274,32138,32768,32138,30274,27246,23170,18205,12540,6393};

typedef struct Signal{
int** re;
int** im;
int length;
}Signal;

void fft(Signal* signal,int start,int end);
int quantize(int number);
int bitSwap(int number,int binaryLength);
int* INTToRNS(int INTnumber);
int RNSToINT(int* RNSnumber);
int countM();
int* add(int* rns1, int*rns2);
int* sub(int* rns1, int*rns2);
int* mul(int* rns1, int*rns2);


int main()
{

    //sinus
    	int length=32;
    	int re[32]={0,1,2,1,0,-1,-2,-1,0,1,2,1,0,-1,-2,-1,0,1,2,1,0,-1,-2,-1,0,1,2,1,0,-1,-2,-1};
    	int im[32]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

	int**reRNS = (int**) malloc(length*(MODULO_AMOUNT + 1)*sizeof(int));
	int**imRNS = (int**) malloc(length*(MODULO_AMOUNT + 1)*sizeof(int));
	
	for(int i=0; i<length;i++)
	{
		reRNS[i] = INTToRNS(re[i]);
		imRNS[i] = INTToRNS(im[i]);
		//printf("re %d: %d %d %d %d \n", i, reRNS[i][0],reRNS[i][1],reRNS[i][2],reRNS[i][3]);
		//printf("im %d: %d %d %d %d \n", i, imRNS[i][0],imRNS[i][1],imRNS[i][2],imRNS[i][3]);
	}
	
	
	Signal* signal=(Signal*) malloc(sizeof(Signal));
    	signal->re=reRNS;
    	signal->im=imRNS;
    	signal->length=length;
   

    	fft(signal,0,length);

	int* ree = (int*) malloc(length*sizeof(int));
	int* imm = (int*) malloc(length*sizeof(int));


	for(int i=0; i<length; i++)
	{
		ree[i] = RNSToINT(signal->re[i]);
		imm[i] = RNSToINT(signal->im[i]);
	}
	
	
    //tymczasowe testowanie na chwile obecna wynik jest dobry
    	for(int i=0;i<length;i++)
      	  printf("%d , %d \n",ree[bitSwap(i,5)],imm[bitSwap(i,5)]);

    	getch();

    return 0;
}

void fft(Signal* signal,int start,int length){

    if(length==1)
        return;

//bardzo czesto uzywane
    int len2=length/2;
    int len4=length/4;
    int stride=32/length;

// potrzebna kopia sygnalow
    int** re=(int**) malloc(signal->length*sizeof(int)*(MODULO_AMOUNT + 1));
    int** im=(int**) malloc(signal->length*sizeof(int)*(MODULO_AMOUNT + 1));

    for(int i=0;i<signal->length;i++){
        re[i]=signal->re[i];
        im[i]=signal->im[i];
    }

//pierwsza czesc motylkow
    for(int i=start;i<start+len2;i++){
        signal->re[i]=add(re[i],re[i+len2]);
        signal->im[i]=add(im[i],im[i+len2]);
        signal->re[i+len2]=sub(re[i],re[i+len2]);
        signal->im[i+len2]=sub(im[i],im[i+len2]);
    }

//kolejna czesc motylkow
    if(length>2){

//odswiezamy tablice
    	for(int i=0;i<signal->length;i++){
        	re[i]=signal->re[i];
        	im[i]=signal->im[i];
    		}
        signal->re[start+len2]=add(re[start+len2],im[start+len2+len4]);
        signal->im[start+len2]=sub(im[start+len2],re[start+len2+len4]);
        signal->re[start+len2+len4]=sub(re[start+len2],im[start+len2+len4]);
        signal->im[start+len2+len4]=add(im[start+len2],re[start+len2+len4]);
    	for(int i=start+len2+1;i<start+len2+len4;i++){
       	signal->re[i]=add(re[i],im[i+len4]);
        	signal->im[i]=sub(im[i],re[i+len4]);
        	signal->re[i+len4]=sub(re[i],im[i+len4]);
        	signal->im[i+len4]=add(im[i],re[i+len4]);

		signal->re[i] = add(signal->re[i],INTToRNS(quantize(RNSToINT(mul(signal->im[i],INTToRNS(lc1[(i-start-len2)*stride]))))));
        	signal->im[i] = add(signal->im[i],INTToRNS(quantize(RNSToINT(mul(signal->re[i],INTToRNS(lc2[(i-start-len2)*stride]))))));
        	signal->re[i] = add(signal->re[i],INTToRNS(quantize(RNSToINT(mul(signal->im[i],INTToRNS(lc1[(i-start-len2)*stride]))))));
        	
		signal->re[i+len4] = add(signal->re[i+len4] , INTToRNS(quantize(RNSToINT(mul(signal->im[i+len4],INTToRNS(lc1[(i-start-len2)*3*stride]))))));
        	signal->im[i+len4] = add(signal->im[i+len4] , INTToRNS(quantize(RNSToINT(mul(signal->re[i+len4],INTToRNS(lc2[(i-start-len2)*3*stride]))))));
        	signal->re[i+len4] = add(signal->re[i+len4] , INTToRNS(quantize(RNSToINT(mul(signal->im[i+len4],INTToRNS(lc1[(i-start-len2)*3*stride]))))));
        	
    	}
    	fft(signal,start,len2);
    	fft(signal,start+len2,len4);
    	fft(signal,start+len2+len4,len4);
    }
 
}

int quantize(int number){
    number>>=14;
    number++;
    number>>=1;
    return number;
}

int bitSwap(int number,int binaryLength){
    int temp=0;
    for(int i=0;i<binaryLength/2;i++){
        temp|=(number&(1<<(binaryLength-i-1)))>>(binaryLength-1-2*i)|(number&(1<<(i)))<<(binaryLength-1-2*i);
    }
    if(binaryLength/2*2!=binaryLength)
    temp|=number&(1<<binaryLength/2);
    return temp;
}

int countM()
{
	int M = 1;
	for (int i = 0; i < MODULO_AMOUNT; i++)M *= modulo[i];
	return M;
}

int* INTToRNS(int INTnumber)
{

	int* RNSnumber = (int*) malloc((MODULO_AMOUNT + 1)*sizeof(int));
	for (int i = 0; i < MODULO_AMOUNT; i++) {
		RNSnumber[i] = INTnumber % modulo[i];
		if (RNSnumber[i] < 0)RNSnumber[i] += modulo[i];
		RNSnumber[MODULO_AMOUNT] = INTnumber;
		
	}
	return RNSnumber;
}

int RNSToINT(int* RNSnumber)
{

	int INTnumber = 0;
	int M = countM();
	if ((RNSnumber[0] + RNSnumber[2]) % 2)
	{//x1 + x3 nieparzyste
	
		INTnumber = (M / 2 + (modulo[1] * modulo[2] * RNSnumber[0]) / 2 - modulo[0] * modulo[2] * RNSnumber[1] + (modulo[0] * modulo[1] * RNSnumber[2]) / 2) % M;
		if (INTnumber < 0)INTnumber += M;
		if (RNSnumber[MODULO_AMOUNT] < 0 )INTnumber -= M;
	}
	else
	{//x1 + x3 parzyste
		
		INTnumber = ((modulo[1] * modulo[2] * RNSnumber[0]) / 2 - modulo[0] * modulo[2] * RNSnumber[1] + (modulo[0] * modulo[1] * RNSnumber[2]) / 2) % M;
		if (INTnumber < 0)INTnumber += M;
		if (RNSnumber[MODULO_AMOUNT] < 0)INTnumber -= M;
	}

	return INTnumber;
}
int* add(int* rns1, int*rns2)
{
	
	int* sum = (int*) malloc((MODULO_AMOUNT + 1)*sizeof(int));
	sum[MODULO_AMOUNT] = rns1[MODULO_AMOUNT]+rns2[MODULO_AMOUNT];
	for(int i=0; i<MODULO_AMOUNT; i++)
	{
		int buf = (rns1[i] + rns2[i]) % modulo[i];
		if (buf < 0)buf += modulo[i];
		sum[i] = buf;
	}
	
	return sum;
}
int* sub(int* rns1, int*rns2)
{
	
	int* sub = (int*) malloc((MODULO_AMOUNT + 1)*sizeof(int));
	sub[MODULO_AMOUNT] = rns1[MODULO_AMOUNT]-rns2[MODULO_AMOUNT];
	for(int i=0; i<MODULO_AMOUNT; i++)
	{
		int buf = (rns1[i] - rns2[i]) % modulo[i];
		if (buf < 0)buf += modulo[i];
		sub[i] = buf;
	}
	
	return sub;
}

int* mul(int* rns1, int*rns2)
{
	
	int* mul = (int*) malloc((MODULO_AMOUNT + 1)*sizeof(int));
	mul[MODULO_AMOUNT] = rns1[MODULO_AMOUNT]*rns2[MODULO_AMOUNT];
	for(int i=0; i<MODULO_AMOUNT; i++)
	{
		int buf = (rns1[i] * rns2[i]) % modulo[i];
		if (buf < 0)buf += modulo[i];
		mul[i] = buf;
	}
	
	return mul;
}


