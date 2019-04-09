#ifndef COMMONLANGUAGES_H
#define COMMONLANGUAGES_H

#include "Language.h"

struct CommonLanguage
{
    static const Language* const CPlusPlus;
    static const Language* const CSharp;
    static const Language* const CSS;
    static const Language* const Go;
    static const Language* const Makefile;
    static const Language* const Java;
    static const Language* const JavaScript;
    static const Language* const Json;
    static const Language* const Python;
    static const Language* const Swift;
    static const Language* const Markdown;
    static const Language* const Xml;

    static const Language* const All[12];
};

#endif // COMMONLANGUAGES_H
