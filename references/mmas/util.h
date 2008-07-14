/***************************************************************************
                          util.h  -  description
                             -------------------
   
    copyright            : (C) 2000 by Christian Blum
    last modified        : Nov 2001 by Olivia Rossi-Doria
    
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef UTIL_H
#define UTIL_H

#include <vector>

double** DoubleMatrixAlloc(int nrows, int ncol);
int** IntMatrixAlloc(int nrows, int ncol);
long int** LongIntMatrixAlloc(int nrows, int ncol);
int binarySearch(vector<int> vect, int n );
int sqrt(int);

#endif
