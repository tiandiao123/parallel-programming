// gameoflife.c
// Name: YOUR NAME HERE
// JHED: YOUR JHED HERE

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "mpi.h"

#define DEFAULT_ITERATIONS 64
#define GRID_WIDTH  256
#define DIM  16     // assume a square grid


void update_sub_grid(int id,int num_of_pro_size,int sub_grid[],
	int global_grid[],MPI_Comm communicator){
    
    int world_rank;
    MPI_Comm_rank(communicator,&world_rank);
    int world_size;
    MPI_Comm_rank(communicator,&world_size);
    
    int i = world_rank*num_of_pro_size;
    int end = (world_rank+1)*num_of_pro_size;
    int p = 0;
    printf("currently running process %d",world_rank);

    for(;i<end;i++){
        int row = i/DIM;
        int col = i%DIM;

        int count = global_grid[((row-1+DIM)%DIM)*DIM+(col-1+DIM)%DIM];
        count+=global_grid[((row-1+DIM)%DIM)*DIM+col];
        count+=global_grid[((row-1+DIM)%DIM)*DIM+(col+1)%DIM];
        count+=global_grid[row*DIM+(col-1+DIM)%DIM];
        count+=global_grid[row*DIM+(col+1)%DIM];
        count+=global_grid[((row+1)%DIM)*DIM+(col-1+DIM)%DIM];
        count+=global_grid[((row+1)%DIM)*DIM+col];
        count+=global_grid[((row+1)%DIM)*DIM+(col+1)%DIM];
        if(count>=4 || count<=1){
            sub_grid[p] = 0;
        }else{
        	sub_grid[p] = 1;
        }
        ++p;
    }

}

int* get_neighbors(int i,int dimensional_size){
    int* neig = (int*)malloc(sizeof(int)*8);

    int row = i/DIM;
    int col = i%DIM;
    neig[0] = ((row-1+DIM)%DIM)*DIM + (col-1+DIM)%DIM;
    neig[1] = ((row-1+DIM)%DIM)*DIM + col;
    neig[2] = ((row-1+DIM)%DIM)*DIM + (col+1)%DIM;
    neig[3] = row*DIM + (col-1+DIM)%DIM;
    //neig[4] = i;
    neig[4] = row*DIM + (col+1)%DIM;
    neig[5] = ((row+1)%DIM)*DIM + (col-1+DIM)%DIM;
    neig[6] = ((row+1)%DIM)*DIM + col;
    neig[7] = ((row+1)%DIM)*DIM + (col+1)%DIM;

    return neig;
}

int* get_new_subgrid(int* sub_grid,int num_of_pro_size,int dimensional_size){
    int* new_sub_grid = (int*)malloc(sizeof(int)*(num_of_pro_size-2*dimensional_size));


    int p = 0;
    for(int i=dimensional_size;i<num_of_pro_size-dimensional_size;i++){
        int* neig = get_neighbors(i,dimensional_size);
        int count = 0;
        for(int j=0;j<8;j++){
        	count += sub_grid[*(neig+j)];
        }

        if(sub_grid[i]==1){
            if(count==2 || count==3){
               new_sub_grid[p] = 1;
            }else{
            	new_sub_grid[p] = 0;
            }
        }else{

        	if(count==3){
        		new_sub_grid[p] = 1;
        	}else{
        		new_sub_grid[p] = 0;
        	}

        }
        p++;
    }

    for(int i=0;i<num_of_pro_size-2*dimensional_size;i++){
    	sub_grid[i+dimensional_size] = new_sub_grid[i];
    }

    return new_sub_grid;
}

int main ( int argc, char** argv ) {

	  int global_grid[ 256 ] = 
	   {0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	    0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,
	    1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,
	    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

	  // MPI Standard variable
	  int num_procs;
	  int ID, j;
	  int iters = 0;
	  int num_iterations;

	  // Setup number of iterations
	  if (argc == 1) {
	    num_iterations = DEFAULT_ITERATIONS;
	  }
	  else if (argc == 2) {
	    num_iterations = atoi(argv[1]);
	  }
	  else {
	    printf("Usage: ./gameoflife <num_iterations>\n");
	    exit(1);
	  }

	  // Messaging variables
	  MPI_Status stat;
	  // TODO add other variables as necessary

	  // MPI Setup
	  if ( MPI_Init( &argc, &argv ) != MPI_SUCCESS )
	  {
	    printf ( "MPI_Init error\n" );
	  }

	  MPI_Comm_size ( MPI_COMM_WORLD, &num_procs ); // Set the num_procs
	  MPI_Comm_rank ( MPI_COMM_WORLD, &ID );

	  assert ( DIM % num_procs == 0 );

	  // TODO Setup your environment as necessary
      int num_of_pro_size = (DIM/num_procs+2)*DIM;
      //int* updated_grid = NULL;
      int* sub_grid = (int*)malloc(sizeof(int)*num_of_pro_size);


      MPI_Scatter(global_grid,GRID_WIDTH/num_procs,MPI_INT,
      	&sub_grid[DIM],GRID_WIDTH/num_procs,MPI_INT,0,MPI_COMM_WORLD);


	  for ( iters = 0; iters < num_iterations; iters++ ) {
	    // TODO: Add Code here or a function call to you MPI code

	    //MPI_Barrier(MPI_COMM_WORLD);
        int previous_layer = (ID==0)?(num_procs-1):ID-1;
        int next_layer = (ID==num_procs-1)?0:ID+1;

	    if(ID%2==0){
             MPI_Send(sub_grid+DIM,DIM,MPI_INT,previous_layer,0,MPI_COMM_WORLD);
             MPI_Send(sub_grid+num_of_pro_size-2*DIM,DIM,MPI_INT,next_layer,0,MPI_COMM_WORLD);

             MPI_Recv(sub_grid,DIM,MPI_INT,previous_layer,0,
             	MPI_COMM_WORLD,MPI_STATUS_IGNORE);
             MPI_Recv(sub_grid+num_of_pro_size-DIM,DIM,MPI_INT,
             	next_layer,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
	    }else{

	    	MPI_Recv(sub_grid,DIM,MPI_INT,previous_layer,0,
	    		MPI_COMM_WORLD,MPI_STATUS_IGNORE);
	    	MPI_Recv(sub_grid+num_of_pro_size-DIM,DIM,MPI_INT,
	    		next_layer,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);

	    	MPI_Send(sub_grid+DIM,DIM,MPI_INT,previous_layer,0,MPI_COMM_WORLD);
            MPI_Send(sub_grid+num_of_pro_size-2*DIM,DIM,MPI_INT,next_layer,0,MPI_COMM_WORLD);
	    }
        
	    int* new_sub_grid = get_new_subgrid(sub_grid,num_of_pro_size,DIM);
        //MPI_Barrier(MPI_COMM_WORLD);

        MPI_Gather(new_sub_grid,num_of_pro_size-2*DIM,MPI_INT,
        	global_grid,num_of_pro_size-2*DIM,MPI_INT,0,MPI_COMM_WORLD);

	    // Output the updated grid state
	    if ( ID == 0 ) {
	      printf ( "\nIteration %d: final grid:\n", iters );
	      for ( j = 0; j < GRID_WIDTH; j++ ) {
	        if ( j % DIM == 0 ) {
	          printf( "\n" );
	        }
	        printf ( "%d  ", global_grid[j] );
	      }
	      printf( "\n" );
	    }

	    MPI_Barrier(MPI_COMM_WORLD);

	    free(new_sub_grid);

        
	  }

	  // TODO: Clean up memory
	  free(sub_grid);
	  MPI_Barrier(MPI_COMM_WORLD);
	  MPI_Finalize(); // finalize so I can exit
}

