#include <QCoreApplication>
#include <Utils/contrib/SingleApplication/singleapplication.h>

#include <applicationmanager.hpp>
#include "Utils/range_based_for_loop.hpp"

#include <iostream>
#include <sys/ioctl.h>

static void print_header()
{
    // escape sequences
    static const std::string term_reset = "\033[0m";
    static const std::string term_bold  = "\033[1m";

    struct winsize w;
    int cols(0);

    // get amount of columns of the active terminal window
    if (ioctl(0, TIOCGWINSZ, &w) != -1)
        cols = w.ws_col;

    std::string line;
    std::string header;

    // generate nice header with separator line across terminal window
    if (cols != 0)
    {
        cols -= 2;

        // generate base line
        for (int i = 0; i < cols; i++)
            line.append("─");

        // first line
        header.append(term_reset + "┌" + line + "┐\n");

        // second line
        header.append(
            "│ " + term_bold +
            qUtf8Printable(QCoreApplication::applicationName()) +
            term_reset + ' ' +
            qUtf8Printable(QCoreApplication::applicationVersion()));

        for (int i = 2 + QCoreApplication::applicationName().size()
                       + QCoreApplication::applicationVersion().size(); i < cols; i++)
            header.append(" ");

        header.append("│\n");

        // last line
        header.append("└" + line + "┘\n");
    }

    // generate normal header, eg: for log files
    else
    {
        header.append(
            term_reset + term_bold +
            qUtf8Printable(QCoreApplication::applicationName()) +
            term_reset +
            qUtf8Printable(QCoreApplication::applicationVersion()));
    }

    // print the header
    std::cout << header << std::endl;

    // clean up
    line.clear();
    header.clear();
}

int main(int argc, char **argv)
{
    QCoreApplication::setApplicationName("Music Console");
    QCoreApplication::setApplicationVersion("0.53-6");
    QCoreApplication::setOrganizationName("GhettoGirl");

    print_header();

    SingleApplication a(argc, argv, QObject::trUtf8("\033[1;38;2;166;74;0mNOTICE:\033[0m \033[3monly one instance is allowed!\033[0m"));

    ApplicationManager *appInstance = new ApplicationManager();
    appInstance->enter();

    QObject::connect(&a, &SingleApplication::showUp, appInstance, &ApplicationManager::executeCmd, Qt::QueuedConnection);

    return a.exec();
}
