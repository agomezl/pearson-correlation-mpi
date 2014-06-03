#include <iostream>
#include <cstdio>
#include <mpi.h>
#include <cmath>
#include <cstring>
#include "Util.h"

using namespace std;

int  numtasks, rank, len, rc;
int * * matrixUI;
int * matrixSr;
int users ,movies, curnode, m;

int main(int argc, char *argv[]) {

   char hostname[MPI_MAX_PROCESSOR_NAME];

   rc = MPI_Init(&argc,&argv);
   if (rc != MPI_SUCCESS) {
     printf ("Error starting MPI program. Terminating.\n");
     MPI_Abort(MPI_COMM_WORLD, rc);
   }

   MPI_Comm_size(MPI_COMM_WORLD,&numtasks);
   MPI_Comm_rank(MPI_COMM_WORLD,&rank);
   MPI_Get_processor_name(hostname, &len);
   printf ("Number of tasks= %d My rank= %d Running on %s\n",
           numtasks,rank,hostname);

   // Read the file

   freopen(argv[1],"r",stdin);

   // Make some space for the matrix
   cin >> users >> movies >> m;
   matrixUI = new int*[users];
   for (int i = 0; i < users; i++) matrixUI[i] = new int[movies];
   if (rank == 0) matrixSr = new int[users*m];

   // Fill the matrix whit the dat
   for (int i = 0 ; i < users; i++)
     for (int j = 0; j < movies; j++)
       cin >> matrixUI[i][j];

   if (numtasks > users){
     cout << "too many cores" << endl;
     MPI_Abort(MPI_COMM_WORLD, rc);
   }

   int slice,rem,minU,maxU;
   slice = users / numtasks;
   minU  = rank*slice;
   if (rank + 1 == numtasks){
     maxU = users;
     slice = users / numtasks + users % numtasks;
   }else maxU = rank*slice + slice;

   int vectSR[m*slice];
   double corMatrix [users];
   double best;
   int bestIndx,offset;
   for (int i = minU; i < maxU ; i++) {
     for (int j = 0; j < users; j++) {
       if (i == j) corMatrix[j] = 1.0;
       else corMatrix[j] = corr(i,j);
     }
     for (int j = 0; j < m; j++) {
       best     = -1e9;
       bestIndx = -1;
       for (int k = 0; k < users; k++) {
         if (k == i) continue;
         if (corMatrix[k] > best){
           best = corMatrix[k];
           bestIndx = k;
         }
       }
       vectSR[(i - minU)*m+j] = bestIndx;
       corMatrix[bestIndx] = -1e9;
     }
   }

   int counts[numtasks],offs[numtasks],off;

   MPI_Allgather(&slice,1,MPI_INT,counts ,1,MPI_INT,MPI_COMM_WORLD);

   off = 0;
   for(int j=0;j<numtasks;j++){
     offs[j]=off;
     counts[j] *= m;
     off  += counts[j];
   }

   MPI_Gatherv(vectSR,slice*m,MPI_INT,
               matrixSr,counts,offs,MPI_INT,
               0,MPI_COMM_WORLD);

   if (rank == 0)
     for (int i = 0; i < users; i++){
       for (int j = 0 ; j < m; j++)
         cout << matrixSr[i*m +j] << " " ;
       cout << endl;
     }

    MPI_Finalize();
}

 double corr(int a, int u) {
   double pa,pu;
   pa = pu = 0.0;
   for (int i = 0; i < movies; i++) {
     pa += matrixUI[a][i];
     pu += matrixUI[u][i];
   }
   pa /= movies;
   pu /= movies;
   double con=0;
   for (int i = 0; i < movies; ++i){
     con += (matrixUI[a][i]-pa)*(matrixUI[u][i]-pu);
   }
   double cod1 = 0;
   double cod2 = 0;
   for (int i = 0; i < movies; ++i) {
     cod1 += (matrixUI[a][i]-pa)*(matrixUI[a][i]-pa);
     cod2 += (matrixUI[u][i]-pu)*(matrixUI[u][i]-pu);
   }
   cod1 = sqrt(cod1);
   cod2 = sqrt(cod2);
   return con/(cod1+cod2);
 }
