
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
 * @brief The set of translators stored into a row filter
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// returns the allocator used by the translator set
    template < template < typename > class ALLOC >
    typename DBTranslatorSet< ALLOC >::allocator_type
       DBTranslatorSet< ALLOC >::getAllocator() const {
      return __columns.get_allocator();
    }


    /// remove all the translators from the vector
    template < template < typename > class ALLOC >
    void DBTranslatorSet< ALLOC >::clear() {
      ALLOC< DBTranslator< ALLOC > > allocator(this->getAllocator());
      for (auto translator : __translators) {
        allocator.destroy(translator);
        allocator.deallocate(translator, 1);
      }

      __translators.clear();
      __columns.clear();
      __highest_column = std::size_t(0);
    }


    /// copy the content of another translator set that uses another allocator
    template < template < typename > class ALLOC >
    void DBTranslatorSet< ALLOC >::__copy(
       const DBTranslatorSet< ALLOC >&                          from,
       const typename DBTranslatorSet< ALLOC >::allocator_type& alloc) {
      if (__translators.size() != 0) clear();

      // resize the vectors used in the set. First, we reserve new memory. This
      // will keep the DBTranslatorSet in a correct state, even if the memory
      // allocation fails
      const std::size_t size = from.__translators.size();
      __translators.reserve(size);
      __columns.reserve(size);
      __translators.resize(size);
      __columns.resize(size);

      std::size_t i;
      try {
        for (i = std::size_t(0); i < size; ++i) {
          __translators[i] = from.__translators[i]->clone(alloc);
          __columns[i] = from.__columns[i];
        }
      } catch (...) {
        __translators.resize(i);
        clear();
        throw;
      }

      __highest_column = from.__highest_column;
    }


    /// default constructor
    template < template < typename > class ALLOC >
    INLINE DBTranslatorSet< ALLOC >::DBTranslatorSet(
       const typename DBTranslatorSet< ALLOC >::allocator_type& alloc) :
        __translators(alloc),
        __columns(allocator_type(alloc)) {
      GUM_CONSTRUCTOR(DBTranslatorSet);
    }


    /// copy constructor with a given allocator
    template < template < typename > class ALLOC >
    INLINE DBTranslatorSet< ALLOC >::DBTranslatorSet(
       const DBTranslatorSet< ALLOC >&                          from,
       const typename DBTranslatorSet< ALLOC >::allocator_type& alloc) :
        __translators(alloc),
        __columns(alloc) {
      __copy(from, alloc);

      GUM_CONS_CPY(DBTranslatorSet);
    }


    /// copy constructor
    template < template < typename > class ALLOC >
    INLINE DBTranslatorSet< ALLOC >::DBTranslatorSet(
       const DBTranslatorSet< ALLOC >& from) :
        DBTranslatorSet< ALLOC >(from, from.getAllocator()) {}


    /// move constructor with a given allocator
    template < template < typename > class ALLOC >
    INLINE DBTranslatorSet< ALLOC >::DBTranslatorSet(
       DBTranslatorSet< ALLOC >&&                               from,
       const typename DBTranslatorSet< ALLOC >::allocator_type& alloc) :
        __translators(std::move(from.__translators), alloc),
        __columns(std::move(from.__columns), alloc),
        __highest_column(from.__highest_column) {
      GUM_CONS_MOV(DBTranslatorSet);
    }


    /// move constructor
    template < template < typename > class ALLOC >
    INLINE
       DBTranslatorSet< ALLOC >::DBTranslatorSet(DBTranslatorSet< ALLOC >&& from) :
        DBTranslatorSet< ALLOC >(from, from.getAllocator()) {}


    /// virtual copy constructor with a given allocator
    template < template < typename > class ALLOC >
    DBTranslatorSet< ALLOC >* DBTranslatorSet< ALLOC >::clone(
       const typename DBTranslatorSet< ALLOC >::allocator_type& alloc) const {
      ALLOC< DBTranslatorSet< ALLOC > > allocator(alloc);
      DBTranslatorSet< ALLOC >*         set = allocator.allocate(1);
      try {
        allocator.construct(set, *this, alloc);
      } catch (...) {
        allocator.deallocate(set, 1);
        throw;
      }
      return set;
    }


    /// virtual copy constructor
    template < template < typename > class ALLOC >
    DBTranslatorSet< ALLOC >* DBTranslatorSet< ALLOC >::clone() const {
      return clone(this->getAllocator());
    }


    /// destructor
    template < template < typename > class ALLOC >
    INLINE DBTranslatorSet< ALLOC >::~DBTranslatorSet() {
      clear();
      GUM_DESTRUCTOR(DBTranslatorSet);
    }


    /// copy operator
    template < template < typename > class ALLOC >
    DBTranslatorSet< ALLOC >& DBTranslatorSet< ALLOC >::
                              operator=(const DBTranslatorSet< ALLOC >& from) {
      if (this != &from) {
        clear();
        __copy(from, this->getAllocator());
      }

      return *this;
    }


    /// move operator
    template < template < typename > class ALLOC >
    INLINE DBTranslatorSet< ALLOC >& DBTranslatorSet< ALLOC >::
                                     operator=(DBTranslatorSet< ALLOC >&& from) {
      if (this != &from) {
        clear();
        __translators = std::move(from.__translators);
        __columns = std::move(from.__columns);
        __highest_column = from.__highest_column;
      }

      return *this;
    }


    /// returns the ith translator
    template < template < typename > class ALLOC >
    INLINE DBTranslator< ALLOC >& DBTranslatorSet< ALLOC >::
                                  operator[](const std::size_t k) {
      return *(__translators[k]);
    }


    /// returns the ith translator
    template < template < typename > class ALLOC >
    INLINE const DBTranslator< ALLOC >& DBTranslatorSet< ALLOC >::
                                        operator[](const std::size_t k) const {
      return *(__translators[k]);
    }


    /// inserts a new translator at the end of the translator set
    template < template < typename > class ALLOC >
    template < template < template < typename > class > class Translator >
    std::size_t DBTranslatorSet< ALLOC >::insertTranslator(
       const Translator< ALLOC >& translator,
       const std::size_t          column,
       const bool                 unique_column) {
      // if the unique_column parameter is set to true and there exists already
      // another translator that parses the column, raise a DuplicateElement
      // exception
      const std::size_t size = __translators.size();
      if (unique_column) {
        for (std::size_t i = std::size_t(0); i < size; ++i) {
          if (__columns[i] == column)
            GUM_ERROR(DuplicateElement,
                      "There already exists a DBTranslator that parses Column"
                         << column);
        }
      }

      // reserve some place for the new translator
      __translators.reserve(size + 1);
      __columns.reserve(size + 1);

      // create and add the new translator
      ALLOC< DBTranslator< ALLOC > > allocator(this->getAllocator());
      DBTranslator< ALLOC >*         new_translator = translator.clone(allocator);

      __translators.resize(size + 1);
      __columns.resize(size + 1);
      __translators[size] = new_translator;
      __columns[size] = column;

      // update the highest column
      if (column > __highest_column) __highest_column = column;

      return size;
    }


    /// inserts a new translator for a given variable in the translator set
    template < template < typename > class ALLOC >
    template < template < typename > class XALLOC >
    std::size_t DBTranslatorSet< ALLOC >::insertTranslator(
       const Variable&                                          var,
       const std::size_t                                        column,
       const std::vector< std::string, XALLOC< std::string > >& missing_symbols,
       const bool                                               unique_column) {
      // create the translatator, depending on the type of the variable
      switch (var.varType()) {
        case VarType::Labelized: {
          const LabelizedVariable& xvar =
             static_cast< const LabelizedVariable& >(var);
          DBTranslator4LabelizedVariable< ALLOC > translator(xvar,
                                                             missing_symbols);
          return insertTranslator(translator, column, unique_column);
        }

        case VarType::Discretized: {
          const IDiscretizedVariable& xvar =
             static_cast< const IDiscretizedVariable& >(var);
          DBTranslator4DiscretizedVariable< ALLOC > translator(xvar,
                                                               missing_symbols);
          return insertTranslator(translator, column, unique_column);
        }

        case VarType::Range: {
          const RangeVariable& xvar = static_cast< const RangeVariable& >(var);
          DBTranslator4RangeVariable< ALLOC > translator(xvar, missing_symbols);
          return insertTranslator(translator, column, unique_column);
        }

        case VarType::Continuous: {
          const IContinuousVariable& xvar =
             static_cast< const IContinuousVariable& >(var);
          DBTranslator4ContinuousVariable< ALLOC > translator(xvar,
                                                              missing_symbols);
          return insertTranslator(translator, column, unique_column);
        }

        default:
          GUM_ERROR(NotImplementedYet,
                    "The insertion of the translator for Variable "
                       << var.name()
                       << " is impossible because a translator "
                          "for such variable is not implemented yet");
      }
    }


    /// inserts a new translator for a given variable in the translator set
    template < template < typename > class ALLOC >
    INLINE std::size_t DBTranslatorSet< ALLOC >::insertTranslator(
       const Variable& var, const std::size_t column, const bool unique_column) {
      const std::vector< std::string, ALLOC< std::string > > missing;
      return this->insertTranslator(var, column, missing, unique_column);
    }


    /// erase the kth translator
    template < template < typename > class ALLOC >
    void DBTranslatorSet< ALLOC >::eraseTranslator(const std::size_t k,
                                                   const bool k_is_input_col) {
      ALLOC< DBTranslator< ALLOC > > allocator(this->getAllocator());
      const std::size_t              nb_trans = __translators.size();

      if (!k_is_input_col) {
        if (nb_trans < k) return;

        // remove the translator and its corresponding column
        allocator.destroy(__translators[k]);
        allocator.deallocate(__translators[k], 1);

        const std::size_t colk = __columns[k];
        __translators.erase(__translators.begin() + k);
        __columns.erase(__columns.begin() + k);

        // if the highest column index corresponded to the kth translator,
        // we must recomput it
        if (__highest_column == colk) {
          __highest_column = std::size_t(0);
          for (const auto col : __columns)
            if (__highest_column < col) __highest_column = col;
        }
      } else {
        // remove all the translators parsing the kth column
        auto iter_trans = __translators.rbegin();
        bool translator_found = false;
        for (auto iter_col = __columns.rbegin(); iter_col != __columns.rend();
             ++iter_col, ++iter_trans) {
          if (*iter_col == k) {
            // remove the translator and its corresponding column
            allocator.destroy(*iter_trans);
            allocator.deallocate(*iter_trans, 1);

            __translators.erase((iter_trans + 1).base());
            __columns.erase((iter_col + 1).base());
            translator_found = true;
          }
        }

        // if the highest column index corresponded to one of the translators
        // removed, we must recompute it
        if (translator_found && (k == __highest_column)) {
          __highest_column = std::size_t(0);
          for (const auto col : __columns)
            if (__highest_column < col) __highest_column = col;
        }
      }
    }


    /// ask the kth translator to translate a string in a row of the database
    template < template < typename > class ALLOC >
    template < template < typename > class OTHER_ALLOC >
    INLINE DBTranslatedValue DBTranslatorSet< ALLOC >::translate(
       const std::vector< std::string, OTHER_ALLOC< std::string > >& row,
       const std::size_t                                             k) const {
      return __translators[k]->translate(row[__columns[k]]);
    }


    /// ask the kth translator to translate a string in a row of the database
    template < template < typename > class ALLOC >
    template < template < typename > class OTHER_ALLOC >
    INLINE DBTranslatedValue DBTranslatorSet< ALLOC >::translateSafe(
       const std::vector< std::string, OTHER_ALLOC< std::string > >& row,
       const std::size_t                                             k) const {
      if (__translators.size() <= k)
        GUM_ERROR(UndefinedElement, "Translator #" << k << " could not be found");
      return __translators[k]->translate(row[__columns[k]]);
    }


    /// returns the original string that was translated into translated_val
    template < template < typename > class ALLOC >
    INLINE std::string DBTranslatorSet< ALLOC >::translateBack(
       const DBTranslatedValue translated_val, const std::size_t k) const {
      return __translators[k]->translateBack(translated_val);
    }


    /// returns the original string that was translated into translated_val
    template < template < typename > class ALLOC >
    INLINE std::string DBTranslatorSet< ALLOC >::translateBackSafe(
       const DBTranslatedValue translated_val, const std::size_t k) const {
      if (__translators.size() <= k)
        GUM_ERROR(UndefinedElement, "Translator #" << k << "could not be found");
      return __translators[k]->translateBack(translated_val);
    }


    // indicates whether the kth translator considers a translated_val
    // as a missing value
    template < template < typename > class ALLOC >
    INLINE bool DBTranslatorSet< ALLOC >::isMissingValue(
       const DBTranslatedValue translated_val, const std::size_t k) const {
      return __translators[k]->isMissingValue(translated_val);
    }


    // indicates whether the kth translator considers a translated_val
    // as a missing value
    template < template < typename > class ALLOC >
    INLINE bool DBTranslatorSet< ALLOC >::isMissingValueSafe(
       const DBTranslatedValue translated_val, const std::size_t k) const {
      if (__translators.size() <= k)
        GUM_ERROR(UndefinedElement, "Translator #" << k << "could not be found");
      return __translators[k]->isMissingValue(translated_val);
    }


    /// returns the kth translator
    template < template < typename > class ALLOC >
    INLINE DBTranslator< ALLOC >&
           DBTranslatorSet< ALLOC >::translator(const std::size_t k) {
      return *(__translators[k]);
    }


    /// returns the kth translator
    template < template < typename > class ALLOC >
    INLINE const DBTranslator< ALLOC >&
                 DBTranslatorSet< ALLOC >::translator(const std::size_t k) const {
      return *(__translators[k]);
    }


    /// returns the kth translator
    template < template < typename > class ALLOC >
    INLINE DBTranslator< ALLOC >&
           DBTranslatorSet< ALLOC >::translatorSafe(const std::size_t k) {
      if (__translators.size() <= k)
        GUM_ERROR(UndefinedElement, "Translator #" << k << "could not be found");
      return *(__translators[k]);
    }


    /// returns the kth translator
    template < template < typename > class ALLOC >
    INLINE const DBTranslator< ALLOC >&
                 DBTranslatorSet< ALLOC >::translatorSafe(const std::size_t k) const {
      if (__translators.size() <= k)
        GUM_ERROR(UndefinedElement, "Translator #" << k << "could not be found");
      return *(__translators[k]);
    }


    /// returns the domain size of the variables stored into the kth translator
    template < template < typename > class ALLOC >
    INLINE std::size_t
           DBTranslatorSet< ALLOC >::domainSize(const std::size_t k) const {
      return __translators[k]->domainSize();
    }


    /// returns the domain size of the variables stored into the kth translator
    template < template < typename > class ALLOC >
    INLINE std::size_t
           DBTranslatorSet< ALLOC >::domainSizeSafe(const std::size_t k) const {
      if (__translators.size() <= k)
        GUM_ERROR(UndefinedElement, "Variable #" << k << "could not be found");
      return __translators[k]->domainSize();
    }


    /// returns the variable stored into the kth translator
    template < template < typename > class ALLOC >
    INLINE const Variable&
                 DBTranslatorSet< ALLOC >::variable(const std::size_t k) const {
      return *(__translators[k]->variable());
    }


    /// returns the variable stored into the kth translator
    template < template < typename > class ALLOC >
    INLINE const Variable&
                 DBTranslatorSet< ALLOC >::variableSafe(const std::size_t k) const {
      if (__translators.size() <= k)
        GUM_ERROR(UndefinedElement, "Variable #" << k << "could not be found");
      return *(__translators[k]->variable());
    }


    // indicates whether a reordering is needed to make the kth translator
    // sorted by lexicographical order
    template < template < typename > class ALLOC >
    INLINE bool
       DBTranslatorSet< ALLOC >::needsReordering(const std::size_t k) const {
      return __translators[k]->needsReordering();
    }


    // indicates whether a reordering is needed to make the kth translator
    // sorted by lexicographical order
    template < template < typename > class ALLOC >
    INLINE bool
       DBTranslatorSet< ALLOC >::needsReorderingSafe(const std::size_t k) const {
      if (__translators.size() <= k)
        GUM_ERROR(UndefinedElement, "Variable #" << k << "could not be found");
      return __translators[k]->needsReordering();
    }


    // performs a reordering of the dictionary and returns a mapping
    // from the old translated values to the new ones.
    template < template < typename > class ALLOC >
    INLINE HashTable< std::size_t,
                      std::size_t,
                      ALLOC< std::pair< std::size_t, std::size_t > > >
           DBTranslatorSet< ALLOC >::reorder(const std::size_t k) {
      return __translators[k]->reorder();
    }


    // performs a reordering of the dictionary and returns a mapping
    // from the old translated values to the new ones.
    template < template < typename > class ALLOC >
    INLINE HashTable< std::size_t,
                      std::size_t,
                      ALLOC< std::pair< std::size_t, std::size_t > > >
           DBTranslatorSet< ALLOC >::reorderSafe(const std::size_t k) {
      if (__translators.size() <= k)
        GUM_ERROR(UndefinedElement, "Variable #" << k << "could not be found");
      return __translators[k]->reorder();
    }


    /** @brief returns the column of the input database that will be written
     * in the kth column of the DatabaseTable */
    template < template < typename > class ALLOC >
    INLINE std::size_t
           DBTranslatorSet< ALLOC >::inputColumn(const std::size_t k) const {
      return __columns[k];
    }


    /** @brief returns the column of the input database that will be written
     * in the kth column of the DatabaseTable */
    template < template < typename > class ALLOC >
    INLINE std::size_t
           DBTranslatorSet< ALLOC >::inputColumnSafe(const std::size_t k) const {
      if (__translators.size() <= k)
        GUM_ERROR(UndefinedElement, "Column #" << k << "could not be found");
      return __columns[k];
    }


    /// returns the largest input database column index read by the translators
    template < template < typename > class ALLOC >
    INLINE std::size_t DBTranslatorSet< ALLOC >::highestInputColumn() const {
      return __highest_column;
    }


    /// returns the number of translators stored into the set
    template < template < typename > class ALLOC >
    INLINE std::size_t DBTranslatorSet< ALLOC >::nbTranslators() const {
      return __columns.size();
    }


    /// returns the number of translators stored into the set
    template < template < typename > class ALLOC >
    INLINE std::size_t DBTranslatorSet< ALLOC >::size() const {
      return __columns.size();
    }

    /// returns the set of translators
    template < template < typename > class ALLOC >
    INLINE const
       std::vector< DBTranslator< ALLOC >*, ALLOC< DBTranslator< ALLOC >* > >&
       DBTranslatorSet< ALLOC >::translators() const {
      return __translators;
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
