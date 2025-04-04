// Librerías utilizadas en el programa
#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <fstream>  // librería para manipular archivos
using namespace std;

/* Instrucciones de compilación
g++ lab2-sandrita.cpp -o lab2 -std=c++17
./lab2
*/

// Función para leer e inicializar mapas para el alfabeto
void leer_alfabeto(int &n, map<char, int> &alfabeto, map<int, char> &valores) {
    unsigned char opc;
    string s, nombre, linea;

    cout << "[1] Ingresar como cadena de texto. \n[2] Ingresa símbolo por símbolo.\n";
    cout << "[3] Cargar alfabeto desde archivo. \n[0] Regresar.\n";
    cout << "Selecciona una opción: ";
    cin >> opc;
    cout << endl;

    // Leer desde una cadena
    if(opc == '1') {
        cout << "Ingresa el tamaño del alfabeto: ";
        cin >> n;
        
        // Ingresar alfabeto como una cadena
        cout << "Ingresa el alfabeto como una cadena: ";
        cin.ignore();
        getline(cin, s);

        // Asignamos valores y llaves en los mapas
        for(int i=0; i<n; i++) {
            alfabeto[s[i]] = i;
            valores[i] = s[i];
        }
    }
    // Ingresar elemento uno por uno en el alfabeto
    else if(opc == '2') {
        cout << "Ingresa el tamaño del alfabeto: ";
        cin >> n;

        for(int i=0; i<n; i++) {
            cout << "Ingresa el elemento " << i+1 << ": ";
            cin.ignore();
            char c = getchar();
            alfabeto[c] = i;
            valores[i] = c;
        }
    }
    else if(opc == '0') {
        return;
    }
    // Leer desde un archivo
    else {
        cout << "Ingresa el nombre del archivo: ";
        cin >> nombre;
        ifstream archivo(nombre);       // Abrir el archivo en modo lectura

        if(!archivo) {
            cout << "\n¡No se encontró el archivo! :(" << endl;
            return;
        }
        
        getline(archivo, linea);     // Leemos el valor de n del archivo
        n = stoi(linea);             // Convertimos a int
        getline(archivo, linea);     // Leemos la cadena del archivo

        // Asignamos valores y llaves en los mapas
        for(int i=0; i<n; i++) {
            alfabeto[linea[i]] = i;
            valores[i] = linea[i];
        } 
    }

    // valores de prueba:
    // 53
    // "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ ";
}

// Verificamos si la longitud de la llave de cifrado y mensaje son válidas
bool validar_datos(char c, string s, map<char, int> alfabeto) {
    int cmp = (c == 'K' ? 5 : 30);
    if(s.length() < cmp) {
        cout << "ERROR: |" << c << "| < " << cmp << endl;
        return false;
    }
    for(auto it : s) {
        if(!alfabeto.count(it)) {
            cout << "ERROR: " << it << " ∉ Σ*" << endl;
            return false;
        }
    }
    return true;
}

// Función para cifrar utilizando el algoritmo de Vigenère 
string vigenenere_encipher(int n, map<char, int> alfabeto, map<int, char> valores) {
    int idx = 0;
    string K, m, C = "";

    cout << "Ingresa la llave K: ";
    cin.ignore();
    getline(cin, K);

    if(!validar_datos('K', K, alfabeto)) {
        return C;
    }

    cout << "Ingresa el mensaje m: ";
    getline(cin, m);

    if(!validar_datos('m', m, alfabeto)) {
        return C;
    }

    while(idx < m.length()) {   // Iteramos sobre el mensaje
        for(int i=0; i<K.length() && idx < m.length(); i++) {  // Iteramos con la llave
            int mod = (alfabeto[m[idx++]] + alfabeto[K[i]]) % n;  // Sumamos valores de mensaje y llave
            C += valores[mod];  // Asignamos la sustitución al mensaje cifrado
        }
    }
    return C;
}

// Función para descifrar utilizando el algoritmo de Vigenère
string vigenenere_decipher(int n, map<char, int> alfabeto, map<int, char> valores) {
    int idx = 0;
    string K, m = "", C;

    cout << "Ingresa la llave de cifrado: ";
    cin.ignore();
    getline(cin, K);

    if(!validar_datos('K', K, alfabeto)) {
        return m;
    }

    cout << "Ingresa el mensaje cifrado C: ";
    getline(cin, C);

    if(!validar_datos('C', C, alfabeto)) {
        return C;
    }
    

    while(idx < C.length()) {    // Iteramos sobre el mensaje
        for(int i=0; i<K.length() && idx < C.length(); i++) {   // Iteramos con la llave
            // Restamos valores del mensaje y la llave, además obtenemos un módulo positivo 
            // (C++ no convierte módulos a positivos automáticamente)
            int mod = (((alfabeto[C[idx++]] - alfabeto[K[i]]) % n) + n) % n;
            m += valores[mod];   // Asignamos valores al mensaje descifrado
        }
    }

    return m;
}

int main() {
    // Declaración de variables
    int n, opc;
    string s;
    bool f = 1;
    map<char, int> alfabeto;
    map<int, char> valores;

    while(f) {
        cout << "* * * Polyalphabetic substitution * * *" << endl;
        cout << "[1] Ingresar alfabeto." << endl;
        cout << "[2] Vigenènere encipher." << endl;
        cout << "[3] Vigenènere decipher." << endl;
        cout << "[4] Mostrar alfabeto." << endl;
        cout << "[0] Salir" << endl;
        cout << "Selecciona una opción: ";
        cin >> opc;
        cout << endl;

        switch(opc) {
            // Llamada a función para leer e inicializar alfabeto
            case 1:
                leer_alfabeto(n, alfabeto, valores);
                break;

            // Llamada a función para cifrar mensajes con Vigenère
            case 2:
                s = vigenenere_encipher(n, alfabeto, valores);
                if(!s.empty()) 
                    cout << "Mensaje cifrado es: " << s << endl;
                else 
                    cout << "¡Vuelve a intentarlo! :/" << endl;
                break;

            // Llamada a función para descifrar mensajes con Vigenère
            case 3:
                s = vigenenere_decipher(n, alfabeto, valores);
                if(!s.empty())
                    cout << "Mensaje decifrado es: " << s << endl;
                else 
                    cout << "¡Vuelve a intentarlo! :/" << endl;
                break;

            // Mostrar alfabeto y sus valores asociados
            case 4:
                if(!alfabeto.empty()) {
                    cout << "Clave - Valor" << endl;
                    for(auto it : valores) {
                        cout << "  " << it.second << "   :   " << it.first << endl;
                    }
                    cout << endl;
                }
                else {
                    cout << "¡El alfabeto está vacío! :/" << endl;
                }
                break;

            case 0:
                f = 0;
                break;
        
            default:
                cout << ("¡Ingresa una opción válida! :(") << endl;
        }
        cout << endl;
    }
    return 0;
}

