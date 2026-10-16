#include "document.h"
#include <QString>
#include "textedit.h"
#include <QtDebug>
#include <QRegularExpression>

const static QString signature = "MiniApp Calendar v1.0";

Document::Document()
{
    m_lastError = ERR_NONE;
    init();
}

Document::~Document()
{
    forget();
}

/// @brief Free memory storage
void Document::forget()
{
    delete[] m_dailyText;
}

/// @brief Empty document
void Document::empty()
{
    m_totalMonths = 0;
    m_fileName = "";
    m_dirty = false;
}

/// @brief Initialize document
void Document::init()
{
    m_totalMonths = 0;
    m_maxMonths = GROW_BY;
    m_fileName = "";
    m_dirty = false;

    m_dailyText = new Month[static_cast<unsigned int>(m_maxMonths)];
}

/// @brief Find index for a given month in the array
/// @param year
/// @param month
/// @return
int Document::findMonth(int year, int month)
{
    for (int i = 0; i < m_totalMonths; i++)
    {
        Month &curr = m_dailyText[i];
        if (curr.month() == month && curr.year() == year)
            return i;
    }
    return NOT_FOUND;
}

/// @brief Retrieve monthly memo for a given month
/// @param year
/// @param month
/// @return
QString Document::getMemo(int year, int month)
{
    int i = findMonth(year, month);
    if (i != NOT_FOUND)
    {
        return m_dailyText[i].memo();
    }
    else
    {
        return "";
    }
}

/// @brief Update montly memo for a given month
/// @param year
/// @param month
/// @param memo
void Document::setMemo(int year, int month, QString memo)
{
    int i = findMonth(year, month);
    if (i != NOT_FOUND)
    {
        return m_dailyText[i].memo(memo);
    }
    else
    {
        Month &curr = m_dailyText[m_totalMonths];
        curr.memo(memo);
        curr.year(year);
        curr.month(month);
        m_totalMonths++;
        if (m_totalMonths == m_maxMonths)
        {
            resize(m_maxMonths + GROW_BY);
        }
    }
}

/// @brief Get the text for a given day
/// @param monthIndex
/// @param day
/// @return
QString Document::getDailyText(int monthIndex, int day)
{
    return m_dailyText[monthIndex][day];
}

/// @brief Resize month array to a new size
/// @param size
void Document::resize(int size)
{
    m_maxMonths = qMax(size, m_maxMonths);
    Month *months = new Month[m_maxMonths];
    for (int i = 0; i < m_totalMonths; i++)
    {
        months[i] = m_dailyText[i];
    }
    forget();
    m_dailyText = months;
}

/// @brief Commit current month to memory storage
/// @param year
/// @param month
/// @param textFields
void Document::saveMonth(int year, int month, CTextEdit **textFields)
{
    int monthIndex = findMonth(year, month);
    bool original = monthIndex == -1;

    if (original)
    {
        monthIndex = m_totalMonths;
        Month &curr = m_dailyText[m_totalMonths];
        curr.month(month);
        curr.year(year);
    }

    bool hasContent = false;

    for (int day = 0; day < DAYS_PER_WEEK * ROWS_PER_MONTH; day++)
    {
        QString str = textFields[day]->toPlainText();

        // fix for an old bug in the original MFC implementation of MiniAppCalendar
        if (str == nullptr || textFields[day]->isHidden())
        {
            str = "";
        }

        // quick hack to prevent carriage return from
        // being removed (maintain compatibility).
        str = restoreCR(str);

        if (!textFields[day]->isHidden() && ((original && (str != "")) || (!original && (m_dailyText[monthIndex][day] != str))))
        {
            hasContent = true;
        }

        m_dailyText[monthIndex][day] = str;
    }

    if (original && hasContent)
    {
        m_totalMonths++;
        if (m_totalMonths == m_maxMonths)
        {
            resize(m_maxMonths + GROW_BY);
        }
    }

    if (!m_dirty)
    {
        m_dirty = hasContent;
    }
}

/// @brief Write calendar file to disk
/// @return true for success, false for failure
bool Document::writeFile()
{
    QByteArray ba = m_fileName.toUtf8();
    FILE *tfile = fopen(ba.constData(), "wb");
    if (!tfile)
    {
        qDebug() << "write error" << "\n";
        m_lastError = ERR_WRITE;
        return false;
    }

    writeString(tfile, signature);
    writeInt(tfile, m_totalMonths);

    for (int i = 0; i < m_totalMonths; i++)
    {
        Month &curr = m_dailyText[i];
        writeInt(tfile, curr.year());
        writeInt(tfile, curr.month());
        writeString(tfile, curr.memo());
        for (int x = 0; x < Month::MAX_DAYS; x++)
        {
            writeString(tfile, m_dailyText[i][x]);
        }
    }

    writeInt(tfile, 0);
    fclose(tfile);
    return true;
}

/// @brief Read calendarfile from  disk.
/// @return true for success, false for failure
bool Document::readFile()
{
    QByteArray ba = m_fileName.toUtf8();
    FILE *sfile = fopen(ba.constData(), "rb");
    if (!sfile)
    {
        qDebug() << "read error" << "\n";
        m_lastError = ERR_READ;
        return false;
    }

    QString str;
    if (!readString(sfile, str)) {
        fclose(sfile);
        return false;
    }
    if (str != signature)
    {
        fclose(sfile);
        qDebug() << "wrong signature" << "\n";
        m_lastError = ERR_SIGNATURE;
        return false;
    }

    forget();
    int size = readInt(sfile);
    m_totalMonths = 0;
    m_maxMonths = size + GROW_BY;
    m_dailyText = new Month[m_maxMonths];
    while (size != 0)
    {
        Month &curr = m_dailyText[m_totalMonths];
        curr.year(readInt(sfile));
        curr.month(readInt(sfile));
        QString t;
        if (!readString(sfile,t)) {
            fclose(sfile);
            m_lastError = ERR_READ;
            return false;
        }
        curr.memo(t);
        for (int x = 0; x < Month::MAX_DAYS; x++)
        {
            QString t;
            if (!readString(sfile,t)) {
                fclose(sfile);
                m_lastError = ERR_READ;
                return false;
            }
            m_dailyText[m_totalMonths][x] = t;
        }
        m_totalMonths++;
        size--;
    }
    fclose(sfile);
    return true;
}

/// @brief Write string to the file
/// @param tfile
/// @param str
void Document::writeString(FILE *tfile, const QString &str)
{
    QByteArray ba = str.toUtf8();
    uint32_t size = ba.size();

    if (size < UINT8_MAX) {
        uint8_t b = size;
        fwrite(&b, 1, 1, tfile);
    } else if (size < UINT16_MAX) {
        uint8_t b = UINT8_MAX;
        fwrite(&b, 1, 1, tfile);
        uint16_t w = size;
        fwrite(&w, 1, sizeof(w), tfile);
    } else {
        uint8_t b = UINT8_MAX;
        fwrite(&b, 1, 1, tfile);
        uint16_t w = UINT16_MAX;
        fwrite(&w, 1, sizeof(w), tfile);
        uint32_t ext = size;
        fwrite(&ext, 1, sizeof(ext), tfile);
    }

    if (size)
        fwrite(ba.constData(), 1, size, tfile);
}

/// @brief Read a string from file
/// @param sfile
/// @param str

bool Document::readString(FILE *sfile, QString & str)
{
    str = "";
    uint32_t size = 0;
    if (fread(&size, 1, sizeof(uint8_t), sfile) != sizeof(uint8_t))
        return false;

    if (size == UINT8_MAX) {
        if (fread(&size, 1, sizeof(uint16_t), sfile) != sizeof(uint16_t))
            return false;
        if (size == UINT16_MAX) {
            if (fread(&size, 1, sizeof(uint32_t), sfile) != sizeof(uint32_t))
                return false;
        }
    }
    if (size == UINT32_MAX)        // only value where size + 1 overflows
        return false;

    if (size == 0)
        return true;

    char *buf = new char[size + 1];
    if (!buf) return false;
    size_t got = fread(buf, 1, size, sfile);
    if (got != size) {
        delete [] buf;
        return false;
    }
    buf[size] = 0;
    str = QString::fromUtf8(buf);
    delete []buf;
    return true;
}

/// @brief Read interger value from file
/// @param sfile
/// @return integer value
int Document::readInt(FILE *sfile)
{
    int v;
    fread(&v, 4, 1, sfile);
    return v;
}

/// @brief Write interger value to file
/// @param tfile
/// @param v
void Document::writeInt(FILE *tfile, int v)
{
    fwrite(&v, 4, 1, tfile);
}

/// @brief Return text of the last error triggers
/// @return
int Document::getLastError() const
{
    return m_lastError;
}

/// @brief Change linux EOL to Windows EOL. Preserve compatibility with MFC implementation.
/// @param str
/// @return
QString Document::restoreCR(QString str)
{
    str.replace(QRegularExpression(QStringLiteral("\\r?\\n")), QStringLiteral("\r\n"));
    return str;
}

/// @brief Retrieve total number of months stored
/// @return
int Document::getSize()
{
    return m_totalMonths;
}

/// @brief Retrieve the filepath for this document
/// @return
QString Document::getFileName()
{
    return m_fileName;
}

/// @brief Set the filepath for this document
/// @param t
void Document::setFileName(QString t)
{
    m_fileName = t;
}

/// @brief Returns state Dirty State. Indicating the document was modified.
/// @return
bool Document::isDirty()
{
    return m_dirty;
}

/// @brief Is the filename defined for this document.
/// @return
bool Document::isUntitled()
{
    return m_fileName.isEmpty();
}

/// @brief Set/Clear Dirty Flag
/// @param b
void Document::setDirty(bool b)
{
    m_dirty = b;
}
