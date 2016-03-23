#include "historymanager.hpp"

#include <QFile>
#include <QTextStream>
#include <QStringList>

#include <iostream>
#include <readline/history.h>

HistoryManager *historyManager = nullptr;

HistoryManager::HistoryManager(const QString &file)
{
    this->m_histfile = new QFile(file);

    if (!this->m_histfile->open(QFile::ReadWrite | QFile::Text))
    {
        std::cerr << "\033[1;38;2;120;0;0mATTENTION:\033[0m Unable to open history file!\n"
                     "   Your command line history cannot be saved.\n" << std::endl;
        this->m_file_open = false;
        return;
    }

    else
    {
        this->m_textstream.setDevice(this->m_histfile);
        this->m_file_open = true;
    }

    // make sure file ends with a newline
    QByteArray data = this->m_histfile->readAll();
    if (!data.endsWith('\n'))
    {
        this->m_textstream << "\n";
        this->m_textstream.flush();
        (void) this->m_histfile->flush();
    }

    this->m_entries = QString(data).split(QRegExp("[\n\r]"), QString::SkipEmptyParts);
    data.clear();

    // add entries to GNU/Readline history
    if (!this->m_entries.isEmpty())
    {
        for (const QString &line : this->m_entries)
            add_history(line.toUtf8().constData());
    }
}

HistoryManager::~HistoryManager()
{
    this->close();
}

void HistoryManager::createInstance(const QString &file)
{
    if (!historyManager)
        historyManager = new HistoryManager(file);
}

HistoryManager *HistoryManager::i()
{
    return historyManager;
}

void HistoryManager::setHistIgnorePatterns(const QString &patterns)
{
    for (const QString &p : patterns.split(';'))
        this->m_histignore.append(QRegExp(p, Qt::CaseInsensitive, QRegExp::WildcardUnix));
}

void HistoryManager::close()
{
    if (this->m_file_open)
    {
        this->m_histfile->close();
        this->m_file_open = false;
        delete this->m_histfile;
    }

    this->m_entries.clear();
    this->m_histignore.clear();
}

void HistoryManager::add(const QString &str)
{
    // check HISTIGNORE
    for (const QRegExp &p : this->m_histignore)
        if (p.exactMatch(str))
            return;

    // don't append lines of duplicates to the histfile
    // last 2 entries are checked
    //
    // this prevents history files like this:
    //   song 1
    //   song 2
    //   song 1
    //   song 2
    //   song 1
    //   song 2
    //   ...
    if (!this->m_entries.isEmpty())
    {
        if (this->m_entries.size() > 1)
        {
            if (QString::compare(this->m_entries.last(), str, Qt::CaseInsensitive) == 0 ||
                QString::compare(this->m_entries.at(this->m_entries.size() - 2), str, Qt::CaseInsensitive) == 0)
                return;
        }

        else
        {
            if (QString::compare(this->m_entries.last(), str, Qt::CaseInsensitive) == 0)
                return;
        }
    }

    // add entry to GNU/Readline history
    add_history(str.toUtf8().constData());
    this->m_entries.append(str);

    // write entry to file
    if (this->m_file_open)
    {
        this->m_textstream << str << "\n";
        this->m_textstream.flush();
        (void) this->m_histfile->flush();
    }
}

const QStringList &HistoryManager::history() const
{
    return this->m_entries;
}
