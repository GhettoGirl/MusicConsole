#include "searchkeys.hpp"

#include <SearchPathGens/unicodewhitespacefixer.hpp>

SearchKeys::SearchKeys(const QString &search_term)
{
    // extended search patterns
    if (search_term.contains('|'))
    {

        QStringList tmp = search_term.split('|', QString::SkipEmptyParts);
        if (!tmp.isEmpty())
        {
            this->m_searchPattern = this->createSearchPattern(tmp.at(0));
            tmp.removeFirst();
        }

        for (const QString &t : tmp)
        {

            /// |a term
            if (t.startsWith("a "))
            {
                SearchPattern searchPattern;
                searchPattern.searchPattern = this->createSearchPattern(this->m_searchPattern.pattern() + t.mid(2));
                searchPattern.type = AppendToMainSearch;
                this->m_extendedSearchPatterns.append(searchPattern);
            }

            /// |w term
            else if (t.startsWith("w "))
            {
                SearchPattern searchPattern;
                searchPattern.searchPattern = this->createSearchPattern(t.mid(2));
                searchPattern.type = IncludeIntoMainSearch;
                this->m_extendedSearchPatterns.append(searchPattern);
            }

            /// |wo term
            else if (t.startsWith("wo "))
            {
                SearchPattern searchPattern;
                searchPattern.searchPattern = this->createSearchPattern(t.mid(3));
                searchPattern.type = WithoutAnyOfThis;
                this->m_extendedSearchPatterns.append(searchPattern);
            }

            /// |wg genre
            else if (t.startsWith("wg "))
            {
                SearchPattern searchPattern;
                searchPattern.searchPattern = this->createSearchPattern(t.mid(3));
                searchPattern.type = WithoutGenre;
                this->m_extendedSearchPatterns.append(searchPattern);
            }

            // ignore unknown pattern commands
        }

        tmp.clear();
    }

    // normal search keys
    else
        this->m_searchPattern = this->createSearchPattern(search_term);

    // finalize

    bool addDefault = true;
    for (SearchPattern &p : this->m_extendedSearchPatterns)
    {
        if (p.type == AppendToMainSearch)
        {
            p.type = Default;
            addDefault = false;
        }
    }

    if (addDefault)
    {
        SearchPattern searchPattern;
        searchPattern.searchPattern = this->m_searchPattern;
        searchPattern.type = Default;

        this->m_extendedSearchPatterns.prepend(searchPattern);
    }

    std::stable_sort(this->m_extendedSearchPatterns.begin(), this->m_extendedSearchPatterns.end(), this->sort);
}

SearchKeys::~SearchKeys()
{
    this->m_extendedSearchPatterns.clear();
}

const QList<SearchKeys::SearchPattern> &SearchKeys::searchPatterns() const
{
    return this->m_extendedSearchPatterns;
}

const QList<SearchKeys::SearchPattern> SearchKeys::searchPatterns(SearchPatternType type) const
{
    QList<SearchPattern> patterns;

    for (const SearchPattern &s : this->m_extendedSearchPatterns)
        if (s.type == type)
            patterns.push_back(s);

    return patterns;
}

bool SearchKeys::containsKey(SearchPatternType type) const
{
    for (const SearchPattern &s : this->m_extendedSearchPatterns)
        if (s.type == type)
            return true;
    return false;
}

int SearchKeys::countKeys(SearchPatternType type) const
{
    int c = 0;

    for (const SearchPattern &s : this->m_extendedSearchPatterns)
        if (s.type == type)
            c++;

    return c;
}

bool SearchKeys::empty() const
{
    if (this->m_searchPattern.pattern().isEmpty())
        return true;

    // if string is only made of wildcards, treat it as "empty"
    for (const QChar &c : this->m_searchPattern.pattern())
        if (c != '*')
            return false;

    return true;
}

QRegExp SearchKeys::createSearchPattern(const QString &search_term)
{
    QString search_keys = search_term;

    // Fixup whitespaces in search_term
    static const UnicodeWhitespaceFixer whitespacefixer;
    search_keys = whitespacefixer.processString(search_keys).at(0);

    // Create basic search keys (*search*term*)
    search_keys.replace(' ', '*');
    search_keys.insert(0, '*');
    search_keys.append('*');

    // Replace brackets, parentheses and slashes
    search_keys.replace(QRegExp("[\\[\\]\\(\\)\\\\\\/]"), "*");

    // Clean up search keys (remove occurrent following wildcards) [ eg: '***' becomes '*' ]
    for (int i = 0; i < search_keys.size(); i++)
    {
        if (search_keys.at(i) == '*')
        {
            if (search_keys.size() > i+1)
            {
                if (search_keys.at(i+1) == '*')
                {
                    search_keys.remove(i+1, 1);
                    i = 0;
                }
            }
        }
    }

    QRegExp search_pattern(search_keys, Qt::CaseInsensitive, QRegExp::WildcardUnix);
    search_keys.clear();

    return search_pattern;
}

bool SearchKeys::sort(const SearchPattern &p1, const SearchPattern &p2)
{
    return (p1.type < p2.type);
}
