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
#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    namespace DBTranslators {

      /// named constructor using dynamic allocation
      template < template < typename > class ALLOC, template < typename > class XALLOC >
      DBTranslator< ALLOC >*
         create(const Variable&                                          var,
                const std::vector< std::string, XALLOC< std::string > >& missing_symbols,
                const bool                                               editable_dictionary,
                std::size_t                                              max_dico_entries,
                const allocator_type< ALLOC >&                           alloc) {
        switch (var.varType()) {
          case VarType::Labelized: {
            const LabelizedVariable& xvar = static_cast< const LabelizedVariable& >(var);
            ALLOC< DBTranslator4LabelizedVariable< ALLOC > > allocator(alloc);
            DBTranslator4LabelizedVariable< ALLOC >*         translator = allocator.allocate(1);
            try {
              new ((void*)translator) DBTranslator4LabelizedVariable< ALLOC >(xvar,
                                                                              missing_symbols,
                                                                              editable_dictionary,
                                                                              max_dico_entries,
                                                                              alloc);
            } catch (...) {
              allocator.deallocate(translator, 1);
              throw;
            }
            return translator;
          }

          case VarType::Integer: {
            const IntegerVariable& xvar = static_cast< const IntegerVariable& >(var);
            ALLOC< DBTranslator4IntegerVariable< ALLOC > > allocator(alloc);
            DBTranslator4IntegerVariable< ALLOC >*         translator = allocator.allocate(1);
            try {
              new ((void*)translator) DBTranslator4IntegerVariable< ALLOC >(xvar,
                                                                            missing_symbols,
                                                                            max_dico_entries,
                                                                            alloc);
            } catch (...) {
              allocator.deallocate(translator, 1);
              throw;
            }
            return translator;
          }

          case VarType::Discretized: {
            const IDiscretizedVariable& xvar = static_cast< const IDiscretizedVariable& >(var);
            ALLOC< DBTranslator4DiscretizedVariable< ALLOC > > allocator(alloc);
            DBTranslator4DiscretizedVariable< ALLOC >*         translator = allocator.allocate(1);
            try {
              new ((void*)translator) DBTranslator4DiscretizedVariable< ALLOC >(xvar,
                                                                                missing_symbols,
                                                                                max_dico_entries,
                                                                                alloc);
            } catch (...) {
              allocator.deallocate(translator, 1);
              throw;
            }
            return translator;
          }

          case VarType::Range: {
            const RangeVariable& xvar = static_cast< const RangeVariable& >(var);
            ALLOC< DBTranslator4RangeVariable< ALLOC > > allocator(alloc);
            DBTranslator4RangeVariable< ALLOC >*         translator = allocator.allocate(1);
            try {
              new ((void*)translator) DBTranslator4RangeVariable< ALLOC >(xvar,
                                                                          missing_symbols,
                                                                          editable_dictionary,
                                                                          max_dico_entries,
                                                                          alloc);
            } catch (...) {
              allocator.deallocate(translator, 1);
              throw;
            }
            return translator;
          }

          case VarType::Continuous: {
            const IContinuousVariable& xvar = static_cast< const IContinuousVariable& >(var);
            ALLOC< DBTranslator4ContinuousVariable< ALLOC > > allocator(alloc);
            DBTranslator4ContinuousVariable< ALLOC >*         translator = allocator.allocate(1);
            try {
              new ((void*)translator) DBTranslator4ContinuousVariable< ALLOC >(xvar,
                                                                               missing_symbols,
                                                                               editable_dictionary,
                                                                               alloc);
            } catch (...) {
              allocator.deallocate(translator, 1);
              throw;
            }
            return translator;
          }
        }

        GUM_ERROR(NotImplementedYet,
                  "The translator for Variable "
                     << var.name() << " cannot yet be created yet by named constructor CreatePtr.");
      }


      /// named constructor using dynamic allocation
      template < template < typename > class ALLOC >
      INLINE DBTranslator< ALLOC >* create(const Variable&                var,
                                           const bool                     editable_dictionary,
                                           std::size_t                    max_dico_entries,
                                           const allocator_type< ALLOC >& alloc) {
        return create(var,
                      std::vector< std::string >(),
                      editable_dictionary,
                      max_dico_entries,
                      alloc);
      }

    }   // namespace DBTranslators

  }   // namespace learning

}   // namespace gum


#endif   // DOXYGEN_SHOULD_SKIP_THIS
