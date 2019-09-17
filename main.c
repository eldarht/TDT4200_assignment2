#include <stdlib.h>
#include <stdio.h>
#include "bitmap.h"
#include <mpi.h>

#define XSIZE 2560 // Size of before image
#define YSIZE 2048

int main() {
	MPI_Init(NULL, NULL);

    int worldSize;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int worldRank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    if (worldRank == 0)
    {
    	printf("Rank %d executing load/save\n", worldRank);

		uchar *image = calloc(XSIZE * YSIZE * 3, 1); // Three uchars per pixel (RGB)
		readbmp("before.bmp", image);

		// Alter the image here

		savebmp("after.bmp", image, XSIZE, YSIZE);
		free(image);
    }
    printf("From rank: %d, i think this is good bye\n", worldRank);

    MPI_Finalize();
	return 0;
}
