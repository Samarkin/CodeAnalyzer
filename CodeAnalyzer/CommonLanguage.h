#ifndef COMMONLANGUAGES_H
#define COMMONLANGUAGES_H

#include "Language.h"

struct CommonLanguage
{
    static const Language* const CPlusPlus;
    static const Language* const CSharp;
    static const Language* const Makefile;
    static const Language* const JavaScript;
    static const Language* const Xml;

    static const Language* const All[5];
};

#endif // COMMONLANGUAGES_H
