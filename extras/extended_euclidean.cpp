#include <iostream>
#include <vector>
using namespace std;

vector<int> extended_euclidean(int a, int b) {
    int a0 = a;
    int b0 = b;
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
    cout << "¿Comprobación: sa+tb = r?" << endl;
    cout << s*a + t*b << " = " << r << endl;
    return {r, s, t};
}

int extended_gcd(int a, int b, int& x, int& y) {
    x = 1, y = 0;
    int x1 = 0, y1 = 1, a1 = a, b1 = b;
    while (b1) {
        int q = a1 / b1;
        tie(x, x1) = make_tuple(x1, x - q * x1);
        tie(y, y1) = make_tuple(y1, y - q * y1);
        tie(a1, b1) = make_tuple(b1, a1 - q * b1);
    }
    return a1;
}


int main() {
    int a, b;
    cout << "Ingresa el valor de a: ";
    cin >> a;
    cout << "Ingresa el valor de b: "; 
    cin >> b;

    vector<int> v = extended_euclidean(a, b);
    cout << "r=" << v[0] << ", s=" << v[1] << ", t=" << v[2] << endl;
    return 0;
}

