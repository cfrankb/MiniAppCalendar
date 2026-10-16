#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <QString>
#include <stdio.h>
#include "month.h"

class CTextEdit;
/// @brief Representation of the application document file
///        Calendar file wrapper class.
class Document
{
public:
    Document();
    ~Document();

    void empty();
    int getSize();
    QString getFileName();
    void setFileName(QString t);
    bool isDirty();
    bool isUntitled();
    void setDirty(bool b);

    int findMonth(int year, int month);
    QString getMemo(int year, int month);
    void setMemo(int year, int month, QString memo);
    QString getDailyText(int monthIndex, int day);
    void resize(int size);
    void saveMonth(int year, int month, CTextEdit **textFields);

    bool writeFile();
    bool readFile();
    static int monthAbs(int year, int month)
    {
        return year * 12 + month - 1;
    }

    int getLastError() const;

    static void writeString(FILE *, const QString &);
    static bool readString(FILE *, QString &str);
    static void writeInt(FILE *, int);
    static int readInt(FILE *);
    static QString restoreCR(QString str);

    enum
    {
        ERR_NONE = 0,
        ERR_READ = 1,
        ERR_WRITE = 2,
        ERR_SIGNATURE = 3,
    };
    enum {
        GROW_BY = 100,
        NOT_FOUND = -1,
        DAYS_PER_WEEK = 7,
        ROWS_PER_MONTH=6
    };

protected:
    void init();
    void forget();

    QString m_fileName;
    int m_lastError;
    Month *m_dailyText;
    int m_totalMonths;
    int m_maxMonths;
    bool m_dirty;
};

#endif // DOCUMENT_H
