#ifndef UNICODELATINGEN_HPP
#define UNICODELATINGEN_HPP

#include <QMap>
#include <Utils/searchpathgen.hpp>

/**
 * @brief The UnicodeLatinGen class
 * ================================
 *
 * from Wikipedia
 *   "In CJK (Chinese, Japanese and Korean) computing, graphic characters are traditionally
 *   classed into fullwidth (in Taiwan and Hong Kong: 全形; in CJK and Japanese: 全角) and
 *   halfwidth (in Taiwan and Hong Kong: 半形; in CJK and Japanese: 半角) characters.
 *   With fixed-width fonts, a halfwidth character occupies half the width of a fullwidth character,
 *   hence the name."
 *
 *
 *
 * This SearthPathGen generates a string for the
 *   × Basic-Latin1 form (ASCII) and the
 *   × Fullwidth Latin form
 * of the incoming string.
 *
 *
 *  = Input =                     = Output =
 *
 *   latin1                        latin1 (only), fullwidth (only)
 *   fullwidth                     latin1 (only), fullwidth (only)
 *   latin1 & fullwidth (mixed)    latin1 (only), fullwidth (only)
 *
 *
 *
 * This gen should make it easier to find media which contains fullwidth latin characters in the name.
 * Or if you input fullwidth latin chars in the terminal to find media with only ASCII names.
 *
 *
 * NOTE: contains also the fullwidth numbers and ascii symbols
 *
 */

class UnicodeLatinGen : public SearchPathGen
{
public:
    QStringList processString(const QString &) const;

private:
    static const QMap<QChar, QChar> d_latinmap;
    typedef QMap<QChar, QChar>::const_iterator iterator;

    static bool isLatinChar(const QChar &c);
    static bool isLatin1Char(const QChar &c);
    static bool isFullwidthLatinChar(const QChar &c);

    static QChar toLatin1(const QChar &c);
    static QChar toFullwidthLatin(const QChar &c);
};

#endif // UNICODELATINGEN_HPP
