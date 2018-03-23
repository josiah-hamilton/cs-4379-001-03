#include<mpi.h>
#include<stdio.h>
#include<stdlib.h> // rand(), srand()
#include<time.h>

#define NODES 100
#define EDGES 300
#define SOURCE 0
#define DEBUG

int choose(int*, int, int*);
int graphsynth(int**, int);

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
    
    edge = (int**) calloc(n*n, sizeof(int)); // 2D symmetric-about-diag matrix
    dist = (int*)  callod(n,   sizeof(int));

    graphsynth(edge, n);


    int i, j, count, *found;
    found = (int*) calloc(n,sizeof(int));

    for (i=0; i < n; i++) {     //
        found[i] = 0;
        dist[i] = edge[source];
    }

    found[source] = 1;          // source node is a given

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

int graphsynth(int **edge, int n) {
    for (int i = 0; i < n; i++) {
        edge[i][i] = 0;                 // assume no edges contain one node
        for (int j = 1; j < i; j++) {
            int weight = rand() % 19 + 1;
            if ( weight > 1 ) { // 5% chance of getting a link on a given iter
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
        for (int j = 0; j < i; j++) {
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
            int weight = rand() % 19 + 1;
            edge[i][j] = weight;
            edge[j][i] = weight;
        }
    }
}
