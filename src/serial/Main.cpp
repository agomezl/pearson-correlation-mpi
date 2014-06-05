#include <iostream>
#include <cstdio>
#include <cmath>
#include <cstring>
#include "Util.h"

using namespace std;

int  numtasks, rank, len, rc;
int * * matrixUI;
int * matrixSr;
int users ,movies, curnode, m;

int main(int argc, char *argv[]) {

   freopen(argv[1],"r",stdin);
   // Make some space for the matrix
   cin >> users >> movies >> m;
   matrixUI = new int*[users];
   for (int i = 0; i < users; i++) matrixUI[i] = new int[movies];
   // Fill the matrix whit the dat
   for (int i = 0 ; i < users; i++)
     for (int j = 0; j < movies; j++)
       cin >> matrixUI[i][j];

   //Computes the sub-matrix assigned for that process
   int vectSR[users][m];
   double corMatrix [users];
   double best;
   int bestIndx,offset;
   for (int i = 0; i < users ; i++) {
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
       vectSR[i][j] = bestIndx;
       corMatrix[bestIndx] = -1e9;
     }
   }
   for (int i = 0; i < users; i++){
     for (int j = 0 ; j < m; j++)
       cout << vectSR[i][j] << " " ;
     cout << endl;
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
  for (int i = 0; i < movies; ++i)
    con += (matrixUI[a][i]-pa)*(matrixUI[u][i]-pu);

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
