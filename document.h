#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <QtDebug>
#include <QDate>
#include <QByteArray>
#include <QtCore/QVariant>
//#include <QtGui/QPlainTextEdit>
#include <QtWidgets/QPlainTextEdit>
#include <stdio.h>
#include "month.h"

class Document
{
public:
    Document();
    ~Document();

    void empty();
    int getSize() { return totalMonths; }
    QString getFileName() { return fileName; }
    void setFileName(QString t) { fileName = t; }
    bool isDirty() { return dirty; }
    bool isUntitled() { return fileName == ""; }
    void setDirty(bool b) {  dirty = b; }

    int findMonth(int year, int month);
    QString getMemo(int year, int month);
    void setMemo(int year, int month, QString memo);
    QString getDailyText(int monthIndex, int day);
    void resize(int size);
    void saveMonth(int year, int month, QPlainTextEdit **textFields);

    bool writeFile();
    bool readFile();

    int getLastError() const;

    static void writeString(FILE *, QString);
    static QString readString(FILE *);
    static void writeInt(FILE *, int);
    static int readInt(FILE *);
    static QString restoreCR(QString str);

    enum {
        ERR_NONE=0,
        ERR_READ=1,
        ERR_WRITE=2,
        ERR_SIGNATURE=3};

protected:

    void init();
    void forget();

    QString fileName;
    //QString lastError;
    int lastError;

    Month *dailyText;
    int *months;
    int *years;
    QString *memos;

    int totalMonths;
    int maxMonths;
    bool dirty;

};

#endif // DOCUMENT_H
