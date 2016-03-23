#ifndef UNIVERSALJAPANESEKANALOOKUP_HPP
#define UNIVERSALJAPANESEKANALOOKUP_HPP

#include <Utils/searchpathgen.hpp>

class UniversalJapaneseKanaLookup : public SearchPathGen
{
public:
    QStringList processString(const QString &) const;
};

#endif // UNIVERSALJAPANESEKANALOOKUP_HPP
