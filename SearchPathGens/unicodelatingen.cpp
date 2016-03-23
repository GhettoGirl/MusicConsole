#include "unicodelatingen.hpp"

#define LATIN_CHAR(latin1, fullwidth) \
    {latin1, L##fullwidth}
#define MULTI_CHAR(latin1, fullwidth) \
    {L##latin1, L##fullwidth}

const QMap<QChar, QChar> UnicodeLatinGen::d_latinmap = {

    // upper-case
    LATIN_CHAR('A','Ａ'), LATIN_CHAR('B','Ｂ'), LATIN_CHAR('C','Ｃ'), LATIN_CHAR('D','Ｄ'), LATIN_CHAR('E','Ｅ'),
    LATIN_CHAR('F','Ｆ'), LATIN_CHAR('G','Ｇ'), LATIN_CHAR('H','Ｈ'), LATIN_CHAR('I','Ｉ'), LATIN_CHAR('J','Ｊ'),
    LATIN_CHAR('K','Ｋ'), LATIN_CHAR('L','Ｌ'), LATIN_CHAR('M','Ｍ'), LATIN_CHAR('N','Ｎ'), LATIN_CHAR('O','Ｏ'),
    LATIN_CHAR('P','Ｐ'), LATIN_CHAR('Q','Ｑ'), LATIN_CHAR('R','Ｒ'), LATIN_CHAR('S','Ｓ'), LATIN_CHAR('T','Ｔ'),
    LATIN_CHAR('U','Ｕ'), LATIN_CHAR('V','Ｖ'), LATIN_CHAR('W','Ｗ'), LATIN_CHAR('X','Ｘ'), LATIN_CHAR('Y','Ｙ'),
    LATIN_CHAR('Z','Ｚ'),

    // lower-case
    LATIN_CHAR('a','ａ'), LATIN_CHAR('b','ｂ'), LATIN_CHAR('c','ｃ'), LATIN_CHAR('d','ｄ'), LATIN_CHAR('e','ｅ'),
    LATIN_CHAR('f','ｆ'), LATIN_CHAR('g','ｇ'), LATIN_CHAR('h','ｈ'), LATIN_CHAR('i','ｉ'), LATIN_CHAR('j','ｊ'),
    LATIN_CHAR('k','ｋ'), LATIN_CHAR('l','ｌ'), LATIN_CHAR('m','ｍ'), LATIN_CHAR('n','ｎ'), LATIN_CHAR('o','ｏ'),
    LATIN_CHAR('p','ｐ'), LATIN_CHAR('q','ｑ'), LATIN_CHAR('r','ｒ'), LATIN_CHAR('s','ｓ'), LATIN_CHAR('t','ｔ'),
    LATIN_CHAR('u','ｕ'), LATIN_CHAR('v','ｖ'), LATIN_CHAR('w','ｗ'), LATIN_CHAR('x','ｘ'), LATIN_CHAR('y','ｙ'),
    LATIN_CHAR('z','ｚ'),

    // numbers
    LATIN_CHAR('0','０'), LATIN_CHAR('1','１'), LATIN_CHAR('2','２'), LATIN_CHAR('3','３'), LATIN_CHAR('4','４'),
    LATIN_CHAR('5','５'), LATIN_CHAR('6','６'), LATIN_CHAR('7','７'), LATIN_CHAR('8','８'), LATIN_CHAR('9','９'),

    // symbols
    LATIN_CHAR('!','！'), LATIN_CHAR('#','＃'), LATIN_CHAR('$','＄'), LATIN_CHAR('%','％'), LATIN_CHAR(',','、'),
    LATIN_CHAR('&','＆'), LATIN_CHAR('(','（'), LATIN_CHAR(')','）'), LATIN_CHAR('*','＄'), LATIN_CHAR('+','＋'),
    LATIN_CHAR('-','－'), LATIN_CHAR('.','．'), LATIN_CHAR('/','／'), LATIN_CHAR(':','：'), LATIN_CHAR(';','；'),
    LATIN_CHAR('<','＜'), LATIN_CHAR('>','＞'), LATIN_CHAR('=','＝'), LATIN_CHAR('?','？'), LATIN_CHAR('~','～'),
    LATIN_CHAR('|','｜'), LATIN_CHAR('{','｛'), LATIN_CHAR('}','｝'), LATIN_CHAR('[','［'), LATIN_CHAR(']','］'),
    LATIN_CHAR('@','＠'), LATIN_CHAR('_','＿'), LATIN_CHAR('^','＾'), LATIN_CHAR('`','｀'),

    LATIN_CHAR('"','”'), LATIN_CHAR('\'','’'), LATIN_CHAR('\\','＼'),

    // other
    MULTI_CHAR('·','・')
};

QStringList UnicodeLatinGen::processString(const QString &str) const
{
    QString latin1 = str;
    QString fullwidth = str;

    for (QChar &c : latin1)
        c = this->toLatin1(c);

    for (QChar &c : fullwidth)
        c = this->toFullwidthLatin(c);

    QStringList list;
    list.append(latin1);
    list.append(fullwidth);

    latin1.clear();
    fullwidth.clear();

    return list;
}

bool UnicodeLatinGen::isLatinChar(const QChar &c)
{
    for (UnicodeLatinGen::iterator i = UnicodeLatinGen::d_latinmap.begin();
         i != UnicodeLatinGen::d_latinmap.end(); ++i)
    {
        if (c != i.key() || c != i.value())
            return false;
    }

    return true;
}

bool UnicodeLatinGen::isLatin1Char(const QChar &c)
{
    for (UnicodeLatinGen::iterator i = UnicodeLatinGen::d_latinmap.begin();
         i != UnicodeLatinGen::d_latinmap.end(); ++i)
    {
        if (c != i.key())
            return false;
    }

    return true;
}

bool UnicodeLatinGen::isFullwidthLatinChar(const QChar &c)
{
    for (UnicodeLatinGen::iterator i = UnicodeLatinGen::d_latinmap.begin();
         i != UnicodeLatinGen::d_latinmap.end(); ++i)
    {
        if (c != i.value())
            return false;
    }

    return true;
}

QChar UnicodeLatinGen::toLatin1(const QChar &c)
{
    for (UnicodeLatinGen::iterator i = UnicodeLatinGen::d_latinmap.begin();
         i != UnicodeLatinGen::d_latinmap.end(); ++i)
    {
        if (c == i.value())
            return i.key();
    }

    return c;
}

QChar UnicodeLatinGen::toFullwidthLatin(const QChar &c)
{
    for (UnicodeLatinGen::iterator i = UnicodeLatinGen::d_latinmap.begin();
         i != UnicodeLatinGen::d_latinmap.end(); ++i)
    {
        if (c == i.key())
            return i.value();
    }

    return c;
}
