#ifndef HISTORYMANAGER_HPP
#define HISTORYMANAGER_HPP

#include <QString>
#include <QTextStream>

class QFile;

// the HistoryManager class is responsible for reading and writing the histfile
// the histfile is written in real-time, similar to most shells (bash, zsh, etc.)

class HistoryManager
{
public:
    static void createInstance(const QString &file);
    static HistoryManager *i();
    ~HistoryManager();

    void setHistIgnorePatterns(const QString &patterns);

    void close(); // close the file before the dtor

    void add(const QString &);

    const QStringList &history() const;

private:
    HistoryManager(const QString &file);

    QList<QRegExp> m_histignore;

    QFile *m_histfile;
    QTextStream m_textstream;
    bool m_file_open;

    QStringList m_entries; // backup list for the history command (Commands/CmdHistory.hpp)
};

#endif // HISTORYMANAGER_HPP
