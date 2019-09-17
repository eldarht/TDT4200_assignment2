#include <stdlib.h>
#include <stdio.h>
#include "bitmap.h"
#include <mpi.h>
#include <string.h>

#define XSIZE 2560 // Size of before image
#define YSIZE 2048

int main() {
	MPI_Init(NULL, NULL);
    int worldSize;
    MPI_Comm_size(MPI_COMM_WORLD, &worldSize);

    int worldRank;
    MPI_Comm_rank(MPI_COMM_WORLD, &worldRank);

    uchar *image = NULL; 
    if (worldRank == 0) // Only root needs to store the actual image
    {
    	printf("Rank %d executing load/save\n", worldRank);

		image = calloc(XSIZE * YSIZE * 3, 1); // Three uchars per pixel (RGB)
		readbmp("before.bmp", image);
    }

    // The part of the picture that can be evenly distributed among the processes
    int sectionSize = (XSIZE * YSIZE * 3) / worldSize;

    // Distribute the image
    uchar *imageSection = malloc(sizeof(uchar) * sectionSize);
    MPI_Scatter(image, sectionSize, MPI_UNSIGNED_CHAR,
        imageSection, sectionSize, MPI_UNSIGNED_CHAR,
        0, MPI_COMM_WORLD
    );

    if (worldRank % 2)
    {
        invert(imageSection, sectionSize);
    }

    uchar *resultImage = NULL; 
    if (worldRank == 0) // Only root needs space to store the actual result
    {
        resultImage = calloc(XSIZE * YSIZE * 3, 1);
    }

    // Reassemble the image
    MPI_Gather(imageSection, sectionSize, MPI_UNSIGNED_CHAR,
        resultImage, sectionSize, MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD
    );

    free(imageSection);  

    if (worldRank == 0) // Only root can save and cleanup
    {
        printf("From rank %d:Fixing the last %d values.\n",worldRank, (XSIZE * YSIZE * 3) % worldSize);
        zero(&image[sectionSize * worldSize], (XSIZE * YSIZE * 3) % worldSize);

        memcpy(resultImage, &image[sectionSize * worldSize], (XSIZE * YSIZE * 3) % worldSize);
        savebmp("after.bmp", resultImage, XSIZE, YSIZE);
        
        free(image);
        free(resultImage);
    }



    printf("From rank: %d, i think this is good bye\n", worldRank);

    MPI_Finalize();
	return 0;
}
