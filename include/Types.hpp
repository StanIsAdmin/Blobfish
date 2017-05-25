#ifndef TYPES_H
#define TYPES_H

#include <array>

typedef uint8_t byte; // one byte (8 bits)
typedef unsigned int uint; // unsigned value

template<uint size>
using bytefield = std::array<byte, size>; // array of SIZE bytes

#endif //TYPES_H