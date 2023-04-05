#include "login_dialog.h"
#include "ui_login_dialog.h"

#include <QDir>
#include <QKeyEvent>
#include <QClipboard>
#include <QTextStream>
#include <QRegExpValidator>

#define NEW_USER_HEIGHT        360
#define LOG_USER_HEIGHT        270
#define LINE_FOCUS_IN_STYLE  "background: #2F80ED;"
#define LINE_FOCUS_OUT_STYLE "*{ background: rgba(35, 35, 35, 0.09);}\
                              *:hover{ background: rgba(35, 35, 35, 0.24);}"


LoginDialog::LoginDialog(Mode mode, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog),
    _loginmode(mode),
    _contextMenu(nullptr),
    _leftKeyReturn(nullptr),
    _rightKeyReturn(nullptr)
{
    ui->setupUi(this);

    setModal(true);
    makeContextMenu();
    ui->frameMsg->hide();
    ui->labelSecondTitle->hide();

    setWindowFlags(windowFlags() | Qt::WindowSystemMenuHint | Qt::WindowMinimizeButtonHint);

    QRegExpValidator* validator = new QRegExpValidator(QRegExp("[^а-яёА-ЯЁ]+"), this);

    ui->lineEditPasswd->setEchoMode(QLineEdit::Password);
    ui->lineEditPasswd->setValidator(validator);
    ui->lineEditPasswdRetype->setEchoMode(QLineEdit::Password);
    ui->lineEditPasswdRetype->setValidator(validator);

    ui->lineEditUser->installEventFilter(this);
    ui->lineEditPasswd->installEventFilter(this);
    ui->lineEditPasswdRetype->installEventFilter(this);

    _leftKeyReturn = new QShortcut(this);
    _leftKeyReturn->setKey(Qt::Key_Return);
    connect(_leftKeyReturn, SIGNAL(activated()),
            this, SLOT(on_pushButtonOK_clicked()));

    _rightKeyReturn = new QShortcut(this);
    _rightKeyReturn->setKey(Qt::Key_Enter);
    connect(_rightKeyReturn, SIGNAL(activated()),
            this, SLOT(on_pushButtonOK_clicked()));


    switch (mode)
    {
    case NEW_USER:
        setFixedSize(width(), NEW_USER_HEIGHT);
        setWindowTitle(tr("Создание нового пользователя"));
        break;
    case LOG_USER:
        ui->framePswConfirmGeneral->hide();
        setFixedSize(width(), LOG_USER_HEIGHT);
        setWindowTitle(tr("Аутентификация пользователя"));
        break;
    default:
        break;
    }

}

LoginDialog::~LoginDialog()
{
    delete _rightKeyReturn;
    delete _leftKeyReturn;
    delete _contextMenu;

    delete ui;
}

void LoginDialog::on_pushButtonOK_clicked()
{
    setWarningMsg("", false);

    auto login  = ui->lineEditUser->text();
    auto passwd = ui->lineEditPasswd->text();
    auto passwdRetype = ui->lineEditPasswdRetype->text();

    if(_loginmode == NEW_USER ||
       _loginmode == LOG_USER)
    {
        if(login.isEmpty())
        {
            auto msg = tr("Имя пользователя отсутствует");
            ui->lineEditUser->setFocus();
            setWarningMsg(msg, true);
            return;
        }
    }

    if(_loginmode == NEW_USER ||
       _loginmode == LOG_USER)
    {
        if(passwd.isEmpty())
        {
            auto msg = tr("Пароль отсутствует");
            ui->lineEditPasswd->setFocus();
            setWarningMsg(msg, true);
            return;
        }
    }

    if(_loginmode == NEW_USER)
    {
        if(passwd != passwdRetype)
        {
            auto msg = tr("Пароли не совпадают");
            ui->lineEditPasswdRetype->setFocus();
            setWarningMsg(msg, true);
            return;
        }
    }


    this->accept();
}

void LoginDialog::on_pushButtonCancel_clicked()
{
    this->reject();
}


void LoginDialog::makeContextMenu()
{
    if(_contextMenu) delete _contextMenu;

    _contextMenu = new QMenu(nullptr);

    QString menuStyle("QMenu{"
                      "background-color: rgb(255, 255, 255);"
                      "border: 1px solid #ABABAB;"
#ifdef _MSC_VER
                      "border-right: 2px solid #ABABAB;"
#endif
                      "}"
                      "QMenu::item{"
                      "background-color: rgb(255, 255, 255);"
                      "color: rgb(0, 0, 0);"
                      "}"
                      "QMenu::item:selected{"
                      "background-color: rgb(23, 65, 231);"
                      "color: rgb(255, 255, 255);"
                      "} ");

    _contextMenu -> setStyleSheet(menuStyle);

    QAction *copyItem =
            _contextMenu->addAction(tr("Копировать"));
    QAction *cutItem =
            _contextMenu->addAction(tr("Вырезать"));
    QAction *selectItem =
            _contextMenu->addAction(tr("Выделить всё"));

    copyItem   -> setShortcut(QKeySequence(Qt::CTRL + Qt::Key::Key_C));
    cutItem ->    setShortcut(QKeySequence(Qt::CTRL + Qt::Key::Key_X));
    selectItem -> setShortcut(QKeySequence(Qt::CTRL + Qt::Key::Key_A));
}

void LoginDialog::execContextMenu(const QPoint &pos, QLineEdit *edit)
{
    if(!_contextMenu || !edit)
        return;

    QAction *sel = _contextMenu->exec(edit->mapToGlobal(pos));

    if (!sel) return;
    int actionSize = _contextMenu->actions().size();
    if( actionSize < 3) return;

    QAction *copyItem   = _contextMenu->actions().at(0);
    QAction *CutItem    = _contextMenu->actions().at(1);
    QAction *selectItem = _contextMenu->actions().at(2);


    if (sel == copyItem)
    {
        QClipboard *clipboard = QGuiApplication::clipboard();
        clipboard->setText(edit->selectedText());
    }
    else if (sel == selectItem)
    {
        int sizeText = edit->text().size();
        edit->setSelection(0, sizeText);
    }
    else if (sel == CutItem)
    {
        auto keyEvent = new QKeyEvent(QEvent::KeyPress,
                                      Qt::CTRL + Qt::Key::Key_X,
                                      Qt::NoModifier);
        edit->event(keyEvent);
        delete keyEvent;
    }
}

bool LoginDialog::eventFilter(QObject *sender, QEvent *event)
{
    if(!event)
        return false;

    if(sender == ui->lineEditUser)
    {
        setFocusStyle(event, ui->lineUser);
    }
    else if(sender == ui->lineEditPasswd)
    {
        setFocusStyle(event, ui->linePsw);
    }
    else if(sender == ui->lineEditPasswdRetype)
    {
        setFocusStyle(event, ui->linePswConfirm);
    }

    QObject::eventFilter(sender,event);

    return false;
}

void LoginDialog::setFocusStyle(QEvent *event, QFrame *line)
{
    if (event->type()==QEvent::FocusIn)
    {
        line->setStyleSheet(LINE_FOCUS_IN_STYLE);
    }
    else if (event->type()==QEvent::FocusOut)
    {
        line->setStyleSheet(LINE_FOCUS_OUT_STYLE);
    }
    else if (event->type()==QEvent::Enter)
    {
        line->setFixedHeight(2);
    }
    else if (event->type()==QEvent::Leave)
    {
        line->setFixedHeight(1);
    }
}

void LoginDialog::on_toolButtonShowPsw_toggled(bool checked)
{
    ui->pushButtonShowPswRetype->setChecked(checked);
    ui->lineEditPasswd->setEchoMode(checked?QLineEdit::Normal:QLineEdit::Password);
    ui->lineEditPasswdRetype->setEchoMode(checked?QLineEdit::Normal:QLineEdit::Password);
}

void LoginDialog::on_pushButtonShowPswRetype_toggled(bool checked)
{
    ui->lineEditPasswdRetype->setEchoMode(checked?QLineEdit::Normal:QLineEdit::Password);
}

void LoginDialog::setWarningMsg(const QString &message, bool show)
{
    ui->frameMsg->setHidden(!show);
    ui->labelMsg->setText(message);
}

void LoginDialog::on_lineEditUser_customContextMenuRequested(const QPoint &pos)
{
    execContextMenu(pos, ui->lineEditUser);
}


void LoginDialog::on_lineEditPasswd_customContextMenuRequested(const QPoint &pos)
{
    execContextMenu(pos, ui->lineEditPasswd);
}

void LoginDialog::on_lineEditPasswdRetype_customContextMenuRequested(const QPoint &pos)
{
    execContextMenu(pos, ui->lineEditPasswdRetype);
}

QString LoginDialog::getUserLogin()
{
    return ui->lineEditUser->text();
}

QString LoginDialog::getPasswd()
{
    return ui->lineEditPasswd->text();
}



