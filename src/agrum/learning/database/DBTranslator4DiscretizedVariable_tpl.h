
/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
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
 * @brief The databases' cell translators for discretized variables
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#include <utility>
#include <vector>
#include <sstream>

#include <agrum/learning/database/DBTranslator4DiscretizedVariable.h>
#include <agrum/learning/database/DBCell.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {


    /// default constructor with a discretized variable as translator
    template < template < typename > class ALLOC >
    template < typename GUM_SCALAR, template < typename > class XALLOC >
    DBTranslator4DiscretizedVariable< ALLOC >::DBTranslator4DiscretizedVariable(
       const DiscretizedVariable< GUM_SCALAR >&                 var,
       const std::vector< std::string, XALLOC< std::string > >& missing_symbols,
       std::size_t                                              max_dico_entries,
       const typename DBTranslator4DiscretizedVariable< ALLOC >::allocator_type&
          alloc) :
        DBTranslator< ALLOC >(DBTranslatedValueType::DISCRETE,
                              missing_symbols,
                              false,
                              max_dico_entries,
                              alloc),
        __variable(var.name(), var.description()) {
      // check that the variable has not too many entries
      if (var.domainSize() > max_dico_entries) {
        GUM_ERROR(SizeError,
                  "the dictionary induced by the variable is too large");
      }

      // copy the ticks of var into our internal variable
      const auto& ticks = var.ticks();
      for (const auto tick : ticks) {
        __variable.addTick((float)tick);
      }

      // the the bounds of the discretized variable
      const float lower_bound = (float)ticks[0];
      const float upper_bound = (float)ticks.back();

      // remove all the missing symbols corresponding to a number between
      // lower_bound and upper_bound
      for (auto iter = this->_missing_symbols.beginSafe();
           iter != this->_missing_symbols.endSafe();
           ++iter) {
        if (DBCell::isReal(*iter)) {
          const float missing_val = std::stof(*iter);
          if ((missing_val >= lower_bound) && (missing_val <= upper_bound)) {
            this->_missing_symbols.erase(iter);
          }
        }
      }

      // add the content of the variable into the back dictionary
      std::size_t size = 0;
      for (const auto& label : var.labels()) {
        // if the label corresponds to a missing value, then remove it from
        // the set of missing symbols. If, in addition, it has already
        // been entered into the back_dictionary, then, this has been done
        // because the label corresponded to a missing value, so we should
        // remove the label as well from the back_dictionary.
        if (this->_missing_symbols.exists(label)) {
          this->_missing_symbols.erase(label);
        }

        this->_back_dico.insert(size, label);
        ++size;
      }

      // store a copy of the variable, that should be used by method variable ()
      __real_variable = var.clone();

      GUM_CONSTRUCTOR(DBTranslator4DiscretizedVariable);
    }


    /// default constructor with a IDiscretized variable as translator
    template < template < typename > class ALLOC >
    template < template < typename > class XALLOC >
    DBTranslator4DiscretizedVariable< ALLOC >::DBTranslator4DiscretizedVariable(
       const IDiscretizedVariable&                              var,
       const std::vector< std::string, XALLOC< std::string > >& missing_symbols,
       std::size_t                                              max_dico_entries,
       const typename DBTranslator4DiscretizedVariable< ALLOC >::allocator_type&
          alloc) :
        DBTranslator< ALLOC >(DBTranslatedValueType::DISCRETE,
                              missing_symbols,
                              false,
                              max_dico_entries,
                              alloc),
        __variable(var.name(), var.description()) {
      // check that the variable has not too many entries
      if (var.domainSize() > max_dico_entries) {
        GUM_ERROR(SizeError,
                  "the dictionary induced by the variable is too large");
      }

      // copy the ticks of var into our internal variable
      const auto ticks = var.ticksAsDoubles();
      for (const auto tick : ticks) {
        __variable.addTick((float)tick);
      }

      // the the bounds of the discretized variable
      const float lower_bound = float(ticks[0]);
      const float upper_bound = float(ticks.back());

      // remove all the missing symbols corresponding to a number between
      // lower_bound and upper_bound
      for (auto iter = this->_missing_symbols.beginSafe();
           iter != this->_missing_symbols.endSafe();
           ++iter) {
        if (DBCell::isReal(*iter)) {
          const float missing_val = std::stof(*iter);
          if ((missing_val >= lower_bound) && (missing_val <= upper_bound)) {
            this->_missing_symbols.erase(iter);
          }
        }
      }

      // add the content of the variable into the back dictionary
      std::size_t size = 0;
      for (const auto& label : var.labels()) {
        // if the label corresponds to a missing value, then remove it from
        // the set of missing symbols. If, in addition, it has already
        // been entered into the back_dictionary, then, this has been done
        // because the label corresponded to a missing value, so we should
        // remove the label as well from the back_dictionary.
        if (this->_missing_symbols.exists(label)) {
          this->_missing_symbols.erase(label);
        }

        this->_back_dico.insert(size, label);
        ++size;
      }

      // store a copy of the variable, that should be used by method variable ()
      __real_variable = var.clone();

      GUM_CONSTRUCTOR(DBTranslator4DiscretizedVariable);
    }


    /// default constructor with a discretized variable as translator
    template < template < typename > class ALLOC >
    template < typename GUM_SCALAR >
    DBTranslator4DiscretizedVariable< ALLOC >::DBTranslator4DiscretizedVariable(
       const DiscretizedVariable< GUM_SCALAR >& var,
       std::size_t                              max_dico_entries,
       const typename DBTranslator4DiscretizedVariable< ALLOC >::allocator_type&
          alloc) :
        DBTranslator< ALLOC >(
           DBTranslatedValueType::DISCRETE, false, max_dico_entries, alloc),
        __variable(var.name(), var.description()) {
      // check that the variable has not too many entries
      if (var.domainSize() > max_dico_entries) {
        GUM_ERROR(SizeError,
                  "the dictionary induced by the variable is too large");
      }

      // copy the ticks of var into our internal variable
      const auto& ticks = var.ticks();
      for (const auto tick : ticks) {
        __variable.addTick((float)tick);
      }

      // add the content of the variable into the back dictionary
      std::size_t size = 0;
      for (const auto& label : var.labels()) {
        this->_back_dico.insert(size, label);
        ++size;
      }

      // store a copy of the variable, that should be used by method variable ()
      __real_variable = var.clone();

      GUM_CONSTRUCTOR(DBTranslator4DiscretizedVariable);
    }


    /// default constructor with a IDiscretized variable as translator
    template < template < typename > class ALLOC >
    DBTranslator4DiscretizedVariable< ALLOC >::DBTranslator4DiscretizedVariable(
       const IDiscretizedVariable& var,
       std::size_t                 max_dico_entries,
       const typename DBTranslator4DiscretizedVariable< ALLOC >::allocator_type&
          alloc) :
        DBTranslator< ALLOC >(
           DBTranslatedValueType::DISCRETE, false, max_dico_entries, alloc),
        __variable(var.name(), var.description()) {
      // check that the variable has not too many entries
      if (var.domainSize() > max_dico_entries) {
        GUM_ERROR(SizeError,
                  "the dictionary induced by the variable is too large");
      }

      // copy the ticks of var into our internal variable
      const auto ticks = var.ticksAsDoubles();
      for (const auto tick : ticks) {
        __variable.addTick((float)tick);
      }

      // add the content of the variable into the back dictionary
      std::size_t size = 0;
      for (const auto& label : var.labels()) {
        this->_back_dico.insert(size, label);
        ++size;
      }

      // store a copy of the variable, that should be used by method variable ()
      __real_variable = var.clone();

      GUM_CONSTRUCTOR(DBTranslator4DiscretizedVariable);
    }


    /// copy constructor with a given allocator
    template < template < typename > class ALLOC >
    DBTranslator4DiscretizedVariable< ALLOC >::DBTranslator4DiscretizedVariable(
       const DBTranslator4DiscretizedVariable< ALLOC >& from,
       const typename DBTranslator4DiscretizedVariable< ALLOC >::allocator_type&
          alloc) :
        DBTranslator< ALLOC >(from, alloc),
        __variable(from.__variable) {
      // store a copy of the variable, that should be used by method variable ()
      __real_variable = from.__real_variable->clone();

      GUM_CONS_CPY(DBTranslator4DiscretizedVariable);
    }


    /// copy constructor
    template < template < typename > class ALLOC >
    DBTranslator4DiscretizedVariable< ALLOC >::DBTranslator4DiscretizedVariable(
       const DBTranslator4DiscretizedVariable< ALLOC >& from) :
        DBTranslator4DiscretizedVariable< ALLOC >(from, from.getAllocator()) {}


    /// move constructor with a given allocator
    template < template < typename > class ALLOC >
    DBTranslator4DiscretizedVariable< ALLOC >::DBTranslator4DiscretizedVariable(
       DBTranslator4DiscretizedVariable< ALLOC >&& from,
       const typename DBTranslator4DiscretizedVariable< ALLOC >::allocator_type&
          alloc) :
        DBTranslator< ALLOC >(std::move(from), alloc),
        __variable(std::move(from.__variable)) {
      // moves the copy of the variable, that should be used by method variable ()
      __real_variable = from.__real_variable;
      from.__real_variable = nullptr;

      GUM_CONS_MOV(DBTranslator4DiscretizedVariable);
    }


    /// move constructor
    template < template < typename > class ALLOC >
    DBTranslator4DiscretizedVariable< ALLOC >::DBTranslator4DiscretizedVariable(
       DBTranslator4DiscretizedVariable< ALLOC >&& from) :
        DBTranslator4DiscretizedVariable< ALLOC >(std::move(from),
                                                  from.getAllocator()) {}


    /// virtual copy constructor with a given allocator
    template < template < typename > class ALLOC >
    DBTranslator4DiscretizedVariable< ALLOC >*
       DBTranslator4DiscretizedVariable< ALLOC >::clone(
          const typename DBTranslator4DiscretizedVariable< ALLOC >::allocator_type&
             alloc) const {
      ALLOC< DBTranslator4DiscretizedVariable< ALLOC > > allocator(alloc);
      DBTranslator4DiscretizedVariable< ALLOC >*         translator =
         allocator.allocate(1);
      try {
        allocator.construct(translator, *this, alloc);
      } catch (...) {
        allocator.deallocate(translator, 1);
        throw;
      }
      return translator;
    }


    /// virtual copy constructor
    template < template < typename > class ALLOC >
    INLINE DBTranslator4DiscretizedVariable< ALLOC >*
           DBTranslator4DiscretizedVariable< ALLOC >::clone() const {
      return clone(this->getAllocator());
    }


    /// destructor
    template < template < typename > class ALLOC >
    INLINE DBTranslator4DiscretizedVariable<
       ALLOC >::~DBTranslator4DiscretizedVariable() {
      if (__real_variable != nullptr) delete __real_variable;

      GUM_DESTRUCTOR(DBTranslator4DiscretizedVariable);
    }


    /// copy operator
    template < template < typename > class ALLOC >
    DBTranslator4DiscretizedVariable< ALLOC >&
          DBTranslator4DiscretizedVariable< ALLOC >::
          operator=(const DBTranslator4DiscretizedVariable< ALLOC >& from) {
      if (this != &from) {
        DBTranslator< ALLOC >::operator=(from);
        __variable = from.__variable;

        if (__real_variable != nullptr) delete __real_variable;
        __real_variable = from.__real_variable->clone();
      }

      return *this;
    }


    /// move operator
    template < template < typename > class ALLOC >
    DBTranslator4DiscretizedVariable< ALLOC >&
          DBTranslator4DiscretizedVariable< ALLOC >::
          operator=(DBTranslator4DiscretizedVariable< ALLOC >&& from) {
      if (this != &from) {
        DBTranslator< ALLOC >::operator=(std::move(from));
        __variable = std::move(from.__variable);

        if (__real_variable != nullptr) delete __real_variable;
        __real_variable = from.__real_variable;
        from.__real_variable = nullptr;
      }

      return *this;
    }


    /// returns the translation of a string, as found in the current dictionary
    template < template < typename > class ALLOC >
    INLINE DBTranslatedValue DBTranslator4DiscretizedVariable< ALLOC >::translate(
       const std::string& str) {
      // try to get the index of str within the discretized variable.
      try {
        return DBTranslatedValue{std::size_t(__variable[str])};
      } catch (gum::Exception&) {
        // check for a missing symbol
        if (this->isMissingSymbol(str))
          return DBTranslatedValue{std::numeric_limits< std::size_t >::max()};

        // check if the back_dictionary does not contain str. This enables
        // to execute translate ( translateBack ( translate ( str ) ) )
        // without raising an exception
        try {
          return DBTranslatedValue{this->_back_dico.first(str)};
        } catch (gum::Exception&) {
          if (!DBCell::isReal(str)) {
            GUM_ERROR(TypeError,
                      "String \""
                         << str
                         << "\" cannot be translated because it is not a number");
          } else {
            GUM_ERROR(UnknownLabelInDatabase,
                      "The translation of \"" << str << "\" could not be found");
          }
        }
      }
    }


    /// returns the original value for a given translation
    template < template < typename > class ALLOC >
    INLINE std::string DBTranslator4DiscretizedVariable< ALLOC >::translateBack(
       const DBTranslatedValue translated_val) const {
      try {
        return this->_back_dico.second(translated_val.discr_val);
      } catch (Exception&) {
        // check if this is a missing value
        if ((translated_val.discr_val == std::numeric_limits< std::size_t >::max())
            && !this->_missing_symbols.empty())
          return *(this->_missing_symbols.begin());
        else
          GUM_ERROR(UnknownLabelInDatabase,
                    "The back translation of \"" << translated_val.discr_val
                                                 << "\" could not be found");
      }
    }


    /// indicates whether the translator has an editable dictionary or not
    template < template < typename > class ALLOC >
    INLINE bool
       DBTranslator4DiscretizedVariable< ALLOC >::hasEditableDictionary() const {
      return false;
    }


    /// sets/unset the editable dictionary mode
    template < template < typename > class ALLOC >
    INLINE void
       DBTranslator4DiscretizedVariable< ALLOC >::setEditableDictionaryMode(bool) {
    }


    /// indicates whether the translations should be reordered
    template < template < typename > class ALLOC >
    bool DBTranslator4DiscretizedVariable< ALLOC >::needsReordering() const {
      return false;
    }


    /// returns a mapping to reorder the current dictionary and updates it
    template < template < typename > class ALLOC >
    INLINE HashTable< std::size_t,
                      std::size_t,
                      ALLOC< std::pair< std::size_t, std::size_t > > >
           DBTranslator4DiscretizedVariable< ALLOC >::reorder() {
      return HashTable< std::size_t,
                        std::size_t,
                        ALLOC< std::pair< std::size_t, std::size_t > > >();
    }


    /// returns the domain size of a variable corresponding to the translations
    template < template < typename > class ALLOC >
    INLINE std::size_t
           DBTranslator4DiscretizedVariable< ALLOC >::domainSize() const {
      return __variable.domainSize();
    }


    /// returns the variable stored into the translator
    template < template < typename > class ALLOC >
    INLINE const IDiscretizedVariable*
                 DBTranslator4DiscretizedVariable< ALLOC >::variable() const {
      return __real_variable;
    }


    /// returns the translation of a missing value
    template < template < typename > class ALLOC >
    INLINE DBTranslatedValue
           DBTranslator4DiscretizedVariable< ALLOC >::missingValue() const {
      return DBTranslatedValue{std::numeric_limits< std::size_t >::max()};
    }


  } /* namespace learning */

} /* namespace gum */


#endif /* DOXYGEN_SHOULD_SKIP_THIS */
