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
 * @brief The set of translators stored into a row filter
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// returns the allocator used by the translator set
    template < template < typename > class ALLOC >
    typename DBTranslatorSet< ALLOC >::allocator_type
       DBTranslatorSet< ALLOC >::getAllocator() const {
      return columns__.get_allocator();
    }


    /// remove all the translators from the vector
    template < template < typename > class ALLOC >
    void DBTranslatorSet< ALLOC >::clear() {
      ALLOC< DBTranslator< ALLOC > > allocator(this->getAllocator());
      for (auto translator: translators__) {
        allocator.destroy(translator);
        allocator.deallocate(translator, 1);
      }

      translators__.clear();
      columns__.clear();
      highest_column__ = std::size_t(0);
    }


    /// copy the content of another translator set that uses another allocator
    template < template < typename > class ALLOC >
    void DBTranslatorSet< ALLOC >::copy__(
       const DBTranslatorSet< ALLOC >&                          from,
       const typename DBTranslatorSet< ALLOC >::allocator_type& alloc) {
      if (translators__.size() != 0) clear();

      // resize the vectors used in the set. First, we reserve new memory. This
      // will keep the DBTranslatorSet in a correct state, even if the memory
      // allocation fails
      const std::size_t size = from.translators__.size();
      translators__.reserve(size);
      columns__.reserve(size);
      translators__.resize(size);
      columns__.resize(size);

      std::size_t i;
      try {
        for (i = std::size_t(0); i < size; ++i) {
          translators__[i] = from.translators__[i]->clone(alloc);
          columns__[i]     = from.columns__[i];
        }
      } catch (...) {
        translators__.resize(i);
        clear();
        throw;
      }

      highest_column__ = from.highest_column__;
    }


    /// default constructor
    template < template < typename > class ALLOC >
    INLINE DBTranslatorSet< ALLOC >::DBTranslatorSet(
       const typename DBTranslatorSet< ALLOC >::allocator_type& alloc) :
        translators__(alloc),
        columns__(allocator_type(alloc)) {
      GUM_CONSTRUCTOR(DBTranslatorSet);
    }


    /// copy constructor with a given allocator
    template < template < typename > class ALLOC >
    INLINE DBTranslatorSet< ALLOC >::DBTranslatorSet(
       const DBTranslatorSet< ALLOC >&                          from,
       const typename DBTranslatorSet< ALLOC >::allocator_type& alloc) :
        translators__(alloc),
        columns__(alloc) {
      copy__(from, alloc);

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
        translators__(std::move(from.translators__), alloc),
        columns__(std::move(from.columns__), alloc),
        highest_column__(from.highest_column__) {
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
    DBTranslatorSet< ALLOC >&
       DBTranslatorSet< ALLOC >::operator=(const DBTranslatorSet< ALLOC >& from) {
      if (this != &from) {
        clear();
        copy__(from, this->getAllocator());
      }

      return *this;
    }


    /// move operator
    template < template < typename > class ALLOC >
    INLINE DBTranslatorSet< ALLOC >&
       DBTranslatorSet< ALLOC >::operator=(DBTranslatorSet< ALLOC >&& from) {
      if (this != &from) {
        clear();
        translators__    = std::move(from.translators__);
        columns__        = std::move(from.columns__);
        highest_column__ = from.highest_column__;
      }

      return *this;
    }


    /// returns the ith translator
    template < template < typename > class ALLOC >
    INLINE DBTranslator< ALLOC >&
       DBTranslatorSet< ALLOC >::operator[](const std::size_t k) {
      return *(translators__[k]);
    }


    /// returns the ith translator
    template < template < typename > class ALLOC >
    INLINE const DBTranslator< ALLOC >&
       DBTranslatorSet< ALLOC >::operator[](const std::size_t k) const {
      return *(translators__[k]);
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
      const std::size_t size = translators__.size();
      if (unique_column) {
        for (std::size_t i = std::size_t(0); i < size; ++i) {
          if (columns__[i] == column)
            GUM_ERROR(DuplicateElement,
                      "There already exists a DBTranslator that parses Column"
                         << column)
        }
      }

      // reserve some place for the new translator
      translators__.reserve(size + 1);
      columns__.reserve(size + 1);

      // create and add the new translator
      ALLOC< DBTranslator< ALLOC > > allocator(this->getAllocator());
      DBTranslator< ALLOC >*         new_translator = translator.clone(allocator);

      translators__.resize(size + 1);
      columns__.resize(size + 1);
      translators__[size] = new_translator;
      columns__[size]     = column;

      // update the highest column
      if (column > highest_column__) highest_column__ = column;

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
          const LabelizedVariable& xvar
             = static_cast< const LabelizedVariable& >(var);
          DBTranslator4LabelizedVariable< ALLOC > translator(xvar,
                                                             missing_symbols);
          return insertTranslator(translator, column, unique_column);
        }

        case VarType::Discretized: {
          const IDiscretizedVariable& xvar
             = static_cast< const IDiscretizedVariable& >(var);
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
          const IContinuousVariable& xvar
             = static_cast< const IContinuousVariable& >(var);
          DBTranslator4ContinuousVariable< ALLOC > translator(xvar,
                                                              missing_symbols);
          return insertTranslator(translator, column, unique_column);
        }

        default:
          GUM_ERROR(NotImplementedYet,
                    "The insertion of the translator for Variable "
                       << var.name()
                       << " is impossible because a translator "
                          "for such variable is not implemented yet")
      }
    }


    /// inserts a new translator for a given variable in the translator set
    template < template < typename > class ALLOC >
    INLINE std::size_t
           DBTranslatorSet< ALLOC >::insertTranslator(const Variable&   var,
                                                  const std::size_t column,
                                                  const bool unique_column) {
      const std::vector< std::string, ALLOC< std::string > > missing;
      return this->insertTranslator(var, column, missing, unique_column);
    }


    /// erase the kth translator
    template < template < typename > class ALLOC >
    void DBTranslatorSet< ALLOC >::eraseTranslator(const std::size_t k,
                                                   const bool k_is_input_col) {
      ALLOC< DBTranslator< ALLOC > > allocator(this->getAllocator());
      const std::size_t              nb_trans = translators__.size();

      if (!k_is_input_col) {
        if (nb_trans < k) return;

        // remove the translator and its corresponding column
        allocator.destroy(translators__[k]);
        allocator.deallocate(translators__[k], 1);

        const std::size_t colk = columns__[k];
        translators__.erase(translators__.begin() + k);
        columns__.erase(columns__.begin() + k);

        // if the highest column index corresponded to the kth translator,
        // we must recomput it
        if (highest_column__ == colk) {
          highest_column__ = std::size_t(0);
          for (const auto col: columns__)
            if (highest_column__ < col) highest_column__ = col;
        }
      } else {
        // remove all the translators parsing the kth column
        auto iter_trans       = translators__.rbegin();
        bool translator_found = false;
        for (auto iter_col = columns__.rbegin(); iter_col != columns__.rend();
             ++iter_col, ++iter_trans) {
          if (*iter_col == k) {
            // remove the translator and its corresponding column
            allocator.destroy(*iter_trans);
            allocator.deallocate(*iter_trans, 1);

            translators__.erase((iter_trans + 1).base());
            columns__.erase((iter_col + 1).base());
            translator_found = true;
          }
        }

        // if the highest column index corresponded to one of the translators
        // removed, we must recompute it
        if (translator_found && (k == highest_column__)) {
          highest_column__ = std::size_t(0);
          for (const auto col: columns__)
            if (highest_column__ < col) highest_column__ = col;
        }
      }
    }


    /// ask the kth translator to translate a string in a row of the database
    template < template < typename > class ALLOC >
    template < template < typename > class OTHER_ALLOC >
    INLINE DBTranslatedValue DBTranslatorSet< ALLOC >::translate(
       const std::vector< std::string, OTHER_ALLOC< std::string > >& row,
       const std::size_t                                             k) const {
      return translators__[k]->translate(row[columns__[k]]);
    }


    /// ask the kth translator to translate a string in a row of the database
    template < template < typename > class ALLOC >
    template < template < typename > class OTHER_ALLOC >
    INLINE DBTranslatedValue DBTranslatorSet< ALLOC >::translateSafe(
       const std::vector< std::string, OTHER_ALLOC< std::string > >& row,
       const std::size_t                                             k) const {
      if (translators__.size() <= k)
        GUM_ERROR(UndefinedElement, "Translator #" << k << " could not be found")

      return translators__[k]->translate(row[columns__[k]]);
    }


    /// returns the original string that was translated into translated_val
    template < template < typename > class ALLOC >
    INLINE std::string DBTranslatorSet< ALLOC >::translateBack(
       const DBTranslatedValue translated_val,
       const std::size_t       k) const {
      return translators__[k]->translateBack(translated_val);
    }


    /// returns the original string that was translated into translated_val
    template < template < typename > class ALLOC >
    INLINE std::string DBTranslatorSet< ALLOC >::translateBackSafe(
       const DBTranslatedValue translated_val,
       const std::size_t       k) const {
      if (translators__.size() <= k)
        GUM_ERROR(UndefinedElement, "Translator #" << k << "could not be found")

      return translators__[k]->translateBack(translated_val);
    }


    // indicates whether the kth translator considers a translated_val
    // as a missing value
    template < template < typename > class ALLOC >
    INLINE bool DBTranslatorSet< ALLOC >::isMissingValue(
       const DBTranslatedValue translated_val,
       const std::size_t       k) const {
      return translators__[k]->isMissingValue(translated_val);
    }


    // indicates whether the kth translator considers a translated_val
    // as a missing value
    template < template < typename > class ALLOC >
    INLINE bool DBTranslatorSet< ALLOC >::isMissingValueSafe(
       const DBTranslatedValue translated_val,
       const std::size_t       k) const {
      if (translators__.size() <= k)
        GUM_ERROR(UndefinedElement, "Translator #" << k << "could not be found")

      return translators__[k]->isMissingValue(translated_val);
    }


    /// returns the kth translator
    template < template < typename > class ALLOC >
    INLINE DBTranslator< ALLOC >&
           DBTranslatorSet< ALLOC >::translator(const std::size_t k) {
      return *(translators__[k]);
    }


    /// returns the kth translator
    template < template < typename > class ALLOC >
    INLINE const DBTranslator< ALLOC >&
                 DBTranslatorSet< ALLOC >::translator(const std::size_t k) const {
      return *(translators__[k]);
    }


    /// returns the kth translator
    template < template < typename > class ALLOC >
    INLINE DBTranslator< ALLOC >&
           DBTranslatorSet< ALLOC >::translatorSafe(const std::size_t k) {
      if (translators__.size() <= k)
        GUM_ERROR(UndefinedElement, "Translator #" << k << "could not be found")

      return *(translators__[k]);
    }


    /// returns the kth translator
    template < template < typename > class ALLOC >
    INLINE const DBTranslator< ALLOC >&
       DBTranslatorSet< ALLOC >::translatorSafe(const std::size_t k) const {
      if (translators__.size() <= k)
        GUM_ERROR(UndefinedElement, "Translator #" << k << "could not be found")

      return *(translators__[k]);
    }


    /// returns the domain size of the variables stored into the kth translator
    template < template < typename > class ALLOC >
    INLINE std::size_t
           DBTranslatorSet< ALLOC >::domainSize(const std::size_t k) const {
      return translators__[k]->domainSize();
    }


    /// returns the domain size of the variables stored into the kth translator
    template < template < typename > class ALLOC >
    INLINE std::size_t
           DBTranslatorSet< ALLOC >::domainSizeSafe(const std::size_t k) const {
      if (translators__.size() <= k)
        GUM_ERROR(UndefinedElement, "Variable #" << k << "could not be found")

      return translators__[k]->domainSize();
    }


    /// returns the variable stored into the kth translator
    template < template < typename > class ALLOC >
    INLINE const Variable&
                 DBTranslatorSet< ALLOC >::variable(const std::size_t k) const {
      return *(translators__[k]->variable());
    }


    /// returns the variable stored into the kth translator
    template < template < typename > class ALLOC >
    INLINE const Variable&
       DBTranslatorSet< ALLOC >::variableSafe(const std::size_t k) const {
      if (translators__.size() <= k)
        GUM_ERROR(UndefinedElement, "Variable #" << k << "could not be found")

      return *(translators__[k]->variable());
    }


    // indicates whether a reordering is needed to make the kth translator
    // sorted by lexicographical order
    template < template < typename > class ALLOC >
    INLINE bool
       DBTranslatorSet< ALLOC >::needsReordering(const std::size_t k) const {
      return translators__[k]->needsReordering();
    }


    // indicates whether a reordering is needed to make the kth translator
    // sorted by lexicographical order
    template < template < typename > class ALLOC >
    INLINE bool
       DBTranslatorSet< ALLOC >::needsReorderingSafe(const std::size_t k) const {
      if (translators__.size() <= k)
        GUM_ERROR(UndefinedElement, "Variable #" << k << "could not be found")

      return translators__[k]->needsReordering();
    }


    // performs a reordering of the dictionary and returns a mapping
    // from the old translated values to the new ones.
    template < template < typename > class ALLOC >
    INLINE HashTable< std::size_t,
                      std::size_t,
                      ALLOC< std::pair< std::size_t, std::size_t > > >
           DBTranslatorSet< ALLOC >::reorder(const std::size_t k) {
      return translators__[k]->reorder();
    }


    // performs a reordering of the dictionary and returns a mapping
    // from the old translated values to the new ones.
    template < template < typename > class ALLOC >
    INLINE HashTable< std::size_t,
                      std::size_t,
                      ALLOC< std::pair< std::size_t, std::size_t > > >
           DBTranslatorSet< ALLOC >::reorderSafe(const std::size_t k) {
      if (translators__.size() <= k)
        GUM_ERROR(UndefinedElement, "Variable #" << k << "could not be found")

      return translators__[k]->reorder();
    }


    /** @brief returns the column of the input database that will be written
     * in the kth column of the DatabaseTable */
    template < template < typename > class ALLOC >
    INLINE std::size_t
           DBTranslatorSet< ALLOC >::inputColumn(const std::size_t k) const {
      return columns__[k];
    }


    /** @brief returns the column of the input database that will be written
     * in the kth column of the DatabaseTable */
    template < template < typename > class ALLOC >
    INLINE std::size_t
           DBTranslatorSet< ALLOC >::inputColumnSafe(const std::size_t k) const {
      if (translators__.size() <= k)
        GUM_ERROR(UndefinedElement, "Column #" << k << "could not be found")

      return columns__[k];
    }


    /// returns the largest input database column index read by the translators
    template < template < typename > class ALLOC >
    INLINE std::size_t DBTranslatorSet< ALLOC >::highestInputColumn() const {
      return highest_column__;
    }


    /// returns the number of translators stored into the set
    template < template < typename > class ALLOC >
    INLINE std::size_t DBTranslatorSet< ALLOC >::nbTranslators() const {
      return columns__.size();
    }


    /// returns the number of translators stored into the set
    template < template < typename > class ALLOC >
    INLINE std::size_t DBTranslatorSet< ALLOC >::size() const {
      return columns__.size();
    }

    /// returns the set of translators
    template < template < typename > class ALLOC >
    INLINE const
       std::vector< DBTranslator< ALLOC >*, ALLOC< DBTranslator< ALLOC >* > >&
       DBTranslatorSet< ALLOC >::translators() const {
      return translators__;
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
