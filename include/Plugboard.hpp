#ifndef PERMUTATION_H
#define PERMUTATION_H

#include <chrono>
#include <cstdlib>
#include <fstream>
#include <ios>
#include <iostream>
#include <list>
#include <string>
#include <array>

#include "Types.hpp"

/* A Plugboard maps values of the same set together, by pairs.
 * The domain and image of the permutation is the range [0, SIZE-1]. 
 */
template<uint SIZE>
class Plugboard {
  public:
    // Constructor takes a reference to the key
    Plugboard(const bytefield<SIZE>& key);

    // Access operator computes the permutation of value x
    byte operator[](const byte x) const;

  protected:
    const bytefield<SIZE>& keyRef; // Reference to the key as const
};

template <uint SIZE>
Plugboard<SIZE>::Plugboard(const bytefield<SIZE>& key) :
    keyRef(key) {
}

template<uint SIZE> inline
byte Plugboard<SIZE>::operator[](const byte x) const {
    return keyRef[x];
}

#endif //PERMUTATION_H
