#ifndef MUSICCONSOLE_HPP
#define MUSICCONSOLE_HPP

#include <QThread>

#include "configmanager.hpp"

#include <Utils/medialibrarymodel.hpp>

#include <Sys/command.hpp>

class MusicConsole : public QThread
{
    Q_OBJECT

public:
    explicit MusicConsole(QObject *parent = 0);
    ~MusicConsole();

    int statusCode() const;

    void executeCmd() const;

protected:
    void run() Q_DECL_OVERRIDE;

private:

    void installSearchPathGens();

    void prepareToQuit();
    int m_statusCode;

    MediaLibraryModel *m_media = nullptr;

    QList<Command*> m_commands;

    ConfigManager *m_config;

    struct ConsoleCommand {
        QString cmd;
        QString args;
    };

private:

    // helper functions
    void userInput(QList<ConsoleCommand> &commands);
    QStringList createNameFilters(const QString &filters) const;

    // different static helper functions
    static bool cmdHasOnlySpaces(const QString &s);
    static void removeTrailingSpaces(QString &s);
    static void simplifyString(QString &s); // extends QString::simplified() with UnicodeWhitespaceFixer
};

#endif // MUSICCONSOLE_HPP
