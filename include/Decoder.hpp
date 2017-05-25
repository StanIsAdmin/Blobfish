#ifndef DECODER_HPP
#define DECODER_HPP

#include "AbstractCoder.hpp"

template <uint SIZE>
class Decoder : public Coder<SIZE> {
  public:
    // Default constructor
    Decoder(const bytefield<SIZE>& newKey);

    // Encodes a character. 'x' must be in the interval [0, 255].
    byte code(const byte x);

    // Changes the encoding key
    void setKey(const bytefield<SIZE>& newKey);

  protected:
    // Computes the inverse key
    void makeInverseKey();

    using Coder<SIZE>::key;
    using Coder<SIZE>::count;

    bytefield<SIZE> inverseKey; // Key used for inverse permutation

    // Permutation that swaps pairs, based on the key.
    Plugboard<SIZE> plugboard;

    // Rotor that xors values, based on the key
    Rotor<SIZE> rotor;
};

template <uint SIZE>
Decoder<SIZE>::Decoder(const bytefield<SIZE>& newKey) :
    Coder<SIZE>(newKey),
    inverseKey(),
    plugboard(inverseKey),
    rotor(key) {
    makeInverseKey();
}

template <uint SIZE>
byte Decoder<SIZE>::code(const byte x) {
    // Compute result
    byte result = plugboard[x];
    result = rotor[result];
    result = plugboard[result];

    count++; // Increment counter

    // Modify key every SIZE
    if (count % SIZE == 0) {
        this->xorKey();
        makeInverseKey();
    }
    return result;
}

template <unsigned int SIZE>
void Decoder<SIZE>::setKey(const bytefield<SIZE>& newKey) {
    key = newKey; // Copy of password
    makeInverseKey();
}

template<uint SIZE>
void Decoder<SIZE>::makeInverseKey() {
    for (unsigned char i = 0; i < SIZE; ++i) {
        inverseKey[key[i]] = i;
    }
}

#endif /* DECODER_HPP */

