#include "month.h"

Month::Month()
{
}

QString & Month::operator [] (int i) {
    return m_dailyText[i];
}

void Month::setText(int i, QString s) {
    m_dailyText[i] = s;
}

int Month::month()
{
    return m_month;
}

int Month::year()
{
    return m_year;
}

void Month::month(int m)
{
    m_month = m;
}

void Month::year(int y) {
    m_year = y;
}

QString Month::memo()
{
    return m_memo;
}

void Month::memo(const QString m)
{
    m_memo = m;
}


