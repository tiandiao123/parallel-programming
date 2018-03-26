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
      int num_of_pro_size = GRID_WIDTH/num_procs;
      int* updated_grid = NULL;
      int* sub_grid = (int*)malloc(sizeof(int)*num_of_pro_size);


	  for ( iters = 0; iters < num_iterations; iters++ ) {
	    // TODO: Add Code here or a function call to you MPI code
        MPI_Barrier(MPI_COMM_WORLD);
        MPI_Scatter(global_grid,num_of_pro_size,MPI_INT,sub_grid,
        	num_of_pro_size,MPI_INT,0,MPI_COMM_WORLD);

        update_sub_grid(ID,num_of_pro_size,
        	sub_grid,global_grid,MPI_COMM_WORLD);

        MPI_Barrier(MPI_COMM_WORLD);

        if(ID==0){
         	updated_grid=(int *)malloc(sizeof(int)*GRID_WIDTH);
         	assert(updated_grid!=NULL);
        }

        MPI_Gather(sub_grid,1,MPI_INT,updated_grid,1,
        	MPI_INT,0,MPI_COMM_WORLD);
        MPI_Barrier(MPI_COMM_WORLD);

	    if(ID==0){
	    	int p=0;
	    	for(;p<256;p++){
	    		global_grid[p] = *(updated_grid+p);
	    	}
	    }

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

        
	  }

	  // TODO: Clean up memory
	  free(sub_grid);
	  if(ID==0){
	  	free(updated_grid);
	  }
	  MPI_Barrier(MPI_COMM_WORLD);
	  MPI_Finalize(); // finalize so I can exit
}

void my_bcast(void* data, int count, MPI_Datatype datatype, int root, MPI_Comm communicator){

    

}
