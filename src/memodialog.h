#ifndef MEMODIALOG_H
#define MEMODIALOG_H

#include <QDialog>

class QPlainTextEdit;
class QPushButton;
class QResizeEvent;

class MemoDialog : public QDialog
{

    Q_OBJECT

public:
    MemoDialog(QWidget *parent=NULL);
    QPlainTextEdit *memoText;

private:
    QPushButton *okButton;
    QPushButton *cancelButton;

    void resizeEvent ( QResizeEvent * event );
};


#endif // MEMODIALOG_H
