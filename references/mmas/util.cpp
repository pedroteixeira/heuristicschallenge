/***************************************************************************
                          util.cpp  -  description
                             -------------------
    begin                : Fri Nov 23 2001
    copyright            : (C) 2001 by Olivia Rossi-Doria
    email                : o.rossi-doria@napier.ac.uk
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream.h>

double ** DoubleMatrixAlloc(int nrows, int ncol)
{
  double **matrix;

  if ((matrix=(double**) malloc(sizeof(double)*nrows*ncol + sizeof(double*)*nrows)) == NULL){
    printf("Memory allocation error");
    exit(1);
  }

  for (int r=0; r<nrows; r++){
    matrix[r] = (double *) (matrix+nrows)+r*ncol;
  }

  return matrix;
}

int ** IntMatrixAlloc(int nrows, int ncol)
{
  int **matrix;

  if ((matrix=(int**) malloc(sizeof(int)*nrows*ncol + sizeof(int*)*nrows)) == NULL){
    printf("Memory allocation error");
    exit(1);
  }

  for (int r=0; r<nrows; r++){
    matrix[r] = (int *) (matrix+nrows)+r*ncol;
  }

  return matrix;
}

long int ** LongIntMatrixAlloc(int nrows, int ncol)
{
  long int **matrix;

  if ((matrix=(long int**) malloc(sizeof(long int)*nrows*ncol + sizeof(long int*)*nrows)) == NULL){
    printf("Memory allocation error");
    exit(1);
  }

  for (int r=0; r<nrows; r++){
    matrix[r] = (long int *) (matrix+nrows)+r*ncol;
  }

  return matrix;
}

int binarySearch(vector<int> vect, int n)
{
  int l, r;
  l=0;
  r= (int) vect.size();
  while( r >= l )
    {
      int m = (l+r)/2;
      if( n == vect[m])
	return m;
      if( n < vect[m])
	r = m-1;
      else
	l = m+1;
    }
  return -1;
}

int sqrt(int n)
{
  int s,i;
  for( i = 1; i < n; i++){
    s = i * i;
    if( s > n)
      break;
  }
  return( i-1);
}
