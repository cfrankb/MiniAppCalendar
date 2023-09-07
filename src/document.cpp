#include "document.h"
#include <QString>
#include <QtWidgets/QPlainTextEdit>
#include <QtDebug>


QString signature = "MiniApp Calendar v1.0";

Document::Document()
{
    m_lastError = ERR_NONE;
    init();
}

Document::~Document()
{
    forget();
}

void Document::forget()
{
    delete [] m_dailyText;
}

void Document::empty()
{
    m_totalMonths = 0;
    m_fileName = "";
    m_dirty = false;
}

void Document::init()
{
    m_totalMonths = 0;
    m_maxMonths = GROW_BY;
    m_fileName = "";
    m_dirty = false;

    m_dailyText = new Month[static_cast<unsigned int>(m_maxMonths)];
}

int Document::findMonth(int year, int month) {
    for (int i=0; i < m_totalMonths; i++) {
        Month & curr = m_dailyText[i];
        if (curr.month()  == month && curr.year() == year)
            return i;
    }
    return NOT_FOUND;
}

QString Document::getMemo(int year, int month) {
    int i = findMonth(year, month);
    if (i != NOT_FOUND) {
        return m_dailyText[i].memo();
    }
    else {
        return "";
    }
}

void Document::setMemo(int year, int month, QString memo) {
    int i = findMonth(year, month);
    if (i != NOT_FOUND) {
        return m_dailyText[i].memo(memo);
    }
    else {
        Month &curr = m_dailyText[m_totalMonths];
        curr.memo(memo);
        curr.year(year);
        curr.month(month);
        m_totalMonths ++;
        if (m_totalMonths == m_maxMonths) {
            resize(m_maxMonths + GROW_BY);
        }
    }
}

QString Document::getDailyText(int monthIndex, int day) {
    return m_dailyText[monthIndex][day];
}

void Document::resize(int size) {
    m_maxMonths = qMax(size, m_maxMonths);
    Month *months = new Month[m_maxMonths];
    for (int i=0; i < m_totalMonths; i++) {
        months[i] = m_dailyText[i];
    }
    forget();
    m_dailyText = months;
}

void Document::saveMonth(int year, int month, QPlainTextEdit **textFields)
{
    int monthIndex = findMonth(year, month);
    bool original = monthIndex == -1;

    if (original) {
        monthIndex = m_totalMonths;
        Month &curr = m_dailyText[m_totalMonths];
        curr.month(month);
        curr.year(year);
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
            ||  (!original && (m_dailyText[monthIndex][day] != str)))) {
            hasContent = true;
        }

        m_dailyText[monthIndex][day] = str;
    }

    if (original && hasContent) {
        m_totalMonths ++;
        if (m_totalMonths == m_maxMonths) {
            resize(m_maxMonths + GROW_BY);
        }
    }

    if (!m_dirty) {
        m_dirty = hasContent;
    }
}

bool Document::writeFile()
{
    QByteArray ba = m_fileName.toLocal8Bit();
    FILE *tfile = fopen(ba.constData(), "wb");
    if (!tfile) {
        qDebug() << "write error" << "\n";
        m_lastError = ERR_WRITE;
        return false;
    }

    writeString(tfile, signature);
    writeInt(tfile, m_totalMonths);

    for (int i = 0; i < m_totalMonths; i++) {
        Month &curr = m_dailyText[i];
        writeInt(tfile, curr.year());
        writeInt(tfile, curr.month());
        writeString(tfile, curr.memo());
        for (int x = 0; x < Month::MAX_DAYS; x++) {
            writeString(tfile, m_dailyText[i][x]);
        }
    }

    writeInt(tfile, 0);
    fclose(tfile);
    return true;
}

bool Document::readFile()
{
    QByteArray ba = m_fileName.toLocal8Bit();
    FILE *sfile = fopen(ba.constData(), "rb");
    if (!sfile) {
        qDebug() << "read error" << "\n";
        m_lastError = ERR_READ;
        return false;
    }

    QString str = readString(sfile);
    if (str != signature) {
        qDebug() << "wrong signature" << "\n";
        m_lastError = ERR_SIGNATURE;
        return false;
    }

    forget();
    int size = readInt(sfile);
    m_totalMonths = 0;
    m_maxMonths = size + GROW_BY;
    m_dailyText = new Month[m_maxMonths];
    while (size != 0) {
        Month &curr = m_dailyText[m_totalMonths];
        curr.year(readInt(sfile));
        curr.month(readInt(sfile));
        char *t = readString(sfile);
        curr.memo(t);
        delete []t;
        for (int x = 0; x < Month::MAX_DAYS; x++) {
            char *t = readString(sfile);
            m_dailyText[m_totalMonths][x] = t;
            delete []t;
        }
        m_totalMonths++;
        size--;
    }
    fclose(sfile);
    return true;
}

void Document::writeString(FILE *tfile, QString str)
{
    int size =  strlen(str.toStdString().c_str());
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

char * Document::readString(FILE *sfile)
{
    unsigned long size = 0;
    fread(&size, 1, 1, sfile);
    //TODO: implement full 32bits
    if (size == 255) {
        fread(&size, 2, 1, sfile);
    }
    char *buf = new char[size + 1];
    buf[size] = 0;
    fread(buf, size, 1, sfile);
    return buf;
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
    return m_lastError;
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

int Document::getSize() {
    return m_totalMonths;
}

QString Document::getFileName() {
    return m_fileName;
}

void Document::setFileName(QString t) {
    m_fileName = t;
}

bool Document::isDirty() {
    return m_dirty;
}

bool Document::isUntitled() {
    return m_fileName.isEmpty();
}

void Document::setDirty(bool b) {
    m_dirty = b;
}
