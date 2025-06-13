#include <bits/stdc++.h>
#include <iostream>
#include <fstream>
#include <cryptopp/des.h>
#include <cryptopp/osrng.h>
#include <cryptopp/modes.h>
#include "Base64.h"
using namespace std;

// Instrucciones de compilación
/*
g++ lab07-main.cpp Base64.cpp -o lab07 -std=c++20 -lcryptopp
*/

class ODS{
public:   // Asignación de constantes
    static constexpr int KEYLENGTH = 32;
    static constexpr int BLOCKSIZE = 16;
    using bitK = bitset<KEYLENGTH>;
    using bitB = bitset<BLOCKSIZE>;
    static constexpr bitK PHI = 0x9e3779b9;

private:
    const array<int, ODS::BLOCKSIZE> SBOX = {0xd, 0xb, 0x7, 0xa, 0x0, 0xf, 0x9, 0x2, 
                                 0x6, 0x5, 0x1, 0x8, 0xe, 0xc, 0x3, 0x4},
                      invSBOX = {0x4, 0xa, 0x7, 0xe, 0xf, 0x9, 0x8, 0x2, 
                                 0xb, 0x6, 0x3, 0x1, 0xd, 0x0, 0xc, 0x5};
    const array<int, ODS::BLOCKSIZE> P = {3, 15, 5, 13, 14, 8, 10, 12, 9, 6, 16, 7, 11, 1, 4, 2},
                      invP = {14, 16, 1, 15, 3, 10, 12, 6, 9, 7, 13, 8, 4, 5, 2, 11};
    bitK key, roundPrimes[5];
    bitB subkeys[5];
    Base64 base64;

public:
    // Función para generar una clave ODS segura de manera aleatoria
    bitK getRandomKey() {
        // Crear un generador de números pseudo aleatorios
        CryptoPP::AutoSeededRandomPool prng;

        // Crear una clave ODS con un bloque de bytes aleatorio
        CryptoPP::SecByteBlock skey(ODS::KEYLENGTH / 8);
        prng.GenerateBlock(skey, skey.size());

        // Convertir a cadena binaria y luego a bitset
        string binaryKey;
        for(auto &it : skey) binaryKey += bitset<8>(static_cast<unsigned char>(it)).to_string();
        return bitK(binaryKey);
    }
 
    bitK leftShift(int c, bitK K) {
        bitK aux;
        for(int i=0; i<c; i++) {
            aux[c-1-i] = K[ODS::KEYLENGTH-1-i];
        }
        K <<= c;
        K |= aux;
        return K;
    }

    bitB round(int i, bitK curr) {
        // Corrimiento circular a la izquierda de 3*i posiciones
        curr = leftShift(3*(i+1), curr);

        // Extraemos los 16 bits más significativos
        bitB subkey = 0;
        
        for(int i=ODS::KEYLENGTH-1; i>=ODS::BLOCKSIZE; i--) {
            subkey[i - BLOCKSIZE] = curr[i];
        }
        // XOR con la constante de ronda
        bitB roundConst = bitB(roundPrimes[i].to_ullong());
        return subkey ^ roundConst;
    }

    bitB getPermutation(const std::array<int, ODS::BLOCKSIZE> &permutation, bitB value, int size) {
        bitB p = 0;
        for(int i=0; i<permutation.size(); i++) {
            p[permutation.size()-1-i] = value[size - permutation[i]];
        }
        return p;
    }

    bitB applySBox(bitB value) {
        bitB res;
        for (int i = 0; i < 4; i++) {
            int nibble = (value >> (i * 4)).to_ulong() & 0xF;
            int s = SBOX[nibble];
            for (int j = 0; j < 4; j++) {
                res[i * 4 + j] = (s >> j) & 1;
            }
        }
        return res;
    }

    bitB applyInvSBox(bitB value) {
        bitB res;
        for (int i = 0; i < 4; i++) {
            int nibble = (value >> (i * 4)).to_ulong() & 0xF;
            int s = invSBOX[nibble];
            for (int j = 0; j < 4; j++) {
                res[i * 4 + j] = (s >> j) & 1;
            }
        }
        return res;
    }

    bitB applyPermutation(bitB value) {
        return getPermutation(P, value, BLOCKSIZE);
    }

    bitB applyInvPermutation(bitB value) {
        return getPermutation(invP, value, BLOCKSIZE);
    }

    void init() {
        // Preronda: XOR de la llave con PHI
        this->key = getRandomKey() ^ PHI;

        // Generamos los 10^7 primos
        this->getRoundPrimes();

        // Aplicamos el key scheduling
        for(int i=0; i<5; i++) {
            this->subkeys[i] = round(i+1, this->key);
        }
    }

    // Criba lineal para generar primos
    vector<int> linearSieve(int N) {
        vector<int> p(N), pr;
        for(int i=2; i<N; i++) {
            if(!p[i]) {
                p[i] = i;
                pr.push_back(i);
            }
            for(int j=0; i*pr[j]<N; j++) {
                p[i*pr[j]] = pr[j];
                if(pr[j] == p[i])
                    break;
            }
        }
        return pr;
    }

    // Función para generar números aleatorios en un rango
    int getRandomInRange(int mn, int mx) {
        CryptoPP::AutoSeededRandomPool prng;
        CryptoPP::Integer random = CryptoPP::Integer(prng, CryptoPP::Integer(mn), CryptoPP::Integer(mx));
        return random.ConvertToLong();
    }

    // Función para obtener 5 números aleatorios
    void getRoundPrimes() {
        int on[] = {2, 3, 5, 7, 10};
        int N = 1e7+5, idx = 0;
        vector<int> pr = linearSieve(N);

        while(idx < 5) {
            int curr = getRandomInRange(1, pr.size()-1);
            if(__builtin_popcount(pr[curr]) == on[idx]) {
                roundPrimes[idx] = pr[curr];
                idx++;
            }
        }
    }

    bitB encryptBlock(bitB plaintext) {
        for (int i = 0; i < 5; i++) {
            plaintext ^= subkeys[i];             // XOR con subclave
            plaintext = applySBox(plaintext); // Sustitución
            plaintext = applyPermutation(plaintext);      // Permutación
        }
        return plaintext;
    }

    bitB decryptBlock(bitB ciphertext) {
        for (int i = 4; i >= 0; i--) {
            ciphertext = applyInvPermutation(ciphertext);      // Permutación inversa
            ciphertext = applyInvSBox(ciphertext); // Sustitución inversa
            ciphertext ^= subkeys[i];             // XOR con subclave
        }
        return ciphertext;
    }

    vector<bitB> textToBlocks(const string& text) {
        vector<bitB> blocks;
        size_t i = 0;
        while (i < text.size()) {
            uint16_t block = 0;
            block |= static_cast<uint8_t>(text[i]) << 8;
            if (i + 1 < text.size()) {
                block |= static_cast<uint8_t>(text[i + 1]);
            } else {
                block |= static_cast<uint8_t>('\0'); // Relleno si es impar
            }
            blocks.emplace_back(bitB(block));
            i += 2;
        }
        return blocks;
    }

    vector<bitB> encrypt(const string &plaintext) {
        vector<bitB> blocks = textToBlocks(plaintext);
        vector<bitB> cipherBlocks;
        for(auto &it : blocks) {
            cipherBlocks.push_back(encryptBlock(it));
       }
       return cipherBlocks;
    }

    vector<bitB> decrypt(string ciphertext) {
        vector<bitB> blocks = textToBlocks(ciphertext);
        vector<bitB> plainBlocks;
        for(auto &it : blocks) {
            plainBlocks.push_back(decryptBlock(it));
        }
        return plainBlocks;
    }

    // Get/set
    uint64_t Key() const { return key.to_ulong(); }
    vector<uint64_t> Subkeys() const {
        vector<uint64_t> v;
        for(int i=0; i<5; i++) {
            v.push_back(subkeys[i].to_ulong());
        }
        return v;
    }
};

ostream &operator<<(ostream &os, const vector<uint64_t> &p) {
    os << "[";
    for (size_t i = 0; i < p.size(); ++i) {
        os << p[i];
        if (i + 1 < p.size()) os << ", ";
    }
    os << "]";
    return os;
}



int main() {
    ODS ods;
    ods.init();

    
    cout << "--- PRUEBAS ---" << endl;
    cout << "LISTADO DE LLAVES" << endl;
    // cout << "K: " << ods.key.to_ullong() << endl;
    // for(int i=0; i<5; i++) {
    //     cout << "K" << i+1 << ": " << ods.subkeys[i].to_ullong() << endl;
    // }

    // cout << "\nAPLICACIÓN DE CAJAS DE SUSTITUCIÓN Y PERMUTACIÓN" << endl;
    // bitset<16> testBlock("1011011101111001");
    // cout << "Bloque original:  " << testBlock.to_ullong() << endl;

    // auto sub = ods.applySBox(testBlock);
    // cout << "SBOX:             " << sub.to_ullong() << endl;

    // auto invSub = ods.applyInvSBox(sub);
    // cout << "INV SBOX:         " << invSub.to_ullong() << endl;

    // auto perm = ods.applyPermutation(testBlock);
    // cout << "Permutado:        " << perm.to_ullong() << endl;

    // auto invPerm = ods.applyInvPermutation(perm);
    // cout << "Permutación Inv:  " << invPerm.to_ullong() << endl;


    // cout << "\nEJEMPLO DE CIFRADO Y DESCIFRADO" << endl;
    // string plaintext = "Hello, world!";
    // vector<bitset<16>> cipherBlocks;

    // cout << "Bloque original: " << b64.base64Encode(testBlock.to_string()) << endl;
    // // auto ciphertext = ods.encrypt(testBlock);
    // // cout << "Bloque cifrado: " << b64.base64Encode(ciphertext.to_string()) << endl;
    // // auto plaintext = ods.decrypt(ciphertext);
    // cout << "Bloque descifrado: " << b64.base64Encode(plaintext.to_string()) << endl;
    return 0;
}
