#include "crypro_functions.h"

#include <string.h>
#include <iostream>

#include <openssl/sha.h>
#include <openssl/evp.h>
#include <openssl/conf.h>
#include <openssl/evperr.h>

#include <QFile>
#include <QBuffer>
#include <QCryptographicHash>
#include <QRandomGenerator>

QByteArray random(unsigned len)
{
    srand(time(0));

    QByteArray res;

    for (size_t i = 0; i < len; ++i)
    {
        res.append(rand() % UINT8_MAX);
    }

    return res;
}


void sha256_hash_string (unsigned char hash[SHA256_DIGEST_LENGTH], char outputBuffer[65])
{
    int i = 0;

    for(i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        sprintf(outputBuffer + (i * 2), "%02x", hash[i]);
    }

    outputBuffer[64] = 0;
}


void sha256(char *string, char outputBuffer[65])
{
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, string, strlen(string));
    SHA256_Final(hash, &sha256);
    int i = 0;
    for(i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        sprintf(outputBuffer + (i * 2), "%02x", hash[i]);
    }
    outputBuffer[64] = 0;
}

QByteArray sha256(const QString& string)
{
    auto bytesStr = string.toLatin1();
    unsigned char hash[SHA256_DIGEST_LENGTH];

    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, bytesStr.data(), bytesStr.size());
    SHA256_Final(hash, &sha256);

    return QByteArray((const char*)hash, SHA256_DIGEST_LENGTH);
}

int sha256_file(char *path, char outputBuffer[65])
{
    FILE *file = fopen(path, "rb");
    if(!file) return -534;

    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    const int bufSize = 32768;
    unsigned char *buffer = (unsigned char *)malloc(bufSize);
    int bytesRead = 0;
    if(!buffer) return ENOMEM;
    while((bytesRead = fread(buffer, 1, bufSize, file)))
    {
        SHA256_Update(&sha256, buffer, bytesRead);
    }
    SHA256_Final(hash, &sha256);

    sha256_hash_string(hash, outputBuffer);
    fclose(file);
    free(buffer);
    return 0;
}

QByteArray aes256cbc(QByteArray encrypted, const QByteArray &key, const char initv[16])
{
    QByteArray buffer;
    auto decoded_buf = QByteArray::fromBase64(encrypted);
    QBuffer buffer_stream(&decoded_buf);
    buffer_stream.open(QBuffer::ReadOnly);

    EVP_CIPHER_CTX *ctx;
    if(!(ctx = EVP_CIPHER_CTX_new()))
    {
        return nullptr;
    }
    if(1 != EVP_DecryptInit_ex(ctx,
                               EVP_aes_256_cbc(),
                               NULL,
                               (unsigned char *)key.data(),
                               (unsigned char *)initv))
    {
        return nullptr;
    }

    int read_len = 0, decrypted_len = 0;

    while(true)
    {
        char encrypted_buf[256] = {0};
        unsigned char decrypted_buf[512]= {0};

        read_len = buffer_stream.read(encrypted.data(), 256);


        if(!EVP_DecryptUpdate(ctx,
                                  decrypted_buf,
                                  &decrypted_len,
                                  (unsigned char *)encrypted_buf,
                                  read_len))
        {
            return nullptr;
        }
        if(read_len < 256)
        {
            int tmplen;
            if (!EVP_DecryptFinal_ex(ctx, decrypted_buf + decrypted_len, &tmplen)) {
                EVP_CIPHER_CTX_free(ctx);
                return 0;
            }
            decrypted_len += tmplen;
            buffer += QByteArray((char*)decrypted_buf, decrypted_len);
            break;
        }
        else {

            buffer += QByteArray((char*)decrypted_buf + decrypted_len);
        }

    }

    EVP_CIPHER_CTX_free(ctx);

    buffer_stream.close();

    return buffer;
}

int decrypt_aes_256(unsigned char *input, unsigned size,
                    unsigned char *key,
                    unsigned char *iv, unsigned char *output)
{
    int session, total;
    auto ctx = EVP_CIPHER_CTX_new();
    EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key, iv);
    EVP_DecryptUpdate(ctx, output, &session, input, size);
    total = session;
    EVP_DecryptFinal_ex(ctx, output + session, &session);
    total += session;
    EVP_CIPHER_CTX_free(ctx);
    return total;
}

int encrypt_aes_256(unsigned char *input, unsigned size,
                    unsigned char *key,
                    unsigned char *iv, unsigned char *output)
{
    int session, total;
    auto ctx = EVP_CIPHER_CTX_new();
    EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key, iv);
    EVP_EncryptUpdate(ctx, output, &session, input, size);
    total = session;
    EVP_EncryptFinal_ex(ctx, output + session, &session);
    total += session;
    EVP_CIPHER_CTX_free(ctx);
    return total;
}

QByteArray decrypt256cbc(QByteArray& encyptData, const QByteArray& key, unsigned char iv[16])
{
    encyptData = QByteArray::fromBase64(encyptData, QByteArray::OmitTrailingEquals);

    QByteArray decryptData(encyptData.size() * 2, ' ');

    auto n = decrypt_aes_256((unsigned char *) encyptData.data(), encyptData.size(),
                             (unsigned char *) key.data(), iv,
                             (unsigned char *) decryptData.data());

    decryptData.resize(n);

    return decryptData;
}

QByteArray encrypt256cbc(QByteArray& decryptData, const QByteArray& key, unsigned char iv[16])
{
    QByteArray encyptData;
    encyptData.reserve(decryptData.size() * 2);

    auto n = encrypt_aes_256((unsigned char *) decryptData.data(), decryptData.size(),
                             (unsigned char *) key.data(), iv,
                             (unsigned char *) encyptData.data());

    encyptData.resize(n);

    return encyptData.toBase64(QByteArray::OmitTrailingEquals);
}

void decrypt256cbc(const QString& fileNameEnc, const QString& fileNameDec,
                   const QByteArray& key, unsigned char iv[16])
{
    QFile file(fileNameEnc);
    if(!  file.open(QFile::ReadOnly))
    {
        return;
    }

    QFile fileOut(fileNameDec);
    if(!  file.open(QFile::WriteOnly))
    {
         file.close();
         return;
    }

    auto dataEncrypt = file.readAll();
    auto dataDec = decrypt256cbc(dataEncrypt, key, iv);

    fileOut.write(dataDec);
    fileOut.close();
    file.close();
}

void encrypt256cbc(const QString& fileNameDec, const QString& fileNameEnc,
             const QByteArray& key, unsigned char iv[16])
{
    QFile file(fileNameDec);
    if(!file.open(QFile::ReadOnly))
    {
        return;
    }

    QFile fileOut(fileNameEnc);
    if(!fileOut.open(QFile::WriteOnly))
    {
         file.close();
         return;
    }

    auto dataDecrypt = file.readAll();
    auto dataEnc = encrypt256cbc(dataDecrypt, key, iv);

    fileOut.write(dataEnc);
    fileOut.close();
    file.close();
}

