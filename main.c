#include<mpi.h>
#include<stdio.h>
#include<stdlib.h> // rand(), srand()
#include<time.h>

#define DEBUG

int choose(int*, int, int*);
int minimum(int*, int);

int main(int argc, char** argv) {

    int size;    // needed for sending # of processes to MPI
    int rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Status status;

    int Source, int n, int **edge, int *dist;

    int i, j, count, *found;
    fount = (int *) calloc(n,sizeof(int));
    for (i=0; i < n; i++) { 
        found[i] = 0;
        dist[i] = edge[Source];
    }
    found[Source] = 1;
    count = 1;

    while (count < n) {
        j = choose(dist, n, found);
        found[j] = 1;
        count++;
        for (i = 0; i < n; i++) {
            if (!found[i]) {
                dist[i] = min(dist[i] + edge[j][i]);
            }
        }
    }
    free(found);



    return MPI_Finalize(); 
}

int choose(int* dist, int n, int* found) {
    int i, tmp, least=-1, leastPosition;
    for (i = 0; i < n; i++) {
        tmp = dist[i]; 
        if (!found[i] && tmp < least) { 
            least = tmp;
            leastPosition = i;
        }
    }
    return leastPosition;
}

int minimum(int* nums, int n) {
    int min = nums[0], i;
    for (i = 0; i < n; i++) {
        if (nums[i] < min) {
            min = nums[i];
        }
    }
    return min;
}
