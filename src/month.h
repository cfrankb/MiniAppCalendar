#ifndef MONTH_H
#define MONTH_H

#include <QString>
/// @brief Storage Representation Interface for Month
class Month
{
public:
    Month();

    enum
    {
        MAX_DAYS = 42 // Days per week (7) * Rows (6)
    };

    QString &operator[](int i);
    void setText(int i, const QString &s);

    int month();
    int year();
    void month(int m);
    void year(int);
    QString memo();
    void memo(const QString& m);

protected:
    int m_month;
    int m_year;
    QString m_dailyText[MAX_DAYS];
    QString m_memo;
};

#endif // MONTH_H
