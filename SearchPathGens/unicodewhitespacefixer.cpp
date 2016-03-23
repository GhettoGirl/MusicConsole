#include "unicodewhitespacefixer.hpp"

const QList<ushort> UnicodeWhitespaceFixer::d_whitespaces =
{
    // Whitespace (Unicode character property "WSpace=Y")
    0x0009, // character tabulation
    0x000A, // line feed
    0x000B, // line tabulation
    0x000C, // device control two
    0x000D, // device control three
    0x0020, // space
    0x0080, // next line
    0x00A0, // no-break space
    0x1680, // ogham space mark
    0x2000, // en quad
    0x2001, // em quad
    0x2002, // en space
    0x2003, // em space
    0x2004, // three-per-em space
    0x2005, // four-per-em space
    0x2006, // six-per-em space
    0x2007, // figure space
    0x2008, // punctuation space
    0x2009, // thin space
    0x200A, // hair space
    0x2028, // line separator
    0x2029, // paragraph separator
    0x202F, // narrow no-break space
    0x205F, // medium mathematical space
    0x3000, // ideographic space (mainly CKJ)

    // Related characters
    0x180E, // mongolian vowel separator
    0x200B, // zero width space
    0x200C, // zero width non-joiner
    0x200D, // zero width joiner
    0x2060, // word joiner
    0xFEFF  // zero width non-breaking space
};

QStringList UnicodeWhitespaceFixer::processString(const QString &str) const
{
    QString data = str;
    int size = data.size();

    for (int i = 0; i < size; i++)
        if (this->isWhitespace(data.at(i).unicode()))
            data.replace(i, 1, QChar(0x20));

    return QStringList(data);
}

void UnicodeWhitespaceFixer::processTextFileData(QString *bytes) const
{
    if (bytes->isEmpty())
        return;

    int size = bytes->size();

    for (int i = 0; i < size; i++)
        if (this->isWhitespace(bytes->at(i).unicode()) &&
                bytes->at(i) != '\n')
            bytes->replace(i, 1, QChar(0x20));
}

bool UnicodeWhitespaceFixer::isWhitespace(const ushort &c) const
{
    // first check using Qt, if this returns false check against my whitespace set
    if (QChar(c).isSpace())
        return true;

    // Define temporary buffers
    ushort _data_array[1];
    ushort _char_array[1] = {c};

    /// use unicode map from qt for conversion
    // eg: QString::fromUtf16(_data_array, 1);
    // the first value of _data_array holds the current whitespace character
    // the first value of _char_array holds the character 'c' from params

    for (int i = 0; i < this->d_whitespaces.size(); i++)
    {
        _data_array[0] = this->d_whitespaces.at(i);
        if (QString::fromUtf16(_data_array, 1) == QString::fromUtf16(_char_array, 1))
            return true;
    }

    // No match
    return false;
}
