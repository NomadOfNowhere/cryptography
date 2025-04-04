#include <iostream>
#include <vector>
#include <cryptopp/des.h>
#include <cryptopp/cryptlib.h>    
#include <cryptopp/integer.h>
#include <cryptopp/osrng.h>
#include <cryptopp/base64.h>
#include <cryptopp/files.h>
#include <cryptopp/hex.h>
#include <cryptopp/modes.h>
#include <cryptopp/pkcspad.h>
#include <cryptopp/padlkrng.h>
#include <iomanip>
#include <bitset>
#include <fstream>  // librería para manipular archivos
// g++ lab4-def.cpp -o lab4 -std=c++17 -lcryptopp

using namespace std;

#define bits unsigned short
#define trunc 0x3ff;       // Sólo los 10 bits menos significativos - 0b1111111111
#define leftTrunc 0x3e0    // Parte izquierda - 0b1111100000
#define rightTrunc 0x1f    // Parte derecha -   0b0000011111
#define N 10
#define M 8

// Posiciones de las permutaciones
vector<int> P10 = {3, 5, 2, 7, 4, 10, 1, 9, 8, 6},
            P8 = {6, 3, 7, 4, 8, 5, 10, 9},
            P4 = {2, 4, 3, 1},
            IP = {2, 6, 3, 1, 4, 8, 5, 7},
            invIp = {4, 1, 3, 5, 7, 2, 8, 6},
            EP = {4, 1, 2, 3, 2, 3, 4, 1},

            S0[] = {{1, 0, 3, 2}, 
                    {3, 2, 1, 0},
                    {0, 2, 1, 3},
                    {3, 1, 3, 2}},

            S1[] = {{0, 1, 2, 3},
                    {2, 0, 1, 3},
                    {3, 0, 1, 0},
                    {2, 1, 0, 3}};
string base64Chars;

bits getPermutation(vector<int> permutation, bits value, int size) {
    bits p = 0;
    for(int i=0; i<permutation.size(); i++) {
        if((1 << (size - permutation[i])) & value) {
            p |= (1 << (permutation.size() - 1 - i));
        }
    }
    return p;
}

string showBinNumber(bits b, int sz) {
    string s = "";
    for(int i=sz-1; i>=0; i--) {
        s += (((1 << i) & b) ? '1' : '0');
    }
    return s;
}   

bits getShift(bits value, int shifts) {
    bits left = (((value & leftTrunc) << shifts) | (value >> ((N>>1) - shifts))) & leftTrunc;
    bits right = (((value & rightTrunc) << shifts) | ((value & rightTrunc) >> ((N>>1) - shifts))) & rightTrunc;
    return (left | right) & trunc;
}

vector<bits> getSDESSubkeys(bits key) {
    cout << "Key: " << showBinNumber(key, N) << " - " << key << endl;
    
    // Paso 1: Se aplica P10 a la key
    bits P10_key = getPermutation(P10, key, N);
    
    // Paso 2: P10_key se divide en dos mitades y se les aplica un corrimiento circular de 1 bit
    bits CI_1 = getShift(P10_key, 1);

    // Paso 3: Se aplica P8 al corrimiento circular (CI_1) para obtener la llave 1
    bits K1 = getPermutation(P8, CI_1, N);

    // Paso 4: CI_1 se divide en dos mitades y se le aplica un corrimiento circular de 2 bits
    bits CI_2 = getShift(CI_1, 2);

    // Paso 5: Se aplica P8 al corrimiento circular de 2 bits (CI_2)
    bits K2 = getPermutation(P8, CI_2, N);

    return {K1, K2};
}


// Para cifrar: subkeys = {K1, K2}
// Para descifrar: subkeys = {K2, K1}
bits SDES(vector<bits> subkeys, bits message) {
    // Paso inicial: Se aplica IP al mensaje
    bits ip = getPermutation(IP, message, M);

    // Paso 1: Se divide el mensaje ip en bloques izquierdo y derecho
    bits L = 0, R = 0;
    for(int i=M-1; i>=(M>>1); i--) {    // Mitad izquierda
        L |= ((ip >> (M>>1)) & (1 << (i % (M>>1))));
    }
    for(int i=(M>>1)-1; i>=0; i--) {       // Mitad derecha
        R |= ((1 << i) & ip);
    }

    // Se llevarán a cabo 2 rondas
    for(int i=0; i<2; i++) {
        // Paso 2: Se aplica la permutación de expansión (E/P) a la parte derecha (R)
        bits ep = getPermutation(EP, R, M>>1);

        // Paso 3: Aplicar ep ^ K1 (xor)
        bits ep_xor = ep ^ subkeys[i];

        // Paso 4: Consulta en tablas S0 (parte izq) y S1 (parte der)
        // renglón (primer y último bit), columna (segundo y tercer bit)
        bits s_cajas = 0, row, col;

        row = ((ep_xor >> (M-2)) & (1 << 1)) | ((ep_xor >> (M>>1)) & 1);        // S0 - izq
        col = ((ep_xor >> (M-3)) & (1 << 1)) | ((ep_xor >> ((M>>1) + 1)) & 1);
        s_cajas |= (S0[row][col] << 2);

        row = ((ep_xor >> ((M>>1)-2)) & (1 << 1)) | (1 & ep_xor);               // S1 - der
        col = ((ep_xor >> ((M>>1)-3)) & (1 << 1)) | ((ep_xor >> 1) & 1);
        s_cajas |= S1[row][col];

        // Paso 5: Le aplicamos P4 a s_cajas
        bits P4_key = getPermutation(P4, s_cajas, M>>1);

        // Paso 6: Aplicar L ^ P4_key
        L ^= P4_key;

        // Paso 7: Se intercambian bloques para la ronda 2
        if(!i) swap(L, R);
    }

    return getPermutation(invIp, (L<<(M>>1))|R, M);
}

// Llenamos la cadena con los 64 caracteres: A-Z, a-z, 0-9, +, /
void preCalcBase64() {
    char c[] = {'A', 'a', '0', 'Z', 'z', '9'};
    for(int i=0; i<3; i++) {
        for(char j=c[i]; j<=c[i+3]; j++) {
            base64Chars.push_back(j);
        }
    }
    base64Chars += "+/";
    cout << base64Chars << endl;
}

// Función para imprimir en formato hexadecimal
void printHex(const string& label, const CryptoPP::SecByteBlock& data) {
    cout << label;
    for (size_t i = 0; i < data.size(); i++) {
        cout << hex << setw(2) << setfill('0') << (int)data[i];
    }
    cout << endl;
}

// Codificar a base64
string base64Encode(string input) {
    int val = 0, valb = -6;
    string encoded;

    for(unsigned char c: input) {
        val = (val << 8) + c;
        valb += 8;
        while(valb >= 0) {
            encoded.push_back(base64Chars[(val >> valb) & 0x3f]);
            valb -= 6;
        }
    }
    if(valb > -6) {
        encoded.push_back(base64Chars[((val<<8) >> (valb+8)) & 0x3f]);
    }
    while(encoded.size() % 4) {
        encoded.push_back('=');
    }

    return encoded;
}

// Decodificar de base64
string base64Decode(string input) {
    int val = 0, valb = -8;
    string decode;
    vector<int> T(256, -1);

    for(int i=0; i<64; i++) {
        T[base64Chars[i]] = i;
    }

    for(unsigned char c : input) {
        if(T[c] == -1) 
            break;
        
        val = (val << 6) + T[c];
        valb += 6;

        if(valb >= 0) {
            decode.push_back(char((val >> valb) & 0xff));
            valb -= 8;
        }
    }

    return decode;
}

void DES() {

}

// Función para generar una clave DES segura y codificarla en Base64
string getDESRandomKey(CryptoPP::SecByteBlock &iv) {
    // Crear un generador de números pseudo aleatorios
    CryptoPP::AutoSeededRandomPool prng;

    // Crear una clave DES con un bloque de bytes aleatorio
    CryptoPP::SecByteBlock key(CryptoPP::DES::DEFAULT_KEYLENGTH);
    prng.GenerateBlock(key, key.size());

    // Generar vector de inicialización (IV) aleatorio para modo CBC
    prng.GenerateBlock(iv, iv.size());

    // Convertir la clave en una cadena binaria (sin caracteres nulos)
    string stringKey(reinterpret_cast<const char*>(key.data()), key.size());

    // Codificar la clave DES a Base64
    // ¡* Usando crypto++ *! CryptoPP::StringSource(key, key.size(), true, new CryptoPP::Base64Encoder(new CryptoPP::StringSink(encodedKey), false));
    return base64Encode(stringKey);    
}

void DESDecryptFile(string keyBase64, CryptoPP::SecByteBlock iv) {
    string key = base64Decode(keyBase64), filename, decryptedText;
    CryptoPP::AutoSeededRandomPool prng;

    if (key.size() != CryptoPP::DES::DEFAULT_KEYLENGTH) {
        cout << "¡Clave inválida!" << endl;
        return;
    }

    cout << "Ingresa el nombre del archivo cifrado (base64): ";
    cin >> filename;
    ifstream inputFile(filename, ios::binary);

    if (!inputFile) {
        cout << "¡No se encontró el archivo! :(" << endl;
        return;
    }

    // Leer el texto cifrado en base64
    string base64Ciphertext((istreambuf_iterator<char>(inputFile)), istreambuf_iterator<char>());
    inputFile.close();

    // Decodificar base64 al texto cifrado binario
    string decodedCiphertext;
    CryptoPP::StringSource(base64Ciphertext, true, new CryptoPP::Base64Decoder(new CryptoPP::StringSink(decodedCiphertext)));

    // Convertir a SecByteBlock
    CryptoPP::SecByteBlock ciphertext((const CryptoPP::byte*)decodedCiphertext.data(), decodedCiphertext.size());

    // Verificar que el tamaño del texto cifrado sea múltiplo de 8 (tamaño de bloque de DES)
    if (ciphertext.size() % CryptoPP::DES::BLOCKSIZE != 0) {
        cout << "El texto cifrado no tiene un tamaño adecuado para DES." << endl;
        return;
    }

    // Configurar el descifrado en modo CBC
    CryptoPP::CBC_Mode<CryptoPP::DES>::Decryption decryptor;
    decryptor.SetKeyWithIV((const CryptoPP::byte*)key.data(), key.size(), iv);

    // Variables para almacenar el texto descifrado
    CryptoPP::StringSink decryptedSink(decryptedText);

    // Usar StreamTransformationFilter para descifrar
    CryptoPP::StreamTransformationFilter decryptorStream(
        decryptor, new CryptoPP::Redirector(decryptedSink), CryptoPP::StreamTransformationFilter::PKCS_PADDING
    );

    // Procesar el texto cifrado
    decryptorStream.Put(ciphertext, ciphertext.size());
    decryptorStream.MessageEnd();

    // Mostrar el texto descifrado
    cout << "Texto descifrado: " << decryptedText << endl;

    // Guardar el texto descifrado en un archivo
    ofstream outputFile("decrypted.txt");
    outputFile << decryptedText;
    outputFile.close();
}

void DESEncryptFile(string keyBase64, CryptoPP::SecByteBlock iv) {
    string key = base64Decode(keyBase64), filename, plainText;

    if (key.size() != CryptoPP::DES::DEFAULT_KEYLENGTH) {
        cout << "¡Clave inválida!" << endl;
        return;
    }

    cout << "Ingresa el nombre del archivo: ";
    cin >> filename;
    ifstream inputFile(filename, ios::binary);

    if (!inputFile) {
        cout << "¡No se encontró el archivo! :(" << endl;
        return;
    }

    // Lectura del texto plano
    plainText.assign((istreambuf_iterator<char>(inputFile)), istreambuf_iterator<char>());
    CryptoPP::SecByteBlock input((const CryptoPP::byte*)plainText.data(), plainText.size());
    inputFile.close();

    // Cifrado en modo CBC
    CryptoPP::CBC_Mode<CryptoPP::DES>::Encryption encryptor;
    encryptor.SetKeyWithIV((const CryptoPP::byte*)key.data(), key.size(), iv);

    // Agregar padding
    CryptoPP::SecByteBlock ciphertext(input.size() + CryptoPP::DES::BLOCKSIZE);
    CryptoPP::StreamTransformationFilter encryptorStream(
        encryptor, new CryptoPP::ArraySink(ciphertext, ciphertext.size()),
        CryptoPP::StreamTransformationFilter::PKCS_PADDING
    );

    encryptorStream.Put(input, input.size());
    encryptorStream.MessageEnd();

    // Codificar el texto cifrado en base64
    string cipherTextString(reinterpret_cast<const char*>(ciphertext.data()), ciphertext.size());
    CryptoPP::StringSource(ciphertext, ciphertext.size(), true, new CryptoPP::Base64Encoder(new CryptoPP::FileSink("encrypted_base64.txt"), false));

    cout << "Archivo cifrado guardado como 'encrypted_base64.txt'." << endl;
}

void DESEncryptFile2(string keyBase64, CryptoPP::SecByteBlock iv) {
    string key = base64Decode(keyBase64), filename, plainText;

    if(key.size() != CryptoPP::DES::DEFAULT_KEYLENGTH) {
        cout << "¡Clave inválida!" << endl;
    }

    cout << "Ingresa el nombre del archivo: ";
    cin >> filename;
    ifstream inputFile(filename, ios::binary);

    if(!inputFile) {
        cout << "¡No se encontró el archivo! :(" << endl;
        return;
    }

    // Lectura del texto plano
    plainText.assign((istreambuf_iterator<char>(inputFile)), istreambuf_iterator<char>());
    CryptoPP::SecByteBlock input((const CryptoPP::byte*)plainText.data(), plainText.size());
    inputFile.close();

    // Cifrado en modo CBC
    CryptoPP::CBC_Mode<CryptoPP::DES>::Encryption encryptor;
    encryptor.SetKeyWithIV((const CryptoPP::byte*)key.data(), key.size(), iv);

    // Agregar padding
    CryptoPP::SecByteBlock ciphertext(input.size() + CryptoPP::DES::BLOCKSIZE);
    CryptoPP::StreamTransformationFilter encryptorStream(encryptor, new CryptoPP::ArraySink(ciphertext, ciphertext.size()), 
                                                         CryptoPP::StreamTransformationFilter::PKCS_PADDING);
    encryptorStream.Put(input, input.size());
    encryptorStream.MessageEnd();

    string cipherTextString(reinterpret_cast<const char*>(ciphertext.data()), ciphertext.size());


    ofstream outputFile("prueba.txt"); // Abrir el archivo para escribir
    if(!outputFile) {
        cout << "¡No se pudo abrir el archivo! :(" << endl;
        return;
    }
    
    // string base64CipherText = base64Encode(cipherTextString);
    // int idx = 0;
    // while(idx < base64CipherText.length()) {
    //     outputFile << base64CipherText[idx++];
    //     if(!(idx % 64)) {
    //         outputFile << endl;
    //     }
    // }
    // outputFile << endl;

    CryptoPP::StringSource(ciphertext, ciphertext.size(), true, new CryptoPP::Base64Encoder(new CryptoPP::FileSink(outputFile), false));
    outputFile.close();
}

void DESDecryptFile2(string keyBase64, CryptoPP::SecByteBlock iv) {
    string key = base64Decode(keyBase64), filename, plainText;

    if(key.size() != CryptoPP::DES::DEFAULT_KEYLENGTH) {
        cout << "¡Clave inválida!" << endl;
    }

    cout << "Ingresa el nombre del archivo: ";
    cin >> filename;
    ifstream inputFile(filename, ios::binary);

    if(!inputFile) {
        cout << "¡No se encontró el archivo! :(" << endl;
        return;
    }

    // Lectura del texto plano
    plainText.assign((istreambuf_iterator<char>(inputFile)), istreambuf_iterator<char>());
    CryptoPP::SecByteBlock input((const CryptoPP::byte*)plainText.data(), plainText.size());
    inputFile.close();

    // Configurar el descifrado en modo CBC
    CryptoPP::CBC_Mode<CryptoPP::DES>::Decryption decryptor;
    decryptor.SetKeyWithIV((const CryptoPP::byte*)key.data(), key.size(), iv);

    // Variables para almacenar el texto descifrado
    std::string decryptedText;
    CryptoPP::StringSink decryptedSink(decryptedText);

    // Usar StreamTransformationFilter para descifrar
    CryptoPP::StreamTransformationFilter decryptorStream(
        decryptor, 
        new CryptoPP::Redirector(decryptedSink), 
        CryptoPP::StreamTransformationFilter::PKCS_PADDING // El padding usado durante el cifrado
    );


    // Procesar el texto cifrado
    decryptorStream.Put(input, input.size());
    decryptorStream.MessageEnd();

    // Mostrar el texto descifrado
    std::cout << "Texto descifrado: " << decryptedText << std::endl;

    ofstream outputFile("ueba.txt"); // Abrir el archivo para escribir
    if(!outputFile) {
        cout << "¡No se pudo abrir el archivo! :(" << endl;
        return;
    }
}





int main() {
    // Declaración de variables
    // Ejemplo de prueba en clase:
    // key libro = 642,     m = 189
    // key clase = 992      m = 65
    int opc;
    bits key;
    string m, c, k;
    bool f = 1;
    vector<bits> subkeys;
    CryptoPP::SecByteBlock iv(CryptoPP::DES::BLOCKSIZE);
    preCalcBase64();

    int cnt = 0;
    // Menú
    while(f) {
        cout << "* * * SDES * * *" << endl;
        cout << "[1] Generar llaves de ronda o subllaves." << endl;
        cout << "[2] SDES encrypt." << endl;
        cout << "[3] SDES decrypt." << endl;
        cout << "[4] Base64 encode." << endl;
        cout << "[5] Base64 decode." << endl;
        cout << "[6] DES." << endl;
        cout << "[7] Encrypt file." << endl;
        cout << "[0] Salir" << endl;
        cout << "Selecciona una opción: ";
        cin >> opc;
        cout << endl;

        switch(opc) {
            case 1:
                cout << "Ingresa el valor: ";
                cin >> key;
                subkeys = getSDESSubkeys(key);

                for(int i=0; i<2; i++) {
                    cout << "K" << i+1 << ": " << showBinNumber(subkeys[i], M) << " - " << subkeys[i] << endl;
                }cout << endl;

                break;
            case 2:
                if(!subkeys[0] || !subkeys[1]) {
                    cout << "¡Primero genera las subllaves! >:C" << endl;
                }
                else {
                    cout << "Ingresa la cadena a cifrar: ";
                    c = m = "";
                    cin.ignore(); 
                    getline(cin, m);

                    for(auto it : m) {
                        c.push_back(char(SDES(subkeys, it)));
                    }
                    cout << "El mensaje cifrado es: " << base64Encode(c) << endl;
                }
                break;
            case 3:
                if(!subkeys[0] || !subkeys[1]) {
                    cout << "¡Primero genera las subllaves! >:C" << endl;
                }
                else {
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
                    
                }
                break;

            case 4:
                cout << "Ingresa el mensaje codificar: ";
                cin >> m;
                cout << "El mensaje codificado es: " << base64Encode(m) << endl;
                break;
            
            case 5:
                cout << "Ingresa el mensaje a decodificar: ";
                cin >> m;
                cout << "El mensaje decodificado es: " << base64Decode(m) << endl;
                break;
            
            case 6:
                cout << getDESRandomKey(iv) << endl;
                    
                
                // cout << "Key: " << m << endl;
                // cout << "Decoding: " << base64Decode(m) << endl;
                break;
                
            case 7:
                cout << "Ingresa la key: ";
                cin >> k;
                DESEncryptFile(k, iv);
                break;
            case 8:
                cout << "Ingresa la key: ";
                cin >> k;
                DESDecryptFile(k, iv);
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

