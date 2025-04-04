#include <iostream>
#include <string>
#include <fstream>
#include <cryptopp/cryptlib.h>
#include <cryptopp/des.h>
#include <cryptopp/modes.h>
#include <cryptopp/filters.h>
#include <cryptopp/secblock.h>
#include <cryptopp/base64.h>

using namespace CryptoPP;

std::string EncodeBase64(const std::string& input) {
    std::string encoded;
    StringSource(input, true, new Base64Encoder(new StringSink(encoded), false));
    return encoded;
}

std::string DecodeBase64(const std::string& input) {
    std::string decoded;
    StringSource(input, true, new Base64Decoder(new StringSink(decoded)));
    return decoded;
}

std::string ReadFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        throw std::runtime_error("No se pudo abrir el archivo");
    }
    return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
}

void WriteFile(const std::string& filename, const std::string& data, bool binary = false) {
    std::ofstream file(filename, binary ? std::ios::binary : std::ios::out);
    if (!file) {
        throw std::runtime_error("No se pudo escribir el archivo");
    }
    file << data;
}

std::string EncryptDES(const std::string& plainText, const SecByteBlock& key, const byte iv[DES::BLOCKSIZE]) {
    std::string cipherText;
    try {
        CBC_Mode<DES>::Encryption encryption;
        encryption.SetKeyWithIV(key, key.size(), iv);

        StringSource(plainText, true,
                     new StreamTransformationFilter(encryption,
                                                    new StringSink(cipherText)));
    } catch (const Exception& e) {
        std::cerr << "Encryption Error: " << e.what() << std::endl;
    }
    return EncodeBase64(cipherText);
}

std::string DecryptDES(const std::string& cipherTextBase64, const SecByteBlock& key, const byte iv[DES::BLOCKSIZE]) {
    std::string cipherText = DecodeBase64(cipherTextBase64);
    std::string recoveredText;
    try {
        CBC_Mode<DES>::Decryption decryption;
        decryption.SetKeyWithIV(key, key.size(), iv);

        StringSource(cipherText, true,
                     new StreamTransformationFilter(decryption,
                                                    new StringSink(recoveredText)));
    } catch (const Exception& e) {
        std::cerr << "Decryption Error: " << e.what() << std::endl;
    }
    return recoveredText;
}

int main() {
    SecByteBlock key(DES::DEFAULT_KEYLENGTH);
    byte iv[DES::BLOCKSIZE] = {0};

    // Inicialización del key e IV (para ejemplo, todo en ceros)
    memset(key, 0x3f, key.size());
    memset(iv, 0xff, DES::BLOCKSIZE);

    std::string keyBase64 = EncodeBase64(std::string(reinterpret_cast<const char*>(key.data()), key.size()));
    std::cout << "Clave en Base64: " << keyBase64 << std::endl;

    try {
        std::string inputFilename = "input.txt";
        std::string encryptedFilename = "encrypted.txt";
        std::string decryptedFilename = "decrypted.txt";

        std::string plainText = ReadFile(inputFilename);
        std::cout << "Texto original leído del archivo: " << plainText << std::endl;

        std::string cipherTextBase64 = EncryptDES(plainText, key, iv);
        WriteFile(encryptedFilename, cipherTextBase64);
        std::cout << "Texto cifrado (Base64) guardado en " << encryptedFilename << std::endl;

        std::string recoveredText = DecryptDES(ReadFile(encryptedFilename), key, iv);
        WriteFile(decryptedFilename, recoveredText, true);
        std::cout << "Texto descifrado guardado en " << decryptedFilename << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}

// g++ des.cpp -o des -std=c++17 -lcryptopp