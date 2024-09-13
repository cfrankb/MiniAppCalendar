#ifndef TEXTEDIT_H
#define TEXTEDIT_H

#include <QPlainTextEdit>

/// @brief Subclass of QPlainTextEdit to identify the
///        current day from the QPlainTextEdit update event.
class CTextEdit : public QPlainTextEdit
{
public:
    CTextEdit(QWidget *w);
    void setDay(int day);
    int day();

private:
    int m_day;
};

#endif // TEXTEDIT_H
