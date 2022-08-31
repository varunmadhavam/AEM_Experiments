#include  <stdio.h>
#include  <math.h>
#include  <stdint.h>
#include  <stdlib.h>

#define N 1024  //1024 point dft
#define TWOPI 6.2832
#define WTOUINT32 11930464.7083       //0-360 mapped to 0x0 - 0xffffffff : (2^32-1)/360
#define UINT32TOW 0.00000008381903173 //0x0-0xffffffff mapped to 0 - 360 : 360/(2^32-1)
#define FTOUINT32 15907286.28         //0-270 mapped to 0x0-0xffffffff
#define UINT32TOF 0.00000006286427378 //0x0-0xffffffff mapped to 0-270   : 270/(2^32-1)
#define RADTODEGR 57.296              //180/pi for converting radians to degrees.
#define WTOUINT15 364.0777778         //0-90 mapped to 0x0-0x7fff : 32767/90 
#define ATOUINT15 32767               //0-1 mapped to 0x0-0x7fff
#define UINT15TOA 0.00003051850948    //0x0 - 0x7fff mapped to 0-1 : 1/32767

double X[N],Xre[N], Xim[N]; 
double ABS[N];   

//defaults
float FREQ=70.0; //100hz
float AMPL=1;
float PHI=0;

uint16_t A=0;
uint32_t F=0,P=0;

uint16_t sinTable[32768]; //Lookup table for sine
void gensine(void);
uint16_t sine(uint32_t theta);
uint32_t gettheta(double t);
void doDFT(void);

float points = 25; //no of samples in one time period
FILE* fsine,*fdft;
int main(int c,char *argv[]){
    fsine=fopen("plot.dat","w");
    fdft=fopen("dft.dat","w");
    if(c==4) { // get frequency, amplitude and phase as command line arguements
        FREQ=atof(argv[1])-30;
        if(FREQ<0 || FREQ > 270) {
            printf("Error : Unsupported frequency(30-300HZ only)\n");
            exit(1);
        }
        PHI=atof(argv[2]);
        if(PHI<0 || PHI>360) {
            printf("Error : Unsupported Phase(0-360 only)\n");
            exit(1);
        }
        AMPL=atof(argv[3]);
        if(AMPL<0 || AMPL>1) {
            printf("Error : Unsupported Amplitude(0-1 only)\n");
            exit(1);
        }
    }
    
    //convert the frequncy,amplitude and phase into their corresponding fixed point format
    F=FTOUINT32*FREQ;
    A=ATOUINT15*AMPL;
    P=WTOUINT32*PHI;
    
    //double INC = (double)(1.0/((FREQ+30)*points)); 
    //calculate the time increments so as to have point number of samples in one time period. ie 1/(f*points)
    double INC = 1.0/10240.0; //10K sampling
    
    gensine(); //puplate the sine lookup table
    int i;
    for(double t=0;t<=1.0;t+=INC){ // time starts from t=0 till t=0.033 which is one time period of the 30Hz sine wave. Each increment is such that there are "points" no of samples in a time period.
        if(t==0) i=0;
        uint16_t val=sine(gettheta(t)); //get the value of sine from the lookup table.
        if(val&0x8000) { //returned value is 16 bits of which the msb is used to indicate the sign . if 1  it it indicates a negative portion of the sine wave.
            val&=~(0x8000); //set the sign bit as only 15 bits to represents the value.
            double res=((double)val*UINT15TOA)*((double)A*UINT15TOA);
            if(i<N)
                X[i]=-1*res;
            fprintf(fsine,"%lf -%f\n",t,res); //print a - along with the value to indicate -ve part of the sine wave.
        }
        else {
            double res=(val*UINT15TOA)*(A*UINT15TOA);
            if(i<N)
                X[i]=res;
            fprintf(fsine,"%lf %f\n",t,res); // if the msb is not set, it indicates the positive half of the sinewave and the value can be printed as such.
        }
            i++;
  
    }
    doDFT();
    fclose(fsine);
    fclose(fdft);
	return 0;
}

void gensine(void) {
    for(float i=0;i<=90;i+=0.00274658203) { //90 degress divided into 32768 equal parts.
        sinTable[(uint16_t)(i*WTOUINT15)]=sin(i*3.14/180)*ATOUINT15; //store the angle and its corresponding sine value in the lookup table using appropriate fixed point representation.
    }
}

uint16_t sine(uint32_t t) {
    double  w=t*UINT32TOW; //get the real value of theta from its fixed point representation.
    while(w>360.0)
        w=w-360.0; //mod 360
    if(w<=90) { //look up the table and return the value.
        return sinTable[(uint16_t)(w*WTOUINT15)];
    }
    else if(w>90 && w<=180) {
        return sinTable[(uint16_t)((180-w)*WTOUINT15)];
    }
    else if(w>180 && w<=270){
        return sinTable[(uint16_t)((w-180)*WTOUINT15)]|0x8000; //additionally set msb bit as 1 to indicate -ve portion of the sine wave.
    }
    else {
        return sinTable[(uint16_t)((360-w)*WTOUINT15)]|0x8000;  // additionally set msb bit as 1 to indicate -ve portion of the sine wave.
    }
}

uint32_t gettheta(double t) {
    //calculating 2pift + phi by converting the frequency , amplitude and phase from their fixed point forms to real numbers.
    double pit=(6.28*t*RADTODEGR); //2 * pi * t in degrees
    double  f=(F*(UINT32TOF)+30);
    double pift=pit*f; // 2 * pi * t * f
    double w=pift+(P*UINT32TOW); // ( 2 * pi * f * t ) + phi
    return w*WTOUINT32; // return the fixed point representation of the calculated theta(degrees)
}

void doDFT(void) {
    for (int k=0 ; k<N ; ++k)
    {
        Xre[k] = 0;
        for (int n=0 ; n<N ; ++n) Xre[k] += X[n] * cos(n * k * TWOPI / N);

        Xim[k] = 0;
        for (int n=0 ; n<N ; ++n) Xim[k] -= X[n] * sin(n * k * TWOPI / N);
         
        ABS[k] = Xre[k]*Xre[k] + Xim[k]*Xim[k];
        fprintf(fdft,"%d %lf\n",k,sqrt(ABS[k]));
    }
}
