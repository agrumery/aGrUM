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
#ifndef GUM_LEARNING_ROW_GENERATOR_IDENTITY_H
#define GUM_LEARNING_ROW_GENERATOR_IDENTITY_H

#include <agrum/learning/database/filteredRowGenerator.h>

namespace gum {

  namespace learning {

    /** @class RowGeneratorIdentity
     * @brief A filtered row generator that returns exactly the rows it gets
     * in input
     * @ingroup learning_group */
    class RowGeneratorIdentity : public FilteredRowGenerator {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################

      /// @{

      /// default constructor
      RowGeneratorIdentity();

      /// copy constructor
      RowGeneratorIdentity(const RowGeneratorIdentity &);

      /// move constructor
      RowGeneratorIdentity(RowGeneratorIdentity &&);

      /// destructor
      ~RowGeneratorIdentity();

      /// @}

      // ##########################################################################
      /// @name Operators
      // ##########################################################################

      /// @{

      /// copy operator
      RowGeneratorIdentity &operator=(const RowGeneratorIdentity &);

      /// move operator
      RowGeneratorIdentity &operator=(RowGeneratorIdentity &&);

      /// @}

      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################

      /// @{

      /// generates new lines from those the generator gets in input
      FilteredRow &generate();

      /// computes the rows it will provide in output
      unsigned int _computeRows();

      /// @}
    };

  } /* namespace learning */

} /* namespace gum */

/// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#include <agrum/learning/database/filteredRowGenerators/rowGeneratorIdentity.inl>
#endif /* GUM_NO_INLINE */

#endif /* GUM_LEARNING_ROW_GENERATOR_IDENTITY_H */
