#include "month.h"

Month::Month()
{
}
/// @brief Get daily text at a given index.
///        the strings are stored in an array
/// @param i
/// @return
QString &Month::operator[](int i)
{
    return m_dailyText[i];
}

/// @brief Set the daily text at a given index
/// @param i
/// @param s
void Month::setText(int i, QString s)
{
    m_dailyText[i] = s;
}

/// @brief Retrive month index (0, 11)
/// @return
int Month::month()
{
    return m_month;
}

/// @brief Retrieve year index (e.g. 2024)
/// @return
int Month::year()
{
    return m_year;
}

/// @brief Set month index
/// @param m
void Month::month(int m)
{
    m_month = m;
}

/// @brief Set year index
/// @param y
void Month::year(int y)
{
    m_year = y;
}

/// @brief Retrieve month memo
/// @return
QString Month::memo()
{
    return m_memo;
}

/// @brief Retrive montly memo
/// @param m
void Month::memo(const QString m)
{
    m_memo = m;
}
