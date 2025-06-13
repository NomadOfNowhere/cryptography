#include <iostream>
#include "tripleDES.h"
#include "Base64DecoderEncoder.h"
#include <fstream>  // librería para manipular archivos

using namespace std;

// Instrucciones de compilación
// g++ lab5-main.cpp tripleDES.cpp Base64DecoderEncoder.cpp -o lab5 -std=c++17 -lcryptopp

int main() {
    // Declaración de variables
    int opc;
    string tripleDES;
    bool f = 1;
    preCalcBase64();

    // Menú
    while(f) {
        cout << "* * * 3DES * * *" << endl;
        cout << "[1] Generación de llave." << endl;
        cout << "[2] Cifrar." << endl;
        cout << "[3] Descifrar." << endl;
        cout << "[0] Salir." << endl;

        cout << "Selecciona una opción: ";
        cin >> opc;
        cout << endl;

        switch(opc) {
            case 1:
                cout << "La llave aleatoria es: " << get3DESRandomKey() << endl;
                break;
            case 2:
                cout << "Ingresa la llave: ";
                cin >> tripleDES;
                tripleDESEncryptFile(tripleDES);
                break;
            case 3:
                cout << "Ingresa la llave: ";
                cin >> tripleDES;
                tripleDESDecryptFile(tripleDES);
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