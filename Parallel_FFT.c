#include<stdio.h>
#include<math.h>
#include<mpi.h>
#include<stdlib.h>

#define PI 3.14159265

struct num1
{
  float real;
  float img;
};

int reverseBits(int number, int NO_OF_BITS)
{
	int reverse_num = 0, i, temp;

	for (i = 0; i < NO_OF_BITS; i++)
	{
		temp = (number & (1 << i));
		if(temp)
			reverse_num |= (1 << ((NO_OF_BITS - 1) - i));
    }
	
	return reverse_num;
}

int main (int argc, char **argv)
{

	int rank,size,num,i,key,div;
   	double bb1time,ab1time,bb2time,ab2time,starttime,endtime;	
  	
	MPI_Status status;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	MPI_Datatype num1type;
	MPI_Datatype type[2] = { MPI_FLOAT, MPI_FLOAT };
  
	int blocklen[2] = { 4, 4 };
	
	MPI_Aint disp[2];

	if(rank == 0)
    {
		printf ("Enter No. of input values(it should be in form of 2^x) : ");
		scanf ("%d", &num);
    }
	
	bb1time=MPI_Wtime();
	MPI_Bcast(&num, 1, MPI_INT, 0, MPI_COMM_WORLD);
	ab1time=MPI_Wtime();

	struct num1 input[num + 1];
	struct num1 seq[num + 1];
	struct num1 temp[num + 1];

	input[0].real = 0.0;
	input[0].img = 0.0;
	seq[0].real = 0.0;
	seq[0].img = 0.0;
	temp[0].real = 0.0;
	temp[0].img = 0.0;

	disp[0] = 0;
	disp[1] = 4;

	MPI_Type_create_struct(2, blocklen, disp, type, &num1type);
	MPI_Type_commit(&num1type);
  
	if(rank == 0)
    {
		printf("Enter total %d values in floating point formet(separated with space) : ",num);
		
		for (i = 1; i < num + 1; i++)
		{
			scanf ("%f", &input[i].real);
			input[i].img = 0.0;
		}

		for (i = 1; i < num + 1; i++)
		{
			seq[i].real = input[reverseBits (i - 1, log2f (num) / log2f (2)) + 1].real;	// log2 (x) = logy (x) / logy (2)
			seq[i].img = 0.0;
		}

    }
	
	bb2time=MPI_Wtime();
	MPI_Bcast (seq, num + 1, num1type, 0, MPI_COMM_WORLD);
	ab2time=MPI_Wtime();

	key = log2f (num);
	div = 1;
	
	starttime=MPI_Wtime();
	
	while (key--)
    {
		if (rank != 0)
		{
			if( ( (rank+div-1) / div ) % 2 == 0)
			{
				temp[rank].real = seq[rank - div].real 
				+ 
				(cos (PI * ( (rank-1) % (div*2) ) / div) * (seq[rank].real) )
				+
				(sin (PI * ( (rank-1) % (div*2) ) / div) * (seq[rank].img) );
				
				temp[rank].img = seq[rank - div].img 
				+ 
				(cos (PI * ( (rank-1) % (div*2) ) / div) * (seq[rank].img) )
				-
				(sin (PI * ( (rank-1) % (div*2) ) / div) * (seq[rank].real) );
				
				MPI_Send(&temp[rank], 1, num1type, 0, 0, MPI_COMM_WORLD);
			}
			else
			{
				temp[rank].real = seq[rank].real 
				+ 
				(cos (PI * ( (rank-1) % (div*2) ) / div) * (seq[rank+div].real) )
				+
				(sin (PI * ( (rank-1) % (div*2) ) / div) * (seq[rank+div].img) );
				
				temp[rank].img = seq[rank].img 
				+ 
				(cos (PI * ( (rank-1) % (div*2) ) / div) * (seq[rank+div].img) )
				-
				(sin (PI * ( (rank-1) % (div*2) ) / div) * (seq[rank+div].real) );
				
				MPI_Send(&temp[rank], 1, num1type, 0, 0, MPI_COMM_WORLD);
			}
		}
		else
		{
			for (i = 1; i < num + 1; i++)
			MPI_Recv(&temp[i], 1, num1type, i, 0, MPI_COMM_WORLD, &status);
		}
		
		MPI_Barrier(MPI_COMM_WORLD);
		
		if (rank == 0)
		{
			for (i = 1; i < num + 1; i++)
			{
			  seq[i].real = temp[i].real;
			  seq[i].img = temp[i].img;
			}
			div *= 2;
		}

		MPI_Bcast(seq, num + 1, num1type, 0, MPI_COMM_WORLD);
		MPI_Bcast(&div, 1, MPI_INT, 0, MPI_COMM_WORLD);
    }

	endtime=MPI_Wtime();

	if(0 == rank)
    {
		printf ("\n");
		
		for (i = 1; i < num + 1; i++)
		{
			printf ("X[%d] : %f", i - 1, seq[i].real);
			
			if(seq[i].img >= 0)
				printf ("+j%f\n", seq[i].img);
			else
				printf ("-j%f\n", seq[i].img - 2 * seq[i].img);
		}
		
		printf ("\n");
		printf("Time to broadcast num variable : %lf ms\n",(ab1time-bb1time)*1000);
		printf("Time to broadcast input and seq array : %lf ms\n",(ab2time-bb2time)*1000);
		printf("Time to compute FFT parallely : %lf ms\n",(endtime-starttime)*1000);
		printf("Total Time : %lf ms\n",((endtime-starttime)+(ab2time-bb2time)+(ab1time-bb1time))*1000);
		printf ("\n");
    }
  
	MPI_Finalize();

	return 0;
}