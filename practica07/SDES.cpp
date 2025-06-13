#include <iostream>
#include "SDES.h"
#include <vector>
using namespace std;

// Posiciones de las permutaciones
vector<int> P10 = {3, 5, 2, 7, 4, 10, 1, 9, 8, 6},
             P8 = {6, 3, 7, 4, 8, 5, 10, 9},
             P4 = {2, 4, 3, 1},
             IP = {2, 6, 3, 1, 4, 8, 5, 7},
             invIp = {4, 1, 3, 5, 7, 2, 8, 6},
             EP = {4, 1, 2, 3, 2, 3, 4, 1},

             S0[] = {{1, 0, 3, 2}, 
                     {3, 2, 1, 0},
                     {0, 2, 1, 3},
                     {3, 1, 3, 2}},

             S1[] = {{0, 1, 2, 3},
                     {2, 0, 1, 3},
                     {3, 0, 1, 0},
                     {2, 1, 0, 3}};

bits getPermutation(vector<int> permutation, bits value, int size) {
    bits p = 0;
    for(int i=0; i<permutation.size(); i++) {
        if((1 << (size - permutation[i])) & value) {
            p |= (1 << (permutation.size() - 1 - i));
        }
    }
    return p;
}

string showBinNumber(bits b, int sz) {
    string s = "";
    for(int i=sz-1; i>=0; i--) {
        s += (((1 << i) & b) ? '1' : '0');
    }
    return s;
}   

bits getShift(bits value, int shifts) {
    bits left = (((value & leftTrunc) << shifts) | (value >> ((N>>1) - shifts))) & leftTrunc;
    bits right = (((value & rightTrunc) << shifts) | ((value & rightTrunc) >> ((N>>1) - shifts))) & rightTrunc;
    return (left | right) & trunc;
}

vector<bits> getSDESSubkeys(bits key) {    
    // Paso 1: Se aplica P10 a la key
    bits P10_key = getPermutation(P10, key, N);
    
    // Paso 2: P10_key se divide en dos mitades y se les aplica un corrimiento circular de 1 bit
    bits CI_1 = getShift(P10_key, 1);

    // Paso 3: Se aplica P8 al corrimiento circular (CI_1) para obtener la llave 1
    bits K1 = getPermutation(P8, CI_1, N);

    // Paso 4: CI_1 se divide en dos mitades y se le aplica un corrimiento circular de 2 bits
    bits CI_2 = getShift(CI_1, 2);

    // Paso 5: Se aplica P8 al corrimiento circular de 2 bits (CI_2)
    bits K2 = getPermutation(P8, CI_2, N);

    return {K1, K2};
}


// Para cifrar: subkeys = {K1, K2}
// Para descifrar: subkeys = {K2, K1}
bits SDES(vector<bits> subkeys, bits message) {
    // Paso inicial: Se aplica IP al mensaje
    bits ip = getPermutation(IP, message, M);

    // Paso 1: Se divide el mensaje ip en bloques izquierdo y derecho
    bits L = 0, R = 0;
    for(int i=M-1; i>=(M>>1); i--) {    // Mitad izquierda
        L |= ((ip >> (M>>1)) & (1 << (i % (M>>1))));
    }
    for(int i=(M>>1)-1; i>=0; i--) {       // Mitad derecha
        R |= ((1 << i) & ip);
    }

    // Se llevarán a cabo 2 rondas
    for(int i=0; i<2; i++) {
        // Paso 2: Se aplica la permutación de expansión (E/P) a la parte derecha (R)
        bits ep = getPermutation(EP, R, M>>1);

        // Paso 3: Aplicar ep ^ K1 (xor)
        bits ep_xor = ep ^ subkeys[i];

        // Paso 4: Consulta en tablas S0 (parte izq) y S1 (parte der)
        // renglón (primer y último bit), columna (segundo y tercer bit)
        bits s_cajas = 0, row, col;

        row = ((ep_xor >> (M-2)) & (1 << 1)) | ((ep_xor >> (M>>1)) & 1);        // S0 - izq
        col = ((ep_xor >> (M-3)) & (1 << 1)) | ((ep_xor >> ((M>>1) + 1)) & 1);
        s_cajas |= (S0[row][col] << 2);

        row = ((ep_xor >> ((M>>1)-2)) & (1 << 1)) | (1 & ep_xor);               // S1 - der
        col = ((ep_xor >> ((M>>1)-3)) & (1 << 1)) | ((ep_xor >> 1) & 1);
        s_cajas |= S1[row][col];

        // Paso 5: Le aplicamos P4 a s_cajas
        bits P4_key = getPermutation(P4, s_cajas, M>>1);

        // Paso 6: Aplicar L ^ P4_key
        L ^= P4_key;

        // Paso 7: Se intercambian bloques para la ronda 2
        if(!i) swap(L, R);
    }

    return getPermutation(invIp, (L<<(M>>1))|R, M);
}