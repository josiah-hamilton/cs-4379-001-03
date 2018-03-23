#include<mpi.h>
#include<stdio.h>
#include<stdlib.h> // rand(), srand()
#include<time.h>

#define NODES 100
#define EDGES 300
#define SOURCE 0
#define DEBUG

int choose(int*, int, int*);
void graphsynth(int**, int);

int main(int argc, char** argv) {
    srand(time(NULL)); // seed clock for rand()

    int size;    // needed for sending # of processes to MPI
    int rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Status status;

    
    int source, n, **edge, *dist;

    source  = SOURCE;
    n       = NODES;

    int chunksize = n / size;
    int chunkremaindersize = n % size; // rank size - 1 gets the remainder as well
    int edgechunksize = (n*n) / size; 
    int edgechunkremaindersize = (n*n) / size;

    edge = (int**) calloc(n*n, sizeof(int)); // 2D symmetric-about-diag matrix
    dist = (int*)  callod(n,   sizeof(int));

    graphsynth(edge, n);

    if ( rank == 0 ) {
        int i, j, count, *found;
        found = (int*) calloc(n,sizeof(int));

        for (i=0; i < n; i++) {     //
            found[i] = 0;
            dist[i] = edge[source];
        }

        found[source] = 1;          // source node is a given

        count = 1;
        while (count < n) {
            for (int i = 1; i < size; i++) {
                MPI_Send 
            }
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

    } else {  // nonzero ranks
        int **edgechunk = (int**)calloc(edgechunksize,sizeof(int));
        int *foundchunk = (int*) calloc(chunksize, sizeof(int));
        int **edgechunkremainder; //remainder of edges
        int *foundchunkremainder;     //remainder of indeces
        int localmin, remaindermin;

            
        MPI_Recv(edgechunk, edgechunksize, MPI_INT, 0, rank, MPI_COMM_WORLD);
        MPI_Recv(foundchunk, chunksize, MPI_INT, 0, rank*n, MPI_COMM_WORLD);

        localmin = choose(edgechunk,chunksize,foundchunk);

        if (rank == (size - 1)) {
            edgechunkremainder  =(int**)calloc(edgechunkremaindersize,sizeof(int));
            foundchunkremainder =(int*) calloc(chunkremaindersize,sizeof(int));
            MPI_Recv(edgechunk, edgechunkremaindersize, MPI_INT, 0, size, MPI_COMM_WORLD);
            MPI_Recv(foundchunk, chunkremaindersize, MPI_INT, 0, size*n, MPI_COMM_WORLD);
            remaindermin = choose(edgechunkremainder,chunkremaindersize,foundchunkremainder);
            localmin = min(localmin, remaindermin)
        }
        MPI_Send (&localmin, 1, MPI_INT, 0, rank, MPI_COMM_WORLD);
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

void graphsynth(int **edge, int n) {
    int maxweight = 20;
    for (int i = 0; i < n; i++) {
        edge[i][i] = 0;                 // assume no edges contain one node
        for (int j = 1; j < i; j++) {
            int exist  = rand() % maxweight + 1;
            int weight = rand() % maxweight + 1;
            if ( exist > 1 ) { // 5% chance of getting a link on a given iter
                edge[i][j] = 0;
                edge[j][i] = 0;
            } else {
                edge[i][j] = weight;
                edge[j][i] = weight;
            }
        }
    }
    // check that all nodes are connected at least once
    for (int i = 0; i < n; i++) {
        int connected = 0;
        for (int j = 0; (j < i) && (connected == 0); j++) {
            if (edge[i][j] > connected) {
                connected = edge[i][j];
            }
        }
        if (connected == 0) {
            // edge[i][random not i] = random
            int j = i;
            while (j != i ) {
                j = rand()%n;
            }
            int weight = rand() % maxweight + 1;
            edge[i][j] = weight;
            edge[j][i] = weight;
        }
    }
}
