#ifndef tripleDES_H
#define tripleDES_H

#include <iostream>
#include <fstream>
#include "Base64DecoderEncoder.h"
#include "tripleDES.h"
#include <cryptopp/des.h>
#include <cryptopp/osrng.h>
#include <cryptopp/modes.h>

using namespace std;

// CryptoPP::SecByteBlock iv

// string getDESRandomKey();
string get3DESRandomKey();
void tripleDESEncryptFile(string keyBase64);
void tripleDESDecryptFile(string keyBase64);

#endif // tripleDES_H