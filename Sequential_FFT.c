#include<stdio.h>
#include<math.h>
#include<stdlib.h>
#include <time.h>
     
#define PI 3.14159265 

int main (int argc, char **argv) 
{
	int i, k, n, num;
	double cpu_time_used;
    clock_t start, end;
     
	printf ("Enter No. of input values(it should be in form of 2^x) : ");
	scanf ("%d", &num);
  
	float temp,input[num],result_real[num],result_img[num],sum = 0.0;

	printf("Enter total %d values in floating point formet(separated with space) : ",num);
  
	for (i = 0; i < num; i++)
	scanf ("%f", &input[i]);
  
	start = clock();
    
	for(k = 0; k < num; k++) 
	{
		result_real[k] = 0.0;
		result_img[k] = 0.0;
      
		for (n = 0; n < num; n++)
		{
			temp = -2*PI*n*k/num;
			result_real[k]  = result_real[k] + (input[n] * cos (temp));
			result_img[k] = result_img[k] + (input[n] * sin (temp));
		}
    }
  
    end = clock();

	printf ("\n");
 
	for (k = 0; k < num; k++)
    {
		printf ("X[%d] : %f", k, result_real[k]);
		if(result_img[k] >= 0)
			printf ("+j%f\n", result_img[k]);
		else
			printf ("-j%f\n", result_img[k] - 2 * result_img[k]);
	}
  
	printf ("\n");

	cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	printf("Time to compute FFT sequentially : %lf ms\n", cpu_time_used*1000);  
 
	return 0;
}