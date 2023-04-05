#include <QtCore/QCoreApplication>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QHeaderView>
#include <QJsonArray>
#include <QJsonValue>
#include <QTableView>
#include <QLineEdit>
#include <QDebug>
#include <QFile>

#include "crypro_functions.h"
#include "ui_mainwindow.h"
#include "login_dialog.h"
#include "mainwindow.h"

#define DECRYPTED_ON 1
#define ENCRYPTED_ON 0
#define MIN_DATA_LEN 8
#define ENC_FILE     ":/sources/testfiles/password_enc.json"
#define DEC_FILE     ":/sources/testfiles/passwords.json"

unsigned char initVector[16] {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '1', '2', '3', '4', '5', '6'};
const uint8_t correctHash[KEY_SIZE] = {0x8a, 0x35, 0xe0, 0xef, 0x6b, 0x99, 0x5a, 0x3a, 0xf7, 0xa0, 0x84, 0xc7, 0xb3, 0x9f, 0x3f, 0x9a, 0xa9, 0x6f, 0x4c, 0xe6, 0xb9, 0x61, 0xd2, 0x7d, 0x5e, 0xe0, 0x84, 0xa7, 0x79, 0xb9, 0x3e, 0xc3};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),
      model_(nullptr),
      filterProxyModel_(nullptr)
{
    ui->setupUi(this);
    this->hide();

    QMetaObject::invokeMethod(this, "tryLogin", Qt::QueuedConnection);
    connect(ui->lineEditSearch, &QLineEdit::textChanged, this, &MainWindow::filter);

    ui->tableView->viewport()->setMouseTracking(true);
    ui->tableView->setMouseTracking(true);


#if ENCRYPTED_ON
    auto hash = arrayToByteArray(correctHash, KEY_SIZE);
    encrypt256cbc(DEC_FILE, "password_enc.json", hash, initVector);
#endif
}

MainWindow::~MainWindow()
{
    delete filterProxyModel_;
    delete model_;
    delete ui;
}

void MainWindow::tryLogin()
{
    this->hide();

    LoginDialog *loginDlg = new LoginDialog(LoginDialog::LOG_USER, this);

    auto res = loginDlg->exec();
    switch (res)
    {
    case QDialog::Accepted:
        break;
    case QDialog::Rejected:
        qApp->quit();
        return;
        break;
    default:
        qApp->quit();
        return;
        break;
    }

    auto login = loginDlg->getUserLogin() + loginDlg->getPasswd();
    if(checkHash(login))
    {
        showMessage(QMessageBox::Information,
                    "Аутентификация успешна",
                    this, "Проверка пароля");
    }
    else
    {
        showMessage(QMessageBox::Critical,
                    "Неверные входные данные",
                    this, "Проверка пароля");
        qApp->quit();
        return;
    }

    parsingJSon();
    refreshEntries();
    this->show();
}

bool MainWindow::checkHash(const QString &text)
{
    auto hash = sha256(text);

    QByteArray correctBytes =
            arrayToByteArray(correctHash, KEY_SIZE);

    return correctBytes == hash;
}

void MainWindow::parsingJSon()
{
    QFile file;
#if DECRYPTED_ON
    file.setFileName(ENC_FILE);
#else
    file.setFileName(DEC_FILE);
#endif

    file.open(QIODevice::ReadOnly | QIODevice::Text);

    if(!file.isOpen())
    {
        QString msg = QString("Ошибка открытия файла: ") + file.fileName();
        showMessage(QMessageBox::Information,
                    msg, this, "Открытие файла");
        return;
    }

    QString text;
    QByteArray hash;
    {
#if DECRYPTED_ON
        auto data = file.readAll();
        hash = arrayToByteArray(correctHash, KEY_SIZE);

        text = decrypt256cbc(data, hash, initVector);
#else
        text = file.readAll();
#endif

        file.close();
    }

    AuthData data;
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(text.toUtf8(), &error);

    if (doc.isObject())
    {
        QJsonObject json = doc.object();
        QJsonArray jsonArray = json["crindentials"].toArray();

        foreach (const QJsonValue & value, jsonArray)
        {
            if (value.isObject())
            {
                QJsonObject obj = value.toObject();

                data.webSite = obj["site"].toString();
#if DECRYPTED_ON
                QByteArray decData;

                decData = obj["login"].toVariant().toByteArray();
                if(decData.size() < MIN_DATA_LEN)
                {
                    int diff = MIN_DATA_LEN - decData.size();
                    for (int i = 0; i < diff; ++i)
                    {
                        decData.push_back(' ');
                    }
                }
                data.login = encrypt256cbc(decData, hash, initVector);


                decData = obj["password"].toVariant().toByteArray();
                if(decData.size() < MIN_DATA_LEN)
                {
                    int diff = MIN_DATA_LEN - decData.size();
                    for (int i = 0; i < diff; ++i)
                    {
                        decData.push_back(' ');
                    }
                }
                data.password = encrypt256cbc(decData, hash, initVector);

                decData.clear();
#else
                data.login = obj["login"].toVariant().toByteArray();
                data.password = obj["password"].toVariant().toByteArray();
#endif
                listPasswords_.push_back(data);
            }
        }
    }

    if(listPasswords_.isEmpty())
    {
        QString msg = "Ошибка расшифровки файла. Либо файл пуст: "+ file.fileName();
        msg += "\nОшибка: ";
        msg += error.errorString();

        showMessage(QMessageBox::Information,
                    msg, this, "Расшифровка файла");
        qApp->quit();
        return;
    }

    text.clear();
}


void MainWindow::refreshEntries()
{
    allocateData(model_);
    allocateData(filterProxyModel_);

    QString style;
    QFile file(":/sources/resources/styles/styles.qss");
    if(file.open(QFile::ReadOnly | QFile::Text))
    {
        style = file.readAll();
        file.close();
    }
    else
    {
        qDebug() << "Ошибка чтения таблицы стилей";
    }

    auto listHeader = QStringList() << "Сайт" << "Логин" << "Пароль";
    model_->setHorizontalHeaderLabels(listHeader);
    filterProxyModel_->setFilterCaseSensitivity(Qt::CaseInsensitive);
    filterProxyModel_->setFilterKeyColumn(WEBSITE_IDX);
    filterProxyModel_->setSourceModel(model_);

    ui->tableView->setModel(filterProxyModel_);
    ui->tableView->setStyleSheet(style);


    for (auto& data : listPasswords_)
    {
        QString site = data.webSite;

        int index = model_->rowCount();
        model_->insertRow(index);

        auto item = new QStandardItem(site);
        model_->setItem(index, WEBSITE_IDX, item);

        QString login_hided = QString(data.login.length(), '*');
        item = new QStandardItem(login_hided);
        item->setData("Дважды кликните для расшифровки", Qt::ToolTipRole);
        model_->setItem(index, LOGIN_IDX, item);

        QString password_hided = QString(data.password.length(), '*');
        item = new QStandardItem(password_hided);
        item->setData("Дважды кликните для расшифровки", Qt::ToolTipRole);
        model_->setItem(index, PASSWORD_IDX, item);

        ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    }
}

bool MainWindow::eventFilter(QObject *sender, QEvent *event)
{
    if(!event || !sender) return false;



    QObject::eventFilter(sender,event);
    return false;
}

void MainWindow::on_tableView_entered(const QModelIndex &index)
{
    static int prevRow = -1;
    static int prevCol = -1;

    if(index.row() != prevRow || index.row() != prevCol)
    {
        showDataHidden(prevRow, prevCol, true);
        showDataHidden(index.row(), index.column(), false);

        prevRow = index.row();
        prevCol = index.column();
    }
}

void MainWindow::showDataHidden(unsigned row, unsigned col, bool hidden)
{
    if(!model_ || !filterProxyModel_) return;

    if(row >= (unsigned)model_->rowCount() ||
       col >= (unsigned)model_->columnCount()) return;

    if(row >= (unsigned)listPasswords_.size()) return;

    if(col == WEBSITE_IDX) return;

    QByteArray decryptedData;

    if(col == LOGIN_IDX)
    {
        decryptedData = listPasswords_.at(row).login;
    }
    else if(col == PASSWORD_IDX)
    {
        decryptedData = listPasswords_.at(row).password;
    }

    if(hidden)
    {
        QString newText(decryptedData.length(), '*');

        auto item = model_->item(row, col);
        item->setText(newText);

    }
    else
    {
        QString newText = decryptedData;

        auto item = model_->item(row, col);
        item->setText(newText);
    }
}

void MainWindow::filter(const QString &text)
{
    if(!filterProxyModel_) return;

    filterProxyModel_->setFilterRegExp(text);
}


void MainWindow::on_pushButtonSearch_clicked()
{
    ui->lineEditSearch->setFocus();
}


void MainWindow::on_tableView_doubleClicked(const QModelIndex &index)
{
    int row = index.row();
    int col = index.column();

    if(col == WEBSITE_IDX)
    {
        return;
    }

    LoginDialog *loginDlg = new LoginDialog(LoginDialog::LOG_USER, this);

    auto res = loginDlg->exec();

    if(res == QDialog::Rejected)
    {
        delete loginDlg;
        return;
    }

    auto login = loginDlg->getUserLogin() + loginDlg->getPasswd();

    if(checkHash(login))
    {
        QByteArray encryptedData;
        QString text = "Аутентификация успешна\n";
        auto hash = arrayToByteArray(correctHash, KEY_SIZE);

        if(col == LOGIN_IDX)
        {
            encryptedData = listPasswords_.at(row).login;
            text += "login: " + decrypt256cbc(encryptedData, hash, initVector);
        }
        else if(col == PASSWORD_IDX)
        {
            encryptedData = listPasswords_.at(row).password;
            text += "password: " + decrypt256cbc(encryptedData, hash, initVector);
        }


        showMessage(QMessageBox::Information,
                    text,
                    this, "Расшифровка данных");
    }
    else
    {
        showMessage(QMessageBox::Critical,
                    "Неверные входные данные.\n"
                    "Не удалось расшифровать данные таблицы",
                    this, "Расшифровка данных");
    }

     delete loginDlg;
}
