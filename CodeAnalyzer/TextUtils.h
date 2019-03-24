#ifndef UNICODE_H
#define UNICODE_H

#include "FileInfo.h"
#include <QFile>

#define IS_TEXT_CODE_UNIT(unit) (unit > 31 || unit == '\r' || unit == '\n' || unit == '\t')

template<typename codeUnit_t>
bool getCodeUnit(QFile& file, codeUnit_t* pUnit)
{
    return file.read(reinterpret_cast<char*>(pUnit), sizeof *pUnit) == sizeof(codeUnit_t);
}

template<typename codeUnit_t>
bool getCodeUnitAndSwapOctets(QFile& file, codeUnit_t* pUnit);

template<>
inline bool getCodeUnitAndSwapOctets(QFile& file, quint32* pUnit)
{
    if (file.read(reinterpret_cast<char*>(pUnit), sizeof *pUnit) < 4)
    {
        return false;
    }
    *pUnit = ((*pUnit & 0xFF) << 24)
            | ((*pUnit & 0xFF00) << 8)
            | ((*pUnit & 0xFF0000) >> 8)
            | ((*pUnit & 0xFF000000) >> 24);
    return true;
}

template<>
inline bool getCodeUnitAndSwapOctets(QFile& file, quint16* pUnit)
{
    if (file.read(reinterpret_cast<char*>(pUnit), sizeof *pUnit) < 2)
    {
        return false;
    }
    *pUnit = quint16((*pUnit & 0xFF) << 8)
            | ((*pUnit & 0xFF00) >> 8);
    return true;
}

inline Encoding detectEncodingAndAdvanceToFirstCodeUnit(QFile& file)
{
    struct buffer {
        union {
            char raw[4];
            quint32 codeUnits32[1];
            quint16 codeUnits16[2];
            quint8 codeUnits8[4];
        };
    } buffer;

    qint64 buffer_size = file.read(buffer.raw, sizeof(buffer));
    if (buffer_size >= 4 && buffer.codeUnits32[0] == 0xFFFE0000U) {
        // TODO: Don't assume Little Endian CPU - either assert, or support both!
        return Encoding::Utf32BE;
    }
    else if (buffer_size >= 4 && buffer.codeUnits32[0] == 0x0000FEFFU) {
        return Encoding::Utf32LE;
    }
    else if (buffer_size >= 2 && buffer.codeUnits16[0] == 0xFFFEU) {
        file.seek(2);
        return Encoding::Utf16BE;
    }
    else if (buffer_size >= 2 && buffer.codeUnits16[0] == 0xFEFFU) {
        file.seek(2);
        return Encoding::Utf16LE;
    }
    else if (buffer_size >= 3 && buffer.codeUnits8[0] == 0xEF && buffer.codeUnits8[1] == 0xBB && buffer.codeUnits8[2] == 0xBF) {
        file.seek(3);
        return Encoding::Utf8;
    }
    else {
        file.seek(0);
        return Encoding::NoBom;
    }
}

template<typename codeUnit_t>
QString readAllText(QFile& file, bool (*getCodeUnit)(QFile& file, codeUnit_t* pUnit))
{
    QVarLengthArray<QChar> chars;
    codeUnit_t unit;
    while (getCodeUnit(file, &unit))
    {
        if (!IS_TEXT_CODE_UNIT(unit)) return QString{};
        // TODO: Combine UTF-8 and UTF-16 code units into full unicode code points
        chars.append(QChar{unit});
    }
    chars.append(QChar{});
    return QString{chars.data()};
}

#endif // UNICODE_H
