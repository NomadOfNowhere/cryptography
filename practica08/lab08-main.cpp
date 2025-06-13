#include <bits/stdc++.h>
#include <iostream>
#include <fstream>
#include <cryptopp/des.h>
#include <cryptopp/osrng.h>
#include <cryptopp/modes.h>
#include <cryptopp/integer.h>
#include <cryptopp/nbtheory.h>
using namespace std;

using bigint = CryptoPP::Integer;
// g++ lab08-main.cpp -o lab8 -std=c++17 -lcryptopp

bigint getPrime(int bits) {
    CryptoPP::AutoSeededRandomPool prng;
    // return CryptoPP::MaurerProvablePrime(prng, len);
    return CryptoPP::MihailescuProvablePrime(prng, bits);
}

// Función para generar números aleatorios en un rango
bigint getRandomInRange(bigint l, bigint r) {
    CryptoPP::AutoSeededRandomPool prng;
    return CryptoPP::Integer(prng, CryptoPP::Integer(l), CryptoPP::Integer(r));
}

array<bigint, 3> RSA(int bits) {
    bigint p = getPrime(bits), q = getPrime(bits), n, phi, e, d;
    n = p * q;
    phi = (p - 1) * (q - 1);
    
    while(1) {
        e = getRandomInRange(2, phi-1);
        if(CryptoPP::GCD(e, phi) == 1) break;
    }

    d = EuclideanMultiplicativeInverse(e, phi) % n;
    return {e, n, d};
}

vector<bigint> getFactors(bigint n) {
    vector<bigint> factors;
    bigint curr = 2;
    while(n > 1) {
        if(!(n % curr)) {
            factors.push_back(curr);
            while(!(n % curr)) n /= curr;
        }
        curr++;
    }
    return factors;
}

bool isGenerator(bigint g, bigint p, vector<bigint> &factors) {
    for(auto &it : factors) {
        if(CryptoPP::ModularExponentiation(g, (p-1)/it, p) == 1) return false;
    }
    return true;
}

void findGenerators(int bits) {
    for(int i=0; i<3; i++) {
        bigint p = getPrime(bits), g;
        vector<bigint> factors = getFactors(p - 1);

        while(1) {
            g = getRandomInRange(2, p-1);
            if(isGenerator(g, p, factors)) break;
        }
        cout << "¡Iteration " << i+1 << "!" << endl;
        cout << "g = " << g << " for Z*_{" << p << "}" << endl << endl;
    }
}

int main() {
    int opc, bits;
    bool f = 1;

    while(f) {
        cout << "* * * PRIME NUMBERS AND GENERATORS * * *" << endl;
        cout << "[1] Random prime number with n bits." << endl;
        cout << "[2] RSA." << endl;
        cout << "[3] Generators. " << endl;
        cout << "Select an option: ";
        cin >> opc;
        cout << endl;

        switch(opc) {
            case 1:
                cout << "Enter number of bits: ";
                cin >> bits;
                cout << "p = " << getPrime(bits) << endl;
                break;
            case 2: {
                    cout << "Enter number of bits for primes: ";
                    cin >> bits;
                    auto [e, n, d] = RSA(bits);
                    cout << "K_pub: {" << e << ", " << n << "}" << endl;
                    cout << "\nK_priv: " << d << endl;
                }
                break;
            case 3: {
                    cout << "Enter number of bits for primes: ";
                    cin >> bits;
                    findGenerators(bits);
                }
                break;
            default:
                cout << "¡Ingresa una opción válida! :(" << endl;
        }
        cout << endl;
    }
    

    return 0;
}