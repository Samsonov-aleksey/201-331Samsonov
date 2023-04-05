#include "misc.h"

#include <QMessageBox>
#include <QApplication>

QString getOpenFilenameFromDialog(QWidget *parent, QString header,
                                  QString sourcePath, QString mask)
{
    QFileDialog dial(parent,
                     header,
                     sourcePath,
                     mask);
    dial.setLabelText(QFileDialog::Accept, QObject::tr("Открыть"));
    dial.setLabelText(QFileDialog::Reject, QObject::tr("Отмена"));

    if (!dial.exec() || dial.selectedFiles().isEmpty())
    {
        return "";
    }
    return dial.selectedFiles().first();
}

QString getFilenameFromPath(QString &filepath)
{
    if (filepath.isEmpty())
    {
        return QDir::homePath();
    }

    QString res;

    res = filepath;

    int index = res.lastIndexOf(QDir::separator());

    if (index != -1)
    {
        res.remove(0, index + 1);
    }

    return res;
}

int showQuestion(QString message, QWidget *parent)
{
    QApplication::alert(parent, 3000);
    QMessageBox messageBox(QMessageBox::Question,
                           QObject::tr("Подтверждение"),
                           message,
                QMessageBox::Yes | QMessageBox::Cancel,
                parent);

    messageBox.setButtonText(QMessageBox::Yes, QObject::tr("Да"));
    messageBox.setButtonText(QMessageBox::Cancel, QObject::tr("Отмена"));
    return  messageBox.exec();
}

int showMessage(QMessageBox::Icon iconType, QString message, QWidget *parent, QString title)
{
    QApplication::alert(parent, 3000);
    if (iconType == QMessageBox::Icon::Warning)
    {
        if (title.isEmpty())
        {
            title = QObject::tr("Предупреждение");
        }
        QMessageBox::warning(parent,
                             title,
                             message);
    }
    else if (iconType == QMessageBox::Icon::Information)
    {
        if (title.isEmpty())
        {
            title = QObject::tr("Информация");
        }
        QMessageBox::information(parent,
                             title,
                             message);
    }
    else if (iconType == QMessageBox::Icon::Critical)
    {
        if (title.isEmpty())
        {
            title = QObject::tr("Ошибка");
        }
        QMessageBox::critical(parent,
                             title,
                             message);
    }
    return 0;
}

QByteArray arrayToByteArray(const uint8_t data[], unsigned size)
{
    if(!data || !size) QByteArray();

    QByteArray bytesArr;

    for (size_t i = 0; i < size; ++i)
    {
        bytesArr.push_back(data[i]);
    }

    return bytesArr;
}
