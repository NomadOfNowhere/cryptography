#include <iostream>
#include <map>
#include <vector>
#include <string>
using namespace std;

int main() {

    int n;
    map<char, int> alfabeto;

    while(true) {
        int opc;
        char c;
        string s, m, K;
        cout << "[1] Ingresar alfabeto." << endl;
        cout << "[2] Vigenènere encipher." << endl;
        cout << "[3] Vigenènere decipher." << endl;
        cout << "[0] Salir" << endl;
        cout << "Selecciona una opción: ";
        cin >> opc;
        cout << endl;

        switch(opc) {
            case 1:
                cout << "Ingresa el tamaño del alfabeto: ";
                cin >> n;
                
                for(int i=0; i<n; i++) {
                    cout << "Ingresa el elemento " << i+1 << ": ";
                    cin.ignore();
                    c = getchar();
                    alfabeto[c] = i;
                }

                for(auto it : alfabeto) {
                    cout << it.first << " - " << it.second << endl;
                }
                break;

            case 2:
                cout << "Ingresa la llave K: ";
                cin.ignore();
                getline(cin, K);

                if(K.length() < 5) {
                    cout << "ERROR: |K| < 5" << endl;
                    exit(1);
                }
                for(auto it : K) {
                    if(!alfabeto.count(it)) {
                        cout << "ERROR: " << it << " ∉ Σ*" << endl;
                        exit(1);
                    }
                }

                cout << "Ingresa el mensaje m: ";
                getline(cin, m);
                if(m.length() < 30) {
                    cout << "ERROR: |m| < 30" << endl;
                }
                for(auto it : m) {
                    if(!alfabeto.count(it)) {
                        cout << "ERROR: " << it << " ∉ Σ*" << endl;
                        exit(1);
                    }
                }
                
                break;

            case 3:
                break;

            case 0:
                break;
                
            default:
                cout << ("¡Ingresa una opción válida! :(") << endl;
                cout << ("_____________________________") << endl;
        }
        cout << endl;
    }
    return 0;
}