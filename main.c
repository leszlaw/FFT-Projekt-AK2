#include <stdio.h>
#include <stdlib.h>

typedef struct Signal{
int* re;
int* im;
int length;
}Signal;

Signal* fft(Signal* signal);
void butterfly(Signal* signal,int start,int end);

int main()
{

    int length=4;
    int re[4]={1,2,3,4};
    int im[4]={2,1,2,1};

    Signal* signal=(Signal*) malloc(sizeof(Signal));
    signal->re=re;
    signal->im=im;
    signal->length=length;

    butterfly(signal,0,4);

    //tymczasowe testowanie na chwile obecna wynik jest dobry
    for(int i=0;i<length;i++)
        printf("%d + %di \n",signal->re[i],signal->im[i]);


    return 0;
}


Signal* fft(Signal* signal){}


void butterfly(Signal* signal,int start,int length){

//bardzo czesto uzywane
    int len2=length/2;
    int len4=length/4;

// potrzebna kopia sygnalow
    int* re=(int*) malloc(signal->length);
    int* im=(int*) malloc(signal->length);

    for(int i=0;i<length;i++){
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

//odswiezamy tablice
    for(int i=0;i<length;i++){
        re[i]=signal->re[i];
        im[i]=signal->im[i];
    }

//kolejna czesc motylkow
    for(int i=start+len2;i<start+len2+len4;i++){
        signal->re[i]=re[i]+im[i+len4];
        signal->im[i]=im[i]-re[i+len4];
        signal->re[i+len4]=re[i]-im[i+len4];
        signal->im[i+len4]=im[i]+re[i+len4];
    }

}
