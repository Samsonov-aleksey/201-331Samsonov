QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

INCLUDEPATH += "$$PWD/widgets/dialog-login/"
#INCLUDEPATH += С:\Qt\Tools\OpenSSL\Win_x64\include\

#LIBS += \
#        $$PWD\libs\libcrypto.lib \
#        $$PWD\libs\libssl.lib

INCLUDEPATH += $$PWD\libs\OpenSSL-Win64\include

LIBS += \
        $$PWD\libs\OpenSSL-Win64\lib\libcrypto.lib \
        $$PWD\libs\OpenSSL-Win64\lib\libssl.lib

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    crypro_functions.cpp \
    main.cpp \
    mainwindow.cpp \
    misc.cpp \
    widgets/dialog-login/login_dialog.cpp

HEADERS += \
    crypro_functions.h \
    mainwindow.h \
    misc.h \
    widgets/dialog-login/login_dialog.h

FORMS += \
    mainwindow.ui \
    widgets/dialog-login/login_dialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc \
    widgets/dialog-login/logindialog.qrc
