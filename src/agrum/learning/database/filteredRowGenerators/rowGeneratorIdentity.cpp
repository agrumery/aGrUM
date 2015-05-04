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
 * @brief A filtered row generator that returns exactly the rows it gets in input
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include <agrum/config.h>
#include <agrum/learning/database/filteredRowGenerators/rowGeneratorIdentity.h>

/// include the inlined functions if necessary
#ifdef GUM_NO_INLINE
#include <agrum/learning/database/filteredRowGenerators/rowGeneratorIdentity.inl>
#endif /* GUM_NO_INLINE */

namespace gum {

  namespace learning {

    /// default constructor
    RowGeneratorIdentity::RowGeneratorIdentity() {
      GUM_CONSTRUCTOR(RowGeneratorIdentity);
    }

    /// copy constructor
    RowGeneratorIdentity::RowGeneratorIdentity(const RowGeneratorIdentity &from)
        : FilteredRowGenerator(from) {
      GUM_CONS_CPY(RowGeneratorIdentity);
    }

    /// move constructor
    RowGeneratorIdentity::RowGeneratorIdentity(RowGeneratorIdentity &&from)
        : FilteredRowGenerator(std::move(from)) {
      GUM_CONS_MOV(RowGeneratorIdentity);
    }

    /// destructor
    RowGeneratorIdentity::~RowGeneratorIdentity() {
      GUM_DESTRUCTOR(RowGeneratorIdentity);
    }

    /// copy operator
    RowGeneratorIdentity &RowGeneratorIdentity::
    operator=(const RowGeneratorIdentity &from) {
      FilteredRowGenerator::operator=(from);
      return *this;
    }

    /// move operator
    RowGeneratorIdentity &RowGeneratorIdentity::
    operator=(RowGeneratorIdentity &&from) {
      FilteredRowGenerator::operator=(std::move(from));
      return *this;
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
