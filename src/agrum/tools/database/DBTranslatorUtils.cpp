/**
 *
 *   Copyright (c) 2005-2022 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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

#include <agrum/tools/database/DBTranslatorUtils.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/// include the inlined functions if necessary
#  ifdef GUM_NO_INLINE
#    include <agrum/tools/database/DBTranslatorUtils_inl.h>
#  endif /* GUM_NO_INLINE */

namespace gum {

  namespace learning {

    namespace DBTranslators {

      /// named constructor using dynamic allocation
      DBTranslator* create(const Variable&                   var,
                           const std::vector< std::string >& missing_symbols,
                           const bool                        editable_dictionary,
                           std::size_t                       max_dico_entries) {
        switch (var.varType()) {
          case VarType::Labelized: {
            const LabelizedVariable& xvar = static_cast< const LabelizedVariable& >(var);
            return new DBTranslator4LabelizedVariable(xvar,
                                                      missing_symbols,
                                                      editable_dictionary,
                                                      max_dico_entries);
          }

          case VarType::Integer: {
            const IntegerVariable& xvar = static_cast< const IntegerVariable& >(var);

            return new DBTranslator4IntegerVariable(xvar, missing_symbols, max_dico_entries);
          }

          case VarType::Discretized: {
            const IDiscretizedVariable& xvar = static_cast< const IDiscretizedVariable& >(var);
            return new DBTranslator4DiscretizedVariable(xvar, missing_symbols, max_dico_entries);
          }

          case VarType::Range: {
            const RangeVariable& xvar = static_cast< const RangeVariable& >(var);
            return new DBTranslator4RangeVariable(xvar,
                                                  missing_symbols,
                                                  editable_dictionary,
                                                  max_dico_entries);
          }

          case VarType::Continuous: {
            const IContinuousVariable& xvar = static_cast< const IContinuousVariable& >(var);
            return new DBTranslator4ContinuousVariable(xvar, missing_symbols, editable_dictionary);
          }
        }

        GUM_ERROR(NotImplementedYet,
                  "The translator for Variable "
                     << var.name() << " cannot yet be created yet by named constructor CreatePtr.");
      }

    } /* namespace DBTranslators */

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
