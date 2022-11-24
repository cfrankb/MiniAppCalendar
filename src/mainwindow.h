#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtDebug>
//#include <QtGui/QMainWindow>
//#include <QtGui/QPlainTextEdit>
//#include <QtGui/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QLabel>
#include <QDate>
#include "document.h"

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void setDocument( const QString& fileName );

private:
    enum { MaxRecentFiles = 4 };

    Ui::MainWindow *ui;
    QDate date;
    QPlainTextEdit **textFields;
    QLabel **labelDays;
    QLabel **labelNumbers;
    QAction *recentFileActs[MaxRecentFiles];
    QAction *separatorAct[1];
    const char *colorDate(int day_count, QString &text);
    const char *applyRules(const QString &text);

    Document doc;
    bool ready;

    bool highlightToggle;
    bool greyOutPastToggle;
    bool applyRulesToggle;

    void restoreSettings();

    QString getMonthName(int);
    int getMonthSize(int, int);
    void updateCalendar();
    void resizeControls(int width, int height);
    bool updateTitle();
    void updateRecentFileActions();
    void initCalendar();
    void initFileMenu();
    void initToolBar();
    void warningMessage(QString message);
    QString getErrorMessage(int msg) const;

    // file actions

    bool newFile();
    bool maybeSave();
    bool save();
    bool saveAs();
    bool saveFile();
    bool open(QString fileName="");

    // events

    void closeEvent(QCloseEvent *event);
    void resizeEvent (QResizeEvent *event);

private slots:
    void on_actionMemo_triggered();
    void on_actionNew_File_triggered();
    void on_actionSave_as_triggered();
    void on_actionSave_triggered();
    void on_actionCurrent_triggered();
    void on_actionNext_triggered();
    void on_actionPrevious_triggered();
    void on_actionOpen_triggered();
    void openRecentFile();

    void on_actionAbout_Qt_triggered();
    void on_actionAbout_triggered();
    void on_actionhighlight_toggled(bool arg1);
    void on_actiongrey_out_past_toggled(bool arg1);

    void colorTextbox();
    void on_actionApply_rules_toggled(bool arg1);
};

#endif // MAINWINDOW_H
