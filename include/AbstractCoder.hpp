#ifndef CODER_H
#define CODER_H

#include <chrono>
#include <cstdlib>
#include <fstream>
#include <ios>
#include <iostream>
#include <list>
#include <string>
#include <array>

#include "Types.hpp"
#include "Rotor.hpp"

/* Main encoding logic. 
 * Encoder currently can only manage a single pass of char-by-char encoding.
 * TODO Add convenience methods for string encoding and multiple passes.
 */
template <uint SIZE>
class Coder {
  public:
    // Default constructor
    Coder(const bytefield<SIZE>& key);

    // Encodes one byte
    virtual byte code(const byte x) = 0;

    // Sets the encoding key
    virtual void setKey(const bytefield<SIZE>& newKey);
    
    // Virtual destructor
    virtual ~Coder() = default;

  protected:
    // Modifies the key in a determined fashion
    virtual void xorKey();

    bytefield<SIZE> key; // Copy of key used for encryption
    uint count = 0; // Number of chars encoded
};

template <uint SIZE>
Coder<SIZE>::Coder(const bytefield<SIZE>& newKey) :
    key(newKey) {
}

template <unsigned int SIZE>
void Coder<SIZE>::setKey(const bytefield<SIZE>& newKey) {
    key = newKey; // Copy of password
}

template <unsigned int SIZE>
void Coder<SIZE>::xorKey() {
    const byte xorValue = key[(count / SIZE - 1) % SIZE];
    for (byte i = 0; i < SIZE; ++i) {
        key[i] ^= xorValue;
    }
}

#endif //CODER_H
