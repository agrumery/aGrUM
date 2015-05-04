/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}_at_lip6.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
/** @file
 * @brief Contains usefull methods to work with files and directories.
 *
 * @author Vincent RENAUDINEAU and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_DIR_UTILS_H
#define GUM_DIR_UTILS_H

#include <string>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <dirent.h>
#include <unistd.h>

#include <agrum/config.h>

namespace gum {

  class Directory {

    public:
    //! Return true if \a directory is a valid directory, false otherwise.
    static bool isDir(const std::string &directory);

    //! Contructor
    Directory();
    //! Contructor
    Directory(const std::string &directory);
    //! Contructor
    Directory(const Directory &dir);
    //! Destructor
    ~Directory();

    //! Return true if directory has been opened, false otherwise.
    bool isValid() const;
    //! Return directory content.
    std::vector<std::string> entries() const;
    //! Return directory parent.
    Directory parent() const;
    //! Return directory path.
    std::string path() const;
    //! Return directory absolute path.
    std::string absolutePath() const;

    Directory &operator=(const Directory &d);

    private:
    std::string m_dirName;
    mutable DIR *m_dirPtr;

  }; // END CLASS DIRECTORY

} // END NAMESPACE GUM

#endif // GUM_DIR_UTILS_H
