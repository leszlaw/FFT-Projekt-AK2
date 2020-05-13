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

    return 0;
}


Signal* fft(Signal* signal){}


void butterfly(Signal* signal,int start,int length){

//bardzo czesto uzywane wiec tworzymy taka zmienna
    int len2=length/2;

// potrzebna kopia sygnalow
    int* re=(int*) malloc(signal->length);
    int* im=(int*) malloc(signal->length);
    for(int i=0;i<length;i++){
        re[i]=signal->re[i];
        im[i]=signal->im[i];
    }

//pierwsza czesc motylkow
    for(int i=start;i<len2;i++){
        signal->re[i]=re[i]+re[i+len2];
        signal->im[i]=im[i]+im[i+len2];
        signal->re[i+len2]=re[i]-re[i+len2];
        signal->im[i+len2]=im[i]-im[i+len2];
    }

}
