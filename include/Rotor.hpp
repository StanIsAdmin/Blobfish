#ifndef ROTOR_H
#define ROTOR_H

#include <chrono>
#include <cstdlib>
#include <fstream>
#include <ios>
#include <iostream>
#include <list>
#include <string>
#include <array>

#include "Types.hpp"

/* A rotor is mostly a way to xor values based on a key.
 * The key is a bytefield that contains xor values. 
 */
template<uint SIZE>
class Rotor {
  public:
    // Constructor takes a reference to the key
    Rotor(const bytefield<SIZE>& key);

    // Access operator computes the permutation of value x
    byte operator[](const byte x);

    // Set the starting index
    void setindex();

  private:
    uint index = 0; // Index of the xor value to use
    const bytefield<SIZE>& keyRef; // Reference to the key as const
};

template <uint SIZE>
Rotor<SIZE>::Rotor(const bytefield<SIZE>& key) :
    keyRef(key) {
    setindex();
}

template<uint SIZE> inline
byte Rotor<SIZE>::operator[](const byte x) {
    index++; // Increment index
    return x xor keyRef[index % SIZE]; // xor x with value from keyRef at index
}

template<uint SIZE> inline
void Rotor<SIZE>::setindex() {
    // Initialize index as distance between min and max values
    uint start = 0;
    for (; keyRef[start] != 0 && keyRef[start] != SIZE - 1; ++start);
    uint end = start;
    for (; keyRef[end] != 0 && keyRef[end] != SIZE - 1; ++end);
    index = end - start - 1;
}

#endif //ROTOR_H
