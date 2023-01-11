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
 * @brief Definition of abstract classes for import of Influence Diagram from a
 *file
 *
 * All classes used to import an influence diagram from a file must inherit from
 *IDReader
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Jean-Christophe MAGNAN and Christophe
 *GONZALES(_at_AMU)
 */
#ifndef GUM_ID_READER_H
#define GUM_ID_READER_H

#include <agrum/ID/influenceDiagram.h>
#include <agrum/agrum.h>
#include <iostream>
#include <string>

namespace gum {

  /**
   * @class IDReader
   * @brief Pure virtual class for importing an ID from a file.
   * @ingroup id_group
   *
   * All classes used to read an influence diagram from
   * a file must inherit from this class
   */
  template < typename GUM_SCALAR >
  class IDReader {
    public:
    /**
     * Constructor
     * A reader is created to reading a defined file.
     * Note that an ID as to be created before and given in parameter.
     */
    IDReader(InfluenceDiagram< GUM_SCALAR >* infdiag, const std::string& filePath);

    /**
     * Default destructor.
     */
    virtual ~IDReader();

    /**
     * Reads the influence diagram from the file referenced by filePath  given
     * at the
     * creation of class
     * @return Returns the number of error during the parsing (0 if none).
     */
    virtual void proceed() = 0;
  };

} /* namespace gum */

#include "IDReader_tpl.h"

#endif   // GUM_ID_READER_H
