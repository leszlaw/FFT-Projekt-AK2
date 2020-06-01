#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
int lc1[32]={0,3227,6518,9940,13573,17515,21895,26892,32768,39928,49041,61305,79109,108022,164736,332699,0,-332699,-164736,-108022,-79109,-61305,-49041,-39928,-32768,-26892,-21895,-17515,-13573,-9940,-6518,-3227,0};
int lc2[32]={0,-6393,-12540,-18205,-23170,-27246,-30274,-32138,-32768,-32138,-30274,-27246,-23170,-18205,-12540,-6393,0,6393,12540,18205,23170,27246,30274,32138,32768,32138,30274,27246,23170,18205,12540,6393,0};

typedef struct Signal{
int* re;
int* im;
int length;
}Signal;

void fft(Signal* signal,int start,int end);
int quantize(int number);
int bitSwap(int number,int binaryLength);

int main()
{
    //sinus
    int length=32;
    int re[32]={0,1,2,1,0,-1,-2,-1,0,1,2,1,0,-1,-2,-1,0,1,2,1,0,-1,-2,-1,0,1,2,1,0,-1,-2,-1};
    int im[32]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

    Signal* signal=(Signal*) malloc(sizeof(Signal));
    signal->re=re;
    signal->im=im;
    signal->length=length;

    fft(signal,0,length);


    //tymczasowe testowanie na chwile obecna wynik jest dobry
    for(int i=0;i<length;i++)
        printf("%d , %d \n",signal->re[bitSwap(i,5)],signal->im[bitSwap(i,5)]);

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
    int* re=(int*) malloc(signal->length*sizeof(int));
    int* im=(int*) malloc(signal->length*sizeof(int));

    for(int i=0;i<signal->length;i++){
        re[i]=signal->re[i];
        im[i]=signal->im[i];
    }

//pierwsza czesc motylkow
    for(int i=start;i<start+len2;i++){
        signal->re[i]=re[i]+re[i+len2];
        signal->im[i]=im[i]+im[i+len2];
        signal->re[i+len2]=re[i]-re[i+len2];
        signal->im[i+len2]=im[i]-im[i+len2];
    }

//kolejna czesc motylkow
    if(length>2){

//odswiezamy tablice
    for(int i=0;i<signal->length;i++){
        re[i]=signal->re[i];
        im[i]=signal->im[i];
    }
        signal->re[start+len2]=re[start+len2]+im[start+len2+len4];
        signal->im[start+len2]=im[start+len2]-re[start+len2+len4];
        signal->re[start+len2+len4]=re[start+len2]-im[start+len2+len4];
        signal->im[start+len2+len4]=im[start+len2]+re[start+len2+len4];
    for(int i=start+len2+1;i<start+len2+len4;i++){
        signal->re[i]=re[i]+im[i+len4];
        signal->im[i]=im[i]-re[i+len4];
        signal->re[i+len4]=re[i]-im[i+len4];
        signal->im[i+len4]=im[i]+re[i+len4];

        signal->re[i]+=quantize(signal->im[i]*lc1[(i-start-len2)*stride]);
        signal->im[i]+=quantize(signal->re[i]*lc2[(i-start-len2)*stride]);
        signal->re[i]+=quantize(signal->im[i]*lc1[(i-start-len2)*stride]);

        signal->re[i+len4]+=quantize(signal->im[i+len4]*lc1[(i-start-len2)*3*stride]);
        signal->im[i+len4]+=quantize(signal->re[i+len4]*lc2[(i-start-len2)*3*stride]);
        signal->re[i+len4]+=quantize(signal->im[i+len4]*lc1[(i-start-len2)*3*stride]);

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
