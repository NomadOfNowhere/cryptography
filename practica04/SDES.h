#ifndef SDES_H
#define SDES_H

#include <iostream>
#include <vector>

using namespace std;

// Declaración de variables y constantes
#define bits unsigned short
#define trunc 0x3FF       // Sólo los 10 bits menos significativos - 0b1111111111
#define leftTrunc 0x3E0    // Parte izquierda - 0b1111100000
#define rightTrunc 0x1F    // Parte derecha -   0b0000011111
#define N 10
#define M 8

// Declaración de funciones
bits getPermutation(vector<int> permutation, bits value, int size);
string showBinNumber(bits b, int sz);
bits getShift(bits value, int shifts);
vector<bits> getSDESSubkeys(bits key);
bits SDES(vector<bits> subkeys, bits message);

#endif // SDES_H