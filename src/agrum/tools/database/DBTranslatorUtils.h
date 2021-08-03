/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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

/** @file
 * @brief Utilities for DBTranslators
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_DB_TRANSLATOR_UTILS_H
#define GUM_DB_TRANSLATOR_UTILS_H

#include <agrum/agrum.h>

#include <agrum/tools/database/DBTranslator.h>
#include <agrum/tools/database/DBTranslator4LabelizedVariable.h>
#include <agrum/tools/database/DBTranslator4DiscretizedVariable.h>
#include <agrum/tools/database/DBTranslator4IntegerVariable.h>
#include <agrum/tools/database/DBTranslator4RangeVariable.h>
#include <agrum/tools/database/DBTranslator4ContinuousVariable.h>


namespace gum {

  namespace learning {

    namespace DBTranslators {

      template < template < typename > class ALLOC >
      using allocator_type = typename DBTranslator< ALLOC >::allocator_type;


      /// named constructor
      /**
       * @param var the variable that the translator represents. This will
       * implicitly select the appropriate translator to be created
       * (e.g., DBTranslator4RangeVariable).
       * @param editable_dictionary For those translators that can
       * enable/disable the update of their dictionary during the reading of
       * databases (e.g., DBTranslator4LabelizedVariable), this indicates whether
       * we allow or not such updates. For DBTranslator4ContinuousVariable, this
       * corresponds to the fit_range constructor's parameter.
       * @param max_dico_entries For translators that store explicitly their
       * dictionary in memory, this parameter specifies the max number of entries
       * in this dictionary
       * @param alloc the allocator used to allocate everything in memory.
       * @return a DBTranslator specifically designed for the variable in argument and
       * allocated using the allocator of the DBTranslators.
       * @warning DBTranslator is an abstract class. To construct one, you should
       * usually construct one of its inherited classes (e.g.,
       * DBTranslator4LabelizedVariable, DBTranslator4RangeVariable, etc.). The
       * named constructor create will construct the inherited class that suits
       * the variable passed in argument.
       * @ingroup learning_database
       */
      template < template < typename > class ALLOC >
      DBTranslator< ALLOC >*
         create(const Variable& var,
                const bool      editable_dictionary  = false,
                std::size_t     max_dico_entries     = std::numeric_limits< std::size_t >::max(),
                const allocator_type< ALLOC >& alloc = allocator_type< ALLOC >());

      /// named constructor
      /**
       * @param var the variable that the translator represents. This will
       * implicitly select the appropriate translator to be created
       * (e.g., DBTranslator4RangeVariable).
       * @param missing_symbols the set of symbols that the translator should
       * consider as corresponding to missing values in the database
       * @param editable_dictionary For those translators that can
       * enable/disable the update of their dictionary during the reading of
       * databases (e.g., DBTranslator4LabelizedVariable), this indicates whether
       * we allow or not such updates. For DBTranslator4ContinuousVariable, this
       * corresponds to the fit_range constructor's parameter.
       * @param max_dico_entries For translators that store explicitly their
       * dictionary in memory, this parameter specifies the max number of entries
       * in this dictionary
       * @param alloc the allocator used to allocate everything in memory.
       * @return a DBTranslator specifically designed for the variable in argument and
       * allocated using the allocator of the DBTranslators.
       * @warning DBTranslator is an abstract class. To construct one, you should
       * usually construct one of its inherited classes (e.g.,
       * DBTranslator4LabelizedVariable, DBTranslator4RangeVariable, etc.). The
       * named constructor create will construct the inherited class that suits
       * the variable passed in argument.
       * @ingroup learning_database
       */
      template < template < typename > class ALLOC, template < typename > class XALLOC >
      DBTranslator< ALLOC >*
         create(const Variable&                                          var,
                const std::vector< std::string, XALLOC< std::string > >& missing_symbols,
                const bool  editable_dictionary      = false,
                std::size_t max_dico_entries         = std::numeric_limits< std::size_t >::max(),
                const allocator_type< ALLOC >& alloc = allocator_type< ALLOC >());

    } /* namespace DBTranslators */

  } /* namespace learning */

} /* namespace gum */

// always include the template implementation
#include <agrum/tools/database/DBTranslatorUtils_tpl.h>

#endif   // GUM_DB_TRANSLATOR_UTILS_H
