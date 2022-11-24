#include "memodialog.h"

MemoDialog::MemoDialog(QWidget *parent)
    : QDialog(parent)
{
    okButton = new QPushButton(tr("OK"));
    cancelButton = new QPushButton(tr("Cancel"));
    memoText = new QTextEdit;

    QGridLayout *gLayout = new QGridLayout;

    gLayout->addWidget(memoText, 0, 0, Qt::AlignLeft|Qt::AlignTop);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    gLayout->addLayout(buttonLayout, 1, 0, Qt::AlignRight);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(gLayout);
    setLayout(mainLayout);

    connect(okButton, SIGNAL(clicked()),
            this, SLOT(accept()));

    connect(cancelButton, SIGNAL(clicked()),
            this, SLOT(reject()));

    setWindowTitle(tr("Edit monthly memo"));
}

void MemoDialog::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event)
    memoText->setGeometry(QRect(0,0,width(), okButton->y() - 16));
}
