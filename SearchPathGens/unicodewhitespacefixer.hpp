#ifndef UNICODEWHITESPACEFIXER_HPP
#define UNICODEWHITESPACEFIXER_HPP

#include <Utils/searchpathgen.hpp>

class UnicodeWhitespaceFixer : public SearchPathGen
{
public:
    QStringList processString(const QString&) const;

    // skips \n char
    void processTextFileData(QString *) const;

private:
    bool isWhitespace(const ushort &c) const;
    static const QList<ushort> d_whitespaces;
};

#endif // UNICODEWHITESPACEFIXER_HPP
