#ifndef MISC_H
#define MISC_H

#include <QDir>
#include <QDebug>
#include <QObject>
#include <QMessageBox>
#include <QFileDialog>

QString getOpenFilenameFromDialog(QWidget *parent, QString header,
                                  QString sourcePath, QString mask);
QString getFilenameFromPath(QString &filepath);


template <class T>
void clearData(T* &data)
{
    if(!data) return;

    data->clear();

    delete data;
    data = nullptr;
}

template <class T>
void allocateData(T* &data)
{
    if(data)
    {
        clearData(data);
    }

    data = new T;

    data->clear();
}

int showQuestion(QString message, QWidget *parent);
QByteArray arrayToByteArray(const uint8_t data[], unsigned size);
int showMessage(QMessageBox::Icon iconType, QString message, QWidget *parent, QString title);

#endif // MISC_H
