#include <iostream>
#include <vector>
#include <fstream>
#include <iomanip> 
using namespace std;

// x^8 + x^4 + x^3 + x + 1   =    1 0001 1011
uint MOD = 0x11b;
int msb(uint x) { return 31 - __builtin_clz(x); }

uint GF2nMultiplication(int n, uint p, uint q) {
    uint mxMod, idx, end, trunc;
    trunc = (1 << n) - 1;
    mxMod = MOD ^ (1 << n);
    
    idx = msb(p), end = n - 1;
    vector<uint> dp(idx+1);

    dp[0] = q;
    for(int i=1; i<=idx; i++) {
        dp[i] = (dp[i-1] << 1);
        dp[i] &= trunc;
        if(dp[i-1] & (1 << end)) {
            dp[i] ^= mxMod;
            dp[i] &= trunc;
        }
    }

    uint res = 0;
    for(int i=idx; i>=0; i--) {
        if(!((1 << i) & p)) {
            continue;
        }
        res ^= dp[i];
    }

    return res;
}

uint table[0xff+1][0xff+1];
void GF2nMultiTable() {
    for(int i=1; i<=0xff; i++) {
        for(int j=1; j<=0xff; j++) {
            table[i][j] = GF2nMultiplication(8, i, j);
        }
    }

    std::ofstream file("out.txt");
    for(int i=1; i<=0xff; i++) {
        for(int j=1; j<=0xff; j++) {
            file << hex << uppercase << setfill('0') << setw(2) << table[i][j] << " ";
        }
        file << endl;
    }
    file.close();
}

int multiplicativeInverse(uint a) {
    uint m = 0x11b;
    for(int i=1; i<=0xff; i++) {
        if(GF2nMultiplication(8, a, table[a][i]) == 1) {
            return table[a][i];
        }
    }
    return -1;
}

void matrix_multiplication(uint (*p)[4], uint (*q)[4]) {
    uint res[4][4] {};
    for(int row=0; row<4; row++) {
        for(int col=0; col<4; col++) {
            for(int k=0; k<4; k++) {
                res[row][col] ^= GF2nMultiplication(8, p[row][k], q[k][col]);
            }
        }
    }

    for(int i=0; i<4; i++) {
        for(int j=0; j<4; j++) {
            cout << hex << uppercase << setfill('0') << setw(2) << res[i][j] << " ";
        }
        cout << endl;
    }
}

int main() {
    char opc;
    uint a, inv_a, n, p, q, x;
    bool f = 1;

    uint pp[4][4] = {
        {0x02, 0x03, 0x01, 0x01},
        {0x01, 0x02, 0x03, 0x01},
        {0x01, 0x01, 0x02, 0x03},
        {0x03, 0x01, 0x01, 0x02},
    };
    uint qq[4][4] = {
        {0x0e, 0x0b, 0x0d, 0x09},
        {0x09, 0x0e, 0x0b, 0x0d},
        {0x0d, 0x09, 0x0e, 0x0b},
        {0x0b, 0x0d, 0x09, 0x0e},
    };

    while(f) {
        cout << "* * * MULTIPLICATION IN GF(2^N) part 2 * * *" << endl;
        cout << "[1] GF(2^n) multiplication. " << endl;
        cout << "[2] Generate multiplication table." << endl;
        cout << "[3] Multiplicative inverse." << endl;
        cout << "[4] Matrix multiplication check. " << endl;
        cout << "[0] Salir." << endl;
        cout << "Select an option: ";
        cin >> opc;

        switch(opc) {
            case '1':
                cout << "Ingresa el valor de GF(2^n): ";
                cin >> n;
                cout << "Ingresa el valor de m(x): ";
                cin >> hex >> MOD;
                cout << "Ingresa el valor de p(x): ";
                cin >> hex >> p;
                cout << "Ingresa el valor de q(x): ";
                cin >> hex >> q;
                x = GF2nMultiplication(n, p, q);
                cout << "p(x) * q(x) = " << hex << x << endl;
                break;
            case '2':
                GF2nMultiTable();
                break;
            case '3':
                cout << "Ingresa el valor de a (hex): ";
                cin >> hex >> a;
                inv_a = multiplicativeInverse(a);
                if(inv_a == -1) {
                    cout << "¡El número no tiene inverso!" << endl;
                }
                else {
                    cout << "a^(-1) = " << hex << inv_a << endl;
                    cout << "a * a^(-1) mod m(x) = " << GF2nMultiplication(8, a, inv_a) << endl;
                }
                break;
            case '4':
                matrix_multiplication(pp, qq);
                break;
            case '0':
                f = 0;
                break;
            default:
                cout << "¡Select a valid option! :(!" << endl;
        }
        cout << endl;
    }

    return 0;
}


