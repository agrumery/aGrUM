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
 * @brief Contains usefull methods to work with directories.
 *
 * @author Vincent RENAUDINEAU and Pierre-Henri WUILLEMIN
 */

#include <agrum/config.h>
#include <agrum/core/utils_dir.h>

#include <unistd.h>

namespace gum {

  //! Return true if \a directory is a valid directory, false otherwise.
  bool Directory::isDir(const std::string &directory) {
    return Directory(directory).isValid();
  }

  //! Contructor
  Directory::Directory() : m_dirPtr(nullptr) { GUM_CONSTRUCTOR(Directory); }

  //! Contructor
  Directory::Directory(const std::string &directory) : m_dirName(directory) {
    GUM_CONSTRUCTOR(Directory);
    m_dirPtr = opendir(m_dirName.c_str());
  }

  //! Contructor
  Directory::Directory(const Directory &dir) : m_dirName(dir.m_dirName) {
    GUM_CONSTRUCTOR(Directory);
    m_dirPtr = opendir(m_dirName.c_str());
  }

  //! Destructor
  Directory::~Directory() {
    GUM_DESTRUCTOR(Directory);

    if (m_dirPtr != nullptr)
      closedir(m_dirPtr);
  }

  //! Return true if directory has been opened, false otherwise.
  bool Directory::isValid() const { return m_dirPtr != nullptr; }

  //! Return directory content.
  std::vector<std::string> Directory::entries() const {
    std::vector<std::string> result;

    if (!isValid())
      return result;

    rewinddir(m_dirPtr);

    dirent *entry;

    while ((entry = readdir(m_dirPtr)))
      result.push_back(std::string(entry->d_name));

    return result;
  }

  //! Return directory parent.
  Directory Directory::parent() const {
    if (!isValid())
      return Directory();

    return Directory(m_dirName + "../");
  }

  //! Return directory path.
  std::string Directory::path() const { return m_dirName; }

  //! Return directory absolute path.
  std::string Directory::absolutePath() const {
    std::string result;

    if (!isValid())
      return result;

    char oldWD[255];

    if (getcwd(oldWD, 255) == nullptr)
      return result;

    if (chdir(m_dirName.c_str()) != 0)
      return result;

    char absPath[255];

    if (getcwd(absPath, 254) != nullptr)
      result = std::string(absPath) + '/';

    if (chdir(oldWD) != 0)
      std::cerr << "Warning : Could not go to previous working directory. ("
                << __FILE__ << ":" << __LINE__ << ")" << std::endl;

    return result;
  }

  Directory &Directory::operator=(const Directory &d) {
    if (m_dirPtr != nullptr)
      closedir(m_dirPtr);

    m_dirName = d.m_dirName;

    m_dirPtr = opendir(m_dirName.c_str());

    return *this;
  }

} // END NAMESPACE GUM
