#include <applicationmanager.hpp>

ApplicationManager::ApplicationManager(QObject *parent)
    : QObject(parent)
{
    this->app = new MusicConsole(this);
    QObject::connect(this->app, &MusicConsole::finished, this->app, &QObject::deleteLater, Qt::QueuedConnection);
    QObject::connect(this->app, &MusicConsole::finished, this, &ApplicationManager::terminate, Qt::QueuedConnection);
}

ApplicationManager::~ApplicationManager()
{
    delete this->app;
}

void ApplicationManager::enter()
{
    this->app->start();
}

void ApplicationManager::executeCmd()
{
    this->app->executeCmd();
}

void ApplicationManager::terminate()
{
    QCoreApplication::exit(this->app->statusCode());
}
