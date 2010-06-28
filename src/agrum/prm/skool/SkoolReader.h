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
 * @brief Headers of SkoolReader.
 *
 * @author Lionel TORTI
 */
// ============================================================================
#include <fstream>
// ============================================================================
#include <agrum/prm/PRMFactory.h>
#include <agrum/prm/skool/Parser.h>
// ============================================================================
#ifndef GUM_SKOOL_READER_H
#define GUM_SKOOL_READER_H
namespace gum {
namespace prm {
namespace skool {

  class SkoolReader {
    public:

      SkoolReader();

      ~SkoolReader();

      void readFile(const std::string& file);

      /**
       * @brief This methods defines the list of paths to look for skool files.
       * Use / for path separator ! Even on Windows !
       *
       * @param class_path A semicolon separated list of paths.
       */
      void setClassPath(const std::string& class_path);

      gum::prm::PRM* prm();

    private:
      SkoolReader(const SkoolReader& source);

      SkoolReader& operator=(const SkoolReader& source);

      gum::prm::PRMFactory __factory;

      std::vector<std::string> __class_path;
  };

} /* namespace skool */
} /* namespace prm */
} /* namespace gum */
#ifndef GUM_NO_INLINE
#include <agrum/prm/skool/SkoolReader.inl>
#endif // GUM_NO_INLINE
// ============================================================================
#endif /* GUM_SKOOL_READER_H */
// ============================================================================
