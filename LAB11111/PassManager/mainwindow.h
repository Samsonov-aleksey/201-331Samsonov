#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QEvent>
#include <QMainWindow>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

    enum TABLE_IDX
    {
        WEBSITE_IDX = 0,
        LOGIN_IDX,
        PASSWORD_IDX
    };

    struct AuthData
    {
        QString webSite;
        QByteArray login;
        QByteArray password;
    };

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    bool eventFilter(QObject *sender, QEvent *event) override;

private slots:
    void tryLogin();
    void refreshEntries();
    void filter(const QString &text);
    void showDataHidden(unsigned row, unsigned col, bool hidden);

    void on_pushButtonSearch_clicked();
    void on_tableView_entered(const QModelIndex &index);
    void on_tableView_doubleClicked(const QModelIndex &index);

private:
    void parsingJSon();
    bool checkHash(const QString& text);

private:
    Ui::MainWindow *ui;
    QStandardItemModel *model_;
    QList<AuthData> listPasswords_;
    QSortFilterProxyModel *filterProxyModel_;
};

#endif // MAINWINDOW_H
