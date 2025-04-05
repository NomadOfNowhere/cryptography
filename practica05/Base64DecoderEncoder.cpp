#include "Base64DecoderEncoder.h"
#include <vector>
using namespace std;

// Arreglo con la codificación para base64
string base64Chars;

// Llenamos la cadena con los 64 caracteres: A-Z, a-z, 0-9, +, /
void preCalcBase64() {
    char c[] = {'A', 'a', '0', 'Z', 'z', '9'};
    for(int i=0; i<3; i++) {
        for(char j=c[i]; j<=c[i+3]; j++) {
            base64Chars.push_back(j);
        }
    }
    base64Chars += "+/";
}

// Codificar a base64
string base64Encode(string input) {
    int val = 0, valb = -6;
    string encoded;

    for(unsigned char c: input) {
        val = (val << 8) + c;
        valb += 8;
        while(valb >= 0) {
            encoded.push_back(base64Chars[(val >> valb) & 0x3f]);
            valb -= 6;
        }
    }
    if(valb > -6) {
        encoded.push_back(base64Chars[((val<<8) >> (valb+8)) & 0x3f]);
    }
    while(encoded.size() % 4) {
        encoded.push_back('=');
    }

    return encoded;
}

// Decodificar de base64
string base64Decode(string input) {
    int val = 0, valb = -8;
    string decode;
    vector<int> T(256, -1);

    for(int i=0; i<64; i++) {
        T[base64Chars[i]] = i;
    }

    for(unsigned char c : input) {
        if(T[c] == -1) 
            break;
        
        val = (val << 6) + T[c];
        valb += 6;

        if(valb >= 0) {
            decode.push_back(char((val >> valb) & 0xff));
            valb -= 8;
        }
    }

    return decode;
}