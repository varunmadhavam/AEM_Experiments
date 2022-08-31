#include<stdio.h>
#include<stdlib.h>
#include<stdarg.h>
#include<math.h>

#define max_rows 100
#define max_cols 100

int lindex=0;

void swap(float a[max_rows][max_cols], int m1,int m2,int n);
void readMatrix(float a[max_rows][max_cols],int m,int n);
void printMatrix(float a[max_rows][max_cols],int m,int n);
void echeclon(float a[max_rows][max_cols],float b[max_rows][max_cols],int m,int n);
void fillzerosl(float a[max_rows][max_cols],int m,int n);
void filll(float a[max_rows][max_cols],float b[max_rows][max_cols],int rows,int i,int j);
void filllI(float a[max_rows][max_cols],int n);
int setPivot(float a[max_rows][max_cols],int m,int n,int i,int j);

int main() {
    int m=0,n=0;
    float u[max_rows][max_cols],l[max_rows][max_cols];
    printf("Enter the number of rows and columns for the matrix\n");
    scanf("%d%d",&m,&n);
    printf("Please enter the matrix\n");
    readMatrix(u,m,n);
    printf("\nmatrix is\n");
    printMatrix(u,m,n);
    fillzerosl(l,m,m);
    echeclon(u,l,m,n);
    filllI(l,m);
    printf("\nUpper triangular matrix is\n");
    printMatrix(u,m,n);
     printf("\nLower triangular matrix is\n");
    printMatrix(l,m,m);

}

void printMatrix(float a[max_rows][max_cols],int m,int n) {
    for(int i=0;i<m;i++) {
        for(int j=0;j<n;j++) {
            printf("%f ",a[i][j]);
        }
        printf("\n");
    }
}

void readMatrix(float a[max_rows][max_cols],int m,int n) {
    for(int i=0;i<m;i++) {
        for(int j=0;j<n;j++) {
            scanf("%f",&a[i][j]);
        }
    }
}


void echeclon(float a[max_rows][max_cols],float b[max_rows][max_cols],int m,int n) {
    int i=0,j=0;
    while(i<m && j<n) {
        if(i==m-1) {
            break;
        }
        if(a[i][j]) {
            filll(a,b,m,i,j);
            for(int k=i+1;k<m;k++) {
                float div=a[i][j];
                float mul=a[k][j];
                if(a[k][j]!=0 && a[k][j]!=-0) {
                    for(int p=0;p<n;p++) {
                        float e,r;
                        e=a[k][p];
                        r=mul*(a[i][p]/div);
                        if(e==-0.0)
                            e=+0.0;
                        if(r==-0.0)
                            r=+0.0;
                        a[k][p]=(e)-(r);
                    }
                }                
            }
            if(j==n-1) {
                break;
            }
            j++;
            i++;
        }
        else {
            if(j==n-1) {
                break;
            }
            if(setPivot(a,m,n,i,j)==0)
                j++;
        }
    }
}

void swap(float a[max_rows][max_cols], int m1,int m2,int n) {
    for (int i=0;i<n;i++) { 
        int temp = a[m1][i]; 
        a[m1][i] = a[m2][i]; 
        a[m2][i] = temp; 
    } 
    
}

void fillzerosl(float a[max_rows][max_cols],int m,int n) {
    for(int i=0;i<m;i++) {
        for(int j=0;j<n;j++) {
            a[i][j]=0;
        }
    }
}

void filll(float a[max_rows][max_cols],float b[max_rows][max_cols],int rows,int i,int j) {
    float div=a[i][j];
    for(int m=i;m<rows;m++)
        b[m][lindex]=a[m][j]/div;
    lindex++;
}

void filllI(float a[max_rows][max_cols],int n) {
    for(int i=lindex;i<n;i++)
        a[i][i]=1;
}

int setPivot(float a[max_rows][max_cols],int m,int n,int i,int j) {
    for(int k=i+1;i<m;i++) {
        if(a[k][j]) {
            swap(a,i,k,n);
            return 1;
        }
    }
    return 0;
}

/*
sample output

Enter the number of rows and columns for the matrix
3
4
Please enter the matrix
1
-2
0
3
1
-1
-1
4
1
0
-2
5

Upper triangular matrix is
1.000000 -2.000000 0.000000 3.000000 
0.000000 1.000000 -1.000000 1.000000 
0.000000 0.000000 0.000000 0.000000 

Lower triangular matrix is
1.000000 0.000000 0.000000 
1.000000 1.000000 0.000000 
1.000000 2.000000 1.000000

*/