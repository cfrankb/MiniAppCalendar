#ifndef MONTH_H
#define MONTH_H

#include <QtCore/QVariant>


class Month
{
public:
    Month();

    QString & operator [] (int i) { return dailyText[i]; }
    void setText(int i, QString s) { dailyText[i] = s; }

protected:
    QString dailyText[42];


};

#endif // MONTH_H
