#ifndef TEXTEDIT_H
#define TEXTEDIT_H

#include <QPlainTextEdit>

class CTextEdit : public QPlainTextEdit
{
public:
    CTextEdit(QWidget *w) : QPlainTextEdit(w){};
    void setDay(int day) {
        m_day = day;
    }
    int day() {
        return m_day;
    }
private:
    int m_day;
};

#endif // TEXTEDIT_H
