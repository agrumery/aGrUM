/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *   info_at_agrum_dot_org
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


/**
 * @file
 * @brief Contains usefull methods to work with files and directories.
 *
 * @author Vincent RENAUDINEAU and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef GUM_DIR_UTILS_H
#define GUM_DIR_UTILS_H

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#include <agrum/agrum.h>

#ifdef HAVE_UNISTD_H
#  include <unistd.h>
#else
#  include <agrum/tools/core/mvsc/unistd.h>
#endif

#ifdef HAVE_DIRENT_H
#  include <dirent.h>
#else
#  include <agrum/tools/core/mvsc/dirent.h>
#endif


namespace gum {

  /**
   * @class Directory
   * @headerfile utils_dir.h <agrum/tools/core/utils_dir.h>
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
    static bool isDir(const std::string& path);

    /**
     * @brief Contructor.
     */
    Directory();

    /**
     * @brief Contructor.
     * @param directory The path to the directory.
     */
    Directory(const std::string& directory);

    /**
     * @brief Copy contructor.
     * @param dir The gum::Directory to copy.
     */
    Directory(const Directory& dir);

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
    std::vector< std::string > entries() const;

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
    Directory& operator=(const Directory& d);

    private:
    /// The directory path.
    std::string m_dirName;

    /// A pointer towards the Directory stream
    mutable DIR* m_dirPtr;

  };   // END CLASS DIRECTORY

}   // namespace gum

#endif   // GUM_DIR_UTILS_H
