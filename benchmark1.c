#include "mavalloc.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

int main( int argc, char * argv[] )
{
	FILE *f = fopen("benchmark_times.txt", "a");
    if (f == NULL)
    {
        printf("Could not open file");
        return 0;
    }

	clock_t execTime = clock();
	
	//mavalloc_init( 256000, NEXT_FIT );
	
	// copied over all the allocs and frees from main.c
	char * ptr = ( char * ) malloc ( 65535 );
	
	free( ptr ); 
	
	char * ptr1    = (char*)malloc( 65535 );
	char * ptr2    = (char*)malloc( 65 );
	free( ptr1 );
	free( ptr2 );
	
	char * ptr3    = (char*)malloc( 65536 );
	char * ptr4    = (char*)malloc( 64 );
	free( ptr4 );
	free( ptr3 );
	
	char * ptr5  = ( char * ) malloc ( 10000 );
	char * ptr6  = ( char * ) malloc ( 65 );
	free( ptr5 );
	free( ptr6 );
	
	
	
	char * pptr1 = ( char * ) malloc ( 1024 );
	char * buf1 = ( char * ) malloc ( 4 );
	char * pptr6 = ( char * ) malloc ( 16 );
	char * buf2 = ( char * ) malloc ( 4 );
	char * pptr2 = ( char * ) malloc ( 2048 );
	char * buf3 = ( char * ) malloc ( 4 );
	char * pptr7 = ( char * ) malloc ( 16 );
	char * buf4 = ( char * ) malloc ( 4 );
	char * pptr3 = ( char * ) malloc ( 1024 );
	
	free( pptr1 ); 
	free( pptr2 ); 
	free( pptr3 ); 
	
	char * pptr5 = ( char * ) malloc ( 2000 );
	
	char * pptr4 = ( char * ) malloc ( 1000 );
	
	free( pptr4 ); 
	free( pptr5 ); 
	free( pptr6 ); 
	free( pptr7 ); 
	free( buf1 );
	free( buf2 );
	free( buf3 );
	free( buf4 );
	
	
	char * ppptr1    = ( char * ) malloc ( 65535 );
	char * buffer1 = ( char * ) malloc( 4 );
	char * ppptr4    = ( char * ) malloc ( 64 );
	char * buffer2 = ( char * ) malloc( 4 );
	char * ppptr2    = ( char * ) malloc ( 6000 );
	
	free( ppptr1 ); 
	free( ppptr2 ); 
	
	char * ppptr3 = ( char * ) malloc ( 1000 );
	
	free( ppptr3 ); 
	free( ppptr4 ); 
	free( buffer1 ); 
	free( buffer2 ); 
	
	
	char * iptr1    = ( char * ) malloc ( 65535 );
	char * ibuffer1 = ( char * ) malloc( 1 );
	char * iptr4    = ( char * ) malloc ( 65 );
	char * ibuffer2 = ( char * ) malloc( 1 );
	char * iptr2    = ( char * ) malloc ( 1500 );
	
	free( iptr1 ); 
	free( iptr2 ); 
	
	char * iptr3 = ( char * ) malloc ( 1000 );
	
	free( iptr3 ); 
	free( iptr4 ); 
	free( ibuffer1 ); 
	free( ibuffer2 ); 
	
	char * jptr = ( char * ) malloc ( 1000 );
	
	free( jptr ); 
	
	char * kptr = ( char * ) malloc ( 1000 );
	
	free( kptr ); 
	
	char * lptr = ( char * ) malloc ( 1000 );
	
	free( lptr ); 
	
	// creates array of processes of random size, shuffles them, then frees half
	// then reallocates those processes with a new random size
	char * ptrarr[256];
	int i;
	for(i = 0; i < 256; i++)
	{
		ptrarr[i] = ( char * ) malloc ( rand() % 1000 );
	}
	
	for(i = 0; i < 256; i++)
	{
		int j = rand() % 256;
		char * temp = ptrarr[i]; 
		ptrarr[i] = ptrarr[j]; 
		ptrarr[j] = temp;
	}
	
	for(i = 0; i < 128; i++)
	{
		free( ptrarr[i] );
	}
	
	for(i = 0; i < 128; i++)
	{
		ptrarr[i] = ( char * ) malloc ( rand() % 1000 );
	}
	
	for(i = 0; i < 256; i++)
	{
		int j = rand() % 256;
		char * temp = ptrarr[i]; 
		ptrarr[i] = ptrarr[j]; 
		ptrarr[j] = temp;
	}
	
	for(i = 0; i < 256; i++)
	{
		free( ptrarr[i] );
	}
	
	// repeats above process but with less processes of larger size
	char * ptrarr2[64];
	for(i = 0; i < 64; i++)
	{
		ptrarr2[i] = ( char * ) malloc ( rand() % 4000 );
	}
	
	for(i = 0; i < 64; i++)
	{
		int j = rand() % 64;
		char * temp = ptrarr2[i]; 
		ptrarr2[i] = ptrarr2[j]; 
		ptrarr2[j] = temp;
	}
	
	for(i = 0; i < 32; i++)
	{
		free( ptrarr2[i] );
	}
	
	for(i = 0; i < 32; i++)
	{
		ptrarr2[i] = ( char * ) malloc ( rand() % 4000 );
	}
	
	for(i = 0; i < 64; i++)
	{
		int j = rand() % 64;
		char * temp = ptrarr2[i]; 
		ptrarr2[i] = ptrarr2[j]; 
		ptrarr2[j] = temp;
	}
	
	for(i = 0; i < 64; i++)
	{
		free( ptrarr2[i] );
	}
	
	// same thing but now a whole bunch of really small processes
	char * ptrarr3[1024];
	for(i = 0; i < 1024; i++)
	{
		ptrarr3[i] = ( char * ) malloc ( rand() % 250 );
	}
	
	for(i = 0; i < 1024; i++)
	{
		int j = rand() % 1024;
		char * temp = ptrarr3[i]; 
		ptrarr3[i] = ptrarr3[j]; 
		ptrarr3[j] = temp;
	}
	
	for(i = 0; i < 512; i++)
	{
		free( ptrarr3[i] );
	}
	
	for(i = 0; i < 512; i++)
	{
		ptrarr3[i] = ( char * ) malloc ( rand() % 250 );
	}
	
	for(i = 0; i < 1024; i++)
	{
		int j = rand() % 1024;
		char * temp = ptrarr3[i]; 
		ptrarr3[i] = ptrarr3[j]; 
		ptrarr3[j] = temp;
	}
	
	for(i = 0; i < 1024; i++)
	{
		free( ptrarr3[i] );
	}
	
	// all done, destroy mavalloc
	mavalloc_destroy();
	
	// prints out exec time, and adds to file for recording
	execTime = clock() - execTime;
	double nsTime = ((double) execTime) / CLOCKS_PER_SEC * 1000000;
	printf("exec time: %0.0fns\n", nsTime);

    fprintf(f, "%0.0f ", nsTime);
    fclose(f);
	
	
	return 0;
}
