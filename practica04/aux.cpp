#include <iostream>
#include <tuple>
using namespace std;

tuple<int, int, int> extendedGcd(int a, int b){
    if(b == 0){
    if(a > 0) return {a, 1, 0};
        else return {-a, -1, 0};
    }
    else{
    auto[d, x, y] = extendedGcd(b, a%b);
        return {d, y, x - y*(a/b)};
    }
}

 int modularInverse(int a, int m){
    auto[d, x, y] = extendedGcd(a, m);
    if(d != 1) return -1; // inverse doesn’t exist
    
        return x;
 }

int main() {
    cout << modularInverse(28, 75) << endl;
    return 0;
}