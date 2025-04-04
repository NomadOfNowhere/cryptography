#include <iostream>
#include <vector>
#include <tuple>
#include <random>
#include <unordered_set>
#include <time.h>
using namespace std;

// Calcula el inverso modular
int modularInverse(int n, int a) {
    int a0 = n;
    int b0 = a;
    int t0 = 0;
    int t = 1;
    int s0 = 1;
    int s = 0;
    int q = a0 / b0;
    int r = a0 - q * b0;

    while(r > 0) {
        int temp = t0 - q * t;
        t0 = t;
        t = temp;
        temp = s0 - q * s;
        s0 = s;
        s = temp;
        a0 = b0;
        b0 = r;
        q = a0 / b0;
        r = a0 - q * b0;
    }
    r = b0;
    t += (t < 0 ? n : 0);
    return (r != 1 ? -1 : t);
}

// Calcula el extendido de euclides de forma recursiva
tuple<int, int, int> recursiveExtendedGCD(int n, int a) {
	if (a == 0) return {n, 1, 0};
	auto[r, x, y] = recursiveExtendedGCD(a, n % a);
	return {r, y, x - n / a * y};
}

// Calcula el inverso modular utilizando el extended GCD recursivo
int recursiveModInverse(int n, int a) {
    auto[r, x, y] = recursiveExtendedGCD(n, a);
    if(r != 1) {
        return -1;
    }
    return ((y < 0) ? y+n : y);
}

// Lee datos para calcular el inverso modular
void getModularInverse() {
    int n, a;
    cout << "Ingresa el tamaño del alfabeto 'n': ";
    cin >> n;
    cout << "Ingresa el número 'a': ";
    cin >> a;


    int aux = modularInverse(n, a);
    if(aux == -1) {
        cout << "¡El número " << a << " no tiene inverso multiplicativo! :(" << endl;
    }
    else {
        cout << "Inverso modular es: " << aux << endl;
    }
}

// Calcula Zn*
unordered_set<int> getZnStar(int n) {
    unordered_set<int> st;
    for(int i=1; i<n; i++) {
        if(gcd(i, n) == 1) {
            st.insert(i);
        }
    }

    return st;
}

// Retorna el determinante MOD n de una matriz
int getDeterminant(int n, vector<vector<int>> matrix) {
    return (((matrix[0][0] * matrix[1][1] - matrix[0][1] * matrix[1][0]) % n) + n) % n;
}

// Genera una matriz aleatoria con elementos que pertenecen a Zn
vector<vector<int>> randomMatrix(int n) {
    vector<vector<int>> matrix(2, vector<int>(2));
    for(int i=0; i<2; i++) {
        for(int j=0; j<2; j++) {
            matrix[i][j] = random() % n;
        }
    }
    
    return matrix;
}

// Calcula la matriz inversa
vector<vector<int>> getInvMatrix(int n, int invDet, vector<vector<int>> matrix) {
    swap(matrix[0][0], matrix[1][1]);
    matrix[0][1] *= -1;
    matrix[1][0] *= -1;

    for(int i=0; i<2; i++) {
        for(int j=0; j<2; j++) {
            matrix[i][j] = (((matrix[i][j] * invDet) % n) + n) % n;
        }
    }

    return matrix;
}

// Multiplica dos matrices para verificar si se obtiene la matriz identidad
vector<vector<int>> checkMatrix(int n, vector<vector<int>> matrix, vector<vector<int>> invMatrix) {
    vector<vector<int>> aux(2, vector<int>(2));
    for(int i=0; i<2; i++) {
        for(int j=0; j<2; j++) {
            for(int k=0; k<2; k++) {
                aux[i][j] += matrix[i][k] * invMatrix[k][j];
                aux[i][j] %= n;
            }
        }
    }
    return aux;
}

// Despliega la información de una matriz
void showMatrix(vector<vector<int>> K) {
    for(auto row : K) {
        for(auto col : row) {
            cout << col << " ";
        }
        cout << endl;
    }
}

// Obtiene llave válida para Hill Cipher
void getHillCipherKey() {
    int n, det, invDet;
    bool valid = 0;
    unordered_set<int> znStar;
    vector<vector<int>> K, invK, aux;
    cout << "Ingresa el tamaño del alfabeto 'n': ";
    cin >> n;

    znStar = getZnStar(n);

    // Generamos una matriz aleatoria hasta que sea válida
    do{
        K = randomMatrix(n);
        det = getDeterminant(n, K);
        valid = znStar.count(det);
    }
    while(!valid);
    
    // Calculamos el determinante inverso de K
    invDet = modularInverse(n, det);

    // Calculamos la matriz inversa de K
    invK = getInvMatrix(n, invDet, K);

    // Mostramos la información de las matrices
    cout << "\nMatriz K: \n";
    showMatrix(K);
    cout << "Det(K) = " << det << endl;

    cout << "\nMatriz K^(-1): \n";
    showMatrix(invK);
    cout << "Det(K^(-1)) = " << invDet << endl;

    cout << "\nComprobación K·K^(-1): \n";
    aux = checkMatrix(n, K, invK);
    showMatrix(aux);
    cout << endl;
}

int main() {
    // Declaración de variables
    int opc;
    srand(time(0));
    bool f = 1;

    // Menú
    while(f) {
        cout << "* * * Hill Cipher * * *" << endl;
        cout << "[1] Calcular inverso multiplicativo." << endl;
        cout << "[2] Generar llave para Hill Cipher." << endl;
        cout << "[0] Salir" << endl;
        cout << "Selecciona una opción: ";
        cin >> opc;
        cout << endl;

        switch(opc) {
            case 1:
                getModularInverse();
                break;

            case 2:
                getHillCipherKey();
                break;

            case 0:
                f = 0;
                break;

            default:
                cout << "¡Ingresa una opción válida! :(" << endl;
        }
    }

    return 0;
}

