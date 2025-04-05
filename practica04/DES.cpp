#include <iostream>
#include <fstream>
#include "Base64DecoderEncoder.h"
#include "DES.h"
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
using namespace std;

// Función para generar una clave DES segura y codificarla en Base64
string getDESRandomKey() {
    // Crear un generador de números pseudo aleatorios
    CryptoPP::AutoSeededRandomPool prng;

    // Crear una clave DES con un bloque de bytes aleatorio
    CryptoPP::SecByteBlock key(CryptoPP::DES::DEFAULT_KEYLENGTH);
    prng.GenerateBlock(key, key.size());

    // Generar vector de inicialización (IV) aleatorio para modo CBC
    // prng.GenerateBlock(iv, iv.size());

    // Convertir la clave en una cadena binaria (sin caracteres nulos)
    string stringKey(reinterpret_cast<const char*>(key.data()), key.size());

    // Codificar la clave DES a Base64
    // ¡* Usando crypto++ *! CryptoPP::StringSource(key, key.size(), true, new CryptoPP::Base64Encoder(new CryptoPP::StringSink(encodedKey), false));
    return base64Encode(stringKey);
}

void DESEncryptFile(string keyBase64) {
    string key = base64Decode(keyBase64), filename, plainText;

    // Si la clave ingrsada es diferente del tamaño requerido de la llave
    if(key.size() != CryptoPP::DES::DEFAULT_KEYLENGTH) {
        cout << "¡Clave inválida!" << endl;
        return;
    }

    // Abrir archivo
    cout << "Ingresa el nombre del archivo: ";
    cin >> filename;
    ifstream inputFile(filename, ios::binary);

    if(!inputFile) {
        cout << "¡No se encontró el archivo! :(" << endl;
        return;
    }

    // Lectura del texto plano
    plainText.assign((istreambuf_iterator<char>(inputFile)), istreambuf_iterator<char>());
    inputFile.close();

    // Creación de vector de inicialización
    CryptoPP::SecByteBlock iv(CryptoPP::DES::BLOCKSIZE);
    memset(iv, 0x00, iv.size());

    // Configurar el cifrado en modo CBC
    CryptoPP::CBC_Mode<CryptoPP::DES>::Encryption encryptor;
    encryptor.SetKeyWithIV((const CryptoPP::byte*)key.data(), key.size(), iv);

    // Cifrar texto
    string cipherText;
    CryptoPP::StringSource(plainText, true,
                     new CryptoPP::StreamTransformationFilter(encryptor,
                                                    new CryptoPP::StringSink(cipherText)));   
    
    // Generamos el nuevo nombre del archivo
    string newName = filename.substr(0, filename.find_last_of('.')) + "$encrypted" + filename.substr(filename.find_last_of('.'));

    // Renombrar el archivo
    if (rename(filename.c_str(),newName.c_str()) == 0) {
        cout << "Archivo cifrado con éxito como: " << newName << endl;
    } else {
        cout << "¡Error al renombrar el archivo!" << endl;
        return;
    }

    // Escribir en archivo
    ofstream outputFile(newName);
    if(!outputFile) {
        cout << "¡No se pudo abrir el archivo! :(" << endl;
        return;
    }
    outputFile << base64Encode(cipherText);
    outputFile.close();
}

void DESDecryptFile(string keyBase64) {
    string key = base64Decode(keyBase64), filename, cipherTextBase64, cipherText;

    if(key.size() != CryptoPP::DES::DEFAULT_KEYLENGTH) {
        cout << "¡Clave inválida!" << endl;
        return;
    }

    // Abrir archivo
    cout << "Ingresa el nombre del archivo: ";
    cin >> filename;
    ifstream inputFile(filename, ios::binary);

    if(!inputFile) {
        cout << "¡No se encontró el archivo! :(" << endl;
        return;
    }

    // Lectura del texto plano
    cipherTextBase64.assign((istreambuf_iterator<char>(inputFile)), istreambuf_iterator<char>());
    cipherText = base64Decode(cipherTextBase64);
    CryptoPP::SecByteBlock input((const CryptoPP::byte*)cipherText.data(), cipherText.size());
    inputFile.close();

    // Creación de vector de inicialización
    CryptoPP::SecByteBlock iv(CryptoPP::DES::BLOCKSIZE);
    memset(iv, 0x00, iv.size());

    // Configurar el descifrado en modo CBC
    CryptoPP::CBC_Mode<CryptoPP::DES>::Decryption decryptor;
    decryptor.SetKeyWithIV((const CryptoPP::byte*)key.data(), key.size(), iv);

    // Cifrar texto
    string text;
    CryptoPP::StringSource(cipherText, true,
                     new CryptoPP::StreamTransformationFilter(decryptor,
                                                    new CryptoPP::StringSink(text)));

    // Generar el nuevo nombre del archivo
    string newName = filename.substr(0, filename.find_last_of('$')) + filename.substr(filename.find_last_of('.'));

    // Renombrar el archivo
    if (rename(filename.c_str(),newName.c_str()) == 0) {
        cout << "Archivo descifrado con éxito como: " << newName << endl;
    } else {
        cout << "¡Error al renombrar el archivo!" << endl;
        return;
    }
    // Escribir en archivo
    ofstream outputFile(newName);
    if(!outputFile) {
        cout << "¡No se pudo abrir el archivo! :(" << endl;
        return;
    }

    outputFile << text << endl;
}

