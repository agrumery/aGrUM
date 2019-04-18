
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
 * @brief The databases' cell translators for labelized variables
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#include <utility>
#include <vector>

#include <agrum/learning/database/DBTranslator4LabelizedVariable.h>
#include <agrum/learning/database/DBCell.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {


    /// default constructor
    template < template < typename > class ALLOC >
    template < template < typename > class XALLOC >
    DBTranslator4LabelizedVariable< ALLOC >::DBTranslator4LabelizedVariable(
       const std::vector< std::string, XALLOC< std::string > >& missing_symbols,
       std::size_t                                              max_dico_entries,
       const typename DBTranslator4LabelizedVariable< ALLOC >::allocator_type&
          alloc) :
        DBTranslator< ALLOC >(DBTranslatedValueType::DISCRETE,
                              missing_symbols,
                              true,
                              max_dico_entries,
                              alloc),
        __variable("var", "", 0) {
      GUM_CONSTRUCTOR(DBTranslator4LabelizedVariable);
    }


    /// default constructor without missing symbols
    template < template < typename > class ALLOC >
    DBTranslator4LabelizedVariable< ALLOC >::DBTranslator4LabelizedVariable(
       std::size_t max_dico_entries,
       const typename DBTranslator4LabelizedVariable< ALLOC >::allocator_type&
          alloc) :
        DBTranslator< ALLOC >(
           DBTranslatedValueType::DISCRETE, true, max_dico_entries, alloc),
        __variable("var", "", 0) {
      GUM_CONSTRUCTOR(DBTranslator4LabelizedVariable);
    }


    /// default constructor with a labelized variable as translator
    template < template < typename > class ALLOC >
    template < template < typename > class XALLOC >
    DBTranslator4LabelizedVariable< ALLOC >::DBTranslator4LabelizedVariable(
       const LabelizedVariable&                                 var,
       const std::vector< std::string, XALLOC< std::string > >& missing_symbols,
       const bool  editable_dictionary,
       std::size_t max_dico_entries,
       const typename DBTranslator4LabelizedVariable< ALLOC >::allocator_type&
          alloc) :
        DBTranslator< ALLOC >(DBTranslatedValueType::DISCRETE,
                              missing_symbols,
                              editable_dictionary,
                              max_dico_entries,
                              alloc),
        __variable(var) {
      // check that the variable has not too many entries
      if (var.domainSize() > max_dico_entries) {
        GUM_ERROR(SizeError,
                  "the dictionary induced by the variable is too large");
      }

      // add the content of the variable into the back dictionary
      std::size_t size = 0;
      for (const auto& label : var.labels()) {
        // if the label corresponds to a missing value, then remove it from
        // the set of missing symbols.
        if (this->_missing_symbols.exists(label)) {
          this->_missing_symbols.erase(label);
        }

        // insert the label into the back_dictionary
        this->_back_dico.insert(size, label);
        ++size;
      }

      GUM_CONSTRUCTOR(DBTranslator4LabelizedVariable);
    }


    /// default constructor with a labelized variable as translator
    template < template < typename > class ALLOC >
    DBTranslator4LabelizedVariable< ALLOC >::DBTranslator4LabelizedVariable(
       const LabelizedVariable& var,
       const bool               editable_dictionary,
       std::size_t              max_dico_entries,
       const typename DBTranslator4LabelizedVariable< ALLOC >::allocator_type&
          alloc) :
        DBTranslator< ALLOC >(DBTranslatedValueType::DISCRETE,
                              editable_dictionary,
                              max_dico_entries,
                              alloc),
        __variable(var) {
      // check that the variable has not too many entries
      if (var.domainSize() > max_dico_entries) {
        GUM_ERROR(SizeError,
                  "the dictionary induced by the variable is too large");
      }

      // add the content of the variable into the back dictionary
      std::size_t size = 0;
      for (const auto& label : var.labels()) {
        // insert the label into the back_dictionary
        this->_back_dico.insert(size, label);
        ++size;
      }

      GUM_CONSTRUCTOR(DBTranslator4LabelizedVariable);
    }


    /// copy constructor with a given allocator
    template < template < typename > class ALLOC >
    INLINE DBTranslator4LabelizedVariable< ALLOC >::DBTranslator4LabelizedVariable(
       const DBTranslator4LabelizedVariable< ALLOC >& from,
       const typename DBTranslator4LabelizedVariable< ALLOC >::allocator_type&
          alloc) :
        DBTranslator< ALLOC >(from, alloc),
        __variable(from.__variable) {
      GUM_CONS_CPY(DBTranslator4LabelizedVariable);
    }


    /// copy constructor
    template < template < typename > class ALLOC >
    INLINE DBTranslator4LabelizedVariable< ALLOC >::DBTranslator4LabelizedVariable(
       const DBTranslator4LabelizedVariable< ALLOC >& from) :
        DBTranslator4LabelizedVariable< ALLOC >(from, from.getAllocator()) {}


    /// move constructor with a given allocator
    template < template < typename > class ALLOC >
    INLINE DBTranslator4LabelizedVariable< ALLOC >::DBTranslator4LabelizedVariable(
       DBTranslator4LabelizedVariable< ALLOC >&& from,
       const typename DBTranslator4LabelizedVariable< ALLOC >::allocator_type&
          alloc) :
        DBTranslator< ALLOC >(std::move(from), alloc),
        __variable(std::move(from.__variable)) {
      GUM_CONS_MOV(DBTranslator4LabelizedVariable);
    }


    /// move constructor
    template < template < typename > class ALLOC >
    INLINE DBTranslator4LabelizedVariable< ALLOC >::DBTranslator4LabelizedVariable(
       DBTranslator4LabelizedVariable< ALLOC >&& from) :
        DBTranslator4LabelizedVariable< ALLOC >(std::move(from),
                                                from.getAllocator()) {}


    /// virtual copy constructor with a given allocator
    template < template < typename > class ALLOC >
    DBTranslator4LabelizedVariable< ALLOC >*
       DBTranslator4LabelizedVariable< ALLOC >::clone(
          const typename DBTranslator4LabelizedVariable< ALLOC >::allocator_type&
             alloc) const {
      ALLOC< DBTranslator4LabelizedVariable< ALLOC > > allocator(alloc);
      DBTranslator4LabelizedVariable< ALLOC >* translator = allocator.allocate(1);
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
    INLINE DBTranslator4LabelizedVariable< ALLOC >*
           DBTranslator4LabelizedVariable< ALLOC >::clone() const {
      return clone(this->getAllocator());
    }


    /// destructor
    template < template < typename > class ALLOC >
    INLINE
       DBTranslator4LabelizedVariable< ALLOC >::~DBTranslator4LabelizedVariable() {
      GUM_DESTRUCTOR(DBTranslator4LabelizedVariable);
    }


    /// copy operator
    template < template < typename > class ALLOC >
    DBTranslator4LabelizedVariable< ALLOC >&
          DBTranslator4LabelizedVariable< ALLOC >::
          operator=(const DBTranslator4LabelizedVariable< ALLOC >& from) {
      if (this != &from) {
        DBTranslator< ALLOC >::operator=(from);
        __variable = from.__variable;
      }

      return *this;
    }


    /// move operator
    template < template < typename > class ALLOC >
    DBTranslator4LabelizedVariable< ALLOC >&
          DBTranslator4LabelizedVariable< ALLOC >::
          operator=(DBTranslator4LabelizedVariable< ALLOC >&& from) {
      if (this != &from) {
        DBTranslator< ALLOC >::operator=(std::move(from));
        __variable = std::move(from.__variable);
      }

      return *this;
    }


    /// returns the translation of a string, as found in the current dictionary
    template < template < typename > class ALLOC >
    DBTranslatedValue
       DBTranslator4LabelizedVariable< ALLOC >::translate(const std::string& str) {
      // try to get the index of str within the labelized variable. If this
      // cannot be found, try to find if this corresponds to a missing value.
      // Finally, if this is still not a missing value and, if enabled, try
      // to add str as a new label
      try {
        return DBTranslatedValue{std::size_t(__variable[str])};
      } catch (gum::Exception&) {
        // check that this is not a missing value
        if (this->isMissingSymbol(str)) {
          return DBTranslatedValue{std::numeric_limits< std::size_t >::max()};
        }

        // try to add str as a new value if possible
        if (this->hasEditableDictionary()) {
          const std::size_t size = __variable.domainSize();
          if (size >= this->_max_dico_entries)
            GUM_ERROR(SizeError,
                      "String \"" << str << "\" cannot be translated "
                                  << "because the dictionary is already full");
          __variable.addLabel(str);
          this->_back_dico.insert(size, str);
          return DBTranslatedValue{size};
        } else
          GUM_ERROR(UnknownLabelInDatabase,
                    "The translation of \"" << str << "\" could not be found");
      }
    }


    /// returns the original value for a given translation
    template < template < typename > class ALLOC >
    INLINE std::string DBTranslator4LabelizedVariable< ALLOC >::translateBack(
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


    /// indicates whether the translations should be reordered
    template < template < typename > class ALLOC >
    bool DBTranslator4LabelizedVariable< ALLOC >::needsReordering() const {
      // if the variable contains only numbers, they should be increasing
      const auto& labels = __variable.labels();
      float       last_number = std::numeric_limits< float >::lowest();
      float       number;
      bool        only_numbers = true;
      for (const auto& label : labels) {
        if (!DBCell::isReal(label)) {
          only_numbers = false;
          break;
        }
        number = std::stof(label);
        if (number < last_number) return true;
        last_number = number;
      }

      if (!only_numbers) {
        // here we shall examine whether the strings are sorted by
        // lexicographical order
        const std::size_t size = labels.size();
        for (std::size_t i = 1; i < size; ++i) {
          if (labels[i] < labels[i - 1]) return true;
        }
      }

      return false;
    }


    /// returns a mapping to reorder the current dictionary and updates it
    template < template < typename > class ALLOC >
    HashTable< std::size_t,
               std::size_t,
               ALLOC< std::pair< std::size_t, std::size_t > > >
       DBTranslator4LabelizedVariable< ALLOC >::reorder() {
      // check whether the variable contains only numeric values. In this
      // case, we have to sort the values by increasing number
      const auto&       labels = __variable.labels();
      const std::size_t size = labels.size();

      bool only_numbers = true;
      for (const auto& label : labels) {
        if (!DBCell::isReal(label)) {
          only_numbers = false;
          break;
        }
      }

      // assign to each label its current index
      std::vector< std::pair< std::size_t, std::string >,
                   ALLOC< std::pair< std::size_t, std::string > > >
         xlabels;
      xlabels.reserve(size);
      for (std::size_t i = std::size_t(0); i < size; ++i)
        xlabels.push_back(std::make_pair(i, labels[i]));

      // reorder by increasing order
      if (only_numbers)
        std::sort(xlabels.begin(),
                  xlabels.end(),
                  [](const std::pair< std::size_t, std::string >& a,
                     const std::pair< std::size_t, std::string >& b) -> bool {
                    return std::stof(a.second) < std::stof(b.second);
                  });
      else
        std::sort(xlabels.begin(),
                  xlabels.end(),
                  [](const std::pair< std::size_t, std::string >& a,
                     const std::pair< std::size_t, std::string >& b) -> bool {
                    return a.second < b.second;
                  });

      // check whether there were any modification
      bool modifications = false;
      for (std::size_t i = std::size_t(0); i < size; ++i) {
        if (xlabels[i].first != i) {
          modifications = true;
          break;
        }
      }

      // if there were no modification, return an empty update hashtable
      if (!modifications) {
        return HashTable< std::size_t,
                          std::size_t,
                          ALLOC< std::pair< std::size_t, std::size_t > > >();
      }

      // recreate the variable so that the labels correspond to the
      // new ordering
      __variable.eraseLabels();
      for (auto& label : xlabels)
        __variable.addLabel(label.second);

      // create the hashTable corresponding to the mapping from the old
      // indices to the new one
      this->_back_dico.clear();
      HashTable< std::size_t,
                 std::size_t,
                 ALLOC< std::pair< std::size_t, std::size_t > > >
         mapping((Size)size);
      for (std::size_t i = std::size_t(0); i < size; ++i) {
        mapping.insert(xlabels[i].first, i);
        this->_back_dico.insert(i, xlabels[i].second);
      }

      return mapping;
    }


    /// returns the domain size of a variable corresponding to the translations
    template < template < typename > class ALLOC >
    INLINE std::size_t
           DBTranslator4LabelizedVariable< ALLOC >::domainSize() const {
      return __variable.domainSize();
    }


    /// returns the variable stored into the translator
    template < template < typename > class ALLOC >
    INLINE const LabelizedVariable*
                 DBTranslator4LabelizedVariable< ALLOC >::variable() const {
      return &__variable;
    }


    /// returns the translation of a missing value
    template < template < typename > class ALLOC >
    INLINE DBTranslatedValue
           DBTranslator4LabelizedVariable< ALLOC >::missingValue() const {
      return DBTranslatedValue{std::numeric_limits< std::size_t >::max()};
    }


  } /* namespace learning */

} /* namespace gum */


#endif /* DOXYGEN_SHOULD_SKIP_THIS */
