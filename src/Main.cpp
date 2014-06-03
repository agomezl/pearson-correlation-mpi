#include <iostream>
#include <cstdio>
#include <mpi.h>
#include <cmath>
#include <cstring>
#include "Util.h"

using namespace std;

int  numtasks, rank, len, rc;
int * * matrixUI;
int * * matrixSr;
int users ,movies;
MPI_Status status;
int curnode;
int m;

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
   if (rank == 0) {
     matrixSr = new int*[users];
     for (int i = 0; i < users; i++) matrixSr[i] = new int[m];
   }

   // Fill the matrix whit the data
   for (int i = 0 ; i < users; i++) {
     for (int j = 0; j < movies; j++) {
       cin >> matrixUI[i][j];
     }
   }

   if (rank == 0) mainMaster();
   else mainNode();

   MPI_Finalize();
}

void mainMaster(){
  int tuple[2];
  curnode = 0;
  for (int i = 0 ; i < users; i++) {
    MPI_Send(&i,1,MPI_INT,nextNode(),1,MPI_COMM_WORLD);
  }
  for (int i = 1; i < numtasks; ++i) {
    int end = -1;
    MPI_Send(&end,1,MPI_INT,i,1,MPI_COMM_WORLD);
  }
  curnode = 0;
  for (int i = 0; i < users; i++) {
    double row [m+1];
    MPI_Recv(row,m+1,MPI_DOUBLE,nextNode(),2,MPI_COMM_WORLD,&status);
    int idx = row[0];
    memcpy(matrixSr[idx],&row[1],sizeof(double)*m);
    for (int i = 0; i < m; ++i)
      matrixSr[idx][i] = row[i+1];
  }
  for (int i = 0; i < users; ++i) {
    for (int j = 0; j < m; ++j) {
      cout<<matrixSr[i][j]<<" ";
    }
    cout<<endl;
  }

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

void mainNode(){
  double row [m+1];
  double corMatrix [users];
  while(true){
    int idx;
    MPI_Recv(&idx,1,MPI_INT,0,1,MPI_COMM_WORLD,&status);
    if (idx == -1) break;
    for (int i = 0; i < users; ++i) {
      if (i == idx) corMatrix[i] = 1.0;
      else {
        corMatrix[i] = corr(i,idx);
      }
    }

    for (int i = 1; i <= m; ++i) {
      double best = -1e9;
      int bestIdx = -1;
      for (int j = 0; j < users; ++j) {
        if (j == idx) continue;
        if (corMatrix[j] > best){
          best = corMatrix[j];
          bestIdx = j;
        }
      }
      row[i] = bestIdx;
      corMatrix[bestIdx] = -1e9;
    }
    row[0] = idx;
    MPI_Send(row,m+1,MPI_DOUBLE,0,2,MPI_COMM_WORLD);
  }
}

int nextNode(){

  curnode++;
  if(curnode >= numtasks) curnode = 1;
  return curnode;
}
