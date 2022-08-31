#include<stdio.h>
#include<stdint.h>
#include<math.h>

void float32add(float,float,float*);
void float32sub(float,float,float*);
void float32div(float,float,float*);
void float32mul(float,float,float*);

int main() {
    float a,b,c;
    printf("Enter the two floating point numbers: ");
    scanf("%f %f",&a,&b);
    float32add(a,b,&c);
    printf("\nAddition       : This library = %f   ; hw float = %f\n",c,a+b);
    float32sub(a,b,&c);
    printf("Subtraction    : This library = %f   ; hw float = %f\n",c,a-b);
    float32mul(a,b,&c);
    printf("Multiplication : This library = %f   ; hw float = %f\n",c,a*b);
    float32div(a,b,&c);
    printf("Division       : This library = %f   ; hw float = %f\n",c,a/b);
}

void float32add(float a,float b,float* c) {
    uint32_t* z;
    z=(uint32_t*)c; //map the resulting float pointer to an int pointer for easier bit manipulations.
    *z&=0x0; // reset all bits of the result.
    if(a==0) {
        *c=b;
         return;
    }
    else if(b==0) { //if either one of a and b are zero, result of addition is the other operand.
        *c=a;
        return;
    }
    else if(a==NAN || b==NAN) { // if either of the operands are NANs , return NAN.
        *c=NAN;
        return;
    }
    uint32_t* aa,*bb;
    aa=(uint32_t*)&a;
    bb=(uint32_t*)&b;
    uint32_t af = (*aa)&0x007fffff; // get the significant of a
    uint32_t bf = (*bb)&0x007fffff; // get the significant of b
    uint32_t ae = ((*aa)>>23) & 0xff; // get the exponent of a
    uint32_t be = ((*bb)>>23) & 0xff; // get th  exponent of b
    uint32_t as = (*aa)>>31; // get the sign bit of a
    uint32_t bs = (*bb)>>31; // get the sign bit og b
    uint32_t zs = 0; //results sign
    uint32_t ze = 0; //results exponent
    uint32_t zf = 0; //results significant
    
    af=(af|0x00800000); // add the implicit one to a's significant
    bf=(bf|0x00800000); // add the implicit one to b's significant
    
    if(!(as) && !(bs)){ // if both operands are positive
        if(ae>be) { // if the exponents of the operands dont match adjust the operand with the lower exponent to match with the higher exponent.
           bf=bf>>(ae-be);
           ze=ae;
        }
        else {
           af=af>>(be-ae);
           ze=be;   
        }
        zf=af+bf; // add the significants
        if(zf&(1<<24)){ // if the 24th bit is set, it indicates an overflow, ie result is no more in normalise(1.) form. so left shitf once and add 1 to exponent to normalise.
            zf=zf>>1;
            ze++;
        }
    }
    else if(!(as) && bs) { // if the second operand in -ve
        if(ae>be) { // if the exponents of the operands dont match adjust the operand with the lower exponent to match with the higher exponent.
           bf=bf>>(ae-be);
           ze=ae;
        }
        else {
           af=af>>(be-ae);
           ze=be;   
        }
        if(af>=bf) // if a's significant is greater that b's significant, simply subtract b's significant from a's significant.
             zf=af-bf;
        else {
            zf=bf-af; // else subtract a's significant from b's signifiant and set the sign bit to idicate that the result is a -ve number.
            zs=1;
        }
        int k=0;
        while (!(zf&(1<<23))&&k<23) // if after subtraction the significant is not in (1.0) form, right shift until bit 23 is one or we have left shifted for 23 times. Now the significant is normalised.
        {                       
          zf=zf<<1;
          ze--;    // for each left shift , decrement the exponent.
          k++;
        }
    }
    else if(as && !(bs)) {
        float32add(b,a,c);  // is 1st operand is -ve, convert it to the case where the 2nd operands in -ve, since that has already been implemented and addition is commutative.
        return;
    }
    else { //if both the operands are -ve, simply add them as positives and set the sign as -ve.
        float32add(-1*a,-1*b,c);
        *z|=(1<<31);
        return;
    }
    //construct the resultant float from the sign, exponent and significant bits.
    *z=zf&0x007fffff;
    *z|=(zs<<31);
    *z|=(ze<<23);
}

void float32sub(float a,float b,float* c) {
    float32add(a,-1*b,c); //subtraction is just addition a-b = a+(-(b))
}

void float32mul(float a,float b,float* c) {
    uint32_t* z;
    z=(uint32_t*)c;
    *z&=0x0;
    if(a==0 || b==0) { // if either of the operands is 0, result is 0.
        *c=0;
         return;
    }
    else if(a==INFINITY || b==INFINITY) { // if either of the operands is INFINITY, result is INFINITY.
        *c=INFINITY;
         return;
    }
    else if(a==NAN || b==NAN) { // if either of the operands is NAN, result is NAN.
        *c=NAN;
        return;
    }
    uint32_t* aa,*bb;
    aa=(uint32_t*)&a;
    bb=(uint32_t*)&b;
    uint32_t af = (*aa)&0x007fffff; //get the siginifcant of a
    uint32_t bf = (*bb)&0x007fffff; //get the significant of b
    uint32_t ae = ((*aa)>>23) & 0xff; //get the exponent of a
    uint32_t be = ((*bb)>>23) & 0xff; //get the exponent of b
    uint32_t as = (*aa)>>31; //get the sign of a
    uint32_t bs = (*bb)>>31; //get the sign of b
    uint32_t zs = as ^ bs;   //sign of the result is the xor of the sign of the operands.
    uint32_t ze = (ae)+(be)-127; //new exponent is the sum of the exponents in multiplication. remove 127 as the base is included twice.
    af=(af|0x00800000); //add the implicit 1 to a's siginficant
    bf=(bf|0x00800000); //add the implicit 1 to b's significant
    uint64_t zf=(uint64_t)af*(uint64_t)bf; // multiply the significants in 64 bit so that data is not lost on overflow.
    if(zf&((uint64_t)1<<47)) { //normalise on overflow in multiplication
        zf=zf>>24; // if 47th bit is set it indicates an over flow and hence the right shift 24 times to get the normalised value.
        ze++; //increment the exponent by 1 for the extra right shift.
    }
    else { //else already in normalised for. just left shift 23 times to get the significant in the correct location.
        zf=zf>>23;    
    }
    if((int32_t)ze > 254 || (int32_t)ze < 1) { //check if the exponent is in range
        printf("Error: Overflow/Underflow in Multiplication\n"); //else overflow/underflow
        *c=INFINITY;
        return;
    }
    //build te result from the significant, exponent and sign bits.
    *z=(uint32_t)(zf&0x00000000007fffff);
    *z|=(zs<<31);
    *z|=(ze<<23);
}

void float32div(float a,float b,float* c) {
    uint32_t* z;
    z=(uint32_t*)c;
    *z&=0x0;
    if(a==0) { // 0/b = 0
        *c=0;
         return;
    }
    else if(b==0) { //a/0 = INFINITY
        *c=INFINITY;
         return;
    }
    else if(a==NAN || b==NAN) { //if either of the operands is NAN, result is NAN
        *c=NAN;
        return;
    }
    uint32_t* aa,*bb;
    aa=(uint32_t*)&a;
    bb=(uint32_t*)&b;
    uint32_t af = (*aa)&0x007fffff; // get the significant of a
    uint32_t bf = (*bb)&0x007fffff; // get the significant of b
    uint32_t ae = ((*aa)>>23) & 0xff; // get the exponent of a
    uint32_t be = ((*bb)>>23) & 0xff; // get the exponent of b
    uint32_t as = (*aa)>>31; // get the sign of a
    uint32_t bs = (*bb)>>31; // get the sign of b
    uint32_t zs = as ^ bs;  // sign of the result is tehe xor of the sign of the operands
    uint32_t ze = (ae)-(be)+127; //new exponet sis the difference of the exponets + 127 as the bias is subtracted twice.
    af=(af|0x00800000); // add the implicit 1 to a's significant
    bf=(bf|0x00800000); // add the implicit 1 to b's significant
    uint64_t zf=(((uint64_t)af)<<23)/(uint64_t)bf; //divive the significant of a by significant of b
    if(zf&((uint64_t)1<<24)) { //if at all bti 24 is set, indicates an overflow and hence normalise it by right shifting once.
        zf=zf>>1; 
        ze++; //increment the exponent once corresponding to the right shift.
    }
    int k=0;
    while (!(zf&(1<<23))&&k<23) // if the result is not normalised, left shift until but 23 is 1 or we have left shifted 23 times.
        {
          zf=zf<<1; 
          ze--; //for each left shit, decrement the exponent.
          k++;
        }
    if((int32_t)ze > 254 || (int32_t)ze < 1) { // if the exponent is not in range
        printf("Error: Overflow/Underflow in Division\n");
        *c=INFINITY;
        return;
    }
    //build the result from the significant, exponent and sign bits.
    *z=(uint32_t)(zf&0x00000000007fffff);
    *z|=(zs<<31);
    *z|=(ze<<23);
}
