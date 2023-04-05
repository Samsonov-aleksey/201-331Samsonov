#include "mainwindow.h"

#include <QApplication>
#include <QCryptographicHash>
#ifdef _MSC_VER
    #include <Windows.h>
    #include <TlHelp32.h>
    #include <Psapi.h>
#endif
#include <string.h>
#include "misc.h"

#define DEBUG_OUTPUT   1
#define VALID_DEBUGGER "PassManagerDbg.exe"

QString getProcessName(DWORD pid);
bool checkMemorySegment();
long long getParentId();

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

#ifdef _MSC_VER
    if(!checkMemorySegment())
    {
        QString msg;
        msg = QString("Приложение модифицировано. Запуск прерван.");

        showMessage(QMessageBox::Critical,
                    msg,
                    nullptr, "Защита от модификации");

        qApp->quit();
        return -1;
    }

    if(IsDebuggerPresent())
    {
        auto parentPID = getParentId();
        auto parentName = getProcessName(parentPID);
        parentName = getFilenameFromPath(parentName);

        if(parentName != VALID_DEBUGGER)
        {
            QString msg;
            msg = QString("Обнаружен отладчик: %1. Запуск прерван.\n"
                          "PID DBG = %2\n"
                          "PID %3").arg(parentName)
                                   .arg(parentPID)
                                   .arg(GetProcessId(GetCurrentProcess()));

            showMessage(QMessageBox::Critical,
                        msg,
                        nullptr, "Защита от отладки");

            qApp->quit();
            return -1;
        }
        else
        {
            QString msg;
            msg = QString("Обнаружен допустимый отладчик. Запуск программы.");

            showMessage(QMessageBox::Information,
                        msg,
                        nullptr, "Защита от отладки");

            MainWindow mainWidget;
            return a.exec();
        }
    }
    else
    {
        MainWindow mainWidget;
        return a.exec();
    }
#else

 qDebug() << "Work only on Windows...";
 qApp->quit();
 return -1;

#endif
}

long long getParentId()
{
    PROCESSENTRY32 pe32;
    DWORD  curPID = GetProcessId(GetCurrentProcess());
    HANDLE hProcessSnap = CreateToolhelp32Snapshot (TH32CS_SNAPPROCESS, 0);

    pe32.dwSize = sizeof (PROCESSENTRY32);

    Process32First ( hProcessSnap, &pe32 );

    while (Process32Next (hProcessSnap, &pe32))
    {
         if (curPID == pe32.th32ProcessID)
         {
              CloseHandle (hProcessSnap);
              return pe32.th32ParentProcessID;
         }
    }
    CloseHandle (hProcessSnap);

    return -1;
}

QString getProcessName(DWORD pid)
{
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);

    CHAR name[MAX_PATH];
    GetModuleFileNameExA(hProcess, NULL, name, sizeof(name));
    CloseHandle(hProcess);

    return name;
}


bool checkMemorySegment()
{
    unsigned long long moduleBase = (unsigned long long)GetModuleHandle(NULL);
    unsigned long long text_segment_start = moduleBase + 0x1000;

    PIMAGE_DOS_HEADER pIDH = reinterpret_cast<PIMAGE_DOS_HEADER>(moduleBase);
    PIMAGE_NT_HEADERS pINH = reinterpret_cast<PIMAGE_NT_HEADERS>(moduleBase + pIDH->e_lfanew);
    unsigned long long size_of_text = pINH->OptionalHeader.SizeOfCode;

    QByteArray text_segment_contents = QByteArray((char*)text_segment_start, size_of_text);
    QByteArray current_hash =
            QCryptographicHash::hash(text_segment_contents, QCryptographicHash::Sha256);
    QByteArray current_hash_base64 = current_hash.toBase64();

    const QByteArray hash0_base64 = QByteArray("BJz7rTZWdM0RRi4yLltKRW3/OeE8HmV/gh4vbwnwf6Y=");
    text_segment_contents.resize(100);

#if DEBUG_OUTPUT
    qDebug()<< "text_segment_start = " << Qt::hex << text_segment_start;
    qDebug() << "size of text = " << size_of_text;
    qDebug()<< "text_segment_contents = " << Qt::hex << text_segment_contents;
    qDebug() << "current_hash_base64 =" << current_hash_base64;
#endif

    bool checkresult = (current_hash_base64==hash0_base64);

    return checkresult;
}
