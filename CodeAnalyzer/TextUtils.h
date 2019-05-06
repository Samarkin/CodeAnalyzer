#ifndef UNICODE_H
#define UNICODE_H

#include "FileInfo.h"
#include <QFile>
#include "CodePoint.h"

template<typename codeUnit_t>
inline bool isTextCodeUnit(codeUnit_t unit)
{
    return unit > 31 || unit == '\r' || unit == '\n' || unit == '\t';
}

inline bool isTextCodePoint(CodePoint codePoint)
{
    // lower code points are equal to their code units in all supported encodings
    return isTextCodeUnit(codePoint);
}

template<typename codeUnit_t>
inline bool isWhitespaceCodeUnit(codeUnit_t unit)
{
    // it is a coincidence that UTF-8 representation of NBSP (0xA0) is [0xC2, 0xA0]
    return unit == ' ' || unit == '\t' || unit == 0xA0;
}

inline bool isWhitespaceCodePoint(CodePoint codePoint)
{
    // lower code points are equal to their code units in all supported encodings
    return isWhitespaceCodeUnit(codePoint);
}

template<typename codeUnit_t>
inline bool getCodeUnit(QFile& file, codeUnit_t* pUnit)
{
    return file.read(reinterpret_cast<char*>(pUnit), sizeof *pUnit) == sizeof(codeUnit_t);
}

template<typename codeUnit_t>
inline bool getCodeUnitAndSwapOctets(QFile& file, codeUnit_t* pUnit);

template<>
inline bool getCodeUnitAndSwapOctets(QFile& file, quint32* pUnit)
{
    if (!getCodeUnit(file, pUnit))
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
    if (!getCodeUnit(file, pUnit))
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

inline bool getUtf8CodePoint(QFile& file, CodePoint* pChar)
{
    quint8 firstUnit;
    if (!getCodeUnit(file, &firstUnit)) return false;
    if ((firstUnit & 0x80) == 0x00) {
        *pChar = firstUnit;
        return true;
    }
    quint8 unit;
    if ((firstUnit & 0xE0) == 0xC0) {
        quint32 codePoint = firstUnit & 0x1F;
        if (!getCodeUnit(file, &unit)) return false;
        codePoint <<= 6;
        codePoint |= unit & 0x3F;
        *pChar = codePoint;
        return true;
    }
    if ((firstUnit & 0xF0) == 0xE0) {
        quint32 codePoint = firstUnit & 0x0F;
        if (!getCodeUnit(file, &unit)) return false;
        codePoint <<= 6;
        codePoint |= unit & 0x3F;
        if (!getCodeUnit(file, &unit)) return false;
        codePoint <<= 6;
        codePoint |= unit & 0x3F;
        *pChar = codePoint;
        return true;
    }
    if ((firstUnit & 0xF8) == 0xF0) {
        quint32 codePoint = firstUnit & 0x07;
        if (!getCodeUnit(file, &unit)) return false;
        codePoint <<= 6;
        codePoint |= unit & 0x3F;
        if (!getCodeUnit(file, &unit)) return false;
        codePoint <<= 6;
        codePoint |= unit & 0x3F;
        if (!getCodeUnit(file, &unit)) return false;
        codePoint <<= 6;
        codePoint |= unit & 0x3F;
        *pChar = codePoint;
        return true;
    }
    // Otherwise - assume 8-bit encoding
    *pChar = firstUnit;
    return true;
}

template<bool (*getCodeUnit)(QFile&, quint16*)>
inline bool getUtf16CodePoint(QFile& file, CodePoint* pChar)
{
    quint16 leadingSurrogate;
    if (!getCodeUnit(file, &leadingSurrogate))
    {
        return false;
    }
    if ((leadingSurrogate & 0xFC00) != 0xD800)
    {
        // not a surrogate pair
        *pChar = leadingSurrogate;
        return true;
    }
    quint16 trailingSurrogate;
    if (!getCodeUnit(file, &trailingSurrogate))
    {
        return false;
    }
    if ((trailingSurrogate & 0xFC00) != 0xDC00)
    {
        // illegal surrogate pair
        *pChar = 0xFFFD;
        return true;
    }
    quint32 codePoint = leadingSurrogate & 0x3FF;
    codePoint <<= 10;
    codePoint |= trailingSurrogate & 0x3FF;
    *pChar = codePoint;
    return true;
}

template<bool (*getCodeUnit)(QFile&, quint32*)>
inline bool getUtf32CodePoint(QFile& file, CodePoint* pChar)
{
    quint32 codePoint;
    // For UTF-32, code units == code points
    if (!getCodeUnit(file, &codePoint))
    {
        return false;
    }
    *pChar = codePoint;
    return true;
}

template<template<bool (*)(QFile&, CodePoint*)> class readAllFunc>
bool readAll(QFile& file, typename readAllFunc<getUtf8CodePoint>::return_t* retValue)
{
    Encoding encoding = detectEncodingAndAdvanceToFirstCodeUnit(file);
    switch (encoding) {
    case Encoding::NoBom:
        return readAllFunc<getUtf8CodePoint>::read(file, retValue);
    case Encoding::Utf8:
        return readAllFunc<getUtf8CodePoint>::read(file, retValue);
    case Encoding::Utf16BE:
        return readAllFunc<getUtf16CodePoint<getCodeUnitAndSwapOctets>>::read(file, retValue);
    case Encoding::Utf16LE:
        return readAllFunc<getUtf16CodePoint<getCodeUnit>>::read(file, retValue);
    case Encoding::Utf32BE:
        return readAllFunc<getUtf32CodePoint<getCodeUnitAndSwapOctets>>::read(file, retValue);
    case Encoding::Utf32LE:
        return readAllFunc<getUtf32CodePoint<getCodeUnit>>::read(file, retValue);
    }
}

template<bool (*getCodePoint)(QFile&, CodePoint*)>
struct readAsString
{
    typedef QString return_t;
    static bool read(QFile& file, return_t* string)
    {
        QVarLengthArray<QChar> chars;
        CodePoint codePoint{};
        while (getCodePoint(file, &codePoint))
        {
            if (!isTextCodePoint(codePoint)) return false;
            codePoint.appendTo(chars);
        }
        chars.append(QChar{});
        *string = QString{chars.data()};
        return true;
    }
};

inline bool readAllText(QFile& file, QString *string)
{
    return readAll<readAsString>(file, string);
}

#endif // UNICODE_H
