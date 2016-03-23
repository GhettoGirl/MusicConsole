#ifndef APPLICATIONMANAGER_HPP
#define APPLICATIONMANAGER_HPP

#include <Utils/contrib/SingleApplication/singleapplication.h>
#include <musicconsole.hpp>

#include <QObject>
#include <QStringList>

class ApplicationManager : public QObject
{
    Q_OBJECT

public:
    explicit ApplicationManager(QObject *parent = 0);
    ~ApplicationManager();

    void enter();

public slots:
    void executeCmd();

private slots:
    void terminate();

private:
    MusicConsole *app = nullptr;
};

#endif // APPLICATIONMANAGER_HPP
