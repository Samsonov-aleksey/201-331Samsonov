#ifndef LOGIN_DIALOG_H
#define LOGIN_DIALOG_H

#include "misc.h"

#include <QMenu>
#include <QDialog>
#include <QLineEdit>
#include <QShortcut>

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    enum Mode
    {
        NEW_USER = 0,
        LOG_USER
    };

    explicit LoginDialog(Mode mode, QWidget *parent = nullptr);
    ~LoginDialog();

    QString getUserLogin();
    QString getPasswd();

protected:
    bool eventFilter(QObject *sender, QEvent *event) override;

private slots:
    void on_pushButtonOK_clicked();
    void on_pushButtonCancel_clicked();
    void on_toolButtonShowPsw_toggled(bool checked);
    void on_pushButtonShowPswRetype_toggled(bool checked);
    void on_lineEditUser_customContextMenuRequested(const QPoint &pos);
    void on_lineEditPasswd_customContextMenuRequested(const QPoint &pos);
    void on_lineEditPasswdRetype_customContextMenuRequested(const QPoint &pos);

    void setWarningMsg(const QString &message, bool show);

private:
    void makeContextMenu();
    void execContextMenu(const QPoint &pos, QLineEdit *edit);
    void setFocusStyle  (QEvent *event, QFrame *line);


private:
    Ui::LoginDialog *ui;

    Mode _loginmode;
    QMenu *_contextMenu;
    QShortcut *_leftKeyReturn;
    QShortcut *_rightKeyReturn;
};

#endif // LOGIN_DIALOG_H
