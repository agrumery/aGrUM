/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
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
 * @brief Definition of abstract classes for import of Influence Diagram from a file
 *
 * All classes used to import an influence diagram from a file must inherit from
 *IDReader
 *
 * @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_ID_READER_H
#define GUM_ID_READER_H

#include <iostream>
#include <string>
#include <agrum/config.h>
#include <agrum/ID/influenceDiagram.h>

namespace gum {

  /**
   * @class IDReader
   * @brief Pure virtual class for importing an ID from a file.
   * @ingroup id_group
   *
   * All classes used to read an influence diagram from
   * a file must inherit from this class
   */
  template <typename GUM_SCALAR> class IDReader {
    public:
    /**
     * Constructor
     * A reader is created to reading a defined file.
     * Note that an ID as to be created before and given in parameter.
     */
    IDReader(InfluenceDiagram<GUM_SCALAR> *infdiag, const std::string &filePath);

    /**
     * Default destructor.
     */
    virtual ~IDReader();

    /**
     * Reads the influence diagram from the file referenced by filePath  given at the
     * creation of class
     * @return Returns the number of error during the parsing (0 if none).
     */
    virtual void proceed() = 0;
  };

} /* namespace gum */

#include "IDReader.tcc"

#endif // GUM_ID_READER_H
