#include "textedit.h"

CTextEdit::CTextEdit(QWidget *w) : QPlainTextEdit(w) {};

void CTextEdit::setDay(int day)
{
    m_day = day;
}

int CTextEdit::day()
{
    return m_day;
}
