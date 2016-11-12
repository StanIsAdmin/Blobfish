#include <chrono>
#include <cstdlib>
#include <fstream>
#include <ios>
#include <iostream>
#include <list>
#include <string>
#include <vector>

/////////////////////////////////////////////////////////////////////

/* A rotor is a mostly like a counter limited to a maximum value but,
 * from times to times, it decides to incement itself by 2 or 3.
 * These decisions are taken periodically, when the main or secondary
 * counters need to be resetted.
 * The value of a rotor is computed as the sum of its three internal
 * counters.
 * Size is passed as template parameter for performance.
 */
template <unsigned int size>
class Rotor
{
public:
  // When key is set, returns key[value()].
  operator unsigned int() const;
  // Increments the rotor. This may not be as cheap as it looks like.
  void operator ++(int);
  // Resets the rotor to (O, O, O).
  void reset();
  // Sets the state of the three internal counters.
  void set(unsigned int a, unsigned int b, unsigned int c);
  // Sets the key to be used by operator int().
  void setKey(std::string key);
  // Returns the value of the rotor.
  unsigned int value() const;
  
private:
  unsigned int a = 0, b = 0, c = 0;
  unsigned int index = 0;
  std::string key;
};

template <unsigned int size> inline
Rotor<size>::operator unsigned int() const
{
  return key[index];
}

template <unsigned int size> inline
void Rotor<size>::operator ++(int)
{
  a++;
  index++;
  if (a >= size) {
    a = 0;
    b++;
    index++;
    if (b >= size) {
      b = 0;
      c++;
      index++;
      if (c >= size) {
        c = 0;
      }
    }
  }
  if (index >= size) {
    index -= size;
  }
}

template <unsigned int size> inline
void Rotor<size>::reset()
{
  this->a = 0;
  this->b = 0;
  this->c = 0;
}

template <unsigned int size> inline
void Rotor<size>::set(unsigned int a, unsigned int b, unsigned int c)
{
  this->a = a % size;
  this->b = b % size;
  this->c = c % size;
}

template <unsigned int size> inline
void Rotor<size>::setKey(std::string key)
{
  this->key = key;
}

template <unsigned int size> inline
unsigned int Rotor<size>::value() const
{
  return index;
}

/////////////////////////////////////////////////////////////////////

/* A permutation stores a bijection between integers.
 */
class Permutation
{
public:
  // Simple constructors, initializes to identity.
  Permutation();
  // Transforms an integer to another.
  unsigned int operator [](int x) const;
  // Sets the value that operator[] returns when `x' is passed to it.
  void set(int x, unsigned int value);
  
private:
  const static int SIZE = 256;
  unsigned int values[SIZE];
};

Permutation::Permutation()
{
  for (unsigned int i = 0; i < SIZE; ++i) {
    values[i] = i;
  }
}

inline
unsigned int Permutation::operator [](int x) const
{
  return values[x];
}

inline
void Permutation::set(int x, unsigned int value)
{
  values[x] = value;
}

/////////////////////////////////////////////////////////////////////

/* Computes the distance between `a' and `b' in `string'. Two
 * contiguous characters have distance 1.
 */
unsigned int distance(char a, char b, std::string string)
{
  unsigned int start = 0;
  for ( ; string[start] != a && string[start] != b; ++start) ;
  unsigned int end = start;
  for ( ; string[end] != a && string[end] != b; ++end) ;
  return end - start;
}

/////////////////////////////////////////////////////////////////////

/* Main decoding logic. Decoder currently can only manage a single
 * pass of char-by-char encoding.
 * TODO Add convenience methods for string encoding and multiple
 * passes.
 */
class Decoder
{
public:
  // Decodes a character. `x' must be in the interval [0;256[.
  unsigned int decode(const unsigned int &x);
  // Sets the encoding key. The key is a 32 bytes-long string which
  // must contain all characters from 0 to 31.
  void setKey(const std::string &key);

private:
  // Cached array of permutations. permutations[x] is created using
  // createPermutation(x).
  std::vector<Permutation> permutations;
  // Permutation that swaps pairs, based on the key.
  Permutation swapper;
  
  Rotor<8> typeRotor;
  Rotor<32> valueRotor;
  
  // Creates cached permutation usable by encode() when valueRotor's
  // value is `rotorValue'.
  Permutation createPermutation(unsigned int rotorValue);
};

Permutation Decoder::createPermutation(unsigned int rotorValue)
{
  Permutation result;
  unsigned int sentTo;
  for (int i = 0; i < 32; ++i) {
    sentTo = swapper[i];
    sentTo += rotorValue;
    if (sentTo >= 32) {
      sentTo -= 32;
    }
    sentTo = swapper[sentTo];
    result.set(sentTo, i);
  }
  return result;
}

unsigned int Decoder::decode(const unsigned int &x)
{
  unsigned int result = x % 32;
  result = permutations[valueRotor][result];
  unsigned int type = x / 32;
  result -= (typeRotor.value() + type) * 32;
  valueRotor++;
  typeRotor++;
  if (result < 0) {
    result += 256;
  }
  return result;
}

void Decoder::setKey(const std::string &key)
{
  valueRotor.setKey(key);
  valueRotor.reset();
  typeRotor.set(distance(0, 31, key),
                distance(1, 30, key),
                distance(2, 29, key));
  // Update cached data.
  for (int i = 0; i < 32; ++i) {
    swapper.set(key[i], key[31 - i]);
  }
  permutations.clear();
  permutations.reserve(32);
  for (int i = 0; i < 32; ++i) {
    permutations.push_back(createPermutation(i));
  }
}

/////////////////////////////////////////////////////////////////////

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

// Main function (no suspense here :-) )
int main()
{  
  const int N = 1000; // Number of passes
  const char *IN_FILE = "gpl_encrypted.txt"; // Input file
  const char *OUT_FILE = "gpl_decrypted.txt"; // Output file
  
  std::string password = std::string(32, ' '); // Password generation
  for (int i = 0; i < 32; ++i) {
    password[i] = 31 - i;
  }
  
  // Read file data to memory buffer.
  const long size = fileSize(IN_FILE);
  char *data = (char *) malloc(size);
  std::ifstream in;
  in.open(IN_FILE);
  in.read(data, size);
  in.close();
  
  // Begin chronometring.
  using namespace std::chrono;
  high_resolution_clock::time_point t1 = high_resolution_clock::now();
  
  // ==== Decoding ====
  Decoder decoder;
  decoder.setKey(password);
  
  // Create password's Caesar indices.
  unsigned int caesars[N];
  for (int i = 0; i < N; ++i) {
    caesars[i] = password[0] != 0 ? password[0] : password[1];
    for (int l = 0; l < 32; ++l) {
      password[l] = (password[l] + caesars[i]) % 32;
    }
  }
  // Password is now last used + Caesar applied once
  
  for (int i = N - 1; i >= 0; --i) {
    // Substract Caesar
    for (int l = 0; l < 32; ++l) {
      password[l] = password[l] - caesars[i];
      if (password[l] < 0) {
        password[l] += 32;
      }
    }
    decoder.setKey(password);
    
    for (long k = 0; k < size; ++k) {
      data[k] = (char) decoder.decode(data[k]);
    }
  }
  
  // End chronometring and display result.
  high_resolution_clock::time_point t2 =
                                      high_resolution_clock::now();
  duration<double> time_span =
                          duration_cast<duration<double>>(t2 - t1);
  std::cout << size << " bytes decoded " << N << " times in " <<
                    time_span.count() << " seconds." << std::endl;

  // Write to output file.
  std::ofstream out;
  out.open(OUT_FILE);
  out.write(data, size);
  out.close();
  
  // Clean up buffer.
  free(data);
  
  return 0;
}
