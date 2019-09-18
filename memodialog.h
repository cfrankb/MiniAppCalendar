#ifndef MEMODIALOG_H
#define MEMODIALOG_H

#include <QDialog>
#include <QPushButton>
#include <QTextEdit>
#include <QGridLayout>
#include <QResizeEvent>
#include <QtDebug>

class MemoDialog : public QDialog
{

    Q_OBJECT

public:
    MemoDialog(QWidget *parent=NULL);
    QTextEdit *memoText;

private:
    QPushButton *okButton;
    QPushButton *cancelButton;

    void resizeEvent ( QResizeEvent * event );
};


#endif // MEMODIALOG_H
