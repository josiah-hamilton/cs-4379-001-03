#include<mpi.h>
#include<stdio.h>
#include<stdlib.h> // rand(), srand()
#include<time.h>

#define NODES 100
#define EDGES 300
#define SOURCE 0
#define DEBUG

int choose(int*, int, int*);
int minimum(int*, int);
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
            int* localminima = (int*)calloc(size,sizeof(int))
            for (int i = 1; i < size; i++) {
                MPI_Send(dist+i*chunksize,chunksize,MPI_Int,i,i,MPI_COMM_WORLD);
                MPI_Send(found+i*chunksize,chunksize,MPI_Int,i,i+size,MPI_COMM_WORLD);
            }
            localminima[0] = choose(dist, chunksize, found); 

            // To generalize the number of nodes:MPI_Ranks, rank 0 also
            // processes the remainder of the nodes not assigned to others
            for (int i = 1; i < size; i++) {
                MPI_Recv(localminima[i], 1, MPI_Int, 0, i+size+size, MPI_COMM_WORLD);
            }
            
            if (chunkremaindersize > 0) {
                int remaindermin = choose(dist+n/rank,chunkremaindersize,found+n/size);
                localmin = min(localmin, remaindermin);
                if ( remaindermin < localminima[0] ) {
                    localminima[0] = remaindermin;
                }
            }

            //j = choose(dist, n, found);
            j = minimum(localminima, size);
            
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
        int *distchunk  = (int*) calloc(chunksize, sizeof(int));
        int *foundchunk = (int*) calloc(chunksize, sizeof(int));
        int localmin;

            
        MPI_Recv( distchunk, chunksize, MPI_INT, 0, rank,   MPI_COMM_WORLD);
        MPI_Recv(foundchunk, chunksize, MPI_INT, 0, rank+size, MPI_COMM_WORLD);

        localmin = choose(distchunk,chunksize,foundchunk);

        MPI_Send (&localmin, 1, MPI_INT, 0, rank+size+size, MPI_COMM_WORLD);
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
