#include <chrono>
#include <cstdlib>
#include <fstream>
#include <ios>
#include <iostream>
#include <list>
#include <string>
#include <array>


typedef unsigned int uint;
typedef unsigned char byte;

template <uint SIZE>
using bytefield = std::array<byte, SIZE>;


// ---------- PERMUTATION ----------

/* A permutation is a map between values of the same set.
 * The domain and image of the permutation is the range [0, SIZE[
 */
template<uint SIZE>
class Permutation
{
  public:
	// Constructor takes a reference to the key
	Permutation(const bytefield<SIZE>& key);
	
	// Access operator computes the permutation of value x
	virtual byte operator [](const byte x) const;
  
  protected:
	const bytefield<SIZE>& keyRef; // Reference to the key as const
};

template <uint SIZE>
Permutation<SIZE>::Permutation(const bytefield<SIZE>& key):
	keyRef(key)
{
}

template<uint SIZE> inline
byte Permutation<SIZE>::operator [](const byte x) const
{
	return keyRef[x];
}


// ---------- ROTOR ----------

/* A rotor is mostly a way to xor values based on a key.
 * The key is an bytefield that contains xor values.
 */
template<uint SIZE>
class Rotor
{
  public:
	// Constructor takes a reference to the key
	Rotor(const bytefield<SIZE>& key);
	
	// Access operator computes the permutation of value x
	byte operator [](const byte x);
	
	// Set the starting index
	void setindex();

  private:
	uint index = 0; // Index of the xor value to use
	const bytefield<SIZE>& keyRef; // Reference to the key as const
};

template <uint SIZE>
Rotor<SIZE>::Rotor(const bytefield<SIZE>& key):
	keyRef(key)
{
	setindex();
}

template<uint SIZE> inline
byte Rotor<SIZE>::operator [](const byte x)
{
	index++; // Increment index
	return x xor keyRef[index % SIZE]; // xor x with value from keyRef at index
}

template<uint SIZE> inline
void Rotor<SIZE>::setindex()
{
	// Initialize index as distance between min and max values
	uint start = 0;
	for ( ; keyRef[start] != 0 && keyRef[start] != SIZE-1; ++start) ;
	uint end = start;
	for ( ; keyRef[end] != 0 && keyRef[end] != SIZE-1; ++end) ;
	index = end - start - 1;
}


// ---------- CODER ----------

/* Main encoding logic. Encoder currently can only manage a single
 * pass of char-by-char encoding.
 * TODO Add convenience methods for string encoding and multiple
 * passes.
 */
template <uint SIZE>
class Coder
{
  public:  
	// Default constructor
	Coder(const bytefield<SIZE>& newKey);
	
	// Encodes a character. 'x' must be in the interval [0, 255].
	virtual byte code(const byte x) = 0;
	
	// Changes the encoding key
	virtual void changeKey(const bytefield<SIZE>& newKey);

  protected:
	virtual void xorKey(); // Modifies the key in a determined fashion
  
	bytefield<SIZE> key; // Copy of key used for encryption
	uint count = 0; // Number of chars encoded
};

template <unsigned int SIZE>
Coder<SIZE>::Coder(const bytefield<SIZE>& newKey):
	key(newKey)
{
}

template <unsigned int SIZE>
void Coder<SIZE>::changeKey(const bytefield<SIZE>& newKey)
{	
	key = newKey; // Copy of password
}

template <unsigned int SIZE>
void Coder<SIZE>::xorKey()
{	
	const byte xorValue = key[(count / SIZE - 1) % SIZE];
	for (int i = 0; i < SIZE; ++i) {
		key[i] ^= xorValue;
	}
}



// ---------- ENCODER ----------

template <uint SIZE>
class Encoder : public Coder<SIZE>
{
  public:
	// Default constructor
	Encoder(const bytefield<SIZE>& newKey);
  
	// Encodes a character. 'x' must be in the interval [0, 255].
	byte code(const byte x);

  protected:
	using Coder<SIZE>::key;
	using Coder<SIZE>::count;
	
	// Permutation that swaps pairs, based on the key.
	Permutation<SIZE> plugboard;
	
	// Rotor that xors values, based on the key
	Rotor<SIZE> rotor;
};

template <uint SIZE>
Encoder<SIZE>::Encoder(const bytefield<SIZE>& newKey):
	Coder<SIZE>(newKey),
	plugboard(key),
	rotor(key)
{
}

template <uint SIZE>
byte Encoder<SIZE>::code(const byte x)
{
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


// ---------- DECODER ----------

template <uint SIZE>
class Decoder : public Coder<SIZE>
{
  public:
	// Default constructor
	Decoder(const bytefield<SIZE>& newKey);
  
	// Encodes a character. 'x' must be in the interval [0, 255].
	byte code(const byte x);
	
	// Changes the encoding key
	void changeKey(const bytefield<SIZE>& newKey);

  protected:
	// Computes the inverse key
	void makeInverseKey(); 
	
	using Coder<SIZE>::key;
	using Coder<SIZE>::count;
	
	bytefield<SIZE> inverseKey; // Key used for inverse permutation
	
	// Permutation that swaps pairs, based on the key.
	Permutation<SIZE> plugboard;
	
	// Rotor that xors values, based on the key
	Rotor<SIZE> rotor;
};

template <uint SIZE>
Decoder<SIZE>::Decoder(const bytefield<SIZE>& newKey):
	Coder<SIZE>(newKey),
	plugboard(inverseKey),
	rotor(key)
{
	makeInverseKey();
}

template <uint SIZE>
byte Decoder<SIZE>::code(const byte x)
{
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
void Decoder<SIZE>::changeKey(const bytefield<SIZE>& newKey)
{	
	key = newKey; // Copy of password
	makeInverseKey();
}

template<uint SIZE>
void Decoder<SIZE>::makeInverseKey()
{	
	for (uint i=0; i < SIZE; ++i) {
		inverseKey[key[i]] = i;
	}
}


// Computes a file size. Taken from http://www.cppreference.com.
long fileSize(const char *fileName)
{
	long begin, end;
	std::ifstream file(fileName);
	begin = file.tellg();
	file.seekg (0, std::ios::end);
	end = file.tellg();
	file.close();
	return end - begin;
}
