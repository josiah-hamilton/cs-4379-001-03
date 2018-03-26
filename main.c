#include<mpi.h>
#include<stdio.h>
#include<stdlib.h> // rand(), srand()
#include<time.h>

#ifndef NODES
#define NODES 100
#endif
#define EDGES 300
#define SOURCE 0
//#define DEBUG

int choose(int*, int, int*);
int minimum(int*, int);
int min(int, int);
void graphsynth(int*, int);
void printgraph(int*, int);

int main(int argc, char** argv) {

    srand(time(NULL)); // seed clock for rand()

    int size;    // needed for sending # of processes to MPI
    int rank;
    int source, n, *edge, *dist;

    source  = SOURCE;
    n       = NODES;


    edge = (int*) calloc(n*n, sizeof(int)); // 2D symmetric-about-diag matrix
    dist = (int*)  calloc(n,   sizeof(int));

    graphsynth(edge, n);
#ifdef DEBUG
    fprintf(stderr,"Graph generated\n");
    printgraph(edge,n);
#endif

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Status status;

    int chunksize = n / size;
    int chunkremaindersize = n % size; // rank size - 1 gets the remainder as well

    if ( rank == 0 ) {
        int i, j, count, *found, *localminima;
        found = (int*) calloc(n,sizeof(int));
        localminima = (int*)calloc(size,sizeof(int));

        for (i=0; i < n; i++) {     //
            found[i] = 0;
            dist[i] = edge[source];
        }

        found[source] = 1;          // source node is a given

        count = 1;
        while (count < n) {
            for (int i = 1; i < size; i++) {
                MPI_Send( dist+i*chunksize,chunksize,MPI_INT,i,     i,MPI_COMM_WORLD);
                MPI_Send(found+i*chunksize,chunksize,MPI_INT,i,i+size,MPI_COMM_WORLD);
#ifdef DEBUG
                fprintf(stderr,"%d: Sent choose data to %d\n",rank,i);
#endif
            }
            localminima[0] = choose(dist, chunksize, found); 

            // To generalize the number of nodes:MPI_Ranks, rank 0 also
            // processes the remainder of the nodes not assigned to others
            for (int i = 1; i < size; i++) {
                MPI_Recv(localminima+i, 1, MPI_INT, i, i+size+size, MPI_COMM_WORLD, &status);
#ifdef DEBUG
                fprintf(stderr,"%d: Receive local minimum from %d\n",rank,i);
#endif
            }
            
            if (chunkremaindersize > 0) {
                int remaindermin = choose(dist+n/size,chunkremaindersize,found+n/size);
                //localmin = min(localmin, remaindermin);
                localminima[0] = min(localminima[0],remaindermin);
            }

            //j = choose(dist, n, found);
            j = minimum(localminima, size);
            
            found[j] = 1;
            count++;
            for (i = 0; i < n; i++) {
                if (!found[i]) {
                    dist[i] = min(dist[i], edge[n*i+j]);
                }
            }
        }
        free(found);

    } else {  // nonzero ranks
#ifdef DEBUG
        fprintf(stderr,"%d: Here\t size %d\n",rank,size);
#endif
        int count = 1;
        while (count < n) {
            int *distchunk  = (int*) calloc(chunksize, sizeof(int));
            int *foundchunk = (int*) calloc(chunksize, sizeof(int));
            int localmin;

                
            MPI_Recv( distchunk, chunksize, MPI_INT, 0,      rank, MPI_COMM_WORLD, &status);
            MPI_Recv(foundchunk, chunksize, MPI_INT, 0, rank+size, MPI_COMM_WORLD, &status);

            localmin = choose(distchunk,chunksize,foundchunk);

            MPI_Send (&localmin, 1, MPI_INT, 0, rank+size+size, MPI_COMM_WORLD);
            count++;
        }
    }  // nonzero ranks

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

int min(int a, int b) {
    return (a<b) ? a : b;
}

void graphsynth(int *edge, int n) {
    int maxweight = 20;
    for (int i = 0; i < n; i++) {
        edge[n*i+i] = 0;                 // assume no edges contain one node
        for (int j = 1; j < i; j++) {
            int exist  = rand() % maxweight + 1;
            int weight = rand() % maxweight + 1;
            if ( exist > 1 ) { // 5% chance of getting a link on a given iter
                edge[n*i+j] = 0;
                edge[n*j+i] = 0;
            } else if (j != i) {
                edge[n*i+j] = weight;
                edge[n*j+i] = weight;
            }
        }
    }
    // check that all nodes are connected at least once
    for (int i = 0; i < n; i++) {
        int connected = 0;
        for (int j = 0; (j < i) && (connected == 0); j++) {
            if (edge[n*i+j] > connected) {
                connected = edge[n*i+j];
            }
        }
        if (connected == 0) {
            // edge[i][random not i] = random
            int j = i;
            while (j == i ) {
                j = rand()%n;
            }
            int weight = rand() % maxweight + 1;
            edge[n*i+j] = weight;
            edge[n*j+i] = weight;
        }
    }
}

void printgraph(int *edge, int n) {
#ifdef DEBUG
    for (int i = 0; i < n; i++) {
        fprintf(stderr,"\t%d",i);
    }
    fprintf(stderr,"\n_");
    for (int i = 0; i < n; i++) {
        fprintf(stderr,"________",i);
    }
    fprintf(stderr,"\n");
    for (int i = 0; i < n; i++) {
        fprintf(stderr,"%d|\t",i);
        for (int j = 0; j < n; j++) {
            fprintf(stderr,"%d\t",edge[n*i+j]);
        }
        fprintf(stderr,"\n");
    }
#else
    for (int i = 0; i < n; i++) {
        printf("\t%d",i);
    }
    printf("\n_");
    for (int i = 0; i < n; i++) {
        fprintf(stderr,"________",i);
    }
    printf("\n");
    for (int i = 0; i < n; i++) {
        printf("%d|\t",i);
        for (int j = 0; j < n; j++) {
            printf("%d\t",edge[n*i+j]);
        }
        printf("\n");
    }
#endif
}

