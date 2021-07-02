/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
 * @brief The databases' cell translators for integer variables
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */

#include <utility>
#include <vector>
#include <sstream>

#include <agrum/tools/database/DBTranslator4IntegerVariable.h>
#include <agrum/tools/database/DBCell.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {


    /// default constructor with a integer variable as translator
    template < template < typename > class ALLOC >
    template < template < typename > class XALLOC >
    DBTranslator4IntegerVariable< ALLOC >::DBTranslator4IntegerVariable(
       const IntegerVariable&                                  var,
       const std::vector< std::string, XALLOC< std::string > >&                  missing_symbols,
       std::size_t                                                               max_dico_entries,
       const typename DBTranslator4IntegerVariable< ALLOC >::allocator_type& alloc) :
        DBTranslator< ALLOC >(DBTranslatedValueType::DISCRETE,
                              true,
                              missing_symbols,
                              false,
                              max_dico_entries,
                              alloc),
        _variable_(var) {
      // check that the variable has not too many entries
      if (var.domainSize() > max_dico_entries) {
        GUM_ERROR(SizeError, "the dictionary induced by the variable is too large")
      }

      // remove all the missing symbols corresponding to a number belonging to the variable
      for (auto iter = this->missing_symbols_.beginSafe(); iter != this->missing_symbols_.endSafe();
           ++iter) {
        if (DBCell::isInteger(*iter)) {
          const int missing_val = std::stoi(*iter);
          if (_variable_.integerDomain().exists(missing_val)) {
            this->missing_symbols_.erase(iter);
          }
        }
      }

      // add the content of the variable into the back dictionary
      std::size_t size = 0;
      for (const auto& label: var.labels()) {
        // if the label corresponds to a missing value, then remove it from
        // the set of missing symbols. If, in addition, it has already
        // been entered into the back_dictionary, then, this has been done
        // because the label corresponded to a missing value, so we should
        // remove the label as well from the back_dictionary.
        if (this->missing_symbols_.exists(label)) { this->missing_symbols_.erase(label); }

        this->back_dico_.insert(size, label);
        ++size;
      }

      // for debugging purposes
      GUM_CONSTRUCTOR(DBTranslator4IntegerVariable);
    }


    /// default constructor with an integer variable as translator but no missing value
    template < template < typename > class ALLOC >
    DBTranslator4IntegerVariable< ALLOC >::DBTranslator4IntegerVariable(
       const IntegerVariable&                                  var,
       std::size_t                                                               max_dico_entries,
       const typename DBTranslator4IntegerVariable< ALLOC >::allocator_type& alloc) :
        DBTranslator< ALLOC >(DBTranslatedValueType::DISCRETE,
                              true,
                              false,
                              max_dico_entries,
                              alloc),
        _variable_(var) {
      // check that the variable has not too many entries
      if (var.domainSize() > max_dico_entries) {
        GUM_ERROR(SizeError, "the dictionary induced by the variable is too large")
      }

      // add the content of the variable into the back dictionary
      std::size_t size = 0;
      for (const auto& label: var.labels()) {
        this->back_dico_.insert(size, label);
        ++size;
      }

      // for debugging purposes
      GUM_CONSTRUCTOR(DBTranslator4IntegerVariable);
    }


    /// copy constructor with a given allocator
    template < template < typename > class ALLOC >
    DBTranslator4IntegerVariable< ALLOC >::DBTranslator4IntegerVariable(
       const DBTranslator4IntegerVariable< ALLOC >&                          from,
       const typename DBTranslator4IntegerVariable< ALLOC >::allocator_type& alloc) :
        DBTranslator< ALLOC >(from, alloc),
        _variable_(from._variable_) {
      // for debugging purposes
      GUM_CONS_CPY(DBTranslator4IntegerVariable);
    }


    /// copy constructor
    template < template < typename > class ALLOC >
    DBTranslator4IntegerVariable< ALLOC >::DBTranslator4IntegerVariable(
       const DBTranslator4IntegerVariable< ALLOC >& from) :
        DBTranslator4IntegerVariable< ALLOC >(from, from.getAllocator()) {}


    /// move constructor with a given allocator
    template < template < typename > class ALLOC >
    DBTranslator4IntegerVariable< ALLOC >::DBTranslator4IntegerVariable(
       DBTranslator4IntegerVariable< ALLOC >&&                               from,
       const typename DBTranslator4IntegerVariable< ALLOC >::allocator_type& alloc) :
        DBTranslator< ALLOC >(std::move(from), alloc),
        _variable_(std::move(from._variable_)) {
      // for debugging purposes
      GUM_CONS_MOV(DBTranslator4IntegerVariable);
    }


    /// move constructor
    template < template < typename > class ALLOC >
    DBTranslator4IntegerVariable< ALLOC >::DBTranslator4IntegerVariable(
       DBTranslator4IntegerVariable< ALLOC >&& from) :
        DBTranslator4IntegerVariable< ALLOC >(std::move(from), from.getAllocator()) {}


    /// virtual copy constructor with a given allocator
    template < template < typename > class ALLOC >
    DBTranslator4IntegerVariable< ALLOC >* DBTranslator4IntegerVariable< ALLOC >::clone(
       const typename DBTranslator4IntegerVariable< ALLOC >::allocator_type& alloc) const {
      ALLOC< DBTranslator4IntegerVariable< ALLOC > > allocator(alloc);
      DBTranslator4IntegerVariable< ALLOC >*         translator = allocator.allocate(1);
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
    INLINE DBTranslator4IntegerVariable< ALLOC >*
           DBTranslator4IntegerVariable< ALLOC >::clone() const {
      return clone(this->getAllocator());
    }


    /// destructor
    template < template < typename > class ALLOC >
    INLINE DBTranslator4IntegerVariable< ALLOC >::~DBTranslator4IntegerVariable() {
      // for debugging purposes
      GUM_DESTRUCTOR(DBTranslator4IntegerVariable);
    }


    /// copy operator
    template < template < typename > class ALLOC >
    DBTranslator4IntegerVariable< ALLOC >& DBTranslator4IntegerVariable< ALLOC >::operator=(
       const DBTranslator4IntegerVariable< ALLOC >& from) {
      if (this != &from) {
        DBTranslator< ALLOC >::operator=(from);
        _variable_                     = from._variable_;
      }

      return *this;
    }


    /// move operator
    template < template < typename > class ALLOC >
    DBTranslator4IntegerVariable< ALLOC >& DBTranslator4IntegerVariable< ALLOC >::operator=(
       DBTranslator4IntegerVariable< ALLOC >&& from) {
      if (this != &from) {
        DBTranslator< ALLOC >::operator=(std::move(from));
        _variable_                     = std::move(from._variable_);
      }

      return *this;
    }


    /// returns the translation of a string, as found in the current dictionary
    template < template < typename > class ALLOC >
    INLINE DBTranslatedValue
       DBTranslator4IntegerVariable< ALLOC >::translate(const std::string& str) {
      // try to get the index of str within the integer variable.
      try {
        return DBTranslatedValue{std::size_t(_variable_[str])};
      } catch (gum::Exception&) {
        // check for a missing symbol
        if (this->isMissingSymbol(str))
          return DBTranslatedValue{std::numeric_limits< std::size_t >::max()};

        // check if the back_dictionary does not contain str. This enables
        // to execute translate ( translateBack ( translate ( str ) ) )
        // without raising an exception
        try {
          return DBTranslatedValue{this->back_dico_.first(str)};
        } catch (gum::Exception&) {
          if (!DBCell::isInteger(str)) {
            GUM_ERROR(TypeError,
                      "String \"" << str << "\" cannot be translated because it is not a number");
          } else {
            GUM_ERROR(UnknownLabelInDatabase,
                      "The translation of \"" << str << "\" could not be found")
          }
        }
      }
    }


    /// returns the original value for a given translation
    template < template < typename > class ALLOC >
    INLINE std::string DBTranslator4IntegerVariable< ALLOC >::translateBack(
       const DBTranslatedValue translated_val) const {
      try {
        return this->back_dico_.second(translated_val.discr_val);
      } catch (Exception&) {
        // check if this is a missing value
        if ((translated_val.discr_val == std::numeric_limits< std::size_t >::max())
            && !this->missing_symbols_.empty())
          return *(this->missing_symbols_.begin());
        else
          GUM_ERROR(UnknownLabelInDatabase,
                    "The back translation of \"" << translated_val.discr_val
                                                 << "\" could not be found");
      }
    }


    /// indicates whether the translator has an editable dictionary or not
    template < template < typename > class ALLOC >
    INLINE bool DBTranslator4IntegerVariable< ALLOC >::hasEditableDictionary() const {
      return false;
    }


    /// sets/unset the editable dictionary mode
    template < template < typename > class ALLOC >
    INLINE void DBTranslator4IntegerVariable< ALLOC >::setEditableDictionaryMode(bool) {}


    /// indicates whether the translations should be reordered
    template < template < typename > class ALLOC >
    bool DBTranslator4IntegerVariable< ALLOC >::needsReordering() const {
      return false;
    }


    /// returns a mapping to reorder the current dictionary and updates it
    template < template < typename > class ALLOC >
    INLINE HashTable< std::size_t, std::size_t, ALLOC< std::pair< std::size_t, std::size_t > > >
           DBTranslator4IntegerVariable< ALLOC >::reorder() {
      return HashTable< std::size_t,
                        std::size_t,
                        ALLOC< std::pair< std::size_t, std::size_t > > >();
    }


    /// returns the domain size of a variable corresponding to the translations
    template < template < typename > class ALLOC >
    INLINE std::size_t DBTranslator4IntegerVariable< ALLOC >::domainSize() const {
      return _variable_.domainSize();
    }


    /// returns the variable stored into the translator
    template < template < typename > class ALLOC >
    INLINE const IntegerVariable* DBTranslator4IntegerVariable< ALLOC >::variable() const {
      return &_variable_;
    }


    /// returns the translation of a missing value
    template < template < typename > class ALLOC >
    INLINE DBTranslatedValue DBTranslator4IntegerVariable< ALLOC >::missingValue() const {
      return DBTranslatedValue{std::numeric_limits< std::size_t >::max()};
    }


  } /* namespace learning */

} /* namespace gum */


#endif /* DOXYGEN_SHOULD_SKIP_THIS */
