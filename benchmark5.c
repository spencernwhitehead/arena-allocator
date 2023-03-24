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
	
	mavalloc_init( 256000, FIRST_FIT );
	
	// copied over all the allocs and frees from main.c
	char * ptr = ( char * ) mavalloc_alloc ( 65535 );
	
	mavalloc_free( ptr ); 
	
	char * ptr1    = (char*)mavalloc_alloc( 65535 );
	char * ptr2    = (char*)mavalloc_alloc( 65 );
	mavalloc_free( ptr1 );
	mavalloc_free( ptr2 );
	
	char * ptr3    = (char*)mavalloc_alloc( 65536 );
	char * ptr4    = (char*)mavalloc_alloc( 64 );
	mavalloc_free( ptr4 );
	mavalloc_free( ptr3 );
	
	char * ptr5  = ( char * ) mavalloc_alloc ( 10000 );
	char * ptr6  = ( char * ) mavalloc_alloc ( 65 );
	mavalloc_free( ptr5 );
	mavalloc_free( ptr6 );
	
	
	
	char * pptr1 = ( char * ) mavalloc_alloc ( 1024 );
	char * buf1 = ( char * ) mavalloc_alloc ( 4 );
	char * pptr6 = ( char * ) mavalloc_alloc ( 16 );
	char * buf2 = ( char * ) mavalloc_alloc ( 4 );
	char * pptr2 = ( char * ) mavalloc_alloc ( 2048 );
	char * buf3 = ( char * ) mavalloc_alloc ( 4 );
	char * pptr7 = ( char * ) mavalloc_alloc ( 16 );
	char * buf4 = ( char * ) mavalloc_alloc ( 4 );
	char * pptr3 = ( char * ) mavalloc_alloc ( 1024 );
	
	mavalloc_free( pptr1 ); 
	mavalloc_free( pptr2 ); 
	mavalloc_free( pptr3 ); 
	
	char * pptr5 = ( char * ) mavalloc_alloc ( 2000 );
	
	char * pptr4 = ( char * ) mavalloc_alloc ( 1000 );
	
	mavalloc_free( pptr4 ); 
	mavalloc_free( pptr5 ); 
	mavalloc_free( pptr6 ); 
	mavalloc_free( pptr7 ); 
	mavalloc_free( buf1 );
	mavalloc_free( buf2 );
	mavalloc_free( buf3 );
	mavalloc_free( buf4 );
	
	
	char * ppptr1    = ( char * ) mavalloc_alloc ( 65535 );
	char * buffer1 = ( char * ) mavalloc_alloc( 4 );
	char * ppptr4    = ( char * ) mavalloc_alloc ( 64 );
	char * buffer2 = ( char * ) mavalloc_alloc( 4 );
	char * ppptr2    = ( char * ) mavalloc_alloc ( 6000 );
	
	mavalloc_free( ppptr1 ); 
	mavalloc_free( ppptr2 ); 
	
	char * ppptr3 = ( char * ) mavalloc_alloc ( 1000 );
	
	mavalloc_free( ppptr3 ); 
	mavalloc_free( ppptr4 ); 
	mavalloc_free( buffer1 ); 
	mavalloc_free( buffer2 ); 
	
	
	char * iptr1    = ( char * ) mavalloc_alloc ( 65535 );
	char * ibuffer1 = ( char * ) mavalloc_alloc( 1 );
	char * iptr4    = ( char * ) mavalloc_alloc ( 65 );
	char * ibuffer2 = ( char * ) mavalloc_alloc( 1 );
	char * iptr2    = ( char * ) mavalloc_alloc ( 1500 );
	
	mavalloc_free( iptr1 ); 
	mavalloc_free( iptr2 ); 
	
	char * iptr3 = ( char * ) mavalloc_alloc ( 1000 );
	
	mavalloc_free( iptr3 ); 
	mavalloc_free( iptr4 ); 
	mavalloc_free( ibuffer1 ); 
	mavalloc_free( ibuffer2 ); 
	
	char * jptr = ( char * ) mavalloc_alloc ( 1000 );
	
	mavalloc_free( jptr ); 
	
	char * kptr = ( char * ) mavalloc_alloc ( 1000 );
	
	mavalloc_free( kptr ); 
	
	char * lptr = ( char * ) mavalloc_alloc ( 1000 );
	
	mavalloc_free( lptr ); 
	
	// creates array of processes of random size, shuffles them, then frees half
	// then reallocates those processes with a new random size
	char * ptrarr[256];
	int i;
	for(i = 0; i < 256; i++)
	{
		ptrarr[i] = ( char * ) mavalloc_alloc ( rand() % 1000 );
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
		mavalloc_free( ptrarr[i] );
	}
	
	for(i = 0; i < 128; i++)
	{
		ptrarr[i] = ( char * ) mavalloc_alloc ( rand() % 1000 );
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
		mavalloc_free( ptrarr[i] );
	}
	
	// repeats above process but with less processes of larger size
	char * ptrarr2[64];
	for(i = 0; i < 64; i++)
	{
		ptrarr2[i] = ( char * ) mavalloc_alloc ( rand() % 4000 );
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
		mavalloc_free( ptrarr2[i] );
	}
	
	for(i = 0; i < 32; i++)
	{
		ptrarr2[i] = ( char * ) mavalloc_alloc ( rand() % 4000 );
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
		mavalloc_free( ptrarr2[i] );
	}
	
	// same thing but now a whole bunch of really small processes
	char * ptrarr3[1024];
	for(i = 0; i < 1024; i++)
	{
		ptrarr3[i] = ( char * ) mavalloc_alloc ( rand() % 250 );
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
		mavalloc_free( ptrarr3[i] );
	}
	
	for(i = 0; i < 512; i++)
	{
		ptrarr3[i] = ( char * ) mavalloc_alloc ( rand() % 250 );
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
		mavalloc_free( ptrarr3[i] );
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
