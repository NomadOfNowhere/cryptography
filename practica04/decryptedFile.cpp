#include <iostream>
#include "DES.h"
#include "Base64DecoderEncoder.h"
#include "SDES.h"
#include <iostream>
#include <vector>
#include <fstream>  // librería para manipular archivos
#include <cryptopp/cryptlib.h>
#include <cryptopp/osrng.h>
#include <cryptopp/des.h>
#include <cryptopp/integer.h>
#include <cryptopp/osrng.h>
#include <cryptopp/base64.h>
#include <cryptopp/files.h>
#include <cryptopp/hex.h>
#include <cryptopp/modes.h>
#include <cryptopp/pkcspad.h>
#include <cryptopp/padlkrng.h>

using namespace std;

// Instrucciones de compilación
// g++ lab4-main.cpp DES.cpp SDES.cpp Base64DecoderEncoder.cpp -o lab4 -std=c++17 -lcryptopp

void keyGeneration() {
    int opc;
    bits keySDES;
    vector<bits> subkeys;
    cout << "¡Generación de llaves!" << endl;
    cout << "[1] Llaves de subronda SDES." << endl;
    cout << "[2] Llave de DES." << endl;
    cout << "[0] Salir." << endl;
    cout << "Selecciona una opción: ";
    cin >> opc;
    cout << endl;

    switch(opc) {
        case 1:
            cout << "Ingresa la llave: ";
            cin >> keySDES;
            subkeys = getSDESSubkeys(keySDES);

            for(int i=0; i<2; i++) {
                cout << "K" << i+1 << ": " << showBinNumber(subkeys[i], M) << " - " << subkeys[i] << endl;
            }
            break;

        case 2:
            cout << "La llave aleatoria es: " << getDESRandomKey() << endl;
            break;

        case 0:
            return;
        default:
            cout << "¡Ingresa una opción válida!" << endl;
    }
}

void cipher() {
    int opc;
    bits keySDES;
    string m, c, keyDES;
    vector<bits> subkeys;
    cout << "¡Cifrador!" << endl;
    cout << "[1] Cifrar texto usando SDES." << endl;
    cout << "[2] Cifrar archivos usando DES." << endl;
    cout << "[3] Codificar texto usando base64." << endl;
    cout << "[0] Salir." << endl;
    cout << "Selecciona una opción: ";
    cin >> opc;
    cout << endl;

    switch(opc) {
        case 1:
            cout << "Ingresa la llave: ";
            cin >> keySDES;
            cout << "Ingresa la cadena a cifrar: ";
            m = c = "";
            subkeys = getSDESSubkeys(keySDES);
            cin.ignore(); 
            getline(cin, m);
         
            for(auto it : m) {
                c.push_back(char(SDES(subkeys, it)));
            }
            cout << "El mensaje cifrado es: " << base64Encode(c) << endl;

            break;
        case 2:
            cout << "Ingresa la llave: ";
            cin >> keyDES;
            DESEncryptFile(keyDES);
            break;
        
        case 3:
            cout << "Ingresa el mensaje codificar: ";
            cin >> m;
            cout << "El mensaje codificado en base64 es: " << base64Encode(m) << endl;
            break;

        case 0:
            return;

        default:
            cout << "¡Ingresa una opción válida! :(" << endl;
    }
}

void decipher() {
    int opc;
    bits keySDES;
    string m, c, keyDES;
    vector<bits> subkeys;
    cout << "¡Descifrador!" << endl;
    cout << "[1] Descifrar texto usando SDES." << endl;
    cout << "[2] Descifrar archivos usando DES." << endl;
    cout << "[3] Decodificar texto usando base64." << endl;
    cout << "[0] Salir." << endl;
    cout << "Selecciona una opción: ";
    cin >> opc;
    cout << endl;

    switch(opc) {
        case 1:
            cout << "Ingresa la llave: ";
            cin >> keySDES;
            subkeys = getSDESSubkeys(keySDES);
            cout << "Ingresa la cadena a descifrar: ";
            c = m = "";
            cin.ignore(); 
            getline(cin, m);

            c = base64Decode(m);
            m = "";
            for(auto it : c) {
                m.push_back(char(SDES({subkeys[1], subkeys[0]}, it)));
            }
            cout << "El mensaje descifrado es: " << m << endl;
            break;

        case 2:
            cout << "Ingresa la llave: ";
            cin >> keyDES;
            DESDecryptFile(keyDES);
            break;

        case 3:
            cout << "Ingresa el mensaje a decodificar: ";
            cin >> m;
            cout << "El mensaje decodificado es: " << base64Decode(m) << endl;
            break;

        case 0:
            return;

    }
}

int main() {
    // Declaración de variables
    int opc;
    bool f = 1;
    preCalcBase64();

    // Menú
    while(f) {
        cout << "* * * SDES / DES * * *" << endl;
        cout << "[1] Generación de llaves." << endl;
        cout << "[2] Cifrar." << endl;
        cout << "[3] Descifrar." << endl;
        cout << "[0] Salir." << endl;

        
        cout << "Selecciona una opción: ";
        cin >> opc;
        cout << endl;

        switch(opc) {
            case 1:
                keyGeneration();
                break;
            case 2:
                cipher();
                break;
            case 3:
                decipher();
                break;

            case 0:
                f = 0;
                break;

            default:
                cout << "¡Ingresa una opción válida! :(" << endl;
        }
        cout << endl;
    }

    return 0;
}


