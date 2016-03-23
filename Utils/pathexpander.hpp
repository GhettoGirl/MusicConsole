/** ***********************************************************
 *                    ** Music Console **                     *
 *            Copyright © by GhettoGirl, 2013-2015            *
 **************************************************************/

/* === PathExpander
 *
 * expand unix paths to its absolute form
 * reads symlinks
 *
 * . -> current directory
 * .. -> parent directory
 * $VAR -> lookup variable VAR and replace it with its value
 * ${VAR}
 * ~ -> users home directory
 *
 * examples:
 *
 * '/home/user/./folder' -> '/home/user/folder'
 * '$HOME/dir/..'        -> '/home/user'
 *
 * assume '/dir' is a symlink to '/my/dir'
 * '/dir/path'           -> '/my/dir/path'
 *
 */

#ifndef PATHEXPANDER_HPP
#define PATHEXPANDER_HPP

#include <QString>

const QString expandPath(const char *path);
const QString expandPath(const QString &path);

#endif // PATHEXPANDER_HPP
