#ifndef DES_H
#define DES_H

#include <iostream>
#include <vector>
#include <cryptopp/cryptlib.h>
#include <cryptopp/osrng.h>
#include <cryptopp/des.h>
#include <cryptopp/integer.h>
#include <cryptopp/osrng.h>
#include <cryptopp/base64.h>
#include <cryptopp/files.h>
#include <cryptopp/hex.h>
#include <cryptopp/modes.h>
#include <cryptopp/pkcspad.h>
#include <cryptopp/padlkrng.h>
using namespace std;

// CryptoPP::SecByteBlock iv

// string getDESRandomKey();
string getDESRandomKey();
void DESEncryptFile(string keyBase64);
void DESDecryptFile(string keyBase64);

#endif // DES_H