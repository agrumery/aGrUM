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
/**
 * @file
 * @brief Contains usefull methods to work with files and directories.
 *
 * @author Vincent RENAUDINEAU and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_DIR_UTILS_H
#define GUM_DIR_UTILS_H

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#include <agrum/config.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#else
#include <agrum/core/mvsc/unistd.h>
#endif

#ifdef HAVE_DIRENT_H
#include <dirent.h>
#else
#include <agrum/core/mvsc/dirent.h>
#endif


namespace gum {

  /**
   * @class Directory utils_dir.h <agrum/core/utils_dir.h>
   * @brief Cross-platform directory utility.
   * @ingroup utilities_group
   */
  class Directory {

    public:
    /**
     * &brief Return true if \a directory is a valid directory, false
     * otherwise.
     * @param path The path to test.
     * &return Return true if \a directory is a valid directory, false
     * otherwise.
     */
    static bool isDir( const std::string& path );

    /**
     * @brief Contructor.
     */
    Directory();

    /**
     * @brief Contructor.
     * @param directory The path to the directory.
     */
    Directory( const std::string& directory );

    /**
     * @brief Copy contructor.
     * @param dir The gum::Directory to copy.
     */
    Directory( const Directory& dir );

    /**
     * @brief Destructor.
     */
    ~Directory();

    /**
     * @brief Returns true if directory has been opened, false otherwise.
     * @return Returns true if directory has been opened, false otherwise.
     */
    bool isValid() const;

    /**
     * @brief Return directory content.
     * @return Return directory content.
     */
    std::vector<std::string> entries() const;

    /**
     * @brief Returns directory parent.
     * @return Returns directory parent.
     */
    Directory parent() const;

    /**
     * @brief Returns directory path.
     * @return Returns directory path.
     */
    std::string path() const;

    /**
     * @brief Returns directory absolute path.
     * @return Returns directory absolute path.
     */
    std::string absolutePath() const;

    /**
     * @brief Copy operator.
     * @param d The gum::Directory to copy.
     * @return Returns this gum::Directory.
     */
    Directory& operator=( const Directory& d );

    private:
    /// The directory path.
    std::string m_dirName;

    /// A pointer towards the Directory stream
    mutable DIR* m_dirPtr;

  };  // END CLASS DIRECTORY

}  // END NAMESPACE GUM

#endif  // GUM_DIR_UTILS_H
