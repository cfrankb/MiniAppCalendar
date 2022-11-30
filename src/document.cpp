#include "document.h"
#include <QString>

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
    delete [] m_months;
    delete [] m_years;
    delete [] m_memos;
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
    m_maxMonths = 100;
    m_fileName = "";
    m_dirty = false;

    m_dailyText = new Month[static_cast<unsigned int>(m_maxMonths)];
    m_months = new int[static_cast<unsigned int>(m_maxMonths)];
    m_years = new int[static_cast<unsigned int>(m_maxMonths)];
    m_memos = new QString[static_cast<unsigned int>(m_maxMonths)];
}

int Document::findMonth(int year, int month) {
    for (int i=0; i < m_totalMonths; i++)
        if (m_months[i] == month && m_years[i] == year)
            return i;
    return -1;
}

QString Document::getMemo(int year, int month) {
    int indexMonth = findMonth(year, month);
    if (indexMonth != -1) {
        return m_memos[indexMonth];
    }
    else {
        return "";
    }
}

void Document::setMemo(int year, int month, QString memo) {
    int indexMonth = findMonth(year, month);
    if (indexMonth != -1) {
        m_memos[indexMonth] = memo;
    }
    else {
        m_memos[m_totalMonths] = memo;
        m_years[m_totalMonths] = year;
        m_months[m_totalMonths] = month;
        m_totalMonths ++;

        if (m_totalMonths == m_maxMonths) {
            resize(m_maxMonths + 100);
        }
    }
}

QString Document::getDailyText(int monthIndex, int day) {
    return m_dailyText[monthIndex][day];
}

void Document::resize(int size) {

    m_maxMonths = qMax(size, m_maxMonths);

    Month *dailyText2 = new Month[m_maxMonths];
    int *months2 = new int[m_maxMonths];
    int *years2 = new int[m_maxMonths];
    QString *memos2 = new QString[m_maxMonths];

    for (int i=0; i < m_totalMonths; i++) {
        dailyText2[i] = m_dailyText[i];
        months2[i] = m_months[i];
        years2[i] = m_years[i];
        memos2[i] = m_memos[i];
    }

    forget();

    m_dailyText = dailyText2;
    m_months = months2;
    m_years = years2;
    m_memos = memos2;
}

void Document::saveMonth(int year, int month, QPlainTextEdit **textFields)
{
    int monthIndex = findMonth(year, month);
    bool original = monthIndex == -1;

    if (original) {
        monthIndex = m_totalMonths;
        m_months[m_totalMonths] = month;
        m_years[m_totalMonths] = year;
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
            resize(m_maxMonths + 100);
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
        writeInt(tfile, m_years[i]);
        writeInt(tfile, m_months[i]);
        writeString(tfile, m_memos[i]);

        for (int x = 0; x < 42; x++) {
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
    m_maxMonths = size + 100;
    m_dailyText = new Month[m_maxMonths];
    m_months = new int[m_maxMonths];
    m_years = new int[m_maxMonths];
    m_memos = new QString[m_maxMonths];
    while (size != 0) {
        m_years[m_totalMonths] = readInt(sfile);
        m_months[m_totalMonths] = readInt(sfile);
        m_memos[m_totalMonths] = readString(sfile);
        for (int x = 0; x < 42; x++) {
            m_dailyText[m_totalMonths][x] = readString(sfile);
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
    return m_fileName == "";
}

void Document::setDirty(bool b) {
    m_dirty = b;
}
