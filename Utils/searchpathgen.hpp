#ifndef SEARCHPATHGEN_HPP
#define SEARCHPATHGEN_HPP

#include <QString>
#include <QStringList>

// pure abstract base class (virtual)
// the processString() method MUST be implemented
// for the SearchPathGen iterator to work
//
// NOTES:
//  × all SearchPathGens must be defined as pointer object
//  × all SearchPathGens are deleted by the MediaLibraryModel
//    the model performs a nullptr check to prevent a SEGFAULT
//    the gens are deleted after they were used
//  × you may create as much additional member function as you like
//  × put your clean up code into the destructor
//

class SearchPathGen
{
public:
    virtual ~SearchPathGen();

    // this function receives the original relative file path
    virtual QStringList processString(const QString&) const = 0;
};

#endif // SEARCHPATHGEN_HPP
