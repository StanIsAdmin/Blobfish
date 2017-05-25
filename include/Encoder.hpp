#ifndef ENCODER_HPP
#define ENCODER_HPP

#include "Plugboard.hpp"
#include "AbstractCoder.hpp"

template <uint SIZE>
class Encoder : public Coder<SIZE> {
  public:
    // Default constructor
    Encoder(const bytefield<SIZE>& newKey);

    // Encodes a character. 'x' must be in the interval [0, 255].
    byte code(const byte x);

  protected:
    using Coder<SIZE>::key;
    using Coder<SIZE>::count;

    // Permutation that swaps pairs, based on the key.
    Plugboard<SIZE> plugboard;

    // Rotor that xors values, based on the key
    Rotor<SIZE> rotor;
};

template <uint SIZE>
Encoder<SIZE>::Encoder(const bytefield<SIZE>& newKey) :
    Coder<SIZE>(newKey),
    plugboard(key),
    rotor(key) {
}

template <uint SIZE>
byte Encoder<SIZE>::code(const byte x) {
    // Compute result
    byte result = plugboard[x];
    result = rotor[result];
    result = plugboard[result];

    count++; // Increment counter

    // Modify key every SIZE
    if (count % SIZE == 0) {
        this->xorKey();
    }
    return result;
}

#endif /* ENCODER_HPP */
