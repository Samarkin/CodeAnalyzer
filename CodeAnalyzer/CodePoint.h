#ifndef CODEPOINT_H
#define CODEPOINT_H

#include <QChar>

class CodePoint
{
    quint32 value;

public:
    CodePoint() = default;

    CodePoint(quint32 value) : value(value) {}

    bool doubleByte() const
    {
        return (value & 0xFFFF0000U) != 0U;
    }

    template<class QCharCollection>
    void appendTo(QCharCollection& s) const
    {
        if (!doubleByte())
        {
            s.append(QChar{quint16(value&0xFFFF)});
        }
        else
        {
            quint32 t = value - 0x10000;
            QChar leadingSurrogate{0xD800U + quint16(t >> 10)};
            QChar trailingSurrogate{0xDC00U + quint16(t & 0x3FF)};
            s.append(leadingSurrogate);
            s.append(trailingSurrogate);
        }
    }

    bool operator==(unsigned char ch) const
    {
        return value == quint32(ch);
    }

    bool operator!=(unsigned char ch) const
    {
        return value != quint32(ch);
    }

    bool operator>(unsigned char ch) const
    {
        return value > quint32(ch);
    }

    bool operator<(unsigned char ch) const
    {
        return value < quint32(ch);
    }
};

#endif // CODEPOINT_H
