#include <iostream>
#include <vector>
#include <tuple>
#include <random>
#include <unordered_set>
#include <time.h>
using namespace std;

tuple<int, int, int> extendedGCD(int n, int a) {
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
    return {r, s, t};
}

tuple<int, int> extendedRecursiveGCD(int n, int a) {
	if (a == 0) return {1, 0};
	auto[x, y] = extendedRecursiveGCD(a, n % a);
	return {y, x - n / a * y};
}

int modularInverse(int n, int a) {
    auto[r, s, t] = extendedGCD(n, a);
    auto[x, y] = extendedRecursiveGCD(n, a);
    cout << r << " - " << " - " << s << " - " << t << endl;
    cout << x << " - " << y << endl;

    if(n*x + a*y != 1) {
        cout << "NO INVERSE" << endl;
    }

    if(r != 1) {
        return -1;
    }
    else {
        if(t < 0) 
            t += n;
        return t;
    }
}

void getModularInverse() {
    int n, a;
    cout << "Ingresa el tamaño del alfabeto 'n': ";
    cin >> n;
    cout << "Ingresa el número 'a': ";
    cin >> a;

    int aux = modularInverse(n, a);
    if(aux == -1) {
        cout << "¡El número " << a << " no tiene inverso multiplicativo! :()" << endl;
    }
    else {
        cout << "Inverso modular es: " << aux << endl;
    }
}


unordered_set<int> getZnStar(int n) {
    unordered_set<int> st;
    for(int i=1; i<n; i++) {
        if(gcd(i, n) == 1) {
            st.insert(i);
        }
    }

    return st;
}

int getDeterminant(int n, vector<vector<int>> matrix) {
    return (((matrix[0][0] * matrix[1][1] - matrix[0][1] * matrix[1][0]) % n) + n) % n;
}

vector<vector<int>> randomMatrix(int n) {
    vector<vector<int>> matrix(2, vector<int>(2));
    for(int i=0; i<2; i++) {
        for(int j=0; j<2; j++) {
            matrix[i][j] = random() % n;
        }
    }
    
    return matrix;
}

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

void showMatrix(vector<vector<int>> K) {
    for(auto row : K) {
        for(auto col : row) {
            cout << col << " ";
        }
        cout << endl;
    }
}

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
    int n = 0, opc, aux, a, det;
    srand(time(0));
    vector<vector<int>> matriz;
    vector<int> primes;
    string s;
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
            /*
                if(n) {
                    cout << "El det tiene que ser un múltiplo de: {";
                    primes = primeFactors(n);
                    for(auto it : primes) {
                        cout << it << ", ";
                    }
                    cout << "}" << endl;
                    do {
                        matriz = randomMatrix();
                        det = calcDeterminante(matriz, n);
                    }
                    while(valida(det, n, primes));
 
                    cout << "Matriz válida: " << endl;
                    for(auto it1 : matriz) {
                        for(auto it2 : it1) {
                            cout << it2 << " ";
                        }
                        cout << endl;
                    }

                    cout << "Determinante: " << det << endl;
                }
                else {
                    cout << "¡Ingresa un valor para n!" << endl;
                }   
                */      
                break;

            case 0:
                f = 0;
                break;

            default:
                cout << ("¡Ingresa una opción válida! :(") << endl;
        }
    }

    return 0;
}

