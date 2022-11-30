#ifndef MONTH_H
#define MONTH_H

#include <QString>

class Month
{
public:
    Month();

    QString & operator [] (int i);
    void setText(int i, QString s);

    int month();
    int year();
    void month(int m);
    void year(int);
    QString memo();
    void memo(const QString m);

protected:
    int m_month;
    int m_year;
    QString m_dailyText[42];
    QString m_memo;
};

#endif // MONTH_H
