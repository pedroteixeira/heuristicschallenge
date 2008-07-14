#ifndef RANDOM_H
#define RANDOM_H
#include <algorithm>
#include <vector>
#include <stdlib.h>

class Random {
private:
  /* constants for a pseudo-random number generator, 
     for details see numerical recipes in C 
  */
  static const int IA = 16807;
  static const int IM = 2147483647;
  static const double AM = (1.0/IM);
  static const int IQ = 127773;
  static const int IR = 2836;
  
  double ran01(long *idum);

public:
  Random(const int& arg) : seed(arg) {}
  long int seed;
  double next() { return ran01(&seed);}
  /*    
      FUNCTION:      generates a random vector, quick and dirty
      INPUT:         vector dimension
      OUTPUT:        returns pointer to vector, 
                     free memory after using the vector
      (SIDE)EFFECTS: none
  */
  long int* generate_array(const int & size);
  /*
   * alternative
   */
  vector<long int> generate_random_vector(const int& size ) {
    vector<long int> V(size);
    generate(V.begin(), V.end(), rand);
    return V;
  }

};
#endif
