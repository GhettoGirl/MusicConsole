#ifndef SEARCHKEYS_HPP
#define SEARCHKEYS_HPP

#include <QString>
#include <QList>
#include <QRegExp>

class SearchKeys
{
public:
    SearchKeys(const QString &search_term);
    ~SearchKeys();

    enum SearchPatternType {
        Default,               // <main>
        IncludeIntoMainSearch, // search for something |w also search for this
        WithoutAnyOfThis,      // search term          |wo without this terms
        WithoutGenre,          // search term          |wg genre        <--- filter out track of genre 'genre', the file needs to be tagged
                                                                          // for this to work

        // internal, do not check against it, there is never a match
        AppendToMainSearch,    // search term          |a one |a two    <--- extends to this 'search term one' and 'search term two'
    };

    struct SearchPattern {
        QRegExp searchPattern;
        SearchPatternType type;
    };

    const QList<SearchPattern> &searchPatterns() const;
    const QList<SearchPattern> searchPatterns(SearchPatternType) const;

    bool containsKey(SearchPatternType) const;
    int countKeys(SearchPatternType) const;

    bool empty() const;

private:
    QRegExp createSearchPattern(const QString &search_term);

    static bool sort(const SearchPattern &p1, const SearchPattern &p2);

    QList<SearchPattern> m_extendedSearchPatterns;
    QRegExp m_searchPattern;
};

#endif // SEARCHKEYS_HPP
