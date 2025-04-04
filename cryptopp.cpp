// test.cpp
// Generate two 64 bits random integers and multiplies them.
#include <cryptopp/cryptlib.h>
#include <cryptopp/osrng.h>
#include <cryptopp/integer.h>
#include <iostream>
#include <iomanip>

// g++ cryptopp.cpp -o crypto -std=c++17 -lcryptopp
// g++ test.cpp -o test -s -l:libcryptopp.a   
using namespace std;

void test() {
    // Create a pseudo random number generator
    CryptoPP::AutoSeededRandomPool prng;

    for(int i=0; i<10; i++) {
        cout << i << " " << hex << CryptoPP::Integer(prng, 64) << endl;
    }

}

int main(int argc, char* argv[])
{

    CryptoPP::AutoSeededRandomPool prng;
    CryptoPP::Integer x(prng, 64), y(prng, 64);

    std::cout << "x = " << std::hex << x << std::endl;
    std::cout << "y = " << std::hex << y << std::endl;
    std::cout << "x*y = " << std::hex << x*y << std::endl;

    return 0;
}
