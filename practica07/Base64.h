#ifndef BASE64_H
#define BASE64_H

#include <iostream>
using namespace std;

class Base64 {
private:
    // Arreglo con la codificación para base64
    string base64Chars;

public:
Base64();                    // Constructor
string encode(string input); // Codificar a base64
string decode(string input); // Decodificar de base64
};

#endif // BASE64_H
