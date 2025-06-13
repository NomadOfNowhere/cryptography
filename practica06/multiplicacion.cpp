#include <iostream>
#include <vector>
using namespace std;

long long msb(unsigned long long x) {
    return 63 - __builtin_clzll(x);
}

string showPolynomialBase(unsigned long long x, int end) {
    string base = "";
    for(int i=end; i>=0; i--) {
        if(x & (1LL << i)) {
            base += (i > 1 ? "x^"+to_string(i)+" + " : i ? "x + " : "1");
        }
    }
    if(base.back() == ' ') {
        for(int i=0; i<3; i++) {
            base.pop_back();
        }
    }

    return base;
}

string showBinBase(unsigned long long x, int end) {
    string base = "";
    for(int i=end + 3 - (end%4); i>=0; i--) {
        base += (x & (1LL << i) ? '1' : '0');
        if(!(i % 4)) {
            base += ' ';
        }
    }

    return base;
}

void debug(unsigned long long p, unsigned long long q, unsigned long long m, 
           unsigned long long mxMod, unsigned long long res, int end) {

    cout << endl;
    cout << "p: " << showPolynomialBase(p, end) << endl;
    cout << showBinBase(p, end+1) << endl << endl;

    cout << "q: " << showPolynomialBase(q, end) << endl;
    cout << showBinBase(q, end+1) << endl << endl;

    cout << "m: " << showPolynomialBase(m, end+1) << endl;
    cout << showBinBase(m, end+1) << endl << endl;

    cout << "max mod: " << showPolynomialBase(mxMod, end) << endl;
    cout << showBinBase(mxMod, end+1) << endl << endl;

    cout << "Resultado es: " << endl;
    cout << showPolynomialBase(res, end) << endl;
    cout << showBinBase(res, end+1) << endl << endl;
    cout << res << endl;
}

int main() {
    unsigned long long p, q, m, n, trunc, mxMod, idx, end;
    // p = 0x103, q = 0x62, n = 13, m = 0x201b;
    // p = 0xf0000001, q = 0xf, n = 32, m = 0x10000008d;
    // p = 0x2, q = 0xbb, n = 8, m = 0x11b;
    // p = 0x7, q = 0xd, n = 5, m = 0x25;
    // p = 0xfffffffffffffff, q = 0x3fffffff, n = 61, m =  0x2000000000000000;

    cout << "p(x) · q(x) mod m(x)" << endl;
    cout << "Ingresa el tamaño de GF(2^n): ";
    cin >> n;

    cout << "Ingresa polinomio p(x): ";
    cin >> hex >> p;

    cout << "Ingresa polinomio q(x): ";
    cin >> hex >> q;

    cout << "Ingresa polinomio irreducible m(x): ";
    cin >> hex >> m;

    trunc = (1LL << n) - 1;
    mxMod = m ^ (1LL << n);
    
    idx = msb(p), end = n - 1;
    vector<unsigned long long> dp(idx+1);

    dp[0] = q;
    for(int i=1; i<=idx; i++) {
        dp[i] = (dp[i-1] << 1);
        dp[i] &= trunc;
        if(dp[i-1] & (1LL << end)) {
            dp[i] ^= mxMod;
            dp[i] &= trunc;
        }
    }

    unsigned long long res = 0;
    for(int i=idx; i>=0; i--) {
        if(!((1LL << i) & p)) {
            continue;
        }
        res ^= dp[i];
    }

    cout << endl;
    cout << "Datos: ";
    cout << "p(x): " << showPolynomialBase(p, end) << endl;
    cout << "q(x): " << showPolynomialBase(q, end) << endl;
    cout << "m(x): " << showPolynomialBase(m, end+1) << endl;

    cout << "\nEl resultado de p(x)·q(x) mod m(x) en GF(2^" << n << ") es: " << endl;
    cout << "Base polinomial:     " << showPolynomialBase(res, end) << endl;
    cout << "Base binaria:        0b " << showBinBase(res, end) << endl;
    cout << "Base hexadecimal:    0x" << hex << res << endl;

    return 0;
}
