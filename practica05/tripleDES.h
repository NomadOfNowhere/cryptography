#ifndef tripleDES_H
#define tripleDES_H

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
string get3DESRandomKey();
void tripleDESEncryptFile(string keyBase64);
void tripleDESDecryptFile(string keyBase64);

#endif // tripleDES_H