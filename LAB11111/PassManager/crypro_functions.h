#ifndef CRYPRO_FUNCTIONS_H
#define CRYPRO_FUNCTIONS_H

#include <QObject>

#define KEY_SIZE 32

QByteArray sha256(const QString& string);
QByteArray aes256cbc(QByteArray encrypted, const QByteArray &key, const char initv[16]);

QByteArray encrypt256cbc(QByteArray& decryptData, const QByteArray& key, unsigned char iv[16]);
void encrypt256cbc(const QString& fileNameDec, const QString& fileNameEnc,
                   const QByteArray& key, unsigned char iv[16]);
QByteArray decrypt256cbc(QByteArray& encyptData, const QByteArray& key, unsigned char iv[16]);


#endif // CRYPRO_FUNCTIONS_H
