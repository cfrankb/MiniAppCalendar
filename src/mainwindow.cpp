#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QInputDialog>
#include <QtWidgets/QLabel>
#include <QtDebug>
#include <QtWidgets/QPlainTextEdit>
#include <QSettings>
#include "dlgabout.h"
#include "memodialog.h"
#include "textedit.h"

static char appName[] = "MiniApp Calendar";
static char author[] = "cfrankb";
static char fileFilter[] = "MiniApp Calendar files (*.pca)";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    restoreSettings();
    initFileMenu();
    initToolBar();
    initCalendar();

    QAction *actionToolBar = ui->mainToolBar->toggleViewAction();
    actionToolBar->setText(tr("ToolBar"));
    actionToolBar->setStatusTip(tr("Show or hide toolbar"));
    ui->menuView->addAction(actionToolBar);

    ui->actiongrey_out_past->setChecked(greyOutPastToggle);
    ui->actionhighlight->setChecked(highlightToggle);
    ui->actionApply_rules->setChecked(applyRulesToggle);
    updateTitle();
}

MainWindow::~MainWindow()
{
    delete ui;
}

const char *MainWindow::applyRules(const QString &text)
{
    typedef struct {
        long op;
        const char *pattern;
        const char *color;
    } RULE;

    RULE rules[] = {
        {RULE_CONTAINS, "PTO", "QPlainTextEdit {background-color: yellow;}"},
        {RULE_CONTAINS, "SICK", "QPlainTextEdit {background-color: #DDFF80;}"},
        {RULE_CONTAINS, "VACATIONS", "QPlainTextEdit {background-color: pink;}"},
        {RULE_CONTAINS, "STATS HOLIDAY", "QPlainTextEdit {background-color: lime;}"},
        {RULE_STARTSWITH, "***", "QPlainTextEdit {background-color: orange;}"},
        {RULE_STARTSWITH, "@@@", "QPlainTextEdit {background-color: #80A0FF;}"},
        {RULE_STARTSWITH, "$$$", "QPlainTextEdit {background-color: green;}"},
    };

    for (unsigned int i=0; i < sizeof(rules)/sizeof(RULE); ++i) {
        RULE & rule = rules[i];
        if (rule.op == RULE_CONTAINS && text.contains(rule.pattern))  {
            return rule.color;
        } else if (rule.op == RULE_STARTSWITH && text.startsWith(rule.pattern))  {
            return rule.color;
        } else if (rule.op == RULE_ENDSWITH && text.endsWith(rule.pattern))  {
            return rule.color;
        }
    }
    return "";
}

const char *MainWindow::colorDate(int day_count, QString & text)
{
    QDateTime now = QDateTime::currentDateTime();
    int today_year = 0;
    int today_month = 0;
    int today_day = 0;
    now.date().getDate(&today_year, &today_month, &today_day);
    int nowAbs = Document::monthAbs(today_year, today_month);
    int currentAbs = Document::monthAbs(m_date.year(), m_date.month());
    if (currentAbs < nowAbs && greyOutPastToggle) {
        return "QPlainTextEdit {background-color: #eeeeee;}";
    } else if (currentAbs == nowAbs) {
        if ((day_count < today_day + 1) && greyOutPastToggle) {
            return "QPlainTextEdit {background-color: #eeeeee;}";
        } else if ((day_count == today_day + 1) && highlightToggle)  {
            return "QPlainTextEdit {background-color: #ffffee;}";
        }
    }
    return applyRulesToggle ? applyRules(text) : "";
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

    m_date = QDate::currentDate();
    m_date.setDate(m_date.year(), m_date.month(), 1);
    int day = m_date.dayOfWeek() % DAYS_PER_WEEK;

    m_textFields =  new CTextEdit * [Month::MAX_DAYS];
    m_labelDays = new QLabel * [DAYS_PER_WEEK];
    m_labelNumbers = new QLabel * [Month::MAX_DAYS];

    QString text = QString("%1 %2").arg(getMonthName(m_date.month())).arg(m_date.year());
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
        for (x = 0; x < DAYS_PER_WEEK; x++) {
            dd = x + y * DAYS_PER_WEEK;
            m_textFields[dd] =  new CTextEdit(ui->centralWidget);
            m_textFields[dd]->setObjectName(QString::fromUtf8("textFields") + QString::number(dd));
            m_textFields[dd]->setGeometry(QRect(baseX + (len + space) * x, baseY + y * (hei + space), len, hei));
            m_textFields[dd]->setFont(font);
            m_textFields[dd]->setMouseTracking(false);
            m_textFields[dd]->setFrameShape(QFrame::Box);
            m_textFields[dd]->setMidLineWidth(1);
            if (dd >= day && day_count <= getMonthSize(m_date.year(), m_date.month())) {
                text = QString("   " + QString::number(day_count));
                m_textFields[dd]->setVisible(true);
            } else {
                text = QString("");
                m_textFields[dd]->setVisible(false);
            }
            day_count++;
            QString dayText = "";
            m_textFields[dd]->setStyleSheet(colorDate(day_count, dayText));
            m_labelNumbers[dd] = new QLabel(ui->centralWidget);
            m_labelNumbers[dd]->setObjectName(QString::fromUtf8("labelNum") + QString::number(dd));
            m_labelNumbers[dd]->setGeometry(QRect(baseX + (len + space) * x + len - 32, baseY + y * (hei + space) - space, 32, 10));
            m_labelNumbers[dd]->setText(text);
            m_labelNumbers[dd]->setStyleSheet("QLabel { background-color : blue; color : yellow; }");
            m_labelNumbers[dd]->setVisible(!text.isEmpty());
            QObject::connect(m_textFields[dd], &QPlainTextEdit::textChanged,
                             this, &MainWindow::colorTextbox);
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

    for (x = 0; x < DAYS_PER_WEEK; x++) {
        m_labelDays[x] = new QLabel(ui->centralWidget);
        m_labelDays[x]->setObjectName(QString::fromUtf8("labelDay") + QString::number(x));
        m_labelDays[x]->setGeometry(QRect(baseX + (len + space) * x, baseY - space, 32, 10));
        m_labelDays[x]->setText(days[x].mid(0,3));
    }
}

void MainWindow::colorTextbox()
{
    auto sender = static_cast<CTextEdit *>(QObject::sender());
    auto text = sender->toPlainText();
    sender->setStyleSheet(colorDate(sender->day(), text));
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
    saveCurrentMonth();
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

    QString text = QString("%1 %2").arg(getMonthName(m_date.month())).arg(m_date.year());
    ui->labelMonth->setText(text);

    int day = m_date.dayOfWeek() % DAYS_PER_WEEK;
    int x, y, dd;
    int day_count = 1;
    int monthSize = getMonthSize(m_date.year(), m_date.month());
    int monthIndex = m_doc.findMonth(m_date.year(), m_date.month());

    for (y = 0; y < 6; y++) {
        for (x = 0; x < DAYS_PER_WEEK; x++) {
            dd = x + y * DAYS_PER_WEEK;
            QString dayText = "";
            if (dd >= day && day_count <= monthSize) {
                text = QString("   %1").arg(day_count);
                day_count++;
                m_textFields[dd]->setVisible(true);
                if (monthIndex != -1) {
                    dayText = m_doc.getDailyText(monthIndex, dd);
                    m_textFields[dd]->setPlainText(dayText);
                }
                else {
                    m_textFields[dd]->setPlainText("");
                }
                m_textFields[dd]->setStyleSheet(colorDate(day_count, dayText));
                m_textFields[dd]->setDay(day_count);
            } else {
                text = "";
                m_textFields[dd]->setVisible(false);
            }
            m_labelNumbers[dd]->setText(text);
            m_labelNumbers[dd]->setVisible(!text.isEmpty());
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
        for (int x = 0; x < DAYS_PER_WEEK; x ++) {
            int dd = x + y * DAYS_PER_WEEK;
            m_textFields[dd]->setGeometry(QRect(baseX + (len + spaceX) * x, baseY + y * (hei + spaceY), len, hei));
            m_labelNumbers[dd]->setGeometry(QRect(baseX + (len + spaceX) * x + len - 32, baseY + y * (hei + spaceY) - spaceY, 32, 10));
        }
    }

    for (int x = 0; x < 7; x++) {
        m_labelDays[x]->setGeometry(QRect(baseX + (len + spaceX) * x, baseY - spaceY, 32, 10));
    }
}

void MainWindow::saveCurrentMonth()
{
    m_doc.saveMonth(m_date.year(), m_date.month(), m_textFields);
}

void MainWindow::on_actionPrevious_triggered()
{
    saveCurrentMonth();
    m_date = m_date.addMonths(-1);
    updateCalendar();
}

void MainWindow::on_actionNext_triggered()
{
    saveCurrentMonth();
    m_date = m_date.addMonths(1);
    updateCalendar();
}

void MainWindow::on_actionCurrent_triggered()
{
    saveCurrentMonth();
    m_date = QDate::currentDate();
    m_date.setDate(m_date.year(), m_date.month(), 1);
    updateCalendar();
}

void MainWindow::on_actionOpen_triggered()
{
    saveCurrentMonth();
    open("");
}

void MainWindow::on_actionSave_triggered()
{
    saveCurrentMonth();
    save();
}

void MainWindow::on_actionSave_as_triggered()
{
    saveCurrentMonth();
    if (saveAs()) {
        if (!m_doc.writeFile() || !updateTitle())  {
            warningMessage(tr("Can't write file"));
        }
    }
    updateTitle();
}

void MainWindow::on_actionNew_File_triggered()
{
    saveCurrentMonth();
    newFile();
}

bool MainWindow::newFile()
{
    if (maybeSave()) {
        m_doc.empty();
        updateTitle();
        updateCalendar();
    }
    return true;
}

void MainWindow::openRecentFile()
{
    saveCurrentMonth();
    QAction *action = qobject_cast<QAction *>(sender());
    if (action)
        open(action->data().toString());
}

bool MainWindow::maybeSave()
{
    if (m_doc.isDirty()) {
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
            QString oldFileName = m_doc.getFileName();
            m_doc.setFileName(fileName);
            if (!m_doc.readFile())  {
                warningMessage(tr("cannot open file: %1").arg(m_doc.getLastError()));
                m_doc.setFileName(oldFileName);
                // update fileList
                QSettings settings(author, appName);
                QStringList files = settings.value("recentFileList").toStringList();
                files.removeAll(fileName);
                settings.setValue("recentFileList", files);
                updateRecentFileActions();
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
    QString oldFileName = m_doc.getFileName();
    if (m_doc.isUntitled()) {
        if (!saveAs())
            return false;
    }

    if (!m_doc.writeFile() || !updateTitle())  {
        warningMessage(tr("Can't write file"));
        m_doc.setFileName(oldFileName);
        return false;
    }
    return true;
}

bool MainWindow::saveAs()
{
    QFileDialog* fd = new QFileDialog( this,  tr("Save As"));
    fd->setNameFilter(tr(fileFilter));
    fd->setDefaultSuffix("pca");
    fd->setFileMode(QFileDialog::AnyFile);
    fd->setAcceptMode(QFileDialog::AcceptSave);
    fd->selectFile(m_doc.getFileName());
    QString fileName = "";
    if ( fd->exec() == QDialog::Accepted ) {
       QStringList list = fd->selectedFiles();
       fileName = list.first();
    }
    delete fd;
    if (fileName.isEmpty())
        return false;
    m_doc.setFileName(fileName);
    return true;
}

bool MainWindow::updateTitle()
{
    QString file;
    if (m_doc.getFileName().isEmpty()) {
        file = tr("untitled");
    } else {
        file = QFileInfo(m_doc.getFileName()).fileName();
    }

    m_doc.setDirty(false);
    setWindowTitle(tr("%1[*] - %2").arg(file, appName));

    // update fileList
    QSettings settings(author, appName);
    QStringList files = settings.value("recentFileList").toStringList();
    QString fileName = m_doc.getFileName();
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

    int numRecentFiles = qMin(files.size(), static_cast<int>(MaxRecentFiles));
    for (int i = 0; i < numRecentFiles; ++i) {
        QString text = tr("&%1 %2").arg(i + 1).arg(QFileInfo(files[i]).fileName());
        m_recentFileActs[i]->setText(text);
        m_recentFileActs[i]->setData(files[i]);
        m_recentFileActs[i]->setVisible(true);
        m_recentFileActs[i]->setStatusTip(files[i]);
    }
    for (int j = numRecentFiles; j < MaxRecentFiles; ++j)
        m_recentFileActs[j]->setVisible(false);

    m_separatorAct[0]->setVisible(numRecentFiles > 0);
}

void MainWindow::initFileMenu()
{
    ui->menu_File->addSeparator();
    for (int i = 0; i < MaxRecentFiles; i++) {
        m_recentFileActs[i] = new QAction(this);
        m_recentFileActs[i]->setVisible(false);
        ui->menu_File->addAction(m_recentFileActs[i]);
        connect(m_recentFileActs[i], SIGNAL(triggered()),
                this, SLOT(openRecentFile()));
    }

    m_separatorAct[0] = ui->menu_File->addSeparator();
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
    QString text = QString("%1 %2").arg(getMonthName(m_date.month())).arg(m_date.year());
    dialog->setWindowTitle(text);
    QString oldText = m_doc.getMemo(m_date.year(), m_date.month());
    dialog->memoText->setPlainText(oldText);
    if (dialog->exec()) {
        QString newText = Document::restoreCR(dialog->memoText->toPlainText());
        if (newText != oldText) {
            saveCurrentMonth();
            m_doc.setMemo(m_date.year(), m_date.month(), newText);
            m_doc.setDirty(true);
        }
    }
    delete dialog;
}

void MainWindow::warningMessage(QString message)
{
    QMessageBox msgBox(this);
    msgBox.setText(message);
    msgBox.setIcon(QMessageBox::Warning);
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

void MainWindow::on_actionhighlight_toggled(bool arg1)
{
    QSettings settings(author, appName);
    highlightToggle = arg1;
    settings.setValue("visual/highlight_toggle", arg1);
    settings.sync();
    saveCurrentMonth();
    updateCalendar();
}

void MainWindow::on_actiongrey_out_past_toggled(bool arg1)
{
    QSettings settings(author, appName);
    greyOutPastToggle = arg1;
    settings.setValue("visual/grey_out_past_toggle", arg1);
    settings.sync();
    saveCurrentMonth();
    updateCalendar();
}

void MainWindow::on_actionApply_rules_toggled(bool arg1)
{
    QSettings settings(author, appName);
    applyRulesToggle = arg1;
    settings.setValue("visual/apply_rules_toggle", arg1);
    settings.sync();
    saveCurrentMonth();
    updateCalendar();
}

void MainWindow::restoreSettings()
{
    QSettings settings(author, appName);
    highlightToggle = settings.value("visual/highlight_toggle").value<bool>();
    greyOutPastToggle = settings.value("visual/grey_out_past_toggle").value<bool>();
    applyRulesToggle = settings.value("visual/apply_rules_toggle").value<bool>();
}

void MainWindow::on_actionMonth_Go_to_year_triggered()
{
    int year = m_date.year();
    bool ok;
    QString text =
        QInputDialog::getText(this, tr("Go to Year"), tr("Year:"),
                              QLineEdit::Normal, QString("%1").arg(year), &ok);
    enum { MIN_YEAR = 1900, MAX_YEAR = 2200 };
    if (ok && !text.isEmpty()) {
      int newYear = text.toInt();
      if (newYear == year) {
        return;
      }
      if (newYear > MIN_YEAR && newYear <= MAX_YEAR) {
        saveCurrentMonth();
        m_date.setDate(newYear, m_date.month(), m_date.day());
        updateCalendar();
      } else {
        warningMessage(
            tr("Year must be between %1 and %2.").arg(MIN_YEAR).arg(MAX_YEAR));
      }
     }
}
