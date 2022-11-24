#include "document.h"
#include <QString>

static QString signature = "MiniApp Calendar v1.0";

Document::Document()
{
    lastError = ERR_NONE;
    init();
}

Document::~Document()
{
    forget();
}

void Document::forget()
{
    delete [] dailyText;
    delete [] months;
    delete [] years;
    delete [] memos;
}

void Document::empty()
{
    totalMonths = 0;
    fileName = "";
    dirty = false;
}

void Document::init()
{
    totalMonths = 0;
    maxMonths = 100;
    fileName = "";
    dirty = false;

    dailyText = new Month[static_cast<unsigned int>(maxMonths)];
    months = new int[static_cast<unsigned int>(maxMonths)];
    years = new int[static_cast<unsigned int>(maxMonths)];
    memos = new QString[static_cast<unsigned int>(maxMonths)];
}

int Document::findMonth(int year, int month) {
    for (int i=0; i < totalMonths; i++)
        if (months[i] == month && years[i] == year)
            return i;
    return -1;
}

QString Document::getMemo(int year, int month) {
    int indexMonth = findMonth(year, month);
    if (indexMonth != -1) {
        return memos[indexMonth];
    }
    else {
        return "";
    }
}

void Document::setMemo(int year, int month, QString memo) {
    int indexMonth = findMonth(year, month);
    if (indexMonth != -1) {
        memos[indexMonth] = memo;
    }
    else {
        memos[totalMonths] = memo;
        years[totalMonths] = year;
        months[totalMonths] = month;
        totalMonths ++;

        if (totalMonths == maxMonths) {
            resize(maxMonths + 100);
        }
    }
}

QString Document::getDailyText(int monthIndex, int day) {
    return dailyText[monthIndex][day];
}

void Document::resize(int size) {

    maxMonths = qMax(size, maxMonths);

    Month *dailyText2 = new Month[maxMonths];
    int *months2 = new int[maxMonths];
    int *years2 = new int[maxMonths];
    QString *memos2 = new QString[maxMonths];

    for (int i=0; i < totalMonths; i++) {
        dailyText2[i] = dailyText[i];
        months2[i] = months[i];
        years2[i] = years[i];
        memos2[i] = memos[i];
    }

    forget();

    dailyText = dailyText2;
    months = months2;
    years = years2;
    memos = memos2;
}

void Document::saveMonth(int year, int month, QPlainTextEdit **textFields)
{
    int monthIndex = findMonth(year, month);
    bool original = monthIndex == -1;

    if (original) {
        monthIndex = totalMonths;
        months[totalMonths] = month;
        years[totalMonths] = year;
    }

    bool hasContent = false;

    for (int day=0; day < 41; day ++) {
        QString str = textFields[day]->toPlainText();

        // fix for an old bug in the original MFC implementation of MiniAppCalendar
        if (str == nullptr || textFields[day]->isHidden()){
            str = "";
        }

        // quick hack to prevent carriage return from
        // being removed (maintain compatibility).
        str = restoreCR(str);

        if (!textFields[day]->isHidden() && ((original && (str != ""))
            ||  (!original && (dailyText[monthIndex][day] != str)))) {
            hasContent = true;
        }

        dailyText[monthIndex][day] = str;
    }

    if (original && hasContent) {
        totalMonths ++;
        if (totalMonths == maxMonths) {
            resize(maxMonths + 100);
        }
    }

    if (!dirty) {
        dirty = hasContent;
    }
}

bool Document::writeFile()
{
    QByteArray ba = fileName.toLocal8Bit();// fileName.toAscii();
    FILE *tfile = fopen(ba.constData(), "wb");
    if (!tfile) {
        qDebug() << "write error" << "\n";
        lastError = ERR_WRITE;
        return false;
    }

    writeString(tfile, signature);
    writeInt(tfile, totalMonths);

    for (int i = 0; i < totalMonths; i++) {
        writeInt(tfile, years[i]);
        writeInt(tfile, months[i]);
        writeString(tfile, memos[i]);

        for (int x = 0; x < 42; x++) {
            writeString(tfile, dailyText[i][x]);
        }
    }

    writeInt(tfile, 0);
    fclose(tfile);
    return true;
}

bool Document::readFile()
{
    QByteArray ba = fileName.toLocal8Bit();// fileName.toAscii();
    FILE *sfile = fopen(ba.constData(), "rb");
    if (!sfile) {
        qDebug() << "read error" << "\n";
        lastError = ERR_READ;
        return false;
    }

    QString str = readString(sfile);
    if (str != signature) {
        qDebug() << "wrong signature" << "\n";
        lastError = ERR_SIGNATURE;
        return false;
    }

    forget();
    int size = readInt(sfile);
    totalMonths = 0;
    maxMonths = size + 100;
    dailyText = new Month[maxMonths];
    months = new int[maxMonths];
    years = new int[maxMonths];
    memos = new QString[maxMonths];
    while (size != 0) {
        years[totalMonths] = readInt(sfile);
        months[totalMonths] = readInt(sfile);
        memos[totalMonths] = readString(sfile);
        for (int x = 0; x < 42; x++) {
            dailyText[totalMonths][x] = readString(sfile);
        }
        totalMonths++;
        size--;
    }
    fclose(sfile);
    return true;
}

void Document::writeString(FILE *tfile, QString str)
{
    int size = str.length();
    // TODO implement full 32 bits
    if (size < 255) {
        fwrite(&size, 1, 1, tfile);
    } else {
        unsigned char b = 255;
        fwrite(&b, 1, 1, tfile);
        fwrite(&size, 2, 1, tfile);
    }
    QByteArray ba = str.toLocal8Bit();//str.toAscii();
    fwrite(ba.constData(), size, 1, tfile);
}

QString Document::readString(FILE *sfile)
{
    unsigned int size = 0;
    fread(&size, 1, 1, sfile);
    //TODO: implement full 32bits
    if (size == 255) {
        fread(&size, 2, 1, sfile);
    }
    char buf[size + 1];
    buf[size] = 0;
    fread(buf, size, 1, sfile);
    return QString(buf);
}

int Document::readInt(FILE *sfile)
{
    int v;
    fread(&v, 4, 1, sfile);
    return v;
}

void Document::writeInt(FILE *tfile, int v)
{
    fwrite(&v, 4, 1, tfile);
}

int Document::getLastError() const
{
    return lastError;
}

QString Document::restoreCR(QString str)
{
    if (str.lastIndexOf("\n")!= -1 &&
        str.lastIndexOf("\r\n") == -1)  {

        QString tmp;
        for (int i=0; i<str.length(); i++) {
            if (str[i] != '\n') tmp += str[i];
            else tmp += "\r\n";
        }
        str = tmp;
    }
    return str;
}
