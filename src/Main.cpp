#include <string.h>
#include <algorithm>

#include "Coder.hpp"

#define PWD_SIZE 256

/* Main function. */
int main(int argc, char* argv[]) {
	// Parse arguments
	if (argc != 4) {
		std::cout << "unvalid argument count" << std::endl;
		return 1;
	}

	bool encrypting;
	if (strcmp(argv[1], "-e") == 0) {
		encrypting = true;
	} else if (strcmp(argv[1], "-d") == 0) {
		encrypting = false;
	} else {
		std::cout << "unvalid argument values" << std::endl;
		return 1;
	}

	const char *IN_FILE = argv[2]; // Input file
	const char *OUT_FILE = argv[3]; // Output file

	const int T = 1; // Number of passes
	const unsigned int PWD_SIZE = 256; // Range of accepted values
	std::array<unsigned char, PWD_SIZE> password; // Password

	// Generate password (TODO)
	password = {18, 103, 219, 144, 93, 226, 173, 64, 177, 154, 2, 65, 217, 104, 233, 200, 59, 236,
		29, 101, 190, 143, 161, 221, 96, 126, 77, 246, 52, 118, 108, 175, 189, 50, 212, 90, 201, 231,
		88, 109, 102, 207, 119, 151, 100, 97, 6, 244, 216, 24, 10, 218, 192, 199, 3, 203, 53, 234, 147,
		141, 131, 54, 89, 111, 220, 112, 17, 57, 56, 121, 72, 1, 74, 196, 61, 123, 224, 105, 250, 31,
		125, 142, 198, 172, 155, 11, 243, 30, 76, 166, 241, 134, 14, 43, 153, 169, 164, 37, 149, 36,
		184, 146, 171, 35, 70, 206, 58, 67, 75, 62, 252, 130, 225, 45, 27, 208, 223, 187, 15, 181, 227,
		232, 82, 176, 178, 183, 159, 148, 162, 85, 254, 210, 66, 163, 5, 135, 242, 117, 115, 86, 73,
		60, 255, 48, 202, 79, 205, 95, 158, 136, 168, 98, 44, 230, 239, 0, 182, 13, 33, 222, 145, 83,
		185, 167, 157, 170, 214, 46, 42, 165, 92, 39, 81, 71, 179, 78, 237, 139, 253, 68, 132, 4, 110,
		32, 174, 186, 211, 84, 55, 21, 122, 238, 124, 12, 193, 114, 26, 23, 248, 51, 180, 204, 229, 22,
		140, 191, 34, 188, 7, 49, 156, 197, 107, 213, 129, 245, 38, 251, 9, 133, 137, 209, 120, 25, 28,
		150, 194, 19, 116, 80, 47, 235, 138, 240, 228, 113, 215, 106, 128, 99, 195, 69, 152, 91, 20, 40,
		127, 87, 16, 41, 249, 94, 8, 63, 160, 247};

	// Read file data to memory buffer
	const long size = fileSize(IN_FILE);
	char *data = (char *) malloc(size);
	std::ifstream in;
	in.open(IN_FILE);
	in.read(data, size);
	in.close();

	// Begin chronometring
	using namespace std::chrono;
	high_resolution_clock::time_point t1 = high_resolution_clock::now();

	// Coder (encoder or decoder)
	Coder<PWD_SIZE>* coder;
	if (encrypting) {
		coder = new Encoder<PWD_SIZE>(password);
	} else {
		coder = new Decoder<PWD_SIZE>(password);
	}

	// for each pass
	for (int i = 0; i < T; ++i) {

		// Code the data
		for (long k = 0; k < size; ++k) {
			data[k] = coder->code(data[k]);
		}

		// Modifiy the key using Caesar code
		unsigned int caesar = password[0] != 0 ? password[0] : password[1];
		for (unsigned int l = 0; l < PWD_SIZE; ++l) {
			password[l] = (password[l] + caesar) % PWD_SIZE;
		}

		// Set the encoder key
		coder->setKey(password);
	}

	// End chronometring and display result
	high_resolution_clock::time_point t2 = high_resolution_clock::now();
	duration<double> time_span = duration_cast<duration<double>>(t2 - t1);

	if (encrypting) {
		std::cout << size << " bytes encrypted " << T << " times in " <<
			time_span.count() << " seconds." << std::endl;
	} else {
		std::cout << size << " bytes decrypted " << T << " times in " <<
			time_span.count() << " seconds." << std::endl;
	}


	// Write to output file.
	std::ofstream out;
	out.open(OUT_FILE);
	out.write(data, size);
	out.close();

	// Clean up buffer.
	free(data);

	return 0;
}
