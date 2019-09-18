#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtGui>
#include "memodialog.h"
#include <QMessageBox>
#include <QFileDialog>
#include "dlgabout.h"

static char appName[] = "MiniApp Calendar";
static char author[] = "cfrankb";
static char fileFilter[] = "MiniApp Calendar files (*.pca)";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    initFileMenu();
    initToolBar();
    initCalendar();

    QAction *actionToolBar = ui->mainToolBar->toggleViewAction();
    actionToolBar->setText(tr("ToolBar"));
    actionToolBar->setStatusTip(tr("Show or hide toolbar"));
    ui->menuView->addAction(actionToolBar);

    updateTitle();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initCalendar()
{
    int x, y, dd;
    int len = 90;
    int hei = 80;
    int space = 14;
    int baseX = 10;
    int baseY = 80;
    int day_count = 1;

    QFont font;

    date = QDate::currentDate();
    date.setDate(date.year(), date.month(), 1);
    int day = date.dayOfWeek() % 7 ;

    textFields =  new QPlainTextEdit * [42];
    labelDays = new QLabel * [7];
    labelNumbers = new QLabel * [42];

    QString text = getMonthName(date.month()) + " " + QString::number(date.year());
    ui->labelMonth->setText(text);

#ifdef Q_WS_WIN
    font.setPointSize(14);
#else
    font.setPointSize(18);
#endif

    ui->labelMonth->setFont(font);

#ifdef Q_WS_WIN
    font.setPointSize(7);
#else
    font.setPointSize(9);
#endif

    for (y = 0; y < 6; y++) {
        for (x = 0; x < 7; x++) {
            dd = x + y * 7;

            textFields[dd] =  new QPlainTextEdit(ui->centralWidget);
            textFields[dd]->setObjectName(QString::fromUtf8("textFields") + QString::number(dd));
            textFields[dd]->setGeometry(QRect(baseX + (len + space) * x, baseY + y * (hei + space), len, hei));
            textFields[dd]->setFont(font);
            textFields[dd]->setMouseTracking(false);
            textFields[dd]->setFrameShape(QFrame::Box);
            textFields[dd]->setMidLineWidth(1);

            if (dd >= day && day_count <= getMonthSize(date.year(), date.month())) {
                text = QString("   " + QString::number(day_count));
                day_count++;
                textFields[dd]->setVisible(true);
            } else {
                text = QString("");
                textFields[dd]->setVisible(false);
            }

            labelNumbers[dd] = new QLabel(ui->centralWidget);
            labelNumbers[dd]->setObjectName(QString::fromUtf8("labelNum") + QString::number(dd));
            labelNumbers[dd]->setGeometry(QRect(baseX + (len + space) * x + len - 32, baseY + y * (hei + space) - space, 32, 10));
            labelNumbers[dd]->setText(text);
        }
    }

    QString days[] = {
        tr("Sunday"),
        tr("Monday"),
        tr("Tuesday"),
        tr("Wednesday"),
        tr("Thurday"),
        tr("Friday"),
        tr("Saturday")
    };

    for (x = 0; x < 7; x++) {
        labelDays[x] = new QLabel(ui->centralWidget);
        labelDays[x]->setObjectName(QString::fromUtf8("labelDay") + QString::number(x));
        labelDays[x]->setGeometry(QRect(baseX + (len + space) * x, baseY - space, 32, 10));
        labelDays[x]->setText(days[x].mid(0,3));
    }
}

QString MainWindow::getMonthName(int month) {
    QString months[] = {
        tr("January"),
        tr("February"),
        tr("March"),
        tr("April"),
        tr("May"),
        tr("June"),
        tr("July"),
        tr("August"),
        tr("September"),
        tr("October"),
        tr("November"),
        tr("December")
    };

    return months[month - 1];
}

 int MainWindow::getMonthSize(int year, int month) {
    QDate date = QDate(year, month, 1);
    return date.daysInMonth();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    doc.saveMonth(date.year(), date.month(), textFields);
    if (maybeSave()) {
        event->accept();
    } else {
        event->ignore();
    }
    event->accept();
}

void MainWindow::resizeEvent (QResizeEvent *event)
{
    event->accept();
    resizeControls(width(), height() - 32);
}

void MainWindow::updateCalendar() {

    QString text = getMonthName(date.month()) + " " + QString::number(date.year());
    ui->labelMonth->setText(text);

    int day = date.dayOfWeek() % 7;

    int x, y, dd;
    int day_count = 1;
    int monthSize = getMonthSize(date.year(), date.month());
    int monthIndex =  doc.findMonth(date.year(), date.month());

    for (y = 0; y < 6; y++) {
        for (x = 0; x < 7; x++) {
            dd = x + y * 7;
            if (dd >= day && day_count <= monthSize) {
                text = QString("   " + QString::number(day_count));
                day_count++;
                textFields[dd]->setVisible(true);
                if (monthIndex != -1) {
                    textFields[dd]->setPlainText(doc.getDailyText(monthIndex, dd));
                }
                else {
                    textFields[dd]->setPlainText("");
                }

            } else {
                text = "";
                textFields[dd]->setVisible(false);
            }

            labelNumbers[dd]->setText(text);
        }
    }
}

void MainWindow::resizeControls(int width, int height) {

    QRect rectLabel =  ui->labelMonth->frameGeometry();
    QRect rectStatusbar = ui->statusBar->frameGeometry();

    int spaceX = 4;
    int spaceY = 14;
    int baseX = 10;
    int baseY = rectLabel.y() + rectLabel.height() + 16;
    int hei = -spaceY + ( height - baseY -rectStatusbar.height()) / 6;
    int len = -spaceX + (width - baseX) / 7;

    for (int y = 0; y < 6; y ++) {
        for (int x = 0; x < 7; x ++) {
            int dd = x + y * 7;
            textFields[dd]->setGeometry(QRect(baseX + (len + spaceX) * x, baseY + y * (hei + spaceY), len, hei));
            labelNumbers[dd]->setGeometry(QRect(baseX + (len + spaceX) * x + len - 32, baseY + y * (hei + spaceY) - spaceY, 32, 10));
        }
    }

    for (int x = 0; x < 7; x++) {
        labelDays[x]->setGeometry(QRect(baseX + (len + spaceX) * x, baseY - spaceY, 32, 10));
    }
}

void MainWindow::on_actionPrevious_triggered()
{
    doc.saveMonth(date.year(), date.month(), textFields);
    date = date.addMonths(-1);
    updateCalendar();
}

void MainWindow::on_actionNext_triggered()
{
    doc.saveMonth(date.year(), date.month(), textFields);
    date = date.addMonths(1);
    updateCalendar();
}

void MainWindow::on_actionCurrent_triggered()
{
    doc.saveMonth(date.year(), date.month(), textFields);
    date = QDate::currentDate();
    date.setDate(date.year(), date.month(), 1);
    updateCalendar();
}

void MainWindow::on_actionOpen_triggered()
{
    doc.saveMonth(date.year(), date.month(), textFields);
    open("");
}

void MainWindow::on_actionSave_triggered()
{
    doc.saveMonth(date.year(), date.month(), textFields);
    save();
}

void MainWindow::on_actionSave_as_triggered()
{
    doc.saveMonth(date.year(), date.month(), textFields);
    saveAs();
}

void MainWindow::on_actionNew_File_triggered()
{
    doc.saveMonth(date.year(), date.month(), textFields);
    newFile();
}

bool MainWindow::newFile()
{
    if (maybeSave()) {
        doc.empty();
        updateTitle();
        updateCalendar();
    }
    return true;
}

void MainWindow::openRecentFile()
{
    doc.saveMonth(date.year(), date.month(), textFields);

    QAction *action = qobject_cast<QAction *>(sender());
    if (action)
        open(action->data().toString());
}

bool MainWindow::maybeSave()
{
    if (doc.isDirty()) {
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(this, tr(appName),
                     tr("The document has been modified.\n"
                        "Do you want to save your changes?"),
                     QMessageBox::Save | QMessageBox::Discard
                     | QMessageBox::Cancel);
        if (ret == QMessageBox::Save)
            return save();
        else if (ret == QMessageBox::Cancel)
            return false;
    }
    return true;
}

QString MainWindow::getErrorMessage(int msg) const
{
    static QString msgs[] =
    {
        tr("none"),
        tr("read error"),
        tr("write error"),
        tr("wrong signature")
    };

    return msgs[msg];
}

bool MainWindow::open(QString fileName)
{
    if (maybeSave()) {
        if (fileName.isEmpty()) {
            fileName = QFileDialog::getOpenFileName(this, "", fileName, tr(fileFilter));
        }

        if (!fileName.isEmpty()) {
            QString oldFileName = doc.getFileName();
            doc.setFileName(fileName);
            if (!doc.readFile())  {
                warningMessage(tr("cannot open file") + "\n" + doc.getLastError());
                doc.setFileName(oldFileName);

                // update fileList
                QSettings settings(author, appName);
                QStringList files = settings.value("recentFileList").toStringList();
                files.removeAll(fileName);
                settings.setValue("recentFileList", files);
                updateRecentFileActions();
//                QString fileName = doc.getFileName();
                return false;
            }

            updateTitle();
            updateCalendar();
        }
    }

    return true;
}

 void MainWindow::setDocument( const QString& fileName )
 {
     open(fileName);
 }

bool MainWindow::save()
{
    QString oldFileName = doc.getFileName();

    if (doc.isUntitled()) {
        if (!saveAs())
            return false;
    }

    if (!doc.writeFile() || !updateTitle())  {
        warningMessage(tr("Can't write file"));
        doc.setFileName(oldFileName);
        return false;
    }

    return true;
}

bool MainWindow::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"), doc.getFileName(), tr(fileFilter));
    if (fileName.isEmpty())
        return false;

    doc.setFileName(fileName);
    return true;
//
  //  return doc.writeFile() && updateTitle();
}

bool MainWindow::updateTitle()
{
    QString file;

    if (doc.getFileName().isEmpty()) {
        file = tr("untitled");
    } else {
        file = QFileInfo(doc.getFileName()).fileName();
    }

    doc.setDirty(false);
    setWindowTitle(tr("%1[*] - %2").arg( file  )  .arg(tr(appName)));

    // update fileList

    QSettings settings(author, appName);
    QStringList files = settings.value("recentFileList").toStringList();
    QString fileName = doc.getFileName();
    files.removeAll(fileName);
    if (!fileName.isEmpty()) {
        files.prepend(fileName);
        while (files.size() > MaxRecentFiles)
            files.removeLast();
    }

    settings.setValue("recentFileList", files);
    updateRecentFileActions();

    return true;
}

void MainWindow::updateRecentFileActions()
{
    QSettings settings(author, appName);
    QStringList files = settings.value("recentFileList").toStringList();

    int numRecentFiles = qMin(files.size(), (int)MaxRecentFiles);

    for (int i = 0; i < numRecentFiles; ++i) {

        QString text = tr("&%1 %2").arg(i + 1).arg(QFileInfo(files[i]).fileName());
        recentFileActs[i]->setText(text);
        recentFileActs[i]->setData(files[i]);
        recentFileActs[i]->setVisible(true);
        recentFileActs[i]->setStatusTip(files[i]);
    }
    for (int j = numRecentFiles; j < MaxRecentFiles; ++j)
        recentFileActs[j]->setVisible(false);

    separatorAct[0]->setVisible(numRecentFiles > 0);
}

void MainWindow::initFileMenu()
{
    //actionClose = new QAction(MainWindow);
    //actionClose->setObjectName(QString::fromUtf8("actionClose"));
    // complete the File menu

    ui->menu_File->addSeparator();

    for (int i = 0; i < MaxRecentFiles; i++) {
        recentFileActs[i] = new QAction(this);
        recentFileActs[i]->setVisible(false);
        ui->menu_File->addAction(recentFileActs[i]);
        connect(recentFileActs[i], SIGNAL(triggered()),
                this, SLOT(openRecentFile()));
    }

    separatorAct[0] = ui->menu_File->addSeparator();
    ui->actionClose->setStatusTip(tr("Close the application"));
    updateRecentFileActions();

    connect(ui->actionClose, SIGNAL(triggered()), this, SLOT(close()));
    ui->actionClose->setMenuRole(QAction::QuitRole);

    ui->menu_File->addAction(ui->actionClose);

}

void MainWindow::initToolBar()
{
    ui->mainToolBar->setIconSize( QSize(16,16) );
    ui->mainToolBar->addAction(ui->actionNew_File);
    ui->mainToolBar->addAction(ui->actionOpen);
    ui->mainToolBar->addAction(ui->actionSave);
    ui->mainToolBar->addSeparator();
    ui->mainToolBar->addAction(ui->actionPrevious);
    ui->mainToolBar->addAction(ui->actionNext);
    ui->mainToolBar->addAction(ui->actionCurrent);
    ui->mainToolBar->addAction(ui->actionMemo);
}

void MainWindow::on_actionMemo_triggered()
{
    MemoDialog *dialog = new MemoDialog(this);
    dialog->setWindowIcon(QIcon(":/images/images/memo.gif"));
    dialog->setWindowTitle( getMonthName(date.month()) + " " + QString::number(date.year()));
    QString oldText = doc.getMemo(date.year(), date.month());
    dialog->memoText->setText(oldText);
    if (dialog->exec()) {
        QString newText = Document::restoreCR(dialog->memoText->toPlainText());
        if (newText != oldText) {
            doc.saveMonth(date.year(), date.month(), textFields);
            doc.setMemo(date.year(), date.month(), newText);
            doc.setDirty(true);
        }
    }

    delete dialog;
}

void MainWindow::warningMessage(QString message)
{
    QMessageBox msgBox(QMessageBox::Warning, appName, message, 0, this);
    msgBox.exec();
}

void MainWindow::on_actionAbout_Qt_triggered()
{
    QApplication::aboutQt();
}

void MainWindow::on_actionAbout_triggered()
{
    DlgAbout dlg;
    dlg.exec();
}
